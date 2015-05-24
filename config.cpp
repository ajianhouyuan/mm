#include "config.h"

CConfig* CConfig::m_pInstance = NULL;

CConfig::CConfig()
{
	m_bIsDebug = false;
	m_bIsDaemon = false;
}

CConfig ::~CConfig()
{
	delete CConfig::m_pInstance;
	CConfig::m_pInstance = NULL;
}

CConfig *CConfig::get_instance()
{
	if (NULL == CConfig::m_pInstance)
	{
		CConfig::m_pInstance = new CConfig();
	}
	return CConfig::m_pInstance;
}

bool CConfig::IsDebug()
{
	return m_bIsDebug;
}

bool  CConfig::IsDaemon()
{
	return m_bIsDaemon;
}

int CConfig::parse_cmdline(const int argc, char *args[])
{
	const char short_option[] = "bd";
	struct option long_options[] = {
		{"debug", no_argument, NULL, 'b'},
		{"daemon", no_argument, NULL, 'd'},
		{0, 0, 0, 0},
	};

	int c;
	while ((c = getopt_long(argc, args, short_option, long_options, NULL)) != -1)
	{
		switch(c)
		{
			case 'b':
			{
				m_bIsDebug = true;
				g_log->log_info("IsDebug = True");
				break;
			}
			case 'd':
			{
				m_bIsDaemon = true;
				g_log->log_info("IsDaemon = True");
				break;
			}
			default:
				g_log->log_error("illegal argument");
				exit(0);
		}
	}
	return 0;
}
