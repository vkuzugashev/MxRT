#include <iostream>
#include <memory>
#include <exception>
#include <direct.h>
#include "../MxTypes/MxLog.h"
#include "../MxTypes/MxTypes.h"
#include "MxServer.h"

using namespace std;

MxServer server;

#ifdef WIN32
SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;
char* serviceName = "MxRT";

void ControlHandler(DWORD request) {
    switch(request)
    {
        case SERVICE_CONTROL_STOP:
            MxLog::LogInfo("Service stopping ...");
            server.Stop();
            MxLog::LogInfo("Service stopped.");

            serviceStatus.dwWin32ExitCode = 0;
            serviceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus (serviceStatusHandle, &serviceStatus);
            return;

        case SERVICE_CONTROL_SHUTDOWN:
            MxLog::LogInfo("Service stopping ...");
            server.Stop();
            MxLog::LogInfo("Service stopped.");

            serviceStatus.dwWin32ExitCode = 0;
            serviceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus (serviceStatusHandle, &serviceStatus);
            return;

        default:
            break;
    }

    SetServiceStatus (serviceStatusHandle, &serviceStatus);

    return;
}


void WINAPI ServiceMain(DWORD argc, LPSTR* argv) {

    MxLog::LogInfo("ServiceMain()");

    serviceStatus.dwServiceType    = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState    = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted  = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    serviceStatus.dwWin32ExitCode   = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint     = 0;
    serviceStatus.dwWaitHint      = 0;

    MxLog::LogInfo("Register control handler ...");
    serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)ControlHandler);
    if (serviceStatusHandle == (SERVICE_STATUS_HANDLE)0) {
        MxLog::LogError("Fail register control handler!");
        return;
    }

    MxLog::LogInfo("Service starting ...");
    server.Start();
	MxLog::LogInfo("Service started!");

    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus (serviceStatusHandle, &serviceStatus);

    while(serviceStatus.dwCurrentState == SERVICE_RUNNING){
        this_thread::sleep_for(chrono::milliseconds(500));
    }

    return;
}

#endif // WIN32



int main(int argc, char *argv[])
{
    bool isConsole = false;
    try
	{
        MxLog::SetLevel(LOG_TYPE::LEVEL_INFO);

		for (int i = 1; i < argc; i++) {
            string str = string(argv[i]);
			if (str == "-ldebug")
				MxLog::SetLevel(LOG_TYPE::LEVEL_DEBUG);
			else if (str == "-lwarn")
				MxLog::SetLevel(LOG_TYPE::LEVEL_WARN);
			else if (str == "-lerror")
				MxLog::SetLevel(LOG_TYPE::LEVEL_ERROR);
			else if (str == "-console")
                isConsole = true;
            else if(str.substr(0,2)=="-d")
                _chdir(str.substr(2).c_str());
		}

		MxLog::LogOpen();
        MxLog::LogInfo(server.GetName());
        MxLog::LogInfo(server.GetVersion());
        MxLog::LogInfo(server.GetAuthor());

        server.LoadConfig();

        if(isConsole){
            server.Start();
            cin.get();
        } else {
//            SERVICE_TABLE_ENTRY ServiceTable[1];
//            ServiceTable[0].lpServiceName = serviceName;
//            ServiceTable[0].lpServiceProc = ServiceMain;
            SERVICE_TABLE_ENTRY ServiceTable[] {
                {serviceName, ServiceMain},
                {NULL,NULL}
            };


            if(!StartServiceCtrlDispatcher(ServiceTable)){
                DWORD dw = GetLastError();
                char buff[500];
                sprintf_s(buff, "StartServiceCtrlDispatcher(), error %d", dw);
                MxLog::LogError(string(buff));
            }
        }
    }
    catch(...)
    {
        MxLog::LogInfo("Main exception!");
    }

    if(isConsole){
        server.Stop();
    }
    MxLog::LogClose();

    return 0;

}
