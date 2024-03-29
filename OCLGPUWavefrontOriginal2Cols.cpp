/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   OCLGPUWavefrontOriginal2Cols.cpp
 * Author: dcr
 * 
 * Created on December 15, 2021, 8:19 PM
 */

#include "OCLGPUWavefrontOriginal2Cols.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <new>


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
extern int gDid;
extern int gTileLen;
extern int gWorkgroupSize;
extern int gExtendAligned;
extern int gEnqueuedInvocations;
extern double gPrintPeriod;
extern char* gLocalTileMemory;
extern int gMeasureIterationTime;

OCLGPUWavefrontOriginal2Cols::OCLGPUWavefrontOriginal2Cols()
{
    m_buf_P = NULL;
    m_buf_T = NULL;
    m_buf_W = NULL;
    
    auto ocl = OCLUtils::getInstance();
    ocl->selectPlatform(gPid);
    ocl->selectDevice(0);
    
    m_context = ocl->createContext();
    m_queue = ocl->createQueue();
    
    m_W = NULL;
}


OCLGPUWavefrontOriginal2Cols::~OCLGPUWavefrontOriginal2Cols()
{
    auto ocl = OCLUtils::getInstance();
    ocl->releaseMemObject(m_buf_P);
    ocl->releaseMemObject(m_buf_T);
            
    ocl->releaseContext(m_context);
    
    if (m_W != NULL)
        delete [] m_W;
}

