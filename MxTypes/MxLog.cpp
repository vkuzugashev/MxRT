#include "MxLog.h"

volatile LOG_TYPE MxLog::m_level = LOG_TYPE::LEVEL_INFO;
mutex MxLog::m_mtx_log;
char MxLog::m_stime[LOG_TIME_SIZE];
string MxLog::m_logType;
time_t MxLog::m_seconds;
tm MxLog::m_timeinfo;
string MxLog::m_logfile = "";
ofstream MxLog::m_log;

void MxLog::LogOpen() {
	m_logfile = GetLogFile();
	m_log.open(m_logfile, m_log.app);
}

void MxLog::LogClose() {
	if (m_log.is_open())
		m_log.close();
}


string MxLog::GetLogFile() {
	char stime[11];
	time_t now = time(NULL);
	errno_t err = localtime_s(&m_timeinfo, &now);
	strftime(stime, 11, "%Y-%m-%d", &m_timeinfo);
	return "MxRT_" + string(stime) + ".log";
}

void MxLog::LogMsg(LOG_TYPE logType, string msg) {
	if (logType >= m_level) {
		m_mtx_log.lock();

		try {
			m_seconds = time(NULL);
			errno_t err = localtime_s(&m_timeinfo, &m_seconds);
			strftime(m_stime, LOG_TIME_SIZE, LOG_TIME_FORMAT, &m_timeinfo);

			switch (logType) {
			case LOG_TYPE::LEVEL_ERROR:
				m_logType = "ERROR";
				break;
			case LOG_TYPE::LEVEL_DEBUG:
				m_logType = "DEBUG";
				break;
			case LOG_TYPE::LEVEL_INFO:
				m_logType = "INFO";
				break;
			case LOG_TYPE::LEVEL_WARN:
				m_logType = "WARN";
				break;
			default:
				break;
			}

			cout << m_stime << " : " << m_logType << " : " << msg << endl;
			if (m_logfile != "") {
				m_log << m_stime << " : " << m_logType << " : " << msg << endl;
			}

			if (m_logfile != GetLogFile()) {
				LogClose();
				LogOpen();
			}
		}
		catch (exception& e) {
			cout << "MxLog exception: " << e.what() << endl;
			if (m_logfile != "")
				m_log << m_stime << " : WARN : MxLog exception: " << e.what() << endl;

		}
		catch (...) {
			cout << "MxLog exception!" << endl;
			if (m_logfile != "")
				m_log << "MxLog exception!" << endl;
		}

		m_mtx_log.unlock();
	}
}