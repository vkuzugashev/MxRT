#include "MxStorage.h"

MxStorage::MxStorage()
{
    //ctor
}

MxStorage::~MxStorage()
{
    //dtor
    MxLog::LogDebug("Storage is free.");
}

void MxStorage::Start()
{
    m_stop = false;
    // откроем БД
    int rc = sqlite3_open_v2("MxRT.db",&m_con, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);
    // создадим датблицу архива
    const char* sql = "CREATE TABLE IF NOT EXISTS tags( \
                        tm text, \
                        nm text, \
                        tp integer, \
                        ql integer, \
                        bv integer, \
                        iv integer, \
                        fv real, \
                        sv text, \
                        st integer, \
                        primary key(tm, nm))";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(m_con, sql, -1, &stmt, NULL);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);


    const char* sql_ins = " \
    INSERT INTO tags( \
                        tm, \
                        nm, \
                        tp, \
                        ql, \
                        bv, \
                        iv, \
                        fv, \
                        sv, \
                        st) \
                        VALUES( \
                        ?, \
                        ?, \
                        ?, \
                        ?, \
                        ?, \
                        ?, \
                        ?, \
                        ?, \
                        0)";
    rc = sqlite3_prepare_v2(m_con, sql_ins, -1, &m_stmt_ins, NULL);
    if(rc != SQLITE_OK){
        MxLog::LogInfo("Storage Error create prepared statement insert!");
    }
    const char* sql_del = "DELETE FROM tags WHERE tm < ?";
    rc = sqlite3_prepare_v2(m_con, sql_del, -1, &m_stmt_del, NULL);
    if(rc != SQLITE_OK){
        MxLog::LogInfo("Storage Error create prepared statement delete!");
    }
    m_thread = thread(&MxStorage::Run, this);
    MxLog::LogInfo("Storage thread started");
}

void MxStorage::Stop()
{
    m_stop = true;
    m_thread.join();
    sqlite3_finalize(m_stmt_del);
    sqlite3_finalize(m_stmt_ins);
    sqlite3_close(m_con);
    MxLog::LogInfo("Storage thread stopped");
}

void MxStorage::Run()
{
 	char message[255];
    uint16_t _perTran=0;
    MxResult* res = NULL;
    while(!m_stop){
        try{
            MxLog::LogDebug("MxStorage::Run while begin");
            sqlite3_exec(m_con, "BEGIN TRANSACTION", NULL, NULL, NULL);
            if ((res = m_server->QueuePop()) != NULL) {
                while (res != NULL) {
                    char stime[LOG_TIME_SIZE];
					tm timeinfo;
                    errno_t err = localtime_s(&timeinfo, &res->UpdateTime);
                    strftime(stime, TAG_TIME_SIZE, TAG_TIME_FORMAT, &timeinfo);

                    //stringstream strm;
                    sprintf_s(message, "Store %s[%s]", res->Name.c_str(), stime);

                    switch(res->TagType){
                    case TAG_TYPE_BOOL:
                        sprintf_s(message, "%s=%d", message, res->BoolVal);
                        break;
                    case TAG_TYPE_BYTE:
                    case TAG_TYPE_INT16:
					case TAG_TYPE_WORD:
					case TAG_TYPE_INT32:
                        sprintf_s(message, "%s=%ld", message, (long)res->Int64Val);
                        break;
                    case TAG_TYPE_FLOAT:
                        sprintf_s(message, "%s=%f", message, res->FloatVal);
                        break;
                    case TAG_TYPE_STRING:
                        sprintf_s(message, "%s=%s", message, res->StrVal.c_str());
                        break;
                    }

                    //сохранить в БД
                    int rc = sqlite3_bind_text(m_stmt_ins, 1, stime, -1, 0);
                    rc = sqlite3_bind_text(m_stmt_ins, 2, res->Name.c_str(), -1, 0);
                    rc = sqlite3_bind_int(m_stmt_ins, 3, (int)res->TagType);
                    rc = sqlite3_bind_int(m_stmt_ins, 4, (int)res->Quality);
                    rc = sqlite3_bind_int(m_stmt_ins, 5, res->BoolVal);
                    rc = sqlite3_bind_int64(m_stmt_ins, 6, res->Int64Val);
                    rc = sqlite3_bind_double(m_stmt_ins, 7, res->FloatVal);
                    rc = sqlite3_bind_text(m_stmt_ins, 8, res->StrVal.c_str(), -1, 0);
                    rc = sqlite3_step(m_stmt_ins);
                    if (rc == SQLITE_DONE) {
                        sprintf_s(message, "%s %s", message, "OK!");
                        MxLog::LogDebug(message);
                    }
                    else {
                        sprintf_s(message, "%s %s %d", message, "Error insert data into SQLite DB", rc);
                        MxLog::LogWarn(message);
                    }

                    // сбросим состояние
                    sqlite3_reset(m_stmt_ins);
                    sqlite3_clear_bindings(m_stmt_ins);

                    delete res;

                    // выполним сохранение 500 строк
                    if (++_perTran > m_tagCount) {
                        _perTran = 0;
                        //stringstream strm;
                        //time_t seconds = system_clock::to_time_t(system_clock::now() - hours(24)*1);
                        time_t seconds = system_clock::to_time_t(system_clock::now() - hours(2));
						tm timeinfo;
						errno_t err = localtime_s(&timeinfo, &seconds);
                        strftime(stime, TAG_TIME_SIZE, TAG_TIME_FORMAT, &timeinfo);

                        int rc = sqlite3_bind_text(m_stmt_del, 1, stime, -1, 0);
                        rc = sqlite3_step(m_stmt_del);
                        if (rc == SQLITE_DONE) {
                            MxLog::LogDebug("Delete old data OK!");
                        }
                        else {
                            sprintf_s(message, "Error delete old data from SQLite DB %d", rc);
                            MxLog::LogWarn(message);
                        }

                        // сбросим состояние
                        sqlite3_reset(m_stmt_del);
                        sqlite3_clear_bindings(m_stmt_del);

                        sqlite3_exec(m_con, "END TRANSACTION", NULL, NULL, NULL);
                        sqlite3_exec(m_con, "BEGIN TRANSACTION", NULL, NULL, NULL);

                        sprintf_s(message, "Store %d tags", m_tagCount);
                        MxLog::LogInfo(message);
                    }

                    // получим новый результат
                    res = m_server->QueuePop();

                }
            }
            else
                this_thread::sleep_for(chrono::milliseconds(100));
            sqlite3_exec(m_con, "END TRANSACTION", NULL, NULL, NULL);
        }
        catch(exception& e){
            MxLog::LogDebug("MxStorage::Run exception: " + string(e.what()));
        }
        MxLog::LogDebug("MxStorage::Run while end");
    }
}

