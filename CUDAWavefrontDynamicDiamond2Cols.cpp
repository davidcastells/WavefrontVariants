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
 * File:   CUDAWavefrontDynamicDiamond2Cols.cpp
 * Author: dcr
 * 
 * Created on January 14, 2022, 11:49 AM
 */

#include "CUDAWavefrontDynamicDiamond2Cols.h"
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

#define POLAR_W_TO_INDEX(d, r)		((d)+m_k + (((r)%(2*tileLen)) * (2*m_k+1)))
#define INDEX_TO_POLAR_W_D(idx, r)      ((idx) - (m_k) - ((r)%(2*tileLen))*(2*m_k+1))


#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))

extern int verbose;
extern int gPid;
extern int gDid;
extern int gWorkgroupSize;
extern int gExtendAligned;
extern int gPrintPeriod;


CUDAWavefrontDynamicDiamond2Cols::CUDAWavefrontDynamicDiamond2Cols()
{
    m_W = NULL;
}


CUDAWavefrontDynamicDiamond2Cols::~CUDAWavefrontDynamicDiamond2Cols()
{    
    if (m_W != NULL)
        delete [] m_W;

}





void CUDAWavefrontDynamicDiamond2Cols::setInput(const char* P, const char* T, long k)
{
    // this should not be allocated, we only expect a single call
    assert(m_W == NULL);
    
    m_m = strlen(P);
    m_n = strlen(T);
    m_k = k;
    m_tileLen = 3;

    long size = (2*m_tileLen)*(2*k+1);

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
    
    auto ocl = OCLUtils::getInstance();
        
    std::string options = "-D TILE_LEN=" + std::to_string(m_tileLen) + " ";

    std::string plName = ocl->getSelectedPlatformName();
    if (OCLUtils::contains(plName, "Portable Computing Language") && (verbose > 1))
        options += " -D DEBUG ";
    
    if (gExtendAligned)
        options += " -D EXTEND_ALIGNED ";
    
    ocl->createProgramFromSource("WFDD2ColsGPU.cl", options);
    m_kernel = ocl->createKernel("wfdd2cols");
    
    printf("input set\n");
}

void CUDAWavefrontDynamicDiamond2Cols::progress(PerformanceLap& lap, long r, int& lastpercent, long cellsAllocated, long cellsAlive, long numds)
{
    static double lastPrintLap = -1;
    
    double printPeriod = (gPrintPeriod > 0)? gPrintPeriod : 0.5;
    
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
    
    if (elapsed > (lastPrintLap + printPeriod))
    {
        printf((gPrintPeriod > 0)?"\n":"\r");
        //printf("\rcol %ld/%ld %.2f%% cells allocated: %ld alive: %ld elapsed: %d s  estimated: %d s    ", x, m_n, ((double)percent/DECIMALS_PERCENT), cellsAllocated, cellsAlive, (int) elapsed, (int) estimated );
        printf("r %ld/%ld %.2f%% (ds: %ld) elapsed: %d s  estimated: %d s  ", r, m_k, ((double)percent/DECIMALS_PERCENT), numds , (int) elapsed, (int) estimated );
    
        fflush(stdout);
        lastpercent = percent;
        lastPrintLap = elapsed;
    }
}

/**
 * Returns the value (y) which is the next multiple of (m) higher than (x)
 * 
 * y = m*k, so that y > x
 * 
 * @param value
 * @param multiple
 * @return 
 */
long nextMultiple(long value, long multiple)
{
   return ((value + (multiple-1)) / multiple) * multiple; 
}

long previousMultiple(long value, long multiple)
{
   return ((value - (multiple-1)) / multiple) * multiple; 
}

#define NUMBER_OF_INVOCATIONS_PER_READ 100


