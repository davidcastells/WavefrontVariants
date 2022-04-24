/**
 * 
 * Copyright (C) 2021-2022, David Castells-Rufas <david.castells@uab.cat>, 
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

/* 
 * File:   CUDAWavefrontOriginal2Cols.h
 * Author: dcr
 *
 * Created on April 24, 2022, 11:30 AM
 */

#ifndef CUDAWAVEFRONTORIGINAL2COLS_H
#define CUDAWAVEFRONTORIGINAL2COLS_H

#include "Aligner.h"
#include "OCLUtils.h"

class CUDAWavefrontOriginal2Cols : public Aligner
{
public:
    CUDAWavefrontOriginal2Cols();
    virtual ~CUDAWavefrontOriginal2Cols();

    void setInput(const char* P, const char* T, long k);
    long getDistance();
    char* getAlignmentPath(long* distance);
    const char* getDescription(); 

    void setCommonArgs();
    void invokeKernel(long r);
    void progress(PerformanceLap& lap, long r, int& lastpercent, long cellsAllocated, long cellsAlive);
    
	
    long* m_W;
    const char* m_P;
    const char* m_T;
    long m_n;
    long m_m;
    long m_k;
    long m_top;
    int m_tileLen;
    long m_final_d_r[2];
    
    char* m_buf_P;
    char* m_buf_T;
    long* m_buf_W;
    long* m_buf_final_d_r;
    
};

#endif /* CUDAWAVEFRONTORIGINAL2COLS_H */

