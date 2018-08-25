#include "MxConnectorRtLink.h"

extern "C" {
	__declspec(dllexport) void* ConnectorCreate(){
        return new MxConnectorRtLink();
    }
	__declspec(dllexport) void ConnectorFree(void* conn){
        delete (MxConnectorRtLink*)conn;
    }
	__declspec(dllexport) void Open(void* conn){
        ((MxConnectorRtLink*)conn)->Open();
    }
	__declspec(dllexport) void Close(void* conn){
        ((MxConnectorRtLink*)conn)->Close();
    }
	__declspec(dllexport) void SetName(void* conn, string val){
        ((MxConnectorRtLink*)conn)->SetName(val);
    }
	__declspec(dllexport) string GetName(void* conn){
        return ((MxConnectorRtLink*)conn)->GetName();
    }
	__declspec(dllexport) void SetConnString(void* conn, string val){
        ((MxConnectorRtLink*)conn)->SetConnString(val);
    }
	__declspec(dllexport) string GetConnString(void* conn){
        return ((MxConnectorRtLink*)conn)->GetConnString();
    }
	__declspec(dllexport) void SetState(void* conn, bool val){
        ((MxConnectorRtLink*)conn)->SetState(val);
    }
	__declspec(dllexport) bool GetState(void* conn){
        return ((MxConnectorRtLink*)conn)->GetState();
    }
	__declspec(dllexport) void SetTimeout(void* conn, uint8_t val){
        ((MxConnectorRtLink*)conn)->SetTimeout(val);
    }
	__declspec(dllexport) uint8_t GetTimeout(void* conn){
        return ((MxConnectorRtLink*)conn)->GetTimeout();
    }
	/*__declspec(dllexport) vector<MxResult*> Read(void* conn, vector<MxRequest*> tags){
        return ((MxConnectorRtLink*)conn)->Read(tags);
    }*/
}
