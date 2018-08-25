#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "MxServer.h"
#include "MxRuntime.h"
#include "MxScanClass.h"
#include "MxStorage.h"


using namespace std;

MxServer::MxServer()
{
    sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
    sqlite3_initialize();

    m_storage = new MxStorage();
    m_storage->SetServer(this);
}

MxServer::~MxServer()
{
    for(uint8_t i=0; i < m_runtimes.size(); i++)
    {
        delete m_runtimes[i];
    }

    for(uint8_t i=0; i < m_scanclasses.size(); i++)
        delete m_scanclasses[i];

    for (map<string, MxTag*>::iterator it=m_cache.begin(); it!=m_cache.end(); it++)
        delete it->second;

    // очистим очередь если там есть данные
    MxResult* res;
    while((res = QueuePop())!=NULL)
    {
        delete res;
    }

    delete m_storage;

    sqlite3_shutdown();
}

void MxServer::LoadConfig()
{

    MxLog::LogInfo("Load configuration ...");

    string _line;
    fstream f;

    uint16_t modbusPort;
    string modbusAddr;

    //Заполним конфигурацию сервера
    f.open("mxrt.csv", fstream::in);
    while(!f.eof())
    {

        getline(f, _line);

        if(_line.empty() || _line.find('#') == 0)
            continue;

        MxLog::LogDebug(_line);

        stringstream strm;
        strm << _line;

        getline(strm, _line, ';');
        MxLog::LogDebug(_line);
        if("Log.Level" == _line)
        {
            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            uint8_t l = atoi(_line.c_str());
            switch(l)
            {
            case 0:
                MxLog::SetLevel(LOG_TYPE::LEVEL_DEBUG);
                break;
            case 1:
                MxLog::SetLevel(LOG_TYPE::LEVEL_INFO);
                break;
            case 2:
                MxLog::SetLevel(LOG_TYPE::LEVEL_WARN);
                break;
            case 3:
                MxLog::SetLevel(LOG_TYPE::LEVEL_ERROR);
                break;
            }
        }
        else if("Storage.Enable"==_line)
        {
            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            m_storageEnable = atoi(_line.c_str());
        }
        else if("RtLink.Enable"==_line)
        {
            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            m_rtLinkEnable = atoi(_line.c_str());
        }
        else if("RtLink.Port"==_line)
        {
            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            m_rtLinkPort = atoi(_line.c_str());
        }
        else if("RtLinkSql.Enable"==_line)
        {
            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            m_rtLinkSqlEnable = atoi(_line.c_str());
        }
        else if("RtLinkSql.Port"==_line)
        {
            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            m_rtLinkSqlPort = atoi(_line.c_str());
        }
        else if("Modbus.Enable"==_line)
        {
            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            m_modbusEnable = atoi(_line.c_str());
        }
        else if("Modbus.Addr"==_line)
        {
            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            modbusAddr = _line;
        }
        else if("Modbus.Port"==_line)
        {
            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            modbusPort = atoi(_line.c_str());
        }
//        else if("Modbus.StartReg"==_line)
//        {
//            getline(strm, _line, ';');
//            MxLog::LogDebug(_line);
//            modbusStartReg = atoi(_line.c_str());
//        }

    }
    f.close();

    //Настроим modbus
    if(m_modbusEnable){
        m_modbus = new MxModbus();
        m_modbus->SetServer(this);
        m_modbus->SetAddr(modbusAddr);
        m_modbus->SetPort(modbusPort);
//        m_modbus->SetStartReg(modbusStartReg);

        //Заполним конфигурацию сервера
        f.open("modbus.csv", fstream::in);
        while(!f.eof())
        {
            getline(f, _line);
            if(_line.empty() || _line.find('#') == 0)
                continue;

            MxLog::LogDebug(_line);

            stringstream strm;
            strm << _line;

            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            uint16_t regId = atoi(_line.c_str());

            getline(strm, _line, ';');
            MxLog::LogDebug(_line);
            string tagName = _line;

            m_modbus->AddTag(regId, tagName);

        }
        f.close();
    }

    f.open("scanclass.csv", fstream::in);
    while(!f.eof())
    {

        getline(f, _line);

        if(_line.empty() || _line.find('#') == 0)
            continue;


        MxLog::LogDebug(_line);

        stringstream strm;
        strm << _line;

        MxScanClass *sc = new MxScanClass();

        getline(strm, _line, ';');
        MxLog::LogDebug(_line);
        sc->SetName(_line);

        getline(strm, _line, ';');
        MxLog::LogDebug(_line);
        sc->SetInterval(atoi(_line.c_str()));

        //todo
        m_scanclasses.push_back(sc);
    }
    f.close();

    f.open("connector.csv", fstream::in);
    while(!f.eof())
    {

        getline(f, _line);

        if(_line.empty() || _line.find('#') == 0)
            continue;

        MxLog::LogDebug(_line);

        stringstream strm;
        strm << _line;

        getline(strm, _line, ';');
        MxLog::LogDebug(_line);
        MxRuntime *rt = new MxRuntime();
        rt->SetName(_line);

        getline(strm, _line,';');
        MxLog::LogDebug(_line);
        rt->SetPathDll(_line);

        getline(strm, _line,';');
        MxLog::LogDebug(_line);
        rt->SetConnString(_line);

        m_runtimes.push_back(rt);

    }
    f.close();

    f.open("tag.csv", fstream::in);
    while(!f.eof())
    {

        getline(f, _line);

        if(_line.empty() || _line.find('#') == 0)
            continue;

        MxLog::LogDebug(_line);

        stringstream strm;
        strm << _line;

        getline(strm, _line, ';');
        MxLog::LogDebug(_line);
        if(_line.empty())
            continue;

        MxTag* tag = new MxTag();

        tag->SetName(_line);
        MxLog::LogDebug(_line);

        getline(strm, _line,';');
        MxLog::LogDebug(_line);

        int runtime = -1;
        if(!_line.empty())
        {
            //коннектор
            for(uint8_t i=0; i < m_runtimes.size(); i++)
                if(m_runtimes[i]->GetName() == _line)
                {
                    runtime = i;
                    break;
                }
        }

        getline(strm, _line,';');
        MxLog::LogDebug(_line);
        if(!_line.empty())
            tag->SetAddr(_line);

        getline(strm, _line,';');
        MxLog::LogDebug(_line);

        int scan = -1;
        if(!_line.empty())
        {
            for(uint8_t i=0; i < m_scanclasses.size(); i++)
                if(m_scanclasses[i]->GetName() == _line)
                {
                    scan = i;
                    break;
                }
        }

        getline(strm, _line,';');
        MxLog::LogDebug(_line);

        if(_line == "bool")
        {
            tag->SetTagType(TAG_TYPE::TAG_TYPE_BOOL);
        }
        else if(_line == "byte")
        {
            tag->SetTagType(TAG_TYPE::TAG_TYPE_BYTE);
        }
		else if (_line == "word")
		{
			tag->SetTagType(TAG_TYPE::TAG_TYPE_WORD);
		}
		else if(_line == "int16")
        {
            tag->SetTagType(TAG_TYPE::TAG_TYPE_INT16);
        }
        else if(_line=="int32")
        {
            tag->SetTagType(TAG_TYPE::TAG_TYPE_INT32);
        }
		else if (_line == "dword")
		{
			tag->SetTagType(TAG_TYPE::TAG_TYPE_DWORD);
		}
		else if(_line=="float")
        {
            tag->SetTagType(TAG_TYPE::TAG_TYPE_FLOAT);
        }
        else if(_line=="str")
        {
            tag->SetTagType(TAG_TYPE::TAG_TYPE_STRING);
        }

        getline(strm, _line,';');
        MxLog::LogDebug(_line);
        float floatVal = (float)atof(_line.c_str());
        tag->SetMinVal(floatVal);

        getline(strm, _line,';');
        MxLog::LogDebug(_line);
        floatVal = (float)atof(_line.c_str());
        tag->SetMaxVal(floatVal);

        getline(strm, _line,';');
        MxLog::LogDebug(_line);
        floatVal = (float)atof(_line.c_str());
        tag->SetDeadBand(floatVal);

        getline(strm, _line,';');
        MxLog::LogDebug(_line);
        uint8_t intVal = atoi(_line.c_str());
        tag->SetFlushTime(intVal);

        getline(strm, _line,';');
        MxLog::LogDebug(_line);
        floatVal = (float)atof(_line.c_str());
        tag->SetMulti(floatVal);

        if(runtime != -1 && scan != -1)
        {
            vector<MxScanClass*> scs = m_runtimes[runtime]->GetScanclasses();
            if(scs.empty())
            {
                for(uint8_t i=0; i < m_scanclasses.size(); i++)
                {
                    MxScanClass* sc = new MxScanClass();
                    sc->SetName(m_scanclasses[i]->GetName());
                    sc->SetInterval(m_scanclasses[i]->GetInterval());
                    scs.push_back(sc);
                }
            }
            MxRequest* mr = new MxRequest();
            mr->Name = tag->GetName();
            mr->TagType = tag->GetTagType();
            mr->Source = tag->GetAddr();

            vector<MxRequest*> tags = scs[scan]->GetTags();
            tags.push_back(mr);
            scs[scan]->SetTags(tags);
            m_runtimes[runtime]->SetScanclasses(scs);
        }
        //Добавить в теговый кеш
        m_cache.insert(pair<string,MxTag*>(tag->GetName(), tag));
    }
    f.close();

    // удалим пустые сканклассы
    for(uint8_t i=0; i < m_runtimes.size(); i++)
    {
        vector<MxScanClass*> scs = m_runtimes[i]->GetScanclasses();
        for(uint8_t j=0; j < scs.size(); j++)
        {
            if(scs[j]->GetTags().size()==0)
            {
                swap((scs[j]), scs.back());
                scs.pop_back();
                j--;
            }
        }
        m_runtimes[i]->SetScanclasses(scs);
    }

    MxLog::LogInfo("Configuration loaded.");

}

