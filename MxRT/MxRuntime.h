#ifndef MXRUNTIME_H
#define MXRUNTIME_H

#ifdef WIN32
#include <Windows.h>
#define dlopen LoadLibrary
#define dlsym GetProcAddress
#define dlclose FreeLibrary
#endif

#include <thread>
#include <queue>
#include <mutex>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <exception>
#include "..\MxTypes\MxConnector.h"
#include "MxScanClass.h"
#include "../MxTypes/MxLog.h"


using namespace std;


class MxRuntime
{
    public:
        MxRuntime();
        virtual ~MxRuntime();

        void Start();
        void Stop();
        void QueuePush(MxResult*);
        MxResult* QueuePop();

        string GetPathDll() { return m_path_dll; }
        void SetPathDll(string val) { m_path_dll = val; }

        string GetConnString() { return m_connString; }
        void SetConnString(string val) { m_connString = val; }

        void SetScanclasses(vector<MxScanClass*> val){ m_scanclasses = val; };
        vector<MxScanClass*> GetScanclasses(){ return m_scanclasses; };

        void SetName(string val) { m_name = val; };
        string GetName() { return m_name; };

    protected:

    private:
        string m_name;
        string m_path_dll;
        string m_connString;
        MxConnector* m_connector = NULL;
        vector<MxScanClass*> m_scanclasses;
        queue<MxResult*> m_queue;
        thread m_thread;
        volatile bool m_stop;
        mutex m_queue_mtx;


        HINSTANCE m_hdlConnLib;
        typedef void (*DLLSetLogLevel)(LOG_TYPE val);
        typedef MxConnector* (*DLLConnectorCreate)();
        typedef void (*DLLConnectorFree)(MxConnector*);

        DLLSetLogLevel dllSetLogLevel;
        DLLConnectorCreate dllConnectorCreate;
        DLLConnectorFree dllConnectorFree;

        void Run();
};

#endif // MXRUNTIME_H
