#include "MxConnectorOPC.h"

extern "C" 
{
	__declspec(dllexport) void SetLogLevel(LOG_TYPE val) {
		MxLog::SetLevel(val);
	}

	__declspec(dllexport) MxConnector* ConnectorCreate() {
		return new MxConnectorOPC();
	}
	__declspec(dllexport) void ConnectorFree(MxConnector* conn) {
		delete (MxConnectorOPC*)conn;
	}
}
