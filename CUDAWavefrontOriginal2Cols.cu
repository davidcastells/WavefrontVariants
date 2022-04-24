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
 * File:   CUDAWavefrontOriginal2Cols.cpp
 * Author: dcr
 * 
 * Created on April 24, 2022, 11:30 AM
 */

#include "CUDAWavefrontOriginal2Cols.h"
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
extern int gTileLen;
extern int gMeasureIterationTime;
extern int gEnqueuedInvocations;
extern int gWorkgroupSize;

__forceinline__ __device__
long extend(const char* P, const char* T, long m, long n, long pi, long ti)
{
    long e = 0;

    while (pi < m && ti < n)
    {
            if (P[pi] != T[ti])
                    return e;
            e++;
            pi++;
            ti++;
    }

    return e;
}

__forceinline__ __device__
int polarExistsInW(long d, long r)
{
    long x = POLAR_W_TO_CARTESIAN_X(d,r);
    long y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
    return ((x >= 0) && (y >= 0));
}

__forceinline__ __device__
void processCell(char* P, 
        char* T, 
        long m_m, 
        long m_n,
        long m_k, 
        long* m_W,
        long* p_final_d_r,
        long d,
        long r,
        int tileLen)
{
    long m_top = max2(m_m,m_n);

    // we already reached the final point in previous invocations
    if (p_final_d_r[0] >= m_top)
        return;
        
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);

    // early exit for useless work items
    if (!polarExistsInW(d,r))
        return;
            
    if (r == 0)
    {
        if (d == 0)
            // initial case
            m_W[POLAR_W_TO_INDEX(d, r)] = extend(P, T, m_m, m_n, 0, 0);
        else
            m_W[POLAR_W_TO_INDEX(d, r)]  = 0;
            
        // printf("W[d:%ld,r:%ld]=%ld\n", d, r, m_W[POLAR_W_TO_INDEX(d, r)]);
    }
    else
    {
        long diag_up = (polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1)]  : 0;
        long left = (polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1)]  : 0;
        long diag_down = (polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1)]  : 0;

        long compute;

        if (d == 0)
            compute = max3(diag_up, left+1, diag_down);
        else if (d > 0)
            compute = max3(diag_up, left+1, diag_down+1);
        else
            compute = max3(diag_up+1, left+1, diag_down);

        if ((d == final_d) && compute >= m_top)
        {
            m_W[POLAR_W_TO_INDEX(d, r)] = compute;
            p_final_d_r[0] = compute;   // furthest reaching point
            p_final_d_r[1] = r;         // at edit distance = r
            
            // printf("W[d:%ld,r:%ld]=%ld\n", d, r, compute);
            return;
        }

        long ex = POLAR_W_TO_CARTESIAN_X(d, compute);
        long ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

        if ((ex < m_n) && (ey < m_m))
        {
            long extendv = extend(P, T, m_m, m_n, ey, ex);
            long extended = compute + extendv;

            m_W[POLAR_W_TO_INDEX(d, r)] = extended;

            if ((d == final_d) && extended >= m_top)
            {
                p_final_d_r[0] = extended;  // furthest reaching point
                p_final_d_r[1] = r;         // at edit distance = r

                //printf("W[d:%ld,r:%ld]=%ld\n", d, r, extended);
                return;
            }
        }
        else
        {
            m_W[POLAR_W_TO_INDEX(d, r)] = compute;
            // it is impossible to assign the final result here, because it would
            // have been in the previous compute check
        }
    }

}

__global__
void wfo2cols(
        char* P, 
        char* T, 
        long m_m, 
        long m_n, 
        long r0, 
        long m_k,  
        long* m_W,
        long* p_final_d_r,
        int tileLen)
{
    //size_t gid = get_global_id(0);
    size_t gid = blockIdx.x * blockDim.x + threadIdx.x;
    
    
    //long d = gid - (r-1);
    long d0 = r0 - gid*2*tileLen; 
    long m_top = max2(m_m,m_n);

    // printf("\n[POCL] d0=%ld r0=%ld  cv=%ld\n", d0, r0, p_final_d_r[0]);

    
    // we already reached the final point in previous invocations
    if (p_final_d_r[0] >= m_top)
        return;
        
    // Increase
    for (int i=0 ; i < tileLen; i++)
        for (int j=-i; j <= i; j++)
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+i, tileLen);
    
    // Decrease
    for (int i=0 ; i < tileLen; i++)
    {
        int ii = tileLen - 1 -i;
        
        for (int j=-ii; j <= ii; j++)
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+tileLen+i, tileLen);
    }
}