void MxServer::Start()
{
    m_stop = false;

    if(m_storageEnable){
        // установим размер кеша
        m_storage->SetTagCount((int16_t)m_cache.size());
        m_storage->Start();
    }

    for(uint8_t i=0; i < m_runtimes.size(); i++)
    {
        m_runtimes[i]->Start();
    }
    //Запуситм сервер
    m_thread = thread(&MxServer::Run, this);

    if(m_rtLinkEnable){
        //Запустим RtLink сервер
        m_thread_RtLinkCache = thread(&MxServer::RunRtLinkCacheServer, this);
    }

    if(m_rtLinkSqlEnable){
        //Запустим RtLinkSql сервер
        m_thread_RtLinkSql = thread(&MxServer::RunRtLinkSqlServer, this);
    }

    if(m_modbusEnable){
        //Запустим Modbus сервер
        m_modbus->Start();
    }

    MxLog::LogInfo("Server started.");
}

void MxServer::Stop()
{
    m_stop = true;
    m_thread.join();

    if(m_rtLinkEnable){
        m_thread_RtLinkCache.join();
    }

    if(m_rtLinkSqlEnable){
        m_thread_RtLinkSql.join();
    }

    if(m_modbusEnable)
        m_modbus->Stop();

    for(uint8_t i=0; i< m_runtimes.size(); i++)
    {
        m_runtimes[i]->Stop();
    }

    if(m_storageEnable)
        m_storage->Stop();

    MxLog::LogInfo("Server stoped.");
}

