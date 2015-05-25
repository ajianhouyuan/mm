#include "netio.h"

#define EPOLL_COUNT		3420


CNetIO::CNetIO()
{
	epoll_fd = epoll_create(EPOLL_COUNT);
}

CNetIO::~CNetIO()
{
	if (epoll_fd)
	{
		close(epoll_fd);
	}
}

bool CNetIO::is_set(int fd, int flag)
{
	NET_IO_EVENT *temp = get_event(fd);
	return (bool)temp->flags&flag;
}

int CNetIO::set(int fd, int flags, void *data_ptr)
{
	NET_IO_EVENT *socket_io_event = get_event(fd);
	if (socket_io_event->flags & flags)
	{
		return 0;
	}

	int epoll_op = socket_io_event->flags?EPOLL_CTL_MOD:EPOLL_CTL_ADD;

	socket_io_event->flags |= flags;
	socket_io_event->ptr = data_ptr;

	struct epoll_event st_epoll_event;
	st_epoll_event.data.ptr = socket_io_event;
	st_epoll_event.events = socket_io_event->flags;


	if (epoll_ctl(epoll_fd, epoll_op, fd, &st_epoll_event) == -1)
	{
		return -1;
	}

	return 0;
}

st_p_event *CNetIO::wait(int timeout_ms)
{
	NET_IO_EVENT *socket_io_event = NULL;
	struct epoll_event *st_epoll_event = NULL;
	ready_events.clear();

	int ready_count = epoll_wait(epoll_fd, epoll_events, MAX_EVENT, timeout_ms);
	if ( -1 == ready_count)
	{
		if (errno == EINTR)
		{
			return &ready_events;
		}
		return NULL;
	}

	for (int i=0; i < ready_count; i++)
	{
		st_epoll_event = &epoll_events[i];
		socket_io_event = (NET_IO_EVENT*) st_epoll_event->data.ptr;

		socket_io_event->flags = st_epoll_event->events;

		ready_events.push_back(socket_io_event);
	}

	return &ready_events;
}

int CNetIO::del(int fd)
{
	struct epoll_event st_epoll_event;
	if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &st_epoll_event))
	{
		return -1;
	}

	NET_IO_EVENT *net_io_event = get_event(fd);
	net_io_event->flags = 0;
	return 0;
}

int CNetIO::clr(int fd, int flag)
{
	NET_IO_EVENT *socket_io_event = get_event(fd);
	if (!(socket_io_event->flags&flag))
	{
		return 0;
	}

	socket_io_event->flags &= ~flag;
	int epoll_op = socket_io_event->flags?EPOLL_CTL_MOD: EPOLL_CTL_DEL;

	struct epoll_event st_epoll_event;

	st_epoll_event.data.ptr = socket_io_event;
	st_epoll_event.events = socket_io_event->flags;

	int ret = epoll_ctl(epoll_fd, epoll_op, fd, &st_epoll_event);
	if (-1 == ret)
	{
		return -1;
	}
	return 0;
}

NET_IO_EVENT *CNetIO::get_event(int fd)
{
	while (events.size() <= fd)
	{
		NET_IO_EVENT *socket_io_event = new NET_IO_EVENT();
		socket_io_event->fd = fd;
		socket_io_event->flags = 0;

		events.push_back(socket_io_event);
	}

	return events[fd];
}
