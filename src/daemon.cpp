#include "daemon.h"

void CDaemon::daemon_exec()
{
	int pid = fork();

	if (pid == -1)
	{
		g_log->log_error("fork error!");
		exit(1);
	}
	if (pid > 0)
	{
		g_log->log_info("fork success!");
		exit(0);
	}

	if (setsid() == -1)
	{
		g_log->log_error("setsid error!");
		exit(1);
	}

	pid = fork();
	if(-1 == pid)
	{
		g_log->log_error("second fork error!");
		exit(1);
	}

	if (pid>0)
	{
		g_log->log_info("exit first child process!");
		exit(0);
	}

	chdir("/");

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	int fd_null = open("/dev/null",O_RDWR);

	if (-1 == fd_null)
	{
		g_log->log_error("open /dev/null error!");
		exit(1);
	}

	if (dup2(fd_null, STDIN_FILENO) == -1 || dup2(fd_null, STDOUT_FILENO) == -1
			|| dup2(fd_null, STDERR_FILENO) == -1)
	{
		g_log->log_error("dup2 error!");
		exit(1);
	}

	umask(0);
}
