#ifndef MXTYPES_H_INCLUDED
#define MXTYPES_H_INCLUDED

#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

const uint8_t TAG_TIME_SIZE = 20;
const uint8_t LOG_TIME_SIZE = 20;
const char TAG_TIME_FORMAT[] = "%Y-%m-%dT%H:%M:%S";
const char LOG_TIME_FORMAT[] = "%d.%m.%Y %H:%M:%S";


enum class LOG_TYPE : uint8_t{
	LEVEL_DEBUG = 0,
	LEVEL_INFO,
	LEVEL_WARN,
	LEVEL_ERROR
};

enum TAG_TYPE : uint8_t
{
    TAG_TYPE_BOOL = 0,
    TAG_TYPE_BYTE,
	TAG_TYPE_INT16,
	TAG_TYPE_WORD,
    TAG_TYPE_INT32,
	TAG_TYPE_DWORD,
	TAG_TYPE_FLOAT,
    TAG_TYPE_STRING
};

enum TAG_QUALITY : uint16_t
{
    TAG_QUALITY_BAD = 0,
    TAG_QUALITY_GOOD = 198
};

struct MxRequest
{
    string Name;
    TAG_TYPE TagType;
    string Source;
};

struct MxResult
{
    string Name;
    time_t UpdateTime;
    TAG_TYPE TagType;
    TAG_QUALITY Quality;
    bool BoolVal;
    int64_t Int64Val;
	float FloatVal;
    string StrVal;
};


#endif // MXTYPES_H_INCLUDED
