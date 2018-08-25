#ifndef LOG_H
#define LOG_H

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include <ctime>
#include "MxTypes.h"

using namespace std;

class MxLog
{
    public:
		static void LogInfo(string msg) { LogMsg(LOG_TYPE::LEVEL_INFO, msg); }
		static void LogWarn(string msg) { LogMsg(LOG_TYPE::LEVEL_WARN, msg); }
		static void LogError(string msg) { LogMsg(LOG_TYPE::LEVEL_ERROR, msg); }
		static void LogDebug(string msg) { LogMsg(LOG_TYPE::LEVEL_DEBUG, msg); }
		static void SetLevel(LOG_TYPE val) { m_level = val; }
		static LOG_TYPE GetLevel() { return m_level; }
		static void LogOpen();
		static void LogClose();

    protected:

    private:
		static volatile LOG_TYPE m_level;
		static mutex m_mtx_log;
		static char m_stime[LOG_TIME_SIZE];
		static string m_logType;
		static time_t m_seconds;
        static tm m_timeinfo;
		static string m_logfile;
		static ofstream m_log;

		static string GetLogFile();
		static void LogMsg(LOG_TYPE logType, string msg);

};



#endif // LOG_H
