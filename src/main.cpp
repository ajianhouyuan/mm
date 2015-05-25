#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "config.h"
#include "log.h"
#include "daemon.h"
#include "server.h"


CConfig *g_config = CConfig::get_instance();
CLog *g_log = CLog::get_instance();


int main(int argc, char *args[])
{
	g_config->parse_cmdline(argc,args);

//	if (g_config->IsDaemon())
//	{
//		CDaemon::daemon_exec();
//	}
//
//	if (g_config->IsDebug())
//	{
//		g_log->set_level(LEVEL_INFO);
//	}

	CServer *server = CServer::get_instance();
	server->start();


	return 0;
}



