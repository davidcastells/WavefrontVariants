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

// @todo By now, we use statically compiled CUDA code, we should try the NVRTC API to run-time compile the kernel as we do with OpenCL

#define TILE_LEN        3
//#define WORKGROUP_SIZE  120
#define WORKGROUP_SIZE  32

#define SHARED_STORE


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


//#define POLAR_W_TO_INDEX(d, r)		((d)+m_k + (((r)%(2*tileLen)) * (2*m_k+1)))
#define POLAR_W_TO_INDEX(d, r)		((d)+m_k + (((r)%2) * (2*m_k+1)))
//#define INDEX_TO_POLAR_W_D(idx, r)      ((idx) - (m_k) - ((r)%2)*(2*m_k+1))
//#define INDEX_TO_POLAR_W_D(idx, r)      ((idx) - (m_k) - ((r)%(2*tileLen))*(2*m_k+1))


#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))


#define POLAR_LOCAL_W_TO_INDEX(d, r, tl) ((r) >= (tl))? (2*((r)-(tl))*(tl)) - ((r)-(tl))*((r)-(tl)) + (2*(tl)-(r)-1) + (d) + (tl)*(tl) : ((r)*(r)+(r)+d)

// WARNING !!!! This is harcoded here, where should it be ?
// WARNING !!!! This is harcoded here, where should it be ?
// WARNING !!!! This is harcoded here, where should it be ?
// WARNING !!!! This is harcoded here, where should it be ?
#define SHARED_STORE

#ifdef GLOBAL_STORE
    #define LOCAL_TILE_TYPE     __private
#endif

#ifdef REGISTER_STORE
    // we define a tile with len 3
    typedef struct
    {
        long r0_d0;
        long r1_dm1;
        long r1_d0;
        long r1_d1;
        long r2_dm2;
        long r2_dm1;
        long r2_d0;
        long r2_d1;
        long r2_d2;
        long r3_dm2;
        long r3_dm1;
        long r3_d0;
        long r3_d1;
        long r3_d2;
        long r4_dm1;
        long r4_d0;
        long r4_d1;
        long r5_d0;
    } LOCAL_TILE;
    #define LOCAL_TILE_PTR      LOCAL_TILE*
#endif

#ifdef SHARED_STORE
    #define LOCAL_TILE_TYPE     __shared__
    #define LOCAL_TILE_PTR      INT_TYPE*
#endif


extern int verbose;
extern int gPid;
extern int gDid;
extern int gTileLen;
extern int gMeasureIterationTime;
extern int gEnqueuedInvocations;
extern int gWorkgroupSize;

__forceinline__ __device__
INT_TYPE extend(const char* P, const char* T, INT_TYPE m, INT_TYPE n, INT_TYPE pi, INT_TYPE ti)
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
int polarExistsInW(INT_TYPE d, INT_TYPE r)
{
    int ret =  abs(d) <= r;
    //printf("polar exist in W (%ld, %ld) = %d\n", d, r, ret);
    return ret;
}

__forceinline__ __device__ 
int isInLocalBlock(int ld, int lr)
{
    if (lr < 0) return 0;
    if (lr >= 2*TILE_LEN) return 0;
    
    INT_TYPE dec_lr = 2*TILE_LEN -1 - lr;
    INT_TYPE min_lr = min(lr, dec_lr);
    
    if (abs(ld) > min_lr) return 0;
    
    return 1;
}


__forceinline__ __device__
int isInLocalBlockBoundary(int ld, int lr, int tileLen)
{
    int col = lr + abs(ld);
    const int col1 = 2*TILE_LEN - 1;
    const int col2 = col1-1;
    
    return ((col == col1) || (col == col2));
}

#define WRITE_W(d,r, v)         writeToW(m_W, localW, (d), (r), (v), m_k, tileLen, ld, lr)
#define READ_W(d,r, ld, lr)     (readFromW(m_W, localW, (d), (r), m_k, tileLen, ld, lr))

