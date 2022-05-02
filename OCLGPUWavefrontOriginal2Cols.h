/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCLGPUWavefrontOriginal2Cols.h
 * Author: dcr
 *
 * Created on December 15, 2021, 8:19 PM
 */

#ifndef OCLGPUWAVEFRONTORIGINAL2COLS_H
#define OCLGPUWAVEFRONTORIGINAL2COLS_H

#include "Aligner.h"
#include "OCLUtils.h"

class OCLGPUWavefrontOriginal2Cols : public Aligner
{
public:
    OCLGPUWavefrontOriginal2Cols();
    virtual ~OCLGPUWavefrontOriginal2Cols();

    void setInput(const char* P, const char* T, long k);
    long getDistance();
    char* getAlignmentPath(long* distance);
    const char* getDescription(); 

    void setCommonArgs();
    void invokeKernel(long r);
    void progress(PerformanceLap& lap, long r, int& lastpercent, long cellsAllocated, long cellsAlive);
    
	
    long* m_W;
    long m_k;
    long m_top;
    int m_tileLen;
    std::string m_localStore;
    long m_final_d_r[2];
    
    cl_context m_context;
    OCLQueue* m_queue;
    cl_mem m_buf_P;
    cl_mem m_buf_T;
    cl_mem m_buf_W;
    cl_mem m_buf_final_d_r;
    cl_kernel m_kernel;
};

#endif /* OCLGPUWAVEFRONTORIGINAL2COLS_H */

