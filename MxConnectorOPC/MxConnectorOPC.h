#ifndef MXCONNECTOROPC_H
#define MXCONNECTOROPC_H

#pragma once
#pragma warning( disable: 4996 )

#ifdef WIN32
#include <Windows.h>
#include <ctime>
//#define dlopen LoadLibrary
//#define dlsym GetProcAddress
//#define dlclose FreeLibrary
#endif

#include "..\MxTypes\MxConnector.h"
#include "..\MxTypes\MxLog.h"
#include "..\MxTypes\MxTypes.h"
#include "LocalSyncOPCCLient.h"

#if _DEBUG
#pragma comment(lib, "../Debug/mxtypes.lib")
#else
#pragma comment(lib, "../Release/mxtypes.lib")
#endif

class MxConnectorOPC: public MxConnector
{
   public:
        MxConnectorOPC();
        virtual ~MxConnectorOPC();

        bool Open();
        void Close();
        void Read(time_t& timeVal, vector<MxRequest*>& req, vector<MxResult*>& res);
        void SetConnString(string val);
        bool GetState();

    protected:

    private:
        /*void* m_client;*/

		/*HINSTANCE m_hdlConnLib;
        typedef void (*DLLSetLogLevel)(LOG_TYPE val);
        typedef void* (*DLLOpen)(const char* connString);
        typedef void (*DLLClose)(void*);
        typedef bool (*DLLIsConnected)(void*);
        typedef bool (*DLLReadBool)(void*, const char* source, uint16_t& wqlt, FILETIME& timeVal);
        typedef int16_t (*DLLReadInt16)(void*, const char* source, uint16_t& wqlt, FILETIME& timeVal);
        typedef int32_t (*DLLReadInt32)(void*, const char* source, uint16_t& wqlt, FILETIME& timeVal);
        typedef float (*DLLReadFloat)(void*, const char* source, uint16_t& wqlt, FILETIME& timeVal);*/

       /* DLLSetLogLevel dllSetLogLevel;
        DLLOpen dllOpen;
        DLLClose dllClose;
        DLLIsConnected dllIsConnected;

        DLLReadBool dllReadBool;
        DLLReadInt16 dllReadInt16;
        DLLReadInt32 dllReadInt32;
        DLLReadFloat dllReadFloat;*/

		LocalSyncOPCCLient *m_client;

		time_t MxConnectorOPC::FiletimeToTime_t(const FILETIME& ft);
		char m_msg[255];



};

#endif // MXCONNECTOROPC_H