#ifdef REGISTER_STORE
    void inline __attribute__((always_inline)) 
    writeLocalW(LOCAL_TILE_PTR localW, INT_TYPE d, INT_TYPE r, INT_TYPE v)
    {
        //printf("local_WR(%ld,%ld) = %ld\n", d, r, v);
        // max 8
        switch ((r<<3)|(d+2))
        {
            case (0<<3)|(0+2):  localW->r0_d0 = v; break;
            case (1<<3)|(-1+2): localW->r1_dm1 = v; break;
            case (1<<3)|(0+2):  localW->r1_d0 = v; break;
            case (1<<3)|(1+2):  localW->r1_d1 = v; break;
            case (2<<3)|(-2+2): localW->r2_dm2 = v; break;
            case (2<<3)|(-1+2): localW->r2_dm1 = v; break;
            case (2<<3)|(0+2):  localW->r2_d0 = v; break;
            case (2<<3)|(1+2):  localW->r2_d1 = v; break;
            case (2<<3)|(2+2):  localW->r2_d2 = v; break;
            case (3<<3)|(-2+2): localW->r3_dm2 = v; break;
            case (3<<3)|(-1+2): localW->r3_dm1 = v; break;
            case (3<<3)|(0+2):  localW->r3_d0 = v; break;
            case (3<<3)|(1+2):  localW->r3_d1 = v; break;
            case (3<<3)|(2+2):  localW->r3_d2 = v; break;
            case (4<<3)|(-1+2): localW->r4_dm1 = v; break;
            case (4<<3)|(0+2):  localW->r4_d0 = v; break;
            case (4<<3)|(1+2):  localW->r4_d1 = v; break;
            case (5<<3)|(0+2):  localW->r5_d0 = v; break;
            default:
                printf("WRITE ERROR r:%d d:%d\n", r, d);

        }
        
        //printf("localW.r0_d0 = %ld\n", localW->r0_d0 );

    }

    void inline __attribute__((always_inline)) 
    writeToW(INT_TYPE* m_W, LOCAL_TILE_PTR localW, INT_TYPE d, INT_TYPE r, INT_TYPE v, INT_TYPE m_k, int tileLen, int ld, int lr)
    {    
        writeLocalW(localW, ld, lr, v);
        //int lidx = POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen);
        //localW[lidx] = v;

        int inBoundary = isInLocalBlockBoundary(ld, lr);
        
    #ifdef DEBUG
        printf("WR(%ld, %ld, %d) -> local WR(%d, %d, %d) -> WR  = %ld (in boundary: %d)\n", 
                d, r, tileLen, 
                ld, lr, tileLen, v, inBoundary);
    #endif
        
        if (inBoundary)
        {
            m_W[POLAR_W_TO_INDEX(d, r)] = v;
        }
    }
#else
    __forceinline__ __device__
    INT_TYPE readFromW(INT_TYPE* m_W, LOCAL_TILE_PTR localW, INT_TYPE d, INT_TYPE r, INT_TYPE m_k, int tileLen, int ld, int lr)
    {
        int isInLocal = isInLocalBlock(ld, lr);
        
        if (isInLocal)
        {
            int lidx = POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen);
            INT_TYPE lv = localW[lidx];

    #ifdef DEBUG
            printf("RD(%ld, %ld, %d) -> local RD(%d, %d, %d) -> RD idx(%d) = %ld\n", d, r, tileLen, ld, lr, tileLen, lidx, lv);
    #endif
        
            return lv;
        }
        else
        {
            INT_TYPE gv = m_W[POLAR_W_TO_INDEX(d, r)];
            
    #ifdef DEBUG
            printf("RD(%ld, %ld, %d) ->  = %ld\n", d, r, tileLen, gv);
    #endif    
            return gv;
        }
    }
#endif