void MxServer::Run()
{
    // Поток чтения данных из очередей runtime
    while(!m_stop)
    {
        bool _is_no_data = true;
        for(uint8_t i=0; i < m_runtimes.size(); i++)
        {
            MxResult* result = m_runtimes[i]->QueuePop();
            if(result!=NULL)
            {
                // Сохраним в кеше и если тег поменялся
                // то сохраним его в БД
                if(SetTagVal(result->Name,
                             system_clock::from_time_t(result->UpdateTime),
                             result->Quality,
                             result->BoolVal,
                             result->Int64Val,
                             result->FloatVal,
                             result->StrVal))
                {
                    // сохраним в очередь для сохранения
                    if(m_storageEnable)
                        QueuePush(result);
					else
						delete result;
                }
				else
					delete result;

				_is_no_data = false;
            }
        }
        // Если не было данных, то пауза
        if(_is_no_data)
            this_thread::sleep_for(chrono::milliseconds(100));
    }

}

void MxServer::QueuePush(MxResult* val)
{
    m_queue_mtx.lock();
    m_queue.push(val);
    m_queue_mtx.unlock();
}

MxResult* MxServer::QueuePop()
{
    MxLog::LogDebug("MxServer::QueuePop() begin");
    MxResult* res = NULL;
    m_queue_mtx.lock();
    try
    {
        if(!m_queue.empty())
        {
            res = m_queue.front();
            m_queue.pop();
        }
    }
    catch(exception& e)
    {
        MxLog::LogDebug("MxServer::QueuePop() exception: "+string(e.what()));
    }
    m_queue_mtx.unlock();
    MxLog::LogDebug("MxServer::QueuePop() end");
    return res;
}

