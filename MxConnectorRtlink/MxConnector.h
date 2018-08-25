#ifndef MXCONNECTOR_H
#define MXCONNECTOR_H

#include <iostream>
#include <string>
#include <vector>
#include "..\MxTypes\MxTypes.h"

using namespace std;

class MxConnector
{
    public:

        virtual void Open()=0;
        virtual void Close()=0;

        virtual vector<MxResult*> Read(vector<MxRequest*> tags)=0;

        string GetName() { return m_name; }
        void SetName(string val) { m_name = val; }

        string GetConnString() { return m_connString; }
        void SetConnString(string val) { m_connString = val; }

        uint16_t GetTimeout() { return m_timeout; }
        void SetTimeout(uint16_t val) { m_timeout = val; }

        bool GetState() { return m_state; }
        void SetState(bool val) { m_state = val; }

    protected:

        string m_name;
        string m_connString;
        uint16_t m_timeout;
        bool m_state;

};

#endif // MXCONNECTOR_H
