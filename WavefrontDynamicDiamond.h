#ifndef WAVEFRONT_DYNAMIC_DIAMOND_H_INCLUDED
#define WAVEFRONT_DYNAMIC_DIAMOND_H_INCLUDED

#include "Aligner.h"

/**
*/
class WavefrontDynamicDiamond : public Aligner
{
public:
    WavefrontDynamicDiamond();
    virtual ~WavefrontDynamicDiamond();

    void setInput(const char* P, const char* T,  long k);
    long getDistance();
    char* getAlignmentPath(long* distance);
    const char* getDescription(); 


    long* m_W;
    
    long m_k;
    long m_top;
};

#endif