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
    long extendAligned(const char* P, const char* T, long m, long n, long pi, long ti);
    long extend(const char* P, const char* T, long m, long n, long pi, long ti);

    void collectExtendStats(long e);
    void collectPTReadBytes(int inc);
    void collectWWriteBytes(int inc);
    void collectWReadBytes(int inc);
    void collectTime(double lap);
    void printStats();
    
protected:
    void progress(PerformanceLap& lap, long r, int& lastpercent, long cellsAllocated, long cellsAlive);

    long* m_W;
    
    long m_k;
    long m_top;
    
    // stats
    long m_statsExtendBins[10];
    long m_statsMaxExtend;
    long m_statsPTReadBytes;
    long m_statsWReadBytes;
    long m_statsWWriteBytes;
    double m_statsTime;
};

#endif