#ifndef MXSERVER_H
#define MXSERVER_H

#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <map>
#include <queue>
#include <cmath>


#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif // WIN32

#include "../Modbus/modbus.h"
#include "../MxTypes/MxTypes.h"
#include "MxTag.h"
#include "MxScanClass.h"
#include "MxRuntime.h"
#include "MxStorage.h"
#include "MxModbus.h"

#pragma comment(lib, "lib/libsqlite3.a")
#pragma comment(lib, "ws2_32.lib")

#if _DEBUG
#pragma comment(lib, "../Debug/modbus.lib")
#pragma comment(lib, "../Debug/mxtypes.lib")
#else
#pragma comment(lib, "../Release/modbus.lib")
#pragma comment(lib, "../Release/mxtypes.lib")
#endif

using namespace std;
using namespace std::chrono;

class MxStorage;
class MxModbus;


class MxServer
{
    public:
        MxServer();
        virtual ~MxServer();

        void LoadConfig();
        void Start();
        void Stop();
        void QueuePush(MxResult*);
        MxResult* QueuePop();
        bool SetTagVal(string name, system_clock::time_point updateTime, TAG_QUALITY quality, bool boolVal, int64_t& intVal, float& floatVal, string strVal);
        MxTag GetTag(string name);
        string GetName() { return m_name; };
        string GetVersion() { return m_version; };
        string GetAuthor() { return m_author; };

    protected:

    private:
        string m_name = "MxRT runtime data collection station.";
        string m_version = "Version 1.0, 2017";
        string m_author = "Kuzugashev VI, penart@list.ru";


        map<string, MxTag*> m_cache;   //кеш тегов
        queue<MxResult*> m_queue;   //Очередь сообщений
        vector<MxRuntime*> m_runtimes;
        vector<MxScanClass*> m_scanclasses;

        MxStorage* m_storage;
        MxModbus* m_modbus;

        bool m_rtLinkEnable;
        bool m_rtLinkSqlEnable;
        bool m_modbusEnable;
        bool m_storageEnable;

        uint16_t m_rtLinkPort;
        uint16_t m_rtLinkSqlPort;

        thread m_thread;
        thread m_thread_RtLinkCache;
        thread m_thread_RtLinkSql;

        volatile bool m_stop;
        mutex m_queue_mtx;
        mutex m_tag_mtx;

        void Run();
        void RunRtLinkCacheServer();
        void RunRtLinkSqlServer();

};

#endif // MXSERVER_H
