#ifndef SERVER_H_
#define SERVER_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <sstream>

#include "main.h"
#include "netio.h"
#include "mmsocket.h"

using namespace std;

#define	 OP_GET	1
#define OP_SET	2
#define OP_DEL	3

typedef struct _PACKET
{
	int op_type;
	int key_len;
	int value_len;
	int total_len;
	char *pkey;
	char *pvalue;
	char *praw;
}PACKET;


typedef vector<CSocket *> st_ready_list;

class CServer
{
private:
	CServer();
	~CServer();

public:
	static CServer *get_instance();


	int start();

private:
	CSocket *accept_socket();

	int parse_packet(vector<string> *packet_data, vector<char> *recv_data);
	int proc_command(vector<string> &packet_data, string &output);

private:

	static CServer *m_pInstance;
	st_ready_list ready_list;

	CNetIO *netio;
	CSocket *server_socket;

	map<string, string> cache;

};


#endif /* SERVER_H_ */
