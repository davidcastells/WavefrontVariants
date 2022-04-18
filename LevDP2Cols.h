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

#ifndef LEV_DP_2_COLS_H_INCLUDED
#define LEV_DP_2_COLS_H_INCLUDED

#include "Aligner.h"

/**
*/
class LevDP2Cols  : public Aligner
{
public:
    LevDP2Cols();
    virtual ~LevDP2Cols();

    void setInput(const char* P, const char* T, long k);
    long getDistance();
    char* getAlignmentPath(long* distance);
    const char* getDescription(); 

protected:
    void progress(PerformanceLap& lap, long x, int& lastpercent, long cellsAllocated, long cellsAlive);

    long* m_D;
    long m_top;
    
    
    long m_k;
};

#endif