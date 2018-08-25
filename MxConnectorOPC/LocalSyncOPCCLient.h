#pragma once
#ifndef OPC_LOCAL_CLIENT_H
#define OPC_LOCAL_CLIENT_H

#include <vector>
#include <string>
#include <map>
#include <cstdint>

#include "../OPCClientToolKit/opcda.h"
#include "../OPCClientToolKit/OPCClient.h"
#include "../OPCClientToolKit/OPCHost.h"
#include "../OPCClientToolKit/OPCServer.h"
#include "../OPCClientToolKit/OPCGroup.h"
#include "../OPCClientToolKit/OPCItem.h"


class LocalSyncOPCCLient
{
public:
	// constructor and destructor
	LocalSyncOPCCLient();
	virtual ~LocalSyncOPCCLient();

	// Controller actions
	bool Init();
	bool Connect(std::string serverName);
	bool DisConnect();
	bool IsConnected();
	bool Stop();

	// OPC API
	bool IsOPCRuning();
	virtual bool IsOPCConnectedPLC();// this function relis on the specific device
	virtual bool ItemNameFilter(std::string item_name);// if a item's name cannot pass the name fitler, it will not be added.

	// Basic Read Write API
	inline bool SyncWriteItem(std::string item_name, VARIANT* var);
	inline bool SyncReadItem(std::string item_name, VARIANT* var, uint16_t& wqlt, FILETIME& timeVal);

	bool ReadBool(std::string item_name, uint16_t& wqlt, FILETIME& timeVal);
	bool WriteBool(std::string item_name, bool item_value);

	int16_t ReadInt16(std::string item_name, uint16_t& wqlt, FILETIME& timeVal);
	int32_t ReadInt32(std::string item_name, uint16_t& wqlt, FILETIME& timeVal);

	float ReadFloat(std::string item_name, uint16_t& wqlt, FILETIME& timeVal);
	bool WirteFloat(std::string item_name, float item_value);

	uint16_t ReadUint16(std::string item_name, uint16_t& wqlt, FILETIME& timeVal);
	bool WriteUint16(std::string item_name, uint16_t item_value);

	bool ReadUint16Array(std::string item_name, uint16_t* item_value_array, int array_size);
	bool WriteUint16Array(std::string item_name, uint16_t* item_value_array, int array_size);

protected:
	std::map<std::string, COPCItem *> name_item_map_;
	COPCGroup* p_group_;
	unsigned long refresh_rate_;
	COPCHost* p_host_;
	COPCServer* p_opc_server_;
	bool is_env_ready;

	bool CleanOPCMember();
	bool DetectService(char* ServiceName);
	VARIANT read_tmp_variant_;
};

#endif //OPCONTROLLER_H
