#ifndef MXSCANCLASS_H
#define MXSCANCLASS_H

#include <chrono>
#include <vector>
#include <cstring>
#include "..\MxTypes\MxConnector.h"

using namespace std::chrono;

class MxScanClass
{
    public:
        MxScanClass();
        virtual ~MxScanClass();

        string GetName() { return m_name; };
        void SetName(string val) { m_name = val; };

        uint16_t GetInterval() { return m_interval; };
        void SetInterval(uint16_t val) { m_interval = val; };

        system_clock::time_point GetLastrun() { return m_lastrun; };
        void SetLastrun(system_clock::time_point val) { m_lastrun = val; };

        vector<MxRequest*> GetTags() { return m_tags; };
        void SetTags(vector<MxRequest*> val) { m_tags = val; };

        vector<MxResult*> GetResults() { return m_results; };

        // инициализация
        void Init();

    protected:

    private:
        string m_name;
        uint16_t m_interval;
        system_clock::time_point m_lastrun;

        vector<MxRequest*> m_tags;
        vector<MxResult*> m_results;
        bool m_is_init = false;

};

#endif // MXSCANCLASS_H
