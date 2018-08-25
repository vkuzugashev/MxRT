#ifndef MXCONNECTOR_H
#define MXCONNECTOR_H

#include <iostream>
#include <string>
#include <vector>
#include "MxTypes.h"

using namespace std;

class MxConnector
{
    public:

        virtual bool Open()=0;
        virtual void Close()=0;

        virtual void Read(time_t& timeVal, vector<MxRequest*>& req, vector<MxResult*>& res)=0;
        virtual void SetConnString(string val)=0;
        virtual bool GetState()=0;

    protected:

        string m_name;
        string m_connString;
        uint16_t m_timeout;
        bool m_state;

};


#endif // MXCONNECTOR_H
