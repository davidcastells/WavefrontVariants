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

    void setInput(const char* P, const char* T, INT_TYPE k);
    INT_TYPE getDistance();
    char* getAlignmentPath(INT_TYPE* distance);
    const char* getDescription(); 
    INT_TYPE extendAligned(const char* P, const char* T, INT_TYPE m, INT_TYPE n, INT_TYPE pi, INT_TYPE ti);
    INT_TYPE extend(const char* P, const char* T, INT_TYPE m, INT_TYPE n, INT_TYPE pi, INT_TYPE ti);

    void collectExtendStats(INT_TYPE e);
    void collectPTReadBytes(int inc);
    void collectWWriteBytes(int inc);
    void collectWReadBytes(int inc);
    void collectTime(double lap);
    void printStats();
    
protected:
    void progress(PerformanceLap& lap, INT_TYPE r, int& lastpercent, long cellsAllocated, long cellsAlive);

    INT_TYPE* m_W;
    
    INT_TYPE m_k;
    INT_TYPE m_top;
    
    // stats
    long m_statsExtendBins[10];
    long m_statsMaxExtend;
    long m_statsPTReadBytes;
    long m_statsWReadBytes;
    long m_statsWWriteBytes;
    double m_statsTime;
};

#endif