CUDAWavefrontOriginal2Cols::CUDAWavefrontOriginal2Cols()
{
    m_buf_P = NULL;
    m_buf_T = NULL;
    m_buf_W = NULL;
    
    m_W = NULL;
}


CUDAWavefrontOriginal2Cols::~CUDAWavefrontOriginal2Cols()
{
    if (m_W != NULL)
        delete [] m_W;
}

void CUDAWavefrontOriginal2Cols::setInput(const char* P, const char* T, long k)
{
    // this should not be allocated, we only expect a single call
    assert(m_W == NULL);
    
    m_m = strlen(P);
    m_n = strlen(T);
    m_k = k;
    m_tileLen = gTileLen;

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

    m_P = P;
    m_T = T;

    cudaMalloc(&m_buf_P,  m_m * sizeof(char));
    cudaMalloc(&m_buf_T, m_n * sizeof(char));

    printf("creating buffer %.2f GB\n", size*sizeof(long)/(1E9));

    cudaMalloc(&m_buf_W, size * sizeof(long));
    cudaMalloc(&m_buf_final_d_r, 2 * sizeof(long));
    
    
    printf("input set\n");
}

void CUDAWavefrontOriginal2Cols::progress(PerformanceLap& lap, long r, int& lastpercent, long cellsAllocated, long cellsAlive)
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

/*
static long nextMultiple(long value, long multiple)
{
   return ((value + (multiple-1)) / multiple) * multiple; 
}

static long previousMultiple(long value, long multiple)
{
   return ((value - (multiple-1)) / multiple) * multiple; 
}
*/

#define NUMBER_OF_INVOCATIONS_PER_READ gEnqueuedInvocations

long CUDAWavefrontOriginal2Cols::getDistance()
{
    PerformanceLap lap;
    int lastpercent = -1;
    long cellsAllocated = 0;
    long cellsAlive = 0;
        
    cudaMemcpy(m_buf_P, m_P, m_m * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemcpy(m_buf_T, m_T, m_n * sizeof(char), cudaMemcpyHostToDevice);
    
    long h = 2*m_k+1;
    
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long m_top = max2(m_m,m_n);

    setCommonArgs();

    m_final_d_r[0] = 0;     // furthest reaching point
    m_final_d_r[1] = m_top; // estimated distance (now, worst case)
    
    cudaMemcpy(m_buf_final_d_r, m_final_d_r, 2 * sizeof(long), cudaMemcpyHostToDevice );
    

    if (gMeasureIterationTime)
        printf("r,time\n");
        
    for (long r=0; r < m_k; r+= m_tileLen)
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
        
        if ((r % NUMBER_OF_INVOCATIONS_PER_READ) == 0)
        {
            cudaMemcpy(m_final_d_r, m_buf_final_d_r, 2 * sizeof(long), cudaMemcpyDeviceToHost);
            
            if (m_final_d_r[0] >= m_top)
                return m_final_d_r[1];
        }
    }
    
    lastpercent--;
    progress(lap, m_k, lastpercent, cellsAllocated, cellsAlive);

    return m_top;
}
    
void CUDAWavefrontOriginal2Cols::setCommonArgs()
{
}

void CUDAWavefrontOriginal2Cols::invokeKernel(long r)
{
    long k = max2(m_m,m_n);
    
    long threads = (r/m_tileLen)+1;
    
    long blocks = (threads + gWorkgroupSize - 1) / gWorkgroupSize;
    long threadsPerBlock = gWorkgroupSize;
     
    wfo2cols<<<blocks, threadsPerBlock>>>(m_buf_P, m_buf_T, m_m, m_n, r, k, m_buf_W, m_buf_final_d_r, m_tileLen);

}

char* CUDAWavefrontOriginal2Cols::getAlignmentPath(long* distance)
{
    printf("Not implemented yet\n");
    exit(-1);
}
    
const char* CUDAWavefrontOriginal2Cols::getDescription()
{
    return "Wavefront Original 2 columns [ocl_tiles] in OpenCL";
}
