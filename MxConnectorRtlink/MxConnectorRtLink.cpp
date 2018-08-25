#include "MxConnectorRtLink.h"

MxConnectorRtLink::MxConnectorRtLink()
{
    // connect local server
	//m_client = new LocalSyncOPCCLient;
	//m_client->Init();

}

MxConnectorRtLink::~MxConnectorRtLink()
{
    // disconnect and stop
//    if(m_client->IsConnected()){
//        m_client->DisConnect();
//        m_client->Stop();
//        delete m_client;
//    }
}

void MxConnectorRtLink::Open()
{
    m_state = true;
//    if(!m_client->IsConnected()){
//        m_client->Connect(m_progid);
//    }
    MxLog::LogDebug("Connector " + m_name + " open");
}

void MxConnectorRtLink::Close()
{
    m_state = false;
//    if(m_client->IsConnected()){
//        m_client->DisConnect();
//        m_client->Stop();
//    }
    MxLog::LogDebug("Connector " + m_name + " close");
}

vector<MxResult*> MxConnectorRtLink::Read(vector<MxRequest*> tags){
    vector<MxResult*> results;
    //if(m_client->IsConnected()){
        for(uint16_t i=0; i < tags.size(); i++){
            MxResult* res = new MxResult();
            res->Name = tags[i]->Name;
            res->Quality = TAG_QUALITY::TAG_QUALITY_GOOD;
            res->UpdateTime = system_clock::now();

            switch(tags[i]->TagType){
                case TAG_TYPE::TAG_TYPE_BOOL:
                    res->BoolVal = true;    //m_client->ReadBool(tags[i]->Source);
                    break;
                case TAG_TYPE::TAG_TYPE_INT16:
                    res->Int64Val = 55;  //m_client->ReadUint16(tags[i]->Source);
                    break;
                case TAG_TYPE::TAG_TYPE_FLOAT:
                    res->FloatVal = 55.55;  //m_client->ReadFloat(tags[i]->Source);
                    break;
                case TAG_TYPE::STRING:
                    break;
            }

            results.push_back(res);
        }
    //}
    return results;
}
