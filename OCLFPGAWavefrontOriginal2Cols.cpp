/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCLFPGAWavefrontOriginal2Cols.cpp
 * Author: dcr
 * 
 * Created on December 22, 2021, 7:24 PM
 */

#include "OCLFPGAWavefrontOriginal2Cols.h"
#include "utils.h"

#include <assert.h>
#include <string.h>


#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
//#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)		(((y)>(x))? (x) : (y))

//#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)),w)

#define POLAR_W_TO_INDEX(d, r)		((d)+m_k + (((r)%2) * (2*m_k+1)))
#define INDEX_TO_POLAR_W_D(idx, r)      ((idx) - (m_k) - ((r)%2)*(2*m_k+1))


#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))

extern int verbose;
extern int gPid;

OCLFPGAWavefrontOriginal2Cols::OCLFPGAWavefrontOriginal2Cols()
{
    m_buf_P = NULL;
    m_buf_T = NULL;
    m_buf_W = NULL;
    
    auto ocl = OCLUtils::getInstance();
    //ocl->selectPlatform(gPid);
    ocl->selectDevice(0);
    
    m_context = ocl->createContext();
    m_queue = ocl->createQueue();
    
    m_W = NULL;
}



OCLFPGAWavefrontOriginal2Cols::~OCLFPGAWavefrontOriginal2Cols()
{
    auto ocl = OCLUtils::getInstance();
    ocl->releaseMemObject(m_buf_P);
    ocl->releaseMemObject(m_buf_T);
            
    ocl->releaseContext(m_context);
    
    if (m_W != NULL)
        delete [] m_W;
}


void OCLFPGAWavefrontOriginal2Cols::setInput(const char* P, const char* T, long k)
{
    // this should not be allocated, we only expect a single call
    assert(m_W == NULL);
    
    m_m = strlen(P);
    m_n = strlen(T);
    m_k = k;

    long size = 2*(2*k+1);
    m_w_h = size;

    try
    {
        m_W = new long[size];
    }
    catch (const std::bad_alloc& e) 
    {
        printf("FAILED to allocate memory\n");
        exit(-1);
    }

//    assert(m_W);
    m_P = P;
    m_T = T;

    cl_int err;
    
    m_buf_P = clCreateBuffer(m_context, CL_MEM_READ_ONLY, m_m, NULL, &err);
    CHECK_CL_ERRORS(err);
    m_buf_T = clCreateBuffer(m_context, CL_MEM_READ_ONLY, m_n, NULL, &err);
    CHECK_CL_ERRORS(err);
    

    printf("creating buffer %.2f GB\n", size*sizeof(long)/(1E9));

    m_buf_W = clCreateBuffer(m_context, CL_MEM_READ_WRITE, size * sizeof(long), NULL, &err);
    CHECK_CL_ERRORS(err);
    
    m_buf_final_d_r = clCreateBuffer(m_context, CL_MEM_READ_WRITE, 2 * sizeof(long), NULL, &err);
    CHECK_CL_ERRORS(err);
    
    OCLUtils* ocl = OCLUtils::getInstance();
    std::string pn = ocl->getSelectedPlatformName();
    
    if (OCLUtils::contains(pn, "Emulation"))
        ocl->createProgramFromSource("WFO2ColsFPGA.cl");
    else
        if (OCLUtils::contains(pn, "Intel"))
            ocl->createProgramFromBinary("WFO2ColsFPGA.aocx");
        else
            ocl->createProgramFromBinary("WFO2ColsFPGA.xclbin");
    
    m_kernel = ocl->createKernel("wfo2cols");
    
    printf("input set\n");
}

void OCLFPGAWavefrontOriginal2Cols::progress(PerformanceLap& lap, long r, int& lastpercent, long cellsAllocated, long cellsAlive)
{
#define DECIMALS_PERCENT    1000
    if (!verbose)
        return;
    
    double elapsed = lap.stop();
    // linear model
//    double estimated = (elapsed / r) * (m_k);
//    int percent = (r*100.0*DECIMALS_PERCENT/m_k);
    // square model
    double estimated = (elapsed / (r*r)) * (m_k*m_k);
    int percent = (r*r*100.0*DECIMALS_PERCENT/(m_k*m_k));
    
    //if (percent != lastpercent)
    {
        //printf("\rcol %ld/%ld %.2f%% cells allocated: %ld alive: %ld elapsed: %d s  estimated: %d s    ", x, m_n, ((double)percent/DECIMALS_PERCENT), cellsAllocated, cellsAlive, (int) elapsed, (int) estimated );
        printf("\rr %ld/%ld %.2f%% elapsed: %d s  estimated: %d s  ", r, m_k, ((double)percent/DECIMALS_PERCENT) , (int) elapsed, (int) estimated );
    
        fflush(stdout);
        lastpercent = percent;
    }
}

