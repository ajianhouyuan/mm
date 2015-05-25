#include "server.h"

CServer *CServer::m_pInstance = NULL;

CServer::CServer()
{
	netio = new CNetIO();
}

CServer::~CServer()
{

}

CServer *CServer::get_instance()
{
	if (NULL == CServer::m_pInstance)
	{
		CServer::m_pInstance = new CServer();
		printf("[DEBUG]*CServer::get_instance() New CServer\n");
		CServer::m_pInstance->server_socket = CSocket::listen("0.0.0.0",1234);
		printf("[DEBUG]CSocket::listen('0.0.0.0',1234)\n");
	}
	return CServer::m_pInstance;
}

int CServer::start()
{
	printf("[DEBUG]CServer::start()\n");
	bool bis_quit = false;

	netio->set(server_socket->fd(),EPOLLIN, server_socket);

	st_p_event *vec_socket_events;

	while (!bis_quit)
	{
		vec_socket_events = netio->wait(50);

		ready_list.clear();

		for (int i = 0; i < vec_socket_events->size(); i++)
		{
			NET_IO_EVENT *socket_event = vec_socket_events->at(i);
			if (socket_event->ptr == server_socket)
			{
				CSocket *csocket = accept_socket();
				if (NULL != csocket)
				{
					netio->set(csocket->fd(), EPOLLIN, csocket);
				}
			}
			else
			{
				CSocket *conn_csocket = (CSocket*)socket_event->ptr;
				if (socket_event->flags & EPOLLIN)
				{
					int len = conn_csocket->recv();
					if (len <= 0)
					{
						printf("[DEBUG]read del!!!!\n");
						netio->del(conn_csocket->fd());
						delete conn_csocket;
						continue;
					}

					ready_list.push_back(conn_csocket);
				}
				if (socket_event->flags & EPOLLOUT)
				{
					int len  = conn_csocket->send();
					if(len <= 0)
					{
						printf("[DEBUG]write del!!!!\n");
						netio->del(conn_csocket->fd());
						delete conn_csocket;
						continue;
					}

					if (conn_csocket->output_buffer->empty())
					{
						printf("[DEBUG]write empty!!!!\n");
						netio->clr(conn_csocket->fd(), EPOLLOUT);
						netio->set(conn_csocket->fd(), EPOLLIN, conn_csocket);
					}
				}
			}
		}

		for (st_ready_list::iterator it=ready_list.begin(); it != ready_list.end(); it++)
		{
			CSocket *conn_csocket = *it;

			vector<char> *recv_data = conn_csocket->check();
			if (NULL == recv_data)
			{
				printf("[DEBUG]recv_data del!!!!\n");
				netio->del(conn_csocket->fd());
				delete conn_csocket;
				continue;
			}

			if (recv_data->empty())
			{
				netio->set(conn_csocket->fd(), EPOLLIN, conn_csocket);
				continue;
			}

			vector<string> packet_data;
			string output;

			if (parse_packet(&packet_data, recv_data) != -1)
			{
				proc_command(packet_data, output);
			}
			else
			{
				output = "-ERR PARSE_ERROR";
			}

			conn_csocket->output_buffer->assign(output.begin(), output.end());


			if(!conn_csocket->output_buffer->empty())
			{
				netio->set(conn_csocket->fd(), EPOLLOUT, conn_csocket);
			}
			else
			{
				netio->clr(conn_csocket->fd(), EPOLLOUT);
			}

			if(conn_csocket->input_buffer->empty())
			{
				netio->clr(conn_csocket->fd(), EPOLLIN);
				//netio->set(conn_csocket->fd(), EPOLLIN, conn_csocket);
			}
		}
	}
}

CSocket *CServer::accept_socket()
{
	CSocket *csocket = server_socket->accept();

	if (NULL == csocket)
	{
		return NULL;
	}

	csocket->set_nodelay();
	csocket->set_noblock();

	return csocket;
}

int CServer::parse_packet(vector<string> *packet_data, vector<char> *recv_data)
{
	//11\0get\04\0test\0
	char *head = &recv_data->at(0);
	char *pos = head;
	while (pos - head < recv_data->size() )
	{
		string strkeylen(pos);
		int keylen = 0;
		sscanf(strkeylen.c_str(),"%d",&keylen);

		pos += strkeylen.size() + 1;

		if (0 == keylen || pos - head > recv_data->size())
		{
			return -1;
		}

		string keydata(pos, keylen);
		packet_data->push_back(keydata);

		pos += keydata.size() + 1;

		if (pos - head > recv_data->size())
		{
			return -1;
		}
	}
	return 0;
}

int CServer::proc_command(vector<string> &packet_data, string &output)
{
	if(strcmp(packet_data[0].c_str(),"get") == 0)
	{
		map<string, string>::iterator cache_it = cache.find(packet_data[1]);
		if ( cache_it != cache.end())
		{
			string value;

			ostringstream body_num,total_num;
			body_num<<(int)cache_it->second.length();


			value = "5";
			value = value + '\0';
			value = "-DATA";
			value = value + '\0';
			value = value + body_num.str();
			value = value + '\0';
			value = value + cache_it->second;
			value = value + '\0';

			total_num<<(int)value.length();

			output = total_num.str();
			output = output + '\0';
			output = output + value;
		}
		else
		{
			string temp("19\0-ERR KEY_NOT_FOUND\0",22);
			output = temp;
		}

		//-DATA 15/0data_data_data_/0
		//printf("[DEBUG]get key=%s value=%s\n", packet_data[1].c_str(), output.c_str());
	}
	else if ( strcmp(packet_data[0].c_str(),"set") == 0)
	{
		cache[packet_data[1]] = packet_data[2];
		string temp("8\0-OK SET\0",10);
		output = temp;

		//printf("[DEBUG]set key=%s value=%s\n",packet_data[1].c_str(), packet_data[2].c_str());
	}
	else if ( strcmp(packet_data[0].c_str(),"del") == 0)
	{
		map<string, string>::iterator cache_it = cache.find(packet_data[1]);
		if ( cache_it != cache.end())
		{
			cache.erase(cache_it);

			string temp("8\0-OK DEL\0",10);
			output = temp;
		}
		else
		{
			string temp("19\0-ERR KEY_NOT_FOUND\0",22);
			output = temp;
		}

		//printf("[DEBUG]del key=%s value=%s\n", packet_data[1].c_str(), output.c_str());
	}
	else
	{
		string temp("19\0-ERR COMMAND_ERROR\0",22);
		output = temp;
		return -1;
	}

	return 0;
}








