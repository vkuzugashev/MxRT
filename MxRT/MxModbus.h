#ifndef MXMODBUS_H
#define MXMODBUS_H

#include "MxServer.h"
class MxServer;

class MxModbus
{
    public:
        MxModbus();
        virtual ~MxModbus();
        void Start();
        void Stop();

        void AddTag(uint16_t regId, string tagName);
        void SetServer(MxServer *val) { m_server = val; }

        int16_t GetPort() { return m_port; }
        void SetPort(int16_t val) { m_port = val; }

        string GetAddr() { return m_addr; }
        void SetAddr(string val) { m_addr = val; }

        int16_t GetStartReg() { return m_startReg; }
        void SetStartReg(int16_t val) { m_startReg = val; }


protected:

    private:
        volatile bool m_stop;
        MxServer *m_server;
        int16_t m_port = 502;
        string m_addr = "127.0.0.1";
        int16_t m_startReg=0;
        map<uint16_t, string> m_modbus_cache;   //связь регистра и тега

        thread m_thread;

        MxTag* GetTagByID(uint16_t id);
        void Run();
};

#endif // MXMODBUS_H