#ifdef REGISTER_STORE
    INT_TYPE inline __attribute__((always_inline)) 
    readLocalW(LOCAL_TILE_PTR localW, INT_TYPE d, INT_TYPE r)
    {
        INT_TYPE v; 
        
        // max 8
        switch ((r<<3)|(d+2))
        {
            case (0<<3)|(0+2):  v = localW->r0_d0; break;
            case (1<<3)|(-1+2): v = localW->r1_dm1; break;
            case (1<<3)|(0+2):  v = localW->r1_d0; break;
            case (1<<3)|(1+2):  v = localW->r1_d1; break;
            case (2<<3)|(-2+2): v = localW->r2_dm2; break;
            case (2<<3)|(-1+2): v = localW->r2_dm1; break;
            case (2<<3)|(0+2):  v = localW->r2_d0; break;
            case (2<<3)|(1+2):  v = localW->r2_d1; break;
            case (2<<3)|(2+2):  v = localW->r2_d2; break;
            case (3<<3)|(-2+2): v = localW->r3_dm2; break;
            case (3<<3)|(-1+2): v = localW->r3_dm1; break;
            case (3<<3)|(0+2):  v = localW->r3_d0; break;
            case (3<<3)|(1+2):  v = localW->r3_d1; break;
            case (3<<3)|(2+2):  v = localW->r3_d2; break;
            case (4<<3)|(-1+2): v = localW->r4_dm1; break;
            case (4<<3)|(0+2):  v = localW->r4_d0; break;
            case (4<<3)|(1+2):  v = localW->r4_d1; break;
            case (5<<3)|(0+2):  v = localW->r5_d0; break;
            default:
                printf("READ ERROR r:%d d:%d\n", r, d);
        }
        

        //printf("local_RD(%ld,%ld) = %ld\n", d, r, v);
        return v;
    }
    
    INT_TYPE inline __attribute__((always_inline)) 
    readFromW(INT_TYPE* m_W, LOCAL_TILE_PTR localW, INT_TYPE d, INT_TYPE r, INT_TYPE m_k, int tileLen, int ld, int lr)
    {
        int isInLocal = isInLocalBlock(ld, lr);
        
        if (isInLocal)
        {
            INT_TYPE lv = readLocalW(localW, ld, lr);
            

    #ifdef DEBUG
        
            printf("RD(%ld, %ld, %d) -> local RD(%d, %d, %d) -> RD idx(%d) = %ld\n", 
                d, r, tileLen, 
                ld, lr, tileLen, lidx, lv);
    #endif
        
            return lv;
        }
        else
        {
            INT_TYPE gv = m_W[POLAR_W_TO_INDEX(d, r)];
            
    #ifdef DEBUG
            printf("RD(%ld, %ld, %d) ->  = %ld\n", 
                d, r, tileLen, gv);
    #endif    
            return gv;
        }
    }
#else
    __forceinline__ __device__
    void writeToW(INT_TYPE* m_W, INT_TYPE* localW, INT_TYPE d, INT_TYPE r, INT_TYPE v, INT_TYPE m_k, int tileLen, int ld, int lr)
    {    
        int lidx = POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen);

        localW[lidx] = v;

        int inBoundary = isInLocalBlockBoundary(ld, lr, tileLen);
        
    #ifdef DEBUG
        printf("WR(%ld, %ld, %d) -> local WR(%d, %d, %d) -> WR idx(%d) = %ld (in boundary: %d)\n", 
                d, r, tileLen, 
                ld, lr, tileLen, lidx, v, inBoundary);
    #endif
        
        if (inBoundary)
        {
            m_W[POLAR_W_TO_INDEX(d, r)] = v;
        }
    }
#endif


