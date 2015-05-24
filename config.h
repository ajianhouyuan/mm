#ifndef CONFIG_H_
#define CONFIG_H_
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "main.h"


class CConfig
{
private:
	CConfig();
	~CConfig();
	static CConfig *m_pInstance;
public:
	static CConfig *get_instance();
	int parse_cmdline(const int argc, char *args[]);
	bool IsDebug();
	bool IsDaemon();
private:
	bool m_bIsDebug;
	bool m_bIsDaemon;
};

extern CConfig *g_config;
#endif
