#ifndef MXTAG_H
#define MXTAG_H

#include <string>
#include <chrono>
#include "../MxTypes/MxTypes.h"

using namespace std;
using namespace std::chrono;

class MxTag
{
    public:
        MxTag();
        ~MxTag();

        string GetName(){return m_name;};
        void SetName(string val) {m_name = val;};

        system_clock::time_point GetUpdateTime() {return m_updateTime;};
        void SetUpdateTime(system_clock::time_point val) {m_updateTime = val;};

        system_clock::time_point GetChangeTime() {return m_changeTime;};
        void SetChangeTime(system_clock::time_point val) {m_changeTime = val;};

        string GetAddr(){return m_addr;};
        void SetAddr(string val) {m_addr = val;};

        TAG_TYPE GetTagType() {return m_tagType;};
        void SetTagType(TAG_TYPE val) {m_tagType = val;};

        TAG_QUALITY GetQuality() {return m_quality;};
        void SetQuality(TAG_QUALITY val) {m_quality = val;};

        bool GetBoolVal() {return m_boolVal;};
        void SetBoolVal(bool val) {m_boolVal = val;};

		int64_t GetIntVal() {return m_intVal;};
        void SetIntVal(int64_t val) {m_intVal = val;};


        float GetFloatVal() {return m_floatVal;};
        void SetFloatVal(float val) {m_floatVal = val;};

        string GetStrVal() {return m_strVal;};
        void SetStrVal(string val) {m_strVal = val;};

        float GetMinVal() {return m_minVal;};
        void SetMinVal(float val) {m_minVal = val;};

        float GetMaxVal() {return m_maxVal;};
        void SetMaxVal(float val) {m_maxVal = val;};

        float GetDeadBand() {return m_deadBand;};
        void SetDeadBand(float val) {m_deadBand = val;};

        uint8_t GetFlushTime() {return m_flushTime;};
        void SetFlushTime(uint8_t val) {m_flushTime = val;};

        float GetMulti() {return m_multi;};
        void SetMulti(float val) {m_multi = val;};

    private:
        string m_name;
        system_clock::time_point m_updateTime;
        system_clock::time_point m_changeTime;
        string m_addr;
        TAG_TYPE m_tagType;

        float m_minVal;
        float m_maxVal;
        float m_deadBand;
        float m_multi;
        uint8_t m_flushTime;

        TAG_QUALITY m_quality;
        bool m_boolVal;
		int64_t m_intVal;
        float m_floatVal;
        string m_strVal;
};


#endif // MXTAG_H