__forceinline__ __device__
void processCell(char* P, 
        char* T, 
        INT_TYPE m_m, 
        INT_TYPE m_n,
        INT_TYPE m_k, 
        INT_TYPE* m_W,
        INT_TYPE* p_final_d_r,
        INT_TYPE d,
        INT_TYPE r,
        int tileLen,
        LOCAL_TILE_PTR localW,
        int ld,
        int lr,
        int* doRun)
{
    INT_TYPE m_top = max2(m_m,m_n);

    // we already reached the final point in previous invocations
    if (p_final_d_r[0] >= m_top)
    {
        *doRun = 0;
        return;
    }
       
    INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);

    // early exit for useless work items
    if (!polarExistsInW(d,r))
        return;
            
    if (r == 0)
    {
        if (d == 0)
        {
            // initial case
            INT_TYPE extended = extend(P, T, m_m, m_n, 0, 0);
            WRITE_W(d, r, extended);
            
            if ((d == final_d) && extended >= m_top)
            {
                //printf("COMPLETE\n");
                p_final_d_r[0] = extended;  // furthest reaching point
                p_final_d_r[1] = r;         // at edit distance = r
                *doRun = 0;
                return;
            }
        }
        else
            WRITE_W(d, r, 0);
    }
    else
    {
        INT_TYPE diag_up = (polarExistsInW(d+1, r-1))? READ_W(d+1, r-1, ld+1, lr-1)  : 0;   // m_W[POLAR_W_TO_INDEX(d+1, r-1)] 
        INT_TYPE left = (polarExistsInW(d,r-1))? READ_W(d, r-1, ld, lr-1) : 0;              // m_W[POLAR_W_TO_INDEX(d, r-1)]
        INT_TYPE diag_down = (polarExistsInW(d-1,r-1))? READ_W(d-1, r-1, ld-1, lr-1) : 0;   // m_W[POLAR_W_TO_INDEX(d-1, r-1)]  

        //printf("u|l|r = %ld|%ld|%ld\n",  diag_up, left, diag_down);

        INT_TYPE compute;

        if (d == 0)
            compute = max3(diag_up, left+1, diag_down);
        else if (d > 0)
            compute = max3(diag_up, left+1, diag_down+1);
        else
            compute = max3(diag_up+1, left+1, diag_down);

        if ((d == final_d) && compute >= m_top)
        {
            // m_W[POLAR_W_TO_INDEX(d, r)] = compute;
            WRITE_W(d,r, compute);
            p_final_d_r[0] = compute;   // furthest reaching point
            p_final_d_r[1] = r;         // at edit distance = r
            
            // printf("W[d:%ld,r:%ld]=%ld\n", d, r, compute);
            *doRun = 0;
            return;
        }

        INT_TYPE ex = POLAR_W_TO_CARTESIAN_X(d, compute);
        INT_TYPE ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

        if ((ex < m_n) && (ey < m_m))
        {
            INT_TYPE extendv = extend(P, T, m_m, m_n, ey, ex);
            INT_TYPE extended = compute + extendv;

            // m_W[POLAR_W_TO_INDEX(d, r)] = extended;
            WRITE_W(d, r, extended);

            if ((d == final_d) && extended >= m_top)
            {
                p_final_d_r[0] = extended;  // furthest reaching point
                p_final_d_r[1] = r;         // at edit distance = r

                //printf("W[d:%ld,r:%ld]=%ld\n", d, r, extended);
                *doRun = 0;
                return;
            }
        }
        else
        {
            //m_W[POLAR_W_TO_INDEX(d, r)] = compute;
            WRITE_W(d, r, compute);
            // it is impossible to assign the final result here, because it would
            // have been in the previous compute check
        }
    }

}

__global__
void wfo2cols(
        char* P, 
        char* T, 
        INT_TYPE m_m, 
        INT_TYPE m_n, 
        INT_TYPE r0, 
        INT_TYPE m_k,  
        INT_TYPE* m_W,
        INT_TYPE* p_final_d_r,
        int tileLen)
{
#ifdef GLOBAL_STORE
    LOCAL_TILE_TYPE INT_TYPE localW[2*TILE_LEN*TILE_LEN];    
#endif

#ifdef REGISTER_STORE
    LOCAL_TILE localW;
#endif
    
#ifdef SHARED_STORE
    LOCAL_TILE_TYPE INT_TYPE shared_localW[WORKGROUP_SIZE][2*TILE_LEN*TILE_LEN];
    //size_t lid = get_local_id(0);
    size_t lid = threadIdx.x;
    LOCAL_TILE_PTR localW = &shared_localW[lid][0];
#endif

    //size_t gid = get_global_id(0);
    size_t gid = blockIdx.x * blockDim.x + threadIdx.x;
    
    
    INT_TYPE d0 = r0 - gid*2*tileLen; 
    INT_TYPE m_top = max2(m_m,m_n);

    // printf("\n[POCL] d0=%ld r0=%ld  cv=%ld\n", d0, r0, p_final_d_r[0]);

    int doRun = 1;
    
    if (abs(d0) > r0)
        doRun = 0;
    
    if (!doRun)
        return;
        
    // we already reached the final point in previous invocations
    if (p_final_d_r[0] >= m_top)
        return;
        
    // Increase
    for (int i=0 ; i < tileLen; i++)
        for (int j=-i; ((j <= i)  && (doRun)); j++)
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+i, tileLen, localW, j, i, &doRun);
    
    // Decrease
    for (int i=0 ; i < tileLen; i++)
    {
        int ii = tileLen - 1 -i;
        
        for (int j=-ii; ((j <= ii)  && (doRun)); j++)
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+tileLen+i, tileLen, localW, j, tileLen+i, &doRun);
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

