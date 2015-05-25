#include "mmsocket.h"

#define LISTEN_COUNT		3420

CSocket::CSocket()
{
	socket_fd = -1;
	bis_noblock = false;

	input_buffer = new vector<char>;
	output_buffer = new vector<char>;
	recv_buffer = new vector<char>;

	input_pos = 0;
	output_pos = 0;
}

CSocket::~CSocket()
{
	delete input_buffer;
	delete output_buffer;
	delete recv_buffer;
}

void CSocket::set_noblock(bool enable)
{
	if (socket_fd != -1)
	{
		bis_noblock = enable;
		if(enable)
		{
			fcntl(socket_fd, F_SETFL, O_NONBLOCK | O_RDWR);
		}
		else
		{
			fcntl(socket_fd, F_SETFL, O_RDWR);
		}
	}
}

void CSocket::set_nodelay(bool enable)
{
	if (socket_fd != -1)
	{
		int opt = enable ? 1 : 0;
		setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
	}
}

void CSocket::set_keepalive(bool enable)
{
	if (socket_fd != -1)
	{
		int opt = enable ? 1 : 0;
		setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
	}
}

//CSocket *CSocket::connect(char *ip, int port)
//{
//	int sock_fd = -1;
//	struct sockaddr_in addr;
//	memset(&addr, 0, sizeof(addr));
//	addr.sin_family = AF_INET;
//	addr.sin_port = htons((short)port);
//	inet_pton(AF_INET, ip, &addr.sin_addr);
//
//	CSocket *csocket = new CSocket();
//
//	if((sock_fd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
//	{
//		goto err;
//	}
//
//	if (::connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
//	{
//		goto err;
//	}
//
//	csocket->socket_fd = sock_fd;
//	csocket->set_keepalive(true);
//	return csocket;
//
//err:
//	close(sock_fd);
//	return NULL;
//}

CSocket *CSocket::listen(char *ip, int port)
{
	int sock_fd = -1;

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons((short)port);
	inet_pton(AF_INET, ip, &addr.sin_addr);

	int opt = 1;

	CSocket *csocket = new CSocket();

	if ((sock_fd = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		goto err;
	}

	if (::setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		goto err;
	}

	if (::bind(sock_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1)
	{
		goto err;
	}

	if(::listen(sock_fd, LISTEN_COUNT) == -1)
	{
		goto err;
	}



	csocket->socket_fd = sock_fd;
	return csocket;

err:
	close(sock_fd);
	return NULL;

}

CSocket *CSocket::accept()
{
	int connect_socket_fd = -1;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	while ((connect_socket_fd = ::accept(socket_fd, (struct sockaddr*)&addr, &addr_len)) == -1)
	{
		if (errno != EINTR)
		{
			return NULL;
		}
	}

	struct linger opt = {1, 0};
	int ret = ::setsockopt(connect_socket_fd, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));
	if (0 != ret)
	{
		//nothing
	}

	CSocket *csocket = new CSocket();

	csocket->socket_fd = connect_socket_fd;
	csocket->set_keepalive(true);
	return csocket;
}

int CSocket::recv()
{
	vector<char> *buffer = input_buffer;

	if ( buffer->size()==0 )
	{
		input_pos = 0;
	}

	if (input_pos == buffer->size())
	{
		buffer->resize(buffer->size()*2+16, 0);
	}

	while(input_pos < buffer->size())
	{
		int len  = ::read(socket_fd, &(buffer->at(input_pos)), buffer->size()-input_pos);
		//printf("[DEBUG]read len = %d\n", len);
		//printf("[DEBUG]read buffer size = %d\n", buffer->size());
		//printf("[DEBUG]read input_pos = %d\n", input_pos);
		if (-1 == len)
		{
			printf("[DEBUG]recv errno = %d\n",errno);
			if(errno == EINTR)
			{
				continue;
			}
			else if(errno == EWOULDBLOCK)
			{
				break;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			if(len == 0)
			{
				return 0;
			}
			input_pos += len;
		}

		if (!bis_noblock)
		{
			break;
		}
	}

	return input_pos;
}

int CSocket::send()
{
	vector<char> *buffer = output_buffer;

	while(output_pos < output_buffer->size())
	{
		int len = ::write(socket_fd, &buffer->at(output_pos), buffer->size()-output_pos);
		//printf("[DEBUG]write len = %d\n", len);
		//printf("[DEBUG]write buffer size = %d\n", buffer->size());
		//printf("[DEBUG]write output_pos = %d\n", output_pos);
		if (-1 == len)
		{
			printf("[DEBUG]send errno = %d\n",errno);
			if (errno == EINTR)
			{
				continue;
			}
			else if (errno == EWOULDBLOCK)
			{
				break;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			if (0 == len)
			{
				break;
			}
			output_pos += len;

			if (output_pos == output_buffer->size())
			{
				output_buffer->clear();
				int ret = output_pos;
				output_pos = 0;

				return ret;
			}

		}

		if (!bis_noblock)
		{
			break;
		}
	}

	return output_pos;
}

vector<char> *CSocket::check()
{
	recv_buffer->clear();

	if (input_buffer->empty())
	{
		return recv_buffer;
	}

	int size = input_buffer->size();
	char *head = (char*)&(input_buffer->at(0));

	char *body = (char *)memchr(head, '\0', size);
	if (NULL == body)
	{
		recv_buffer->clear();
		return recv_buffer;
	}

	body++;

	int body_len = 0;
	string packet_size_str(head,body-head);
	sscanf(packet_size_str.c_str(), "%d", &body_len);

	if (body_len > input_pos-(body-head))
	{
		recv_buffer->clear();
		return recv_buffer;
	}
	else
	{
		vector<char> temp(body, body + body_len);
		recv_buffer->swap(temp);
		input_buffer->clear();

		return recv_buffer;
	}
}










