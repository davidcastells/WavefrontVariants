#ifndef WAVEFRONT_DYNAMIC_DIAMOND_2_COLS_H_INCLUDED
#define WAVEFRONT_DYNAMIC_DIAMOND_2_COLS_H_INCLUDED

#include "Aligner.h"

/**
*/
class WavefrontDynamicDiamond2Cols : public Aligner
{
public:
    WavefrontDynamicDiamond2Cols();
    virtual ~WavefrontDynamicDiamond2Cols();

    void setInput(const char* P, const char* T, long k);
    long getDistance();
    char* getAlignmentPath(long* distance);
    const char* getDescription(); 

    long* m_W;
    
    
    long m_k;
    long m_top;
};

#endif