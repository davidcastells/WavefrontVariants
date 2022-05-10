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

/* 
 * File:   CUDAWavefrontDynamicDiamond2Cols.h
 * Author: dcr
 *
 * Created on January 14, 2022, 11:49 AM
 */

#ifndef CUDAWAVEFRONTDYNAMICDIAMOND2COLS_H
#define CUDAWAVEFRONTDYNAMICDIAMOND2COLS_H

#include "Aligner.h"


class CUDAWavefrontDynamicDiamond2Cols : public Aligner 
{
public:
    CUDAWavefrontDynamicDiamond2Cols();
    virtual ~CUDAWavefrontDynamicDiamond2Cols();

protected:

    void setInput(const char* P, const char* T, INT_TYPE k);
    INT_TYPE getDistance();
    char* getAlignmentPath(INT_TYPE* distance);
    const char* getDescription(); 

    void setCommonArgs();
    void invokeKernel(INT_TYPE r, INT_TYPE dstart, INT_TYPE numds);
    void progress(PerformanceLap& lap, INT_TYPE r, int& lastpercent, long cellsAllocated, long cellsAlive, INT_TYPE numds);
    
    INT_TYPE* m_W;
    INT_TYPE m_k;
    INT_TYPE m_top;
    int m_tileLen;
    INT_TYPE m_final_d_r[2];
  
    char* m_buf_P;
    char* m_buf_T;
    INT_TYPE* m_buf_W;
    INT_TYPE* m_buf_final_d_r;
    
};

#endif /* CUDAWAVEFRONTDYNAMICDIAMOND2COLS_H */