#define NUMBER_OF_INVOCATIONS_PER_READ 100

long OCLFPGAWavefrontOriginal2Cols::getDistance()
{
    PerformanceLap lap;
    int lastpercent = -1;
    long cellsAllocated = 0;
    long cellsAlive = 0;
    
    m_queue->writeBuffer(m_buf_P, (void*) m_P, m_m);
    m_queue->writeBuffer(m_buf_T, (void*) m_T, m_n);
    
    long h = 2*m_k+1;
    
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long m_top = max2(m_m, m_n);
    
    m_final_d_r[0] = 0;     // furthest reaching point
    m_final_d_r[1] = m_top; // estimated distance (now, worst case)
    
    setCommonArgs();
    
    m_queue->writeBuffer(m_buf_final_d_r, m_final_d_r, 2 * sizeof(long) );
    
    for (long r=0; r < m_k; r++)
    {
        invokeKernel(r);

        progress(lap, r, lastpercent, cellsAllocated, cellsAlive);
        
//        m_queue->readBuffer(m_buf_W, m_W, 2 * h * sizeof(long));
//        
//        printf("\n");
//        for (long i=0; i < h; i++)
//            printf("W[%ld]= %ld\n", i, m_W[(r%2)*h + i]);
        
        if ((r % NUMBER_OF_INVOCATIONS_PER_READ) == 0)
        {
            m_queue->readBuffer(m_buf_final_d_r, m_final_d_r, 2 * sizeof(long));
            
            if (m_final_d_r[0] >= m_top)
                return m_final_d_r[1];
        }
    }
    
    lastpercent--;
    progress(lap, m_k, lastpercent, cellsAllocated, cellsAlive);

    return m_top;
}
    
void OCLFPGAWavefrontOriginal2Cols::setCommonArgs()
{
    cl_int ret;
    ret = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), (void *)&m_buf_P);
    CHECK_CL_ERRORS(ret);
    
    ret = clSetKernelArg(m_kernel, 1, sizeof(cl_mem), (void *)&m_buf_T);
    CHECK_CL_ERRORS(ret);
    
    ret = clSetKernelArg(m_kernel, 2, sizeof(cl_long), (void *)&m_m);
    CHECK_CL_ERRORS(ret);

    ret = clSetKernelArg(m_kernel, 3, sizeof(cl_long), (void *)&m_n);
    CHECK_CL_ERRORS(ret);
    
    //long size = 2*(2*m_k+1);

    long k = max2(m_m,m_n);

    ret = clSetKernelArg(m_kernel, 5, sizeof(cl_long), (void *)&k);
    CHECK_CL_ERRORS(ret);
    
    ret = clSetKernelArg(m_kernel, 6, sizeof(cl_mem), (void *)&m_buf_W);
    CHECK_CL_ERRORS(ret);

    ret = clSetKernelArg(m_kernel, 7, sizeof(cl_mem), (void *)&m_buf_final_d_r);
    CHECK_CL_ERRORS(ret);
}

void OCLFPGAWavefrontOriginal2Cols::invokeKernel(long r)
{
    cl_int ret;
    
    ret = clSetKernelArg(m_kernel, 4, sizeof(cl_long), (void *)&r);
    CHECK_CL_ERRORS(ret);

    //long k = max2(m_m,m_n);
    
    m_queue->invokeKernel1D(m_kernel, 2*r+1);
}

char* OCLFPGAWavefrontOriginal2Cols::getAlignmentPath(long* distance)
{
    printf("Not implemented yet\n");
    exit(-1);
}
    
const char* OCLFPGAWavefrontOriginal2Cols::getDescription()
{
    return "Wavefront Original 2 columns in OpenCL";
}