long CUDAWavefrontDynamicDiamond2Cols::getDistance()
{
    PerformanceLap lap;
    int lastpercent = -1;
    long cellsAllocated = 0;
    long cellsAlive = 0;
    
    
    m_queue->writeBuffer(m_buf_P, (void*) m_P, m_m);
    m_queue->writeBuffer(m_buf_T, (void*) m_T, m_n);
    
    // this is the initial height of the pyramid
    long h = 2*m_k+1;
    
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long m_top = max2(m_m,m_n); // this is the maximum possible value of the W pyramid cells

    setCommonArgs();

    m_final_d_r[0] = 0;     // furthest reaching point
    m_final_d_r[1] = m_top; // estimated distance (now, worst case)
    m_queue->writeBuffer(m_buf_final_d_r, m_final_d_r, 2 * sizeof(long) );

    for (long r=0; r < m_k; r+= m_tileLen)
    {
        long dup_left = r;      // this is for sure a multiple of tile len
        long ddown_left = -r;   // this is for sure a multiple of tile len
        
        long effk = nextMultiple(m_k, m_tileLen*2) + m_tileLen;
        long eff_fd_up = nextMultiple(final_d, m_tileLen);
        long eff_fd_down = previousMultiple(final_d, m_tileLen);
        
        long dup_right = eff_fd_up + (effk-r);
        long ddown_right = eff_fd_down - (effk-r);
        
        // find the next multiple of tilelen
        if ((dup_right % m_tileLen) != 0) dup_right += m_tileLen - (dup_right % m_tileLen);
        if ((ddown_right % m_tileLen) != 0) ddown_right -= m_tileLen - (ddown_right % m_tileLen);
        
        long dup = min2(dup_left, dup_right);
        long ddown = max2(ddown_left, ddown_right);
        
        long dstart = dup;
        long numds = (dup - ddown)+1;             // number of ds 
        long numwi = (((numds-1)/2)/m_tileLen)+1;   // number of workitems
        
        invokeKernel(r, dstart, numwi);

        progress(lap, r, lastpercent, cellsAllocated, cellsAlive, numds);
        
        if ((r % NUMBER_OF_INVOCATIONS_PER_READ) == 0)
        {
            m_queue->readBuffer(m_buf_final_d_r, m_final_d_r, 2 * sizeof(long));
            
            if (verbose > 1)
            {
                printf("Check to continue\nTop: %ld\n", m_top);
                printf("Fur. reaching point: %ld\n", m_final_d_r[0]);
                printf("Edit distance: %ld\n", m_final_d_r[1]);
            }
            
            if (m_final_d_r[0] >= m_top)
                return m_final_d_r[1];  // return the distance
            else
            {
                // update the maximum error to consider, if it is lower than current one
                //printf("w value: %ld - ed: %ld\n", m_final_d_r[0], m_final_d_r[1]);
                long newk = m_top - (m_final_d_r[0] - m_final_d_r[1]);
                if (newk < m_k)
                    m_k = newk;
            }
        }
    }
    
    lastpercent--;
    progress(lap, m_k, lastpercent, cellsAllocated, cellsAlive, 0);

    m_queue->readBuffer(m_buf_final_d_r, m_final_d_r, 2 * sizeof(long));
            
    if (verbose > 1)
    {
        printf("Check to continue\nTop: %ld\n", m_top);
        printf("Fur. reaching point: %ld\n", m_final_d_r[0]);
        printf("Edit distance: %ld\n", m_final_d_r[1]);
    }

    if (m_final_d_r[0] >= m_top)
        return m_final_d_r[1];

    return m_top;
}
    
void CUDAWavefrontDynamicDiamond2Cols::setCommonArgs()
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
    
    ret = clSetKernelArg(m_kernel, 8, sizeof(cl_int), (void *)&m_tileLen);
    CHECK_CL_ERRORS(ret);
}

void CUDAWavefrontDynamicDiamond2Cols::invokeKernel(long r, long dstart, long numds)
{
    cl_int ret;
    
    ret = clSetKernelArg(m_kernel, 4, sizeof(cl_long), (void *)&r);
    CHECK_CL_ERRORS(ret);
    
    ret = clSetKernelArg(m_kernel, 9, sizeof(cl_long), (void *)&dstart);
    CHECK_CL_ERRORS(ret);

    //long k = max2(m_m,m_n);
    
    m_queue->invokeKernel1D(m_kernel, numds, gWorkgroupSize);
    
    

}

char* CUDAWavefrontDynamicDiamond2Cols::getAlignmentPath(long* distance)
{
    printf("Not implemented yet\n");
    exit(-1);
}
    
const char* CUDAWavefrontDynamicDiamond2Cols::getDescription()
{
    return "Wavefront Dynamic Diamond 2 columns [ocl_local_tiles] in CUDA";
}