void OCLGPUWavefrontOriginal2Cols::setInput(const char* P, const char* T, INT_TYPE k)
{
    // this should not be allocated, we only expect a single call
    assert(m_W == NULL);
    
    m_m = strlen(P);
    m_n = strlen(T);
    m_k = k;

    long size = 2*(2*k+1);


    try
    {
        m_W = new INT_TYPE[size];
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

    m_buf_W = clCreateBuffer(m_context, CL_MEM_READ_WRITE, size * sizeof(INT_TYPE), NULL, &err);
    CHECK_CL_ERRORS(err);
    
    m_buf_final_d_r = clCreateBuffer(m_context, CL_MEM_READ_WRITE, sizeof(INT_TYPE), NULL, &err);
    CHECK_CL_ERRORS(err);
    
    auto ocl = OCLUtils::getInstance();
    
    std::string options ="";
//    options += " -D TILE_LEN=" + std::to_string(m_tileLen);
//    options += " -D " + m_localStore;
    options += " -D WORKGROUP_SIZE=" + std::to_string(gWorkgroupSize);
    options += " -D COLUMN_HEIGHT=" + std::to_string(2*k+1);
    options += " -D ROW_ZERO_OFFSET=" + std::to_string(k);
    options += " -D INT_TYPE="  STR_INT_TYPE "";
    options += " ";
    
//    if (gExtendAligned)
//        options += " -D EXTEND_ALIGNED ";
        
    ocl->createProgramFromSource("WFO2ColsGPU.cl", options);
    m_kernel = ocl->createKernel("wfo2cols");
    
    printf("input set\n");
}

void OCLGPUWavefrontOriginal2Cols::progress(PerformanceLap& lap, INT_TYPE r, int& lastpercent, long cellsAllocated, long cellsAlive)
{
    static double lastPrintLap = 0;
    
    double printPeriod = (gPrintPeriod > 0)? gPrintPeriod : 0.5;
    
#define DECIMALS_PERCENT    1000
    if (!verbose)
        return;
    
    double elapsed = lap.stop();
    // linear model
//    double estimated = (elapsed / r) * (m_k);
//    int percent = (r*100.0*DECIMALS_PERCENT/m_k);
    // square model
    double rd = r;
    double kd = m_k;
    double estimated = (elapsed / (rd*rd)) * (kd*kd);
    int percent = (rd*rd*100.0*DECIMALS_PERCENT/(kd*kd));
    
    if (elapsed > (lastPrintLap + printPeriod))
    {
        printf((gPrintPeriod > 0)?"\n":"\r");
        //printf("\rcol %ld/%ld %.2f%% cells allocated: %ld alive: %ld elapsed: %d s  estimated: %d s    ", x, m_n, ((double)percent/DECIMALS_PERCENT), cellsAllocated, cellsAlive, (int) elapsed, (int) estimated );
        printf("r %d/%d %.2f%% elapsed: %d s  estimated: %d s  ", r, m_k, ((double)percent/DECIMALS_PERCENT),  (int) elapsed, (int) estimated );
    
        fflush(stdout);
        lastpercent = percent;
        lastPrintLap = elapsed;
    }
}

INT_TYPE OCLGPUWavefrontOriginal2Cols::getDistance()
{
    PerformanceLap lap;
    int lastpercent = -1;
    long cellsAllocated = 0;
    long cellsAlive = 0;
    
    m_queue->writeBuffer(m_buf_P, (void*) m_P, m_m);
    m_queue->writeBuffer(m_buf_T, (void*) m_T, m_n);
    
    INT_TYPE h = 2*m_k+1;
    
    INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    INT_TYPE m_top = max2(m_m,m_n);

    setCommonArgs();

    if (gMeasureIterationTime)
        printf("r,time\n");
        
    for (INT_TYPE r=0; r < m_k; r++)
    {
        invokeKernel(r);

        if (gMeasureIterationTime)
        {
            if (((r % 1000) == 0) & (r>0))
            {
                lap.stop();
                printf("%ld, %f\n", r, lap.lap());
            }
        }        
        else
            progress(lap, r, lastpercent, cellsAllocated, cellsAlive);
        
        if (m_final_d_r >= m_top)
            return r;
        // printf("final d = %ld\n", m_W[POLAR_W_TO_INDEX(final_d, i)]);
    }
    
    lastpercent--;
    progress(lap, m_k, lastpercent, cellsAllocated, cellsAlive);

    return m_top;
}
    
void OCLGPUWavefrontOriginal2Cols::setCommonArgs()
{
    cl_int ret;
    ret = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), (void *)&m_buf_P);
    CHECK_CL_ERRORS(ret);
    
    ret = clSetKernelArg(m_kernel, 1, sizeof(cl_mem), (void *)&m_buf_T);
    CHECK_CL_ERRORS(ret);
    
    ret = clSetKernelArg(m_kernel, 2, sizeof(CL_INT_TYPE), (void *)&m_m);
    CHECK_CL_ERRORS(ret);

    ret = clSetKernelArg(m_kernel, 3, sizeof(CL_INT_TYPE), (void *)&m_n);
    CHECK_CL_ERRORS(ret);
    
    //long size = 2*(2*m_k+1);

    INT_TYPE k = max2(m_m,m_n);

    ret = clSetKernelArg(m_kernel, 5, sizeof(CL_INT_TYPE), (void *)&k);
    CHECK_CL_ERRORS(ret);
    
    ret = clSetKernelArg(m_kernel, 6, sizeof(cl_mem), (void *)&m_buf_W);
    CHECK_CL_ERRORS(ret);

    ret = clSetKernelArg(m_kernel, 7, sizeof(cl_mem), (void *)&m_buf_final_d_r);
    CHECK_CL_ERRORS(ret);
}

void OCLGPUWavefrontOriginal2Cols::invokeKernel(INT_TYPE r)
{
    cl_int ret;
    
    ret = clSetKernelArg(m_kernel, 4, sizeof(CL_INT_TYPE), (void *)&r);
    CHECK_CL_ERRORS(ret);

    INT_TYPE k = max2(m_m,m_n);
    
    m_queue->invokeKernel1D(m_kernel, 2*k+1, gWorkgroupSize);
    
    m_queue->readBuffer(m_buf_final_d_r, &m_final_d_r, sizeof(CL_INT_TYPE));

}

char* OCLGPUWavefrontOriginal2Cols::getAlignmentPath(INT_TYPE* distance)
{
    printf("Not implemented yet\n");
    exit(-1);
}
    
const char* OCLGPUWavefrontOriginal2Cols::getDescription()
{
    return "Wavefront Original 2 columns in OpenCL";
}
