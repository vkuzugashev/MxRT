#ifndef MXCONNECTORRTLINK_H
#define MXCONNECTORRTLINK_H

#pragma once

#include <chrono>
#include "MxConnector.h"
#include "..\MxTypes\MxLog.h"
#include "..\MxTypes\MxTypes.h"

using namespace std::chrono;

class MxConnectorRtLink: public MxConnector
{
   public:
        MxConnectorRtLink();
        virtual ~MxConnectorRtLink();

        void Open();
        void Close();

        vector<MxResult*> Read(vector<MxRequest*> tags);

    protected:

    private:
        string m_hostname;
        string m_port;
        void* m_client;

};

#endif // MXCONNECTORRTLINK_H
