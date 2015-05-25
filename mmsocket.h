#ifndef MMSOCKET_H_
#define MMSOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

class CSocket
{
public:
	CSocket();
	~CSocket();

	void set_noblock(bool enable = true);
	void set_nodelay(bool enable = true);
	void set_keepalive(bool enable = true);

	static CSocket *connect(char *ip, int port);
	static CSocket *listen(char *ip, int port);
	CSocket *accept();

	int recv();
	int send();

	vector<char> * check();

	vector<char> *input_buffer;
	vector<char> *output_buffer;

	int input_pos;
	int output_pos;

	int fd()
	{
		return socket_fd;
	}

private:

	int socket_fd;
	bool bis_noblock;

	vector<char> *recv_buffer;
};


#endif /* MMSOCKET_H_ */