bool MxServer::SetTagVal(string name, system_clock::time_point updateTime, TAG_QUALITY quality, bool boolVal, int64_t& intVal, float& floatVal, string strVal)
{
    bool is_changed = false;
    m_tag_mtx.lock();
    map<string, MxTag*>::iterator it = m_cache.find(name);
    if(it!=m_cache.end())
    {
        MxTag* _tag = it->second;
		_tag->SetUpdateTime(updateTime);
        _tag->SetQuality(quality);

        switch(_tag->GetTagType())
        {

        case TAG_TYPE_BOOL:
            if(_tag->GetBoolVal()!=boolVal)
            {
                _tag->SetBoolVal(boolVal);
                is_changed = true;
            }
            break;

		case TAG_TYPE_BYTE:
		case TAG_TYPE_INT16:
		case TAG_TYPE_WORD:
		case TAG_TYPE_INT32:
		case TAG_TYPE_DWORD:
			// применим мультипликатор
			//todo исправить!!!
            intVal = intVal * _tag->GetMulti();

            if((_tag->GetIntVal()!=intVal && _tag->GetDeadBand() == 0.0)
                    || (_tag->GetIntVal()!=intVal
                        &&_tag->GetDeadBand() > 0.0
                        && _tag->GetMinVal() != _tag->GetMaxVal()
                        && std::abs(_tag->GetIntVal() - intVal) > (std::abs(_tag->GetMaxVal() - _tag->GetMinVal()) * _tag->GetDeadBand()))
              )
            {
                _tag->SetIntVal(intVal);
                is_changed = true;
            }
            break;

        case TAG_TYPE_FLOAT:
            // применим мультипликатор
            floatVal = floatVal * _tag->GetMulti();

            if((_tag->GetFloatVal()!=floatVal && _tag->GetDeadBand() == 0.0)
                    || (_tag->GetFloatVal()!=floatVal
                        && _tag->GetDeadBand() > 0.0
                        && _tag->GetMinVal() != _tag->GetMaxVal()
                        && std::abs(_tag->GetFloatVal() - floatVal) > (std::abs(_tag->GetMaxVal() - _tag->GetMinVal()) * _tag->GetDeadBand()))
              )
            {
                _tag->SetFloatVal(floatVal);
                is_changed = true;
            }
            break;

        case TAG_TYPE_STRING:
            if(_tag->GetStrVal()!=strVal)
            {
                _tag->SetStrVal(strVal);
                is_changed = true;
            }
            break;
        }

        if(!is_changed)
        {
            duration<double, ratio<1>> time_span = updateTime - _tag->GetChangeTime();
            if(time_span.count() > _tag->GetFlushTime())
            {
                _tag->SetChangeTime(updateTime);
                is_changed = true;
            }
        }
        else
            _tag->SetChangeTime(updateTime);
    }
    m_tag_mtx.unlock();
    return is_changed;
}

MxTag MxServer::GetTag(string name)
{
    MxTag tag;
    m_tag_mtx.lock();
    map<string, MxTag*>::iterator it = m_cache.find(name);
    if(it!=m_cache.end())
    {
        MxTag* _tag = it->second;
        tag.SetAddr(_tag->GetAddr());
        tag.SetBoolVal(_tag->GetBoolVal());
        tag.SetFloatVal(_tag->GetFloatVal());
        tag.SetIntVal(_tag->GetIntVal());
        tag.SetName(_tag->GetName());
        tag.SetQuality(_tag->GetQuality());
        tag.SetStrVal(_tag->GetStrVal());
        tag.SetTagType(_tag->GetTagType());
        tag.SetUpdateTime(_tag->GetUpdateTime());
    }
    m_tag_mtx.unlock();
    return tag;
}

