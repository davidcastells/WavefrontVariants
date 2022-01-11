/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCLGPUWavefrontDynamicDiamond2Cols.h
 * Author: dcr
 *
 * Created on January 9, 2022, 1:24 PM
 */

#ifndef OCLGPUWAVEFRONTDYNAMICDIAMOND2COLS_H
#define OCLGPUWAVEFRONTDYNAMICDIAMOND2COLS_H

#include "Aligner.h"
#include "OCLUtils.h"

class OCLGPUWavefrontDynamicDiamond2Cols : public Aligner
{
public:
    OCLGPUWavefrontDynamicDiamond2Cols();
    virtual ~OCLGPUWavefrontDynamicDiamond2Cols();

protected:

    void setInput(const char* P, const char* T, long k);
    long getDistance();
    char* getAlignmentPath(long* distance);
    const char* getDescription(); 

    void setCommonArgs();
    void invokeKernel(long r, long dstart, long numds);
    void progress(PerformanceLap& lap, long r, int& lastpercent, long cellsAllocated, long cellsAlive, long numds);
    
    long* m_W;
    long m_k;
    long m_top;
    int m_tileLen;
    long m_final_d_r[2];
    
    cl_context m_context;
    OCLQueue* m_queue;
    cl_mem m_buf_P;
    cl_mem m_buf_T;
    cl_mem m_buf_W;
    cl_mem m_buf_final_d_r;
    cl_kernel m_kernel;
};

#endif /* OCLGPUWAVEFRONTDYNAMICDIAMOND2COLS_H */

