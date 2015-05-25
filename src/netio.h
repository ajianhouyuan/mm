#ifndef NETIO_H_
#define NETIO_H_

#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <vector>

#define MAX_EVENT	3420

using namespace std;

typedef struct _NET_IO_EVENT
{
	int fd;
	int flags;
	void *ptr;
}NET_IO_EVENT;

typedef vector<NET_IO_EVENT*> st_p_event;

class CNetIO
{
public:
	CNetIO();
	~CNetIO();

	bool is_set(int fd, int flag);
	int set(int fd, int flags, void *data_ptr);
	st_p_event *wait(int timeout_ms = -1);
	int del(int fd);
	int clr(int fd, int flag);
private:
	NET_IO_EVENT *get_event(int fd);

	st_p_event events;
	st_p_event ready_events;

	struct epoll_event epoll_events[MAX_EVENT];
	int epoll_fd;
};

#endif
