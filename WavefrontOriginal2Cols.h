#ifndef WAVEFRONT_ORIGINAL_2_COLS_H_INCLUDED
#define WAVEFRONT_ORIGINAL_2_COLS_H_INCLUDED

#include "Aligner.h"

/**
*/
class WavefrontOriginal2Cols : public Aligner
{
public:
    WavefrontOriginal2Cols();
    virtual ~WavefrontOriginal2Cols();

    void setInput(const char* P, const char* T, long k);
    long getDistance();
    char* getAlignmentPath(long* distance);
    const char* getDescription(); 

protected:
    void progress(PerformanceLap& lap, long r, int& lastpercent, long cellsAllocated, long cellsAlive);

    long* m_W;
    const char* m_P;
    const char* m_T;
    long m_n;
    long m_m;
    long m_k;
    long m_top;
};

#endif