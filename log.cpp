#include "log.h"

CLog *CLog::m_pInstance = NULL;

CLog::CLog()
{
	m_nLevel = LEVEL_INFO;
	m_pLogFile = fopen("log.log","aw+");
}

CLog::~CLog()
{
	fclose(m_pLogFile);
}

CLog* CLog::get_instance()
{
	if (NULL == CLog::m_pInstance)
	{
		CLog::m_pInstance = new CLog();
	}
	return CLog::m_pInstance;
}

int CLog::set_level(int nLevel)
{
	if (nLevel < 0 || nLevel > 3)
	{
		return -1;
	}
	m_nLevel = nLevel;
	return 1;
}

void CLog::write_log(const char *szLog, int nLevel)
{
	if (NULL == m_pLogFile)
	{
		printf("%s %d\n", "[ERROR]Create LogFile Fail Errno=", errno);
		return;
	}

	time_t now;
	struct tm *tm_now;

	time(&now);
	tm_now = localtime(&now);

	fprintf(m_pLogFile, "%04d-%02d-%02d %02d:%02d:%02d\t", 1900 + tm_now->tm_year, 1 + tm_now->tm_mon,
			tm_now->tm_mday, tm_now->tm_hour,tm_now->tm_min,tm_now->tm_sec);

	string strLevel;
	switch(nLevel)
	{
	case LEVEL_INFO:
		strLevel = "[INFO   ]\t";
		break;
	case LEVEL_WARNING:
		strLevel = "[WARNING]\t";
		break;
	case LEVEL_ERROR:
		strLevel = "[ERROR  ]\t";
	}

	fprintf(m_pLogFile, "%s" , strLevel.c_str());
	fprintf(m_pLogFile, "%s\n", szLog);
}

void CLog::log_info(const char *szLog)
{
	if (0 == m_nLevel)
	{
		write_log(szLog, LEVEL_INFO);
	}
}

void CLog::log_warning(const char *szLog)
{
	if (1 <= m_nLevel)
	{
		write_log(szLog, LEVEL_WARNING);
	}
}

void CLog::log_error(const char *szLog)
{
	if (2 <= m_nLevel)
	{
		write_log(szLog, LEVEL_ERROR);
	}
}
