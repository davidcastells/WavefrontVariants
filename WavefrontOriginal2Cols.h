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

    void setInput(const char* P, const char* T, INT_TYPE k);
    INT_TYPE getDistance();
    char* getAlignmentPath(INT_TYPE* distance);
    const char* getDescription(); 

protected:
    void progress(PerformanceLap& lap, INT_TYPE r, int& lastpercent, long cellsAllocated, long cellsAlive);

    INT_TYPE* m_W;
    INT_TYPE m_k;
    INT_TYPE m_top;
};

#endif