void MxServer::RunRtLinkCacheServer()
{
    // Поток приёма запросов от клиента
    SOCKET sock, listenerSocket;
    struct sockaddr_in addr;
    char buf[8096];
    int bytes_read;
    string sReq="";
    WSADATA wsaData;

#ifdef WIN32
    // Обязательный блок для windows
    if (WSAStartup(MAKEWORD(1,1), &wsaData) == SOCKET_ERROR)
    {
        MxLog::LogError("Error initialising WSA.");
        return ;
    }
#endif // WIN32

    listenerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenerSocket == SOCKET_ERROR)
    {
#ifdef WIN32
        WSACleanup();
#endif // WIN32
        MxLog::LogError("Error create RtLink server socket!");
        return;
    }
    else
        MxLog::LogInfo("RtLink server socket created!");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_rtLinkPort);    //24117
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int iRes = bind(listenerSocket, (const sockaddr *)&addr, (int)sizeof(addr));
    if(iRes == SOCKET_ERROR)
    {
#ifdef WIN32
        WSACleanup();
#endif // WIN32
        MxLog::LogError("Error bind RtLink server socket!");
        return;
    }
    else
        MxLog::LogInfo("Bind RtLink server socket!");

    // начать слушать порт, длина очереди на 1 клиент
    listen(listenerSocket, 1);

    // цикл обработки входящих клиентов
    while(!m_stop)
    {
        // Задаём таймаут
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        fd_set readFDs = { 0 };
        FD_ZERO( &readFDs );
        FD_SET( listenerSocket, &readFDs );

        // ожидать подключения клиента
        int rs = select(0, &readFDs, NULL, NULL, &timeout);
        // Select Error
        if(rs == SOCKET_ERROR || rs == 0)
            continue;

        sock = accept(listenerSocket, NULL, NULL);
        if(sock >= 0)
        {
            sReq = "";
            MxLog::LogInfo("Accept RtLink server client");

            while(!m_stop)
            {
                // считать из порта
                bytes_read = recv(sock, buf, 8095, 0);
                // ошибка чтения из сокета
                if(bytes_read <= 0)
                    break;

                // закончим строку
                buf[bytes_read]=0;
                char* sql_req = buf;
                sql_req = strstr(sql_req, "TAG:");

                if(sql_req!=0)
                {

                    stringstream strm;
                    char stime[LOG_TIME_SIZE];

                    // вырежем C:
                    sql_req = sql_req + 4;
                    char* tagName = strtok(sql_req, ",");
                    while(tagName!=NULL)
                    {
                        map<string, MxTag*>::iterator it = m_cache.find(tagName);
                        if(it!=m_cache.end())
                        {
                            MxTag* _tag = it->second;

                            time_t seconds = system_clock::to_time_t(_tag->GetUpdateTime());
							tm timeinfo;
							errno_t err = localtime_s(&timeinfo, &seconds);
                            strftime(stime, TAG_TIME_SIZE, TAG_TIME_FORMAT, &timeinfo);

                            strm << _tag->GetName() << ";" << stime << ";";
                            strm << (int16_t)_tag->GetTagType() << ";" << (int16_t)_tag->GetQuality() << ";";
                            switch(_tag->GetTagType())
                            {
                            case TAG_TYPE_BOOL:
                                strm << (bool)_tag->GetBoolVal();
                                break;
                            case TAG_TYPE_BYTE:
                            case TAG_TYPE_INT16:
							case TAG_TYPE_WORD:
							case TAG_TYPE_INT32:
							case TAG_TYPE_DWORD:
								strm << (int64_t)_tag->GetIntVal();
                                break;
                            case TAG_TYPE_FLOAT:
                                strm << (float)_tag->GetFloatVal();
                                break;
                            }
                        }
                        strm << "\r\n";
                        tagName = strtok(NULL, ",");
                    }
                    send(sock, strm.str().c_str(), strm.str().size(), 0);
                }
            }
            closesocket(sock);
        }
    }
    shutdown(listenerSocket, 2);
    closesocket(listenerSocket);
#ifdef WIN32
    WSACleanup();
#endif // WIN32

    MxLog::LogInfo("RtLink server socket was closed.");

}

