#include "MxRuntime.h"

using namespace std;

MxRuntime::MxRuntime()
{
    //ctor
}

MxRuntime::~MxRuntime()
{
    for(uint8_t i=0; i < m_scanclasses.size(); i++){
        delete m_scanclasses[i];
    }
    MxLog::LogDebug("Runtime "+m_name+" is free.");
}

void MxRuntime::Start()
{
    // инициализируем буфер памяти
    for(uint8_t i=0; i < m_scanclasses.size(); i++){
        m_scanclasses[i]->Init();
    }

    //todo загрузить коннектор из библиотеки
    m_hdlConnLib = dlopen(m_path_dll.c_str());
    if (m_hdlConnLib!=NULL){
        MxLog::LogInfo("Runtime " + m_name + " connector "+m_path_dll+" loaded");
        dllConnectorCreate = (DLLConnectorCreate)dlsym(m_hdlConnLib, "ConnectorCreate");
        if(dllConnectorCreate!=NULL){
            // установим текущий уровень вывода сообщений
            dllSetLogLevel = (DLLSetLogLevel)dlsym(m_hdlConnLib, "SetLogLevel");
            dllSetLogLevel(MxLog::GetLevel());
            m_connector = dllConnectorCreate();
            m_connector->SetConnString(m_connString);
            MxLog::LogInfo("Runtime " + m_name + " connector string "+m_connString);
        }
		else {
			MxLog::LogWarn("Runtime " + m_name + " method ConnectorCreate() not found");
		}
    }

    m_stop = false;
    m_thread = thread(&MxRuntime::Run, this);
    MxLog::LogInfo("Runtime " + m_name + " thread started");
}

void MxRuntime::Stop()
{
    m_stop = true;
    m_thread.join();
    if(m_hdlConnLib!=NULL){
        dllConnectorFree = (DLLConnectorFree)dlsym(m_hdlConnLib, "ConnectorFree");
        if(dllConnectorFree!=NULL)
            dllConnectorFree(m_connector);
        dlclose(m_hdlConnLib);
    }
    MxLog::LogInfo("Runtime " + m_name + " thread stopped");
}

void MxRuntime::QueuePush(MxResult* val)
{
    MxLog::LogDebug("MxRuntime::QueuePush() begin");
    m_queue_mtx.lock();
    m_queue.push(val);
    m_queue_mtx.unlock();
    MxLog::LogDebug("MxRuntime::QueuePush() end");
}

MxResult* MxRuntime::QueuePop()
{
    MxLog::LogDebug("MxRuntime::QueuePop() begin");
    MxResult* res = NULL;
    m_queue_mtx.lock();
    if(!m_queue.empty()){
        res = m_queue.front();
        m_queue.pop();
    }
    m_queue_mtx.unlock();
    MxLog::LogDebug("MxRuntime::QueuePop() end");
    return res;
}


void MxRuntime::Run()
{
    char stime[LOG_TIME_SIZE];
	char message[255];

    MxLog::LogDebug("Runtime " + m_name + " thread started");
    while(!m_stop){
        MxLog::LogDebug("Runtime " + m_name + " while begin");
        this_thread::sleep_for(chrono::milliseconds(100));
        try{
            if(m_connector!=NULL){
                bool state = m_connector->GetState();
                // проверим подключение
				if(!state){
					state = m_connector->Open();
					this_thread::sleep_for(chrono::seconds(3));
				}
                // если есть подключение выполним запрос
				if(state){
                    //получим текущее время
                    system_clock::time_point now = system_clock::now();
                    for(uint8_t i=0; i < m_scanclasses.size(); i++){
                        if (!m_scanclasses[i]->GetTags().empty()) {
                            duration<double, milli> time_span = now - m_scanclasses[i]->GetLastrun();
                            if (time_span.count() > m_scanclasses[i]->GetInterval()) {
                                MxLog::LogDebug("Scanclass " + m_scanclasses[i]->GetName() + " read tags.");

                                vector<MxRequest*> req = m_scanclasses[i]->GetTags();
                                uint16_t size = (uint16_t)req.size();
                                vector<MxResult*> res = m_scanclasses[i]->GetResults();

                                time_t timeVal;

                                MxLog::LogDebug("Runtime " + m_name + " call Read begin");

                                m_connector->Read(timeVal, req, res);

                                MxLog::LogDebug("Runtime " + m_name + " call Read end");

								tm timeinfo;
								errno_t err = localtime_s(&timeinfo, &timeVal);
								if (err == 0) {
									for(uint16_t l=0; l < size; l++){

                                        MxResult* mr = new MxResult();
                                        mr->Name = res[l]->Name;
                                        mr->UpdateTime = res[l]->UpdateTime;
                                        mr->Quality = res[l]->Quality;
                                        mr->TagType = res[l]->TagType;

                                        switch(res[l]->TagType){
                                        case TAG_TYPE_BOOL:
                                            mr->BoolVal = res[l]->BoolVal;
                                            break;
                                        case TAG_TYPE_BYTE:
										case TAG_TYPE_WORD:
										case TAG_TYPE_INT16:
                                        case TAG_TYPE_INT32:
                                            mr->Int64Val = res[l]->Int64Val;
                                            break;
                                        case TAG_TYPE_FLOAT:
                                            mr->FloatVal = res[l]->FloatVal;
                                            break;
                                        case TAG_TYPE_STRING:
                                            mr->StrVal = res[l]->StrVal;
                                            break;
                                        }

                                        QueuePush(mr);

                                        strftime(stime, TAG_TIME_SIZE, TAG_TIME_FORMAT, &timeinfo);
                                        sprintf_s(message, "Read tag=%s[%s], bool=%d, int64=%ld, float=%f, qlt=%d", res[l]->Name.c_str(), &stime, res[l]->BoolVal, (long)res[l]->Int64Val, res[l]->FloatVal, res[l]->Quality);
                                        MxLog::LogDebug(message);
									}

                                }
                                else{
                                    MxLog::LogWarn("timeVal error");
                                }
									
									

                                m_scanclasses[i]->SetLastrun(now);
                                sprintf_s(message, "Scanclass %s, read %d tags", m_scanclasses[i]->GetName().c_str(), size);
                                MxLog::LogDebug(message);
                            }
                        }
                    }
				}
				else
                    this_thread::sleep_for(chrono::seconds(3));
                //m_connector->Close();
            }
        }
        catch(exception& e){
            if(m_connector!=NULL && m_connector->GetState())
                m_connector->Close();
            MxLog::LogWarn("Runtime " + m_name + " error, " + e.what());
            this_thread::sleep_for(chrono::seconds(1));
        }
        MxLog::LogDebug("Runtime " + m_name + " while end");
    }

	// Закроем коннектор
	if (m_connector!=NULL && m_connector->GetState())
        m_connector->Close();

    MxLog::LogDebug("Runtime " + m_name + " thread stoped");

}
