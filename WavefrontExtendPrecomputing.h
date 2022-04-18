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

#ifndef WAVEFRONT_EXTEND_H_INCLUDED
#define WAVEFRONT_EXTEND_H_INCLUDED

#include "Aligner.h"

/**
*/
class WavefrontExtendPrecomputing : public Aligner
{
public:
    WavefrontExtendPrecomputing();
    virtual ~WavefrontExtendPrecomputing();

    void setInput(const char* P, const char* T, long k);
    void precomputeExtend();
    long getDistance();
    char* getAlignmentPath(long* distance);
    const char* getDescription(); 


    int polarExistsInD(int d, int r);
    int polarExistsInW(int d, int r);

    int* m_W;
    long m_size_W;
    int* m_E;
    long m_size_E;

    long m_k;
    long m_top;
};

#endif