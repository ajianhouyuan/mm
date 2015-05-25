#ifndef DAEMON_H_
#define DAEMON_H_

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "main.h"

class CDaemon
{
public:
	static void daemon_exec();
};



#endif
