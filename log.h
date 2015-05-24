#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string>

#include "main.h"

using namespace std;

#define LEVEL_INFO	0
#define LEVEL_WARNING 1
#define LEVEL_ERROR	2


class CLog
{
private:
	CLog();
	~CLog();
	static CLog *m_pInstance;

	void write_log(const char * szLog, int nLevel);
public:
	static CLog *get_instance();
	int set_level(int nLevel);

	void log_info(const char *szLog);
	void log_warning(const char *szLog);
	void log_error(const char *szLog);


private:
	int m_nLevel;

	FILE *m_pLogFile;
};

extern CLog *g_log;
#endif

