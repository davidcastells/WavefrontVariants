/**
 * 
 * Copyright (C) 2021, David Castells-Rufas <david.castells@uab.cat>, 
 * Universitat Autonoma de Barcelona  
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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