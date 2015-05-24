#include "server.h"

CServer *CServer::m_pInstance = NULL;

CServer::CServer()
{
	memset(&m_server_addr, 0, sizeof(sockaddr_in));
	memset(&m_connect_socket_fd, 0, sizeof(sockaddr_in));

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


//int CServer::init_socket()
//{
//	m_server_addr.sin_family = AF_INET;
//	m_server_addr.sin_addr.s_addr = INADDR_ANY;
//	m_server_addr.sin_port = htons(3420);
//
//	if ((m_server_socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
//	{
//		g_log->log_error("init socket Error");
//		return -1;
//	}
//
//	if (bind(m_server_socket_fd, (struct sockaddr*)&m_server_addr, sizeof(struct sockaddr)) < 0)
//	{
//		g_log->log_error("bind socket Error");
//		return -1;
//	}
//
//	listen(m_server_socket_fd, 1024);
//
//	return 0;
//}

//int CServer::wait_socket()
//{
//	auto_ptr<char> pbuffer(new char[8*1024]);
//	while (true)
//	{
//		socklen_t sock_size = sizeof(struct sockaddr_in);
//		if ((m_connect_socket_fd = accept(m_server_socket_fd, (struct sockaddr *)&m_remote_addr, &sock_size) ) < 0)
//		{
//			g_log->log_error("accept socket Error");
//			return -1;
//		}
//
//		//char *pbuffer = (char*)malloc(8*1024);
//		int len = 0;
//		int total_len = 0;
//
//		while((len = recv(m_connect_socket_fd, pbuffer.get()+total_len, 8*1024 - total_len, 0)) > 0)
//		{
//			total_len = total_len + len;
//			if (total_len >= 8*1024)
//			{
//				break;
//			}
//		}
//
//		//check min len
//		if (total_len < 6)
//		{
//			//TODO:ERROR
//		}
//
//		//get op type
//		int key_len = 0;
//		int value_len = 0;
//		int op_type;
//
//		if (memcmp(pbuffer.get(), "get", 3) == 0)
//		{
//			op_type = OP_GET;
//		}
//		else if (memcmp(pbuffer.get(), "set", 3) == 0)
//		{
//			op_type = OP_SET;
//		}
//		else if (memcmp(pbuffer.get(), "del", 3) == 0)
//		{
//			op_type = OP_DEL;
//		}
//		else
//		{
//			//TODO: ERROR
//		}
//
//		//get first line len
//		int line_len = 0;
//		while (line_len < len && pbuffer.get()[line_len++]!='\n');
//		if (line_len == len)
//		{
//			//TODO:ERROR
//			g_log->log_error("head han't newLine char");
//			return -1;
//		}
//
//		//get keylen start pos
//		char *pkey_len_start = (char*)memchr(pbuffer.get(), ' ', line_len);
//
//		if (pkey_len_start - pbuffer.get() == line_len)
//		{
//			//TODO:ERROR
//		}
//
//		sscanf(pkey_len_start, "%d", &key_len);
//		if (key_len < 0)
//		{
//			//TODO:ERRPR
//		}
//
//		//get valuelen start pos
//		char *pvalue_len_start = (char*)memchr(pkey_len_start, ' ', line_len);
//
//		if (pvalue_len_start - pbuffer.get() == line_len)
//		{
//			//TODO:ERROR
//		}
//
//		sscanf(pvalue_len_start, "%d", &value_len);
//		if (value_len < 0)
//		{
//			//TODO:ERROR
//		}
//
//		int packet_size = key_len + value_len + line_len;
//		char *ppacket_buffer = (char*)malloc( packet_size* sizeof(char));
//		char *ppacket_pos = ppacket_buffer;
//
//		if (NULL == ppacket_buffer)
//		{
//			//TODO:ERROR
//		}
//
//		memcpy(ppacket_buffer, pbuffer.get(), total_len);
//		ppacket_pos = ppacket_pos + total_len;
//
//		if (len > 0)
//		{
//			while((len = recv(m_connect_socket_fd, ppacket_pos, packet_size - total_len, 0)) > 0)
//			{
//				total_len = total_len + len;
//				ppacket_pos = ppacket_pos + len;
//			}
//		}
//
//		PACKET packet;
//		packet.op_type = op_type;
//		packet.key_len = key_len;
//		packet.value_len = value_len;
//		packet.total_len = total_len;
//		packet.praw = ppacket_buffer;
//		packet.pkey = ppacket_buffer + line_len + 1;
//		packet.pvalue = ppacket_buffer + line_len + 1 + key_len;
//
//		dispatch(packet);
//	}
//	return 0;
//}

//int CServer::close_socket()
//{
//
//	return 0;
//}

int CServer::start()
{
	printf("[DEBUG]CServer::start()\n");
	bool bis_quit = false;

	netio->set(server_socket->fd(),EPOLLIN, server_socket);
	//printf("[DEBUG]set(server_socket->fd(),EPOLLIN .....\n");

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
					//ready_list.push_back(conn_csocket);
//					if (conn_csocket->is_error())
//					{
//						continue;
//					}

					int len = conn_csocket->recv();
					if (len <= 0)
					{
						//conn_csocket->mark_error();
						netio->del(conn_csocket->fd());
						delete conn_csocket;
						continue;
					}

					ready_list.push_back(conn_csocket);
				}
				if (socket_event->flags & EPOLLOUT)
				{
//					if (conn_csocket->is_error())
//					{
//						continue;
//					}

					int len  = conn_csocket->send();
					if(len <= 0)
					{
						//conn_csocket->mark_error();
						netio->del(conn_csocket->fd());
						delete conn_csocket;
						continue;
					}

					if (conn_csocket->output_buffer->empty())
					{
						netio->clr(conn_csocket->fd(), EPOLLOUT);
						netio->set(conn_csocket->fd(), EPOLLIN, conn_csocket);
					}
				}
			}
		}

		for (st_ready_list::iterator it=ready_list.begin(); it != ready_list.end(); it++)
		{
			CSocket *conn_csocket = *it;
//			if (conn_csocket->is_error())
//			{
//				netio->del(conn_csocket->fd());
//				delete conn_csocket;
//				continue;
//			}

			vector<char> *recv_data = conn_csocket->check();
			if (NULL == recv_data)
			{
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
				netio->set(conn_csocket->fd(), EPOLLIN, conn_csocket);
			}
			else
			{
				netio->clr(conn_csocket->fd(), EPOLLIN);
			}

		}

	}
}

//int CServer::dispatch(PACKET packet)
//{
//
//	return 0;
//}

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
			output = cache_it->second;
		}
		else
		{
			output = "-ERR KEY_NOT_FOUND";
		}

		//-DATA 15/0data_data_data_/0
		printf("[DEBUG]get key=%s value=%s\n", packet_data[1].c_str(), output.c_str());
	}
	else if ( strcmp(packet_data[0].c_str(),"set") == 0)
	{
		cache.insert(make_pair(packet_data[1],packet_data[2]));
		output = "-OK SET";

		printf("[DEBUG]set key=%s value=%s\n",packet_data[1].c_str(), packet_data[2].c_str());
	}
	else if ( strcmp(packet_data[0].c_str(),"del") == 0)
	{
		map<string, string>::iterator cache_it = cache.find(packet_data[1]);
		if ( cache_it != cache.end())
		{
			cache.erase(cache_it);
			output = "-OK DEL";
		}
		else
		{
			output = "-ERR KEY_NOT_FOUND";
		}

		printf("[DEBUG]del key=%s value=%s\n", packet_data[1].c_str(), output.c_str());
	}
	else
	{
		output = "-ERR COMMAND_ERROR";
		return -1;
	}

	return 0;
}