void CUDAWavefrontOriginal2Cols::setInput(const char* P, const char* T, INT_TYPE k)
{
    // this should not be allocated, we only expect a single call
    assert(m_W == NULL);
    
    m_m = strlen(P);
    m_n = strlen(T);
    m_k = k;
    m_tileLen = gTileLen;
    
    if (gTileLen != TILE_LEN)
    {
        printf("Tile length statically compiled to %d\n", TILE_LEN);
        exit(-1);
    }

    long size = (2)*(2*k+1);

    try
    {
        m_W = new INT_TYPE[size];
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

    printf("creating buffer %.2f GB\n", size*sizeof(INT_TYPE)/(1E9));

    cudaMalloc(&m_buf_W, size * sizeof(INT_TYPE));
    cudaMalloc(&m_buf_final_d_r, 2 * sizeof(INT_TYPE));
    
    
    printf("input set\n");
}

void CUDAWavefrontOriginal2Cols::progress(PerformanceLap& lap, INT_TYPE r, int& lastpercent, long cellsAllocated, long cellsAlive)
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

INT_TYPE CUDAWavefrontOriginal2Cols::getDistance()
{
    PerformanceLap lap;
    int lastpercent = -1;
    long cellsAllocated = 0;
    long cellsAlive = 0;
        
    cudaMemcpy(m_buf_P, m_P, m_m * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemcpy(m_buf_T, m_T, m_n * sizeof(char), cudaMemcpyHostToDevice);
    
    INT_TYPE h = 2*m_k+1;
    
    INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    INT_TYPE m_top = max2(m_m,m_n);

    setCommonArgs();

    m_final_d_r[0] = 0;     // furthest reaching point
    m_final_d_r[1] = m_top; // estimated distance (now, worst case)
    
    cudaMemcpy(m_buf_final_d_r, m_final_d_r, 2 * sizeof(INT_TYPE), cudaMemcpyHostToDevice );
    

    if (gMeasureIterationTime)
        printf("r,time\n");
        
    for (INT_TYPE r=0; r < m_k; r+= m_tileLen)
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
            cudaMemcpy(m_final_d_r, m_buf_final_d_r, 2 * sizeof(INT_TYPE), cudaMemcpyDeviceToHost);
            
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

void CUDAWavefrontOriginal2Cols::invokeKernel(INT_TYPE r)
{
    INT_TYPE k = max2(m_m,m_n);
    
    INT_TYPE threads = (r/m_tileLen)+1;
    
    INT_TYPE blocks = (threads + gWorkgroupSize - 1) / gWorkgroupSize;
    INT_TYPE threadsPerBlock = gWorkgroupSize;
     
    wfo2cols<<<blocks, threadsPerBlock>>>(m_buf_P, m_buf_T, m_m, m_n, r, k, m_buf_W, m_buf_final_d_r, m_tileLen);

}

char* CUDAWavefrontOriginal2Cols::getAlignmentPath(INT_TYPE* distance)
{
    printf("Not implemented yet\n");
    exit(-1);
}
    
const char* CUDAWavefrontOriginal2Cols::getDescription()
{
    return "Wavefront Original 2 columns [ocl_local_tiles] in CUDA";
}
