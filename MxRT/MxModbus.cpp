#include "MxModbus.h"

MxModbus::MxModbus()
{
    //ctor

}

MxModbus::~MxModbus()
{
}

MxTag* MxModbus::GetTagByID(uint16_t id)
{
    MxTag *tag = NULL;
    map<uint16_t, string>::iterator it = m_modbus_cache.find(id);
    if(it!=m_modbus_cache.end())
    {
        MxTag _tag = m_server->GetTag(it->second);
        tag = new MxTag();
        tag->SetAddr(_tag.GetAddr());
        tag->SetBoolVal(_tag.GetBoolVal());
        tag->SetFloatVal(_tag.GetFloatVal());
        tag->SetIntVal(_tag.GetIntVal());
        tag->SetName(_tag.GetName());
        tag->SetQuality(_tag.GetQuality());
        tag->SetStrVal(_tag.GetStrVal());
        tag->SetTagType(_tag.GetTagType());
        tag->SetUpdateTime(_tag.GetUpdateTime());
    }
    return tag;
}

void MxModbus::AddTag(uint16_t regId, string tagName){
    m_modbus_cache.insert(pair<uint16_t,string>(regId, tagName));
}

void MxModbus::Start()
{
    m_stop = false;
    //Запуситм сервер
    m_thread = thread(&MxModbus::Run, this);
    MxLog::LogInfo("Modbus started.");
}

void MxModbus::Stop()
{
    m_stop = true;
    m_thread.join();
    MxLog::LogInfo("Modbus stopped.");
}

void MxModbus::Run()
{
    int socket;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    char message[255];
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc;
    int16_t header_lenght;

    ctx = modbus_new_tcp(m_addr.c_str(), m_port);
    header_lenght = modbus_get_header_length(ctx);

    mb_mapping = modbus_mapping_new(0, 0, 65536, 0);
    if (mb_mapping == NULL)
    {
        sprintf_s(message, "Modbus failed to allocate the mapping: %s\n", modbus_strerror(errno));
        MxLog::LogError(message);
        modbus_free(ctx);
        return;
    }

    socket = modbus_tcp_listen(ctx, 1);

    MxLog::LogInfo("Modbus port opened!");

    while(!m_stop)
    {
        // Задаём таймаут
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        fd_set readFDs = { 0 };
        FD_ZERO( &readFDs );
        FD_SET( socket, &readFDs );

        // ожидать подключения клиента
        int rs = select(0, &readFDs, NULL, NULL, &timeout);
        // Select Error
        if(rs == SOCKET_ERROR || rs == 0){
            MxLog::LogDebug("Modbus no connection!");
            continue;
        }

        modbus_tcp_accept(ctx, &socket);
        MxLog::LogInfo("Modbus new connection!");

        while(!m_stop){

            rc = modbus_receive(ctx, query);
            MxLog::LogDebug("Modbus new query!");
            if (rc != -1){
                /* rc is the query size */
                // если код функции 3 чтение hold регистра
                if(query[header_lenght]==0x03){

                    uint16_t _addr = query[header_lenght+1] << 8;
                    _addr = _addr | query[header_lenght+2];

                    uint16_t _size = query[header_lenght+3] << 8;
                    _size = _size | query[header_lenght+4];


                    if(_addr > 0 && _size > 0){
                        // кол-во прочитанных регистров
                        //query[header_lenght+1] = _size * 2;
                        for(int i=0; i < _size; i++){
                            MxTag* _tag = GetTagByID(_addr+i);
                            uint16_t _val;

                            if(_tag!=NULL){

								char buf[500];
								// modbus запрашивает только регистры типа WORD
                                switch(_tag->GetTagType())
                                {
								case TAG_TYPE_WORD:
									_val = (uint16_t)_tag->GetIntVal();
									break;
								case TAG_TYPE_INT16:
									_val = (int16_t)_tag->GetIntVal();
									break;
                                }

								sprintf_s(buf, "Modbus Read(%s)=%d", _tag->GetName().c_str(), _val);
								MxLog::LogDebug(buf);

                                mb_mapping->tab_registers[_addr+i] = _val;

                                delete _tag;

                            }
                            else{
                                mb_mapping->tab_registers[_addr+i] = 0;
                            }
                        }
                    }
                }
                modbus_reply(ctx, query, rc, mb_mapping);
                MxLog::LogDebug("Modbus send reply!");
            }
            else
            {
                /* Connection closed by the client or error */
                MxLog::LogWarn("Modbus connection closed by the client or error!");
                break;
            }
        }
    }

    sprintf_s(message, "Modbus closed with error: %s", modbus_strerror(errno));
    MxLog::LogInfo(message);

    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);

}
