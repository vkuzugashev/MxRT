#include "MxScanClass.h"

MxScanClass::MxScanClass()
{
    //ctor
    m_lastrun = system_clock::now();
}

MxScanClass::~MxScanClass()
{
    uint16_t size = m_tags.size();

	if (size > 0) {
		// освободим память
		for (uint16_t i = 0; i < size; i++) {
			delete m_tags[i];
		}
	}
	
	size = m_results.size();
	if (size > 0) {
		for(uint16_t i=0; i < size; i++){
			delete m_results[i];
        }
    }
}

void MxScanClass::Init()
{
    if(!m_is_init){

        m_is_init = true;

        uint16_t size = m_tags.size();

        if(size > 0){

			for(uint16_t i=0; i < size; i++){
                MxResult* mr = new MxResult();
                mr->Name = m_tags[i]->Name;
                mr->TagType = m_tags[i]->TagType;
                m_results.push_back(mr);
			}

        }
    }
}
