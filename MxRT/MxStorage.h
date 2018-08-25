#ifndef MXSTORAGE_H
#define MXSTORAGE_H

#include <thread>
#include <sstream>
#include <chrono>
#include <ctime>
#include <exception>
#include "../MxTypes/MxLog.h"
#include "..\MxTypes\MxConnector.h"
#include "MxServer.h"
#include "sqlite3.h"

using namespace std;

class MxServer;

class MxStorage
{
    public:
        MxStorage();
        virtual ~MxStorage();

        void Start();
        void Stop();

        void SetServer(MxServer* val) { m_server = val; };
        void SetTagCount(uint16_t val) { m_tagCount = val; };

    protected:

    private:
        volatile bool m_stop = false;
        thread m_thread;
        MxServer* m_server;
        uint16_t m_tagCount;
        sqlite3* m_con;
        sqlite3_stmt *m_stmt_ins;
        sqlite3_stmt *m_stmt_del;

        void Run();
};

#endif // MXSTORAGE_H