void MxServer::RunRtLinkSqlServer()
{
    // Поток приёма запросов от клиента
    SOCKET sock, listenerSocket;
    struct sockaddr_in addr;
    char buf[8096];
    int bytes_read;
    string sReq="";
    WSADATA wsaData;

#ifdef WIN32
    // Обязательный блок для windows
    if (WSAStartup(MAKEWORD(1,1), &wsaData) == SOCKET_ERROR)
    {
        MxLog::LogError("Error initialising WSA.");
        return ;
    }
#endif // WIN32

    listenerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(listenerSocket == SOCKET_ERROR)
    {
#ifdef WIN32
        WSACleanup();
#endif // WIN32
        MxLog::LogError("Error create RtLinkSql server socket!");
        return;
    }
    else
        MxLog::LogInfo("Create RtLinkSql server socket!");

    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_rtLinkSqlPort);    //24118
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int iRes = bind(listenerSocket, (const sockaddr *)&addr, (int)sizeof(addr));
    if(iRes == SOCKET_ERROR)
    {
#ifdef WIN32
        WSACleanup();
#endif // WIN32
        MxLog::LogError("Error bind RtLinkSql server socket!");
        return;
    }
    else
        MxLog::LogInfo("Bind RtLinkSql server socket!");

    // начать слушать порт, длина очереди на 1 клиент
    listen(listenerSocket, 1);

    // цикл обработки входящих клиентов
    while(!m_stop)
    {
        // Задаём таймаут
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        fd_set readFDs = { 0 };
        FD_ZERO( &readFDs );
        FD_SET( listenerSocket, &readFDs );

        // ожидать подключения клиента
        int rs = select(0, &readFDs, NULL, NULL, &timeout);
        // Select Error
        if(rs == SOCKET_ERROR || rs == 0)
            continue;

        sock = accept(listenerSocket, NULL, NULL);
        if(sock >= 0)
        {
            sReq = "";
            MxLog::LogInfo("Accept RtLinkSql client");

            while(!m_stop)
            {
                // считать из порта
                bytes_read = recv(sock, buf, 8095, 0);
                // ошибка чтения из сокета
                if(bytes_read <= 0)
                    break;

                // закончим строку
                buf[bytes_read]=0;
                char* sql_req = buf;
                sql_req =  strstr(sql_req, "SQL:");
                if(sql_req!=0)
                {

                    // вырежем A:
                    sql_req = sql_req + 4;
                    // выборка данных
                    sqlite3* con;
                    int rc = sqlite3_open_v2("MxRT.db", &con, SQLITE_OPEN_READONLY, NULL);
                    if(rc==SQLITE_OK)
                    {
                        sqlite3_stmt *stmt;
                        rc = sqlite3_prepare_v2(con, sql_req, -1, &stmt, NULL);
                        if(rc==SQLITE_OK)
                        {

                            // считать результат и отправить в порт обратно
                            send(sock, "[", 1, 0);
                            while(!m_stop)
                            {
                                rc = sqlite3_step(stmt);
                                if(rc==SQLITE_ROW)
                                {
                                    stringstream sql_row;
                                    sql_row << "";
                                    int clmn_cnt = sqlite3_column_count(stmt);
                                    for(int i=0; i < clmn_cnt; i++)
                                    {
                                        const char* clmn_name = sqlite3_column_name(stmt, i);
                                        const unsigned char* clmn_value = sqlite3_column_text(stmt, i);
                                        sql_row << ((i==0) ? "" : ";") << clmn_name << "=" << clmn_value;
                                    }
                                    sql_row << "\r\n";
                                    send(sock, sql_row.str().c_str(), sql_row.str().size(), 0);
                                }
                                else if(rc==SQLITE_DONE)
                                {
                                    MxLog::LogInfo("Execute SQL done.");
                                    break;
                                }
                                else
                                {
                                    char msg[500];
                                    sprintf_s(msg,"Error: %d\r\nMessage: execute SQL statement\r\nSQL:%s", rc, sql_req);
                                    send(sock, msg, strlen(msg), 0);
                                    MxLog::LogWarn(string(msg));
                                    break;
                                }
                            }
                            send(sock, "\r\n", 2, 0);
                        }
                        else
                        {
                            char msg[500];
                            sprintf_s(msg,"Error: %d\r\nMessage: prepare SQL statement\r\nSQL:%s", rc, sql_req);
                            send(sock, msg, strlen(msg), 0);
                            MxLog::LogWarn(string(msg));
                        }

                        sqlite3_finalize(stmt);
                    }
                    else
                    {
                        char msg[] = "Error: open db.";
                        send(sock, msg, strlen(msg), 0);
                        MxLog::LogWarn(string(msg));
                    }
                    rc = sqlite3_close(con);
                }
            }
            closesocket(sock);
        }
    }
    shutdown(listenerSocket, 2);
    closesocket(listenerSocket);
#ifdef WIN32
    WSACleanup();
#endif // WIN32
    MxLog::LogInfo("RtLinkSql server socket was closed.");
}


