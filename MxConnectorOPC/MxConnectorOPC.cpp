#include "MxConnectorOPC.h"

MxConnectorOPC::MxConnectorOPC()
{
    m_client = NULL;
    m_state = false;
    MxLog::LogOpen();
}

MxConnectorOPC::~MxConnectorOPC()
{
    Close();
    MxLog::LogOpen();
}

void MxConnectorOPC::SetConnString(string val){
    m_connString = val;
}

bool MxConnectorOPC::GetState()
{
    return m_state;
}

time_t MxConnectorOPC::FiletimeToTime_t(const FILETIME& ft)
{
	ULARGE_INTEGER ull;
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;

	return ull.QuadPart / 10000000ULL - 11644473600ULL;
}

bool MxConnectorOPC::Open()
{
	if (!m_state) {
		m_client = new LocalSyncOPCCLient();
		
		try {
			m_client->Init();
			m_client->Connect(m_connString);
		}
		catch (...) {
			MxLog::LogWarn("Open() error connect to OPC "+m_connString+"!");
		}

        if (m_client->IsConnected()) {
			m_state = true;
            MxLog::LogInfo("Open() OPC server connected!");
        }
        else
        {
            // Закроем клиента и выгрузим
			delete m_client;
            m_client = NULL;
            MxLog::LogWarn("Open() OPC server not connected!");
        }
	}
	return m_state;
}

void MxConnectorOPC::Close()
{
    if(m_state){
        if (m_client != NULL) {
			if (m_client->IsConnected())
				m_client->DisConnect();
			delete m_client;
            MxLog::LogInfo("Close() OPC client close.");
        }
        m_state = false;
    }
}

void MxConnectorOPC::Read(time_t& timeVal, vector<MxRequest*>& req, vector<MxResult*>& res){

	timeVal = time(NULL);

    // получим opcgroup для сканклассса если её ещё нет создадим
    if(m_client != NULL && m_client->IsConnected()){
		MxLog::LogDebug("Read() for()");

		for(uint16_t i=0; i < req.size(); i++){

			MxLog::LogDebug(req[i]->Name);
            res[i]->UpdateTime = timeVal;
			res[i]->Quality = TAG_QUALITY::TAG_QUALITY_GOOD;

			try {
				uint16_t wqlt;
				FILETIME fsTime;
				switch (req[i]->TagType) {
				case TAG_TYPE::TAG_TYPE_BOOL:
					res[i]->BoolVal = m_client->ReadBool(req[i]->Source.c_str(), wqlt, fsTime);
					sprintf(m_msg, "Read(%s,%s)=%d", req[i]->Name.c_str(), req[i]->Source.c_str(), res[i]->BoolVal);
					MxLog::LogDebug(m_msg);
					break;
				case TAG_TYPE::TAG_TYPE_WORD:
					res[i]->Int64Val = m_client->ReadUint16(req[i]->Source.c_str(), wqlt, fsTime);
					sprintf(m_msg, "Read(%s,%s)=%ld", req[i]->Name.c_str(), req[i]->Source.c_str(), (long)res[i]->Int64Val);
					MxLog::LogDebug(m_msg);
					break;
				case TAG_TYPE::TAG_TYPE_INT16:
					res[i]->Int64Val = m_client->ReadInt16(req[i]->Source.c_str(), wqlt, fsTime);
					sprintf(m_msg, "Read(%s,%s)=%ld", req[i]->Name.c_str(), req[i]->Source.c_str(), (long)res[i]->Int64Val);
					MxLog::LogDebug(m_msg);
					break;
				case TAG_TYPE::TAG_TYPE_INT32:
					res[i]->Int64Val = m_client->ReadInt32(req[i]->Source.c_str(), wqlt, fsTime);
					sprintf(m_msg, "Read(%s,%s)=%ld", req[i]->Name.c_str(), req[i]->Source.c_str(), (long)res[i]->Int64Val);
					MxLog::LogDebug(m_msg);
					break;
				case TAG_TYPE::TAG_TYPE_DWORD:
					res[i]->Int64Val = m_client->ReadInt32(req[i]->Source.c_str(), wqlt, fsTime);
					sprintf(m_msg, "Read(%s,%s)=%ld", req[i]->Name.c_str(), req[i]->Source.c_str(), (long)res[i]->Int64Val);
					MxLog::LogDebug(m_msg);
					break;
				case TAG_TYPE::TAG_TYPE_FLOAT:
					res[i]->FloatVal = m_client->ReadFloat(req[i]->Source.c_str(), wqlt, fsTime);
					sprintf(m_msg, "Read(%s,%s)=%f", req[i]->Name.c_str(), req[i]->Source.c_str(), res[i]->FloatVal);
					MxLog::LogDebug(m_msg);
					break;
				case TAG_TYPE::TAG_TYPE_STRING:
					break;
				}
				
				switch(wqlt) {
				case TAG_QUALITY::TAG_QUALITY_BAD:
						res[i]->Quality = TAG_QUALITY::TAG_QUALITY_BAD;
						break;
				case TAG_QUALITY::TAG_QUALITY_GOOD:
						res[i]->Quality = TAG_QUALITY::TAG_QUALITY_GOOD;
						break;
				default:
						res[i]->Quality = TAG_QUALITY::TAG_QUALITY_BAD;
						break;
				}


				res[i]->UpdateTime = FiletimeToTime_t(fsTime);

			}
			catch (exception& e) {
				res[i]->Quality = TAG_QUALITY::TAG_QUALITY_BAD;
				MxLog::LogWarn("Read() Error call OPC client, "+string(e.what()));
			}
			catch (...) {
				res[i]->Quality = TAG_QUALITY::TAG_QUALITY_BAD;
				MxLog::LogWarn("OPC method Read(), error read tag=" + req[i]->Name+", source=" + req[i]->Source);
			}

			//tm* timeinfo = localtime(&timeVal);
			//strftime(stime, LOG_TIME_SIZE, LOG_TIME_FORMAT, timeinfo);
			//MxLog::LogDebug("Read() OPC request tag: "+req[i]->Name+"["+string(stime)+"]");
        }
    }
    else{
        // нет соединения выставим BAD
        for(uint16_t i=0; i < req.size(); i++){
            res[i]->UpdateTime = timeVal;
			res[i]->Quality = TAG_QUALITY::TAG_QUALITY_BAD;
        }
        MxLog::LogWarn("Read() OPC client not connected!");
    }
}
