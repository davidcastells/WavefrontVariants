/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCLFPGAWavefrontOriginal2Cols.h
 * Author: dcr
 *
 * Created on December 22, 2021, 7:24 PM
 */

#ifndef OCLFPGAWAVEFRONTORIGINAL2COLS_H
#define OCLFPGAWAVEFRONTORIGINAL2COLS_H

#include "Aligner.h"
#include "OCLUtils.h"

class OCLFPGAWavefrontOriginal2Cols : public Aligner
{
public:
    OCLFPGAWavefrontOriginal2Cols();
    virtual ~OCLFPGAWavefrontOriginal2Cols();

    void setInput(const char* P, const char* T, long k);
    long getDistance();
    char* getAlignmentPath(long* distance);
    const char* getDescription(); 

    void setCommonArgs();
    void invokeKernel(long r);
    void progress(PerformanceLap& lap, long r, int& lastpercent, long cellsAllocated, long cellsAlive);
    
	
    long* m_W;
    
    
    long m_k;
    long m_w_h;         // height of the W pyramid columns
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

#endif /* OCLFPGAWAVEFRONTORIGINAL2COLS_H */

