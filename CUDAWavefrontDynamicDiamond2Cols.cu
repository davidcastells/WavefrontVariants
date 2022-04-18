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

/////////////////////////////////////////////////////////////////////////////////////////////////


#define POLAR_LOCAL_W_TO_INDEX(d, r, tl) ((r) >= (tl))? (2*((r)-(tl))*(tl)) - ((r)-(tl))*((r)-(tl)) + (2*(tl)-(r)-1) + (d) + (tl)*(tl) : ((r)*(r)+(r)+d)

#define max2(a,b) (((a)>(b))?(a):(b))
#define max3(a,b,c) max2(a, max2(b, c))

#define LOCAL_STORE     
#define GLOBAL_STORE    

/**
 * 
 * @param ld local diagonal coordinate
 * @param lr local radius coordinate
 * @param tileLen
 * @return 
 */
__forceinline__ __device__ 
int isInLocalBlock(int ld, int lr, int tileLen)
{
    if (lr < 0)
        return 0;
    
    // first check that there are in bounds
    if (lr >= tileLen)
    {
        // decreasing tile 
        int dec_lr = 2*tileLen - lr - 1;
        
        if (abs(ld) > dec_lr)
            return 0;
    }
    else
    {
        if (abs(ld) > lr)
            return 0;
    }
    
    // return whether the cell is in the local memory 
    int idx = POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen);
    return ((idx >= 0) && (idx <= 2*tileLen*tileLen));
}

__forceinline__ __device__
int isInLocalBlockBoundary(int ld, int lr, int tileLen)
{
    int maxd = 2*tileLen-lr-1;
    if (abs(ld) == maxd)
        return 1;
    if ((ld >= 0) && (abs(ld+1) == maxd))
        return 1;
    if ((ld <= 0) && (abs(ld-1) == maxd))
        return 1;
    
    return 0;
}

__forceinline__ __device__
long extendUnaligned(GLOBAL_STORE const char* P, 
            GLOBAL_STORE const char* T, long m, long n, long pi, long ti)
{
    long e = 0;

    while (pi < m && ti < n)
    {
	char pc = __ldg(&P[pi]);
	char tc = __ldg(&T[ti]);

            if (pc != tc)
                    return e;
            e++;
            pi++;
            ti++;
    }

    return e;
}

#define ctz(x) (__ffsll(x) - 1)

#define ALIGN_MASK 0xFFFFFFFFFFFFFFF8
 
__forceinline__ __device__
long extendAligned(GLOBAL_STORE const char* P, 
        GLOBAL_STORE const char* T, long m, long n, long pi, long ti)
{
    int pbv; // P valid bytes
    int tbv; // T valid bytes
    
    long pai;
    long tai;
    
    int pbidx;
    int tbidx;
    
    long PV;    // P value
    long TV;    // T value

    int mbv;
    unsigned long mask;
    int neq;
    
    long e = 0;
    
    //long gt = extend(P, T, m, n, pi, ti);
    
loop:
    pai = pi & ALIGN_MASK;
    tai = ti & ALIGN_MASK;
    
    PV = *(GLOBAL_STORE long*)(&P[pai]);
    TV = *(GLOBAL_STORE long*)(&T[tai]);
    
//    printf("pi: %ld ti: %ld pai: %ld tai: %ld \n", pi, ti, pai, tai);
//    printf("PV = 0x%016lX\n", PV);
//    printf("TV = 0x%016lX\n", TV);
    
    pbidx = (pi%8);
    tbidx = (ti%8);
    
    pbv = 8 - pbidx;
    tbv = 8 - tbidx;
    
    if (pbv > (m-pi)) pbv = m-pi;
    if (tbv > (n-ti)) tbv = n-pi;

    mbv = min2(pbv, tbv);    // minimum valid bytes
    
    //assert(mbv);
    if (mbv > 0)
    {
//        mask = (-1L);
//        mask <<= (mbv*8);
//        if (mbv == 8) mask = 0;

//        printf("%d -> %016lX\n", mbv, mask);
    //    
        switch (mbv)
        {
            case 1: mask = 0xFFFFFFFFFFFFFF00; break;
            case 2: mask = 0xFFFFFFFFFFFF0000; break;
            case 3: mask = 0xFFFFFFFFFF000000; break;
            case 4: mask = 0xFFFFFFFF00000000; break;
            case 5: mask = 0xFFFFFF0000000000; break;
            case 6: mask = 0xFFFF000000000000; break;
            case 7: mask = 0xFF00000000000000; break;
            case 8: mask = 0x0000000000000000; break;
        }

//        printf("pbv = %d\n", pbv);
//        printf("tbv = %d\n", tbv);

        PV = PV >> (pbidx*8);
        TV = TV >> (tbidx*8);

//        printf("PV = 0x%016lX\n", PV);
//        printf("TV = 0x%016lX\n", TV);
//        printf("MK = 0x%016lX\n", mask);

        neq = ctz((PV ^ TV) | mask) / 8;

//        printf("neq = %d\n", neq);

        e += neq;

        if (neq == mbv)
        {
            ti += neq;
            pi += neq;
            goto loop;
        }
    }
    
//    if (gStats) collectExtendStats(e);

//    printf("e = %ld\n", e);
    
//    if (e != gt)
//    {
//        printf("ERROR at pi: %ld ti: %ld\n", pi, ti);
//        exit(0);
//    }
    return e;
}



__forceinline__ __device__
long cuda_extend(GLOBAL_STORE const char* P,
        GLOBAL_STORE const char* T, long m, long n, long pi, long ti)
{    
#ifdef EXTEND_ALIGNED
        return extendAligned(P, T, m, n, pi, ti);
#else
        return extendUnaligned(P, T, m, n, pi, ti);
#endif
}

__forceinline__ __device__
int cuda_polarExistsInW(long d, long r)
{
    long x = POLAR_W_TO_CARTESIAN_X(d,r);
    long y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
    return ((x >= 0) && (y >= 0));
}


#define WRITE_W(d,r, v) writeToW(m_W, localW, (d), (r), (v), m_k, tileLen, ld, lr)
#define READ_W(d,r, ld, lr)     (readFromW(m_W, localW, (d), (r), m_k, tileLen, ld, lr))


__forceinline__ __device__
void writeToW(GLOBAL_STORE long* m_W,
        LOCAL_STORE long* localW, long d, long r, long v, long m_k, int tileLen, int ld, int lr)
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

__forceinline__ __device__
long readFromW(GLOBAL_STORE long* m_W,
        LOCAL_STORE long* localW, long d, long r, long m_k, int tileLen, int ld, int lr)
{
    int isInLocal = isInLocalBlock(ld, lr, tileLen);
    
    if (isInLocal)
    {
        int lidx = POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen);
        long lv = localW[lidx];

#ifdef DEBUG
    
        printf("RD(%ld, %ld, %d) -> local RD(%d, %d, %d) -> RD idx(%d) = %ld\n", 
            d, r, tileLen, 
            ld, lr, tileLen, lidx, lv);
#endif
    
        return lv;
    }
    else
    {
        long gv = m_W[POLAR_W_TO_INDEX(d, r)];
        
#ifdef DEBUG
        printf("RD(%ld, %ld, %d) ->  = %ld\n", 
            d, r, tileLen, gv);
#endif    
        return gv;
    }
}

__forceinline__ __device__
void processCell(GLOBAL_STORE char* P, 
        GLOBAL_STORE char* T, 
        long m_m, 
        long m_n,
        long m_k, 
        GLOBAL_STORE long* m_W,
        GLOBAL_STORE long* p_final_d_r,
        long d,
        long r,
        int tileLen,
        LOCAL_STORE long* localW,
        int ld,
        int lr,
        int* doRun)
{
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long m_top = max2(m_m,m_n);

    // early exit for useless work items
    if (!cuda_polarExistsInW(d,r))
        return;
            
    if (r == 0)
    {
        if (d == 0)
            // initial case
            WRITE_W(d, r, cuda_extend(P, T, m_m, m_n, 0, 0));
        else
            WRITE_W(d, r, 0);
    }
    else
    {
        long diag_up = (cuda_polarExistsInW(d+1, r-1))? READ_W(d+1, r-1, ld+1, lr-1) : 0;
        long left = (cuda_polarExistsInW(d,r-1))? READ_W(d, r-1, ld, lr-1) : 0;
        long diag_down = (cuda_polarExistsInW(d-1,r-1))? READ_W(d-1, r-1, ld-1, lr-1) : 0;

        long compute;

        if (d == 0)
            compute = max3(diag_up, left+1, diag_down);
        else if (d > 0)
            compute = max3(diag_up, left+1, diag_down+1);
        else
            compute = max3(diag_up+1, left+1, diag_down);

        if (d == final_d) 
        {
//            printf("COMPLETE\n");
            p_final_d_r[0] = compute;   // furthest reaching point
            p_final_d_r[1] = r;         // at edit distance = r
            
            if (compute >= m_top)
            {
                WRITE_W(d, r, compute); 
                *doRun = 0;
                return;
            }
        }

        long ex = POLAR_W_TO_CARTESIAN_X(d, compute);
        long ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

        if ((ex < m_n) && (ey < m_m))
        {
            long extendv = cuda_extend(P, T, m_m, m_n, ey, ex);
            long extended = compute + extendv;

            WRITE_W(d, r, extended);

            if (d == final_d) 
            {
//                printf("COMPLETE\n");
                p_final_d_r[0] = extended;  // furthest reaching point
                p_final_d_r[1] = r;         // at edit distance = r
                
                if (extended >= m_top)
                {   
                    *doRun = 0;
                    return;
                }
            }
        }
        else
        {
            WRITE_W(d, r, compute);
            // it is impossible to assign the final result here, because it would
            // have been in the previous compute check
        }
    }

}

#define TILE_LEN_MAX 3

/**
 * This is the initial kernel version.
 * The host will create as many work items as the height of the column W
 * most of them will die soon with nothing useful to do
 * 
 * @param P the pattern
 * @param T the text
 * @param m_m the length of the pattern
 * @param m_n the length of the text
 * @param r0 initial radius of the W column to compute
 * @param m_k max number of errors we are going to cover (width of the W pyramid)
 * @param m_W memory for the 2 columns of the W pyramid
 * @param pointer to 2 values (furthest reaching radius, edit distance of the previous value)
 * @param tileLen the length of the tile. The tile will contain 2 * (n)^2, where n is the 
 *                number of columns, n > 1.
 */
__global__ 
void wfdd2cols(
        GLOBAL_STORE char* __restrict__ P, 
        GLOBAL_STORE char* __restrict__ T, 
        long m_m, 
        long m_n, 
        long r0, 
        long m_k,  
        GLOBAL_STORE long* __restrict__ m_W,
        GLOBAL_STORE long* __restrict__ p_final_d_r,
        int tileLen,
        long dstart)
{
    __shared__ long localW[2*TILE_LEN_MAX*TILE_LEN_MAX];

    size_t gid = blockIdx.x; // get_global_id(0);

    //long d = gid - (r-1);
    long d0 = dstart - gid*2*tileLen; 
    long m_top = max2(m_m,m_n);
    //long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    int doRun = 1;
    
    if (abs(d0) > r0)
        doRun = 0;

#ifdef DEBUG
    printf("\ngid: %ld - final_d: %ld run: %d r: %ld dstart: %ld d0: %ld\n", gid, final_d, doRun, r0, dstart, d0);
#endif
    // printf("\n[POCL] d0=%ld r0=%ld  cv=%ld\n", d0, r0, p_final_d_r[0]);
    
    if (!doRun)
        return;
    
    // we already reached the final point in previous invocations
    if (p_final_d_r[0] >= m_top)
        return;
        
    // Increase
    for (int i=0 ; ((i < tileLen) && (doRun)); i++)
        for (int j=-i; ((j <= i) && (doRun)); j++)
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+i, tileLen, localW, j, i, &doRun);
    
    // Decrease
    for (int i=0 ; ((i < tileLen) && (doRun)); i++)
    {
        int ii = tileLen - 1 -i;
        
        for (int j=-ii; ((j <= ii) && (doRun)); j++)
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+tileLen+i, tileLen, localW, j, tileLen+i, &doRun);
    }
}

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

    cudaMalloc(&m_buf_P,  m_m * sizeof(char));
    cudaMalloc(&m_buf_T, m_n * sizeof(char));

    printf("creating buffer %.2f GB\n", size*sizeof(long)/(1E9));

    cudaMalloc(&m_buf_W, size * sizeof(long));
    cudaMalloc(&m_buf_final_d_r, 2 * sizeof(long));
    
//    auto ocl = OCLUtils::getInstance();
//        
//    std::string options = "-D TILE_LEN=" + std::to_string(m_tileLen) + " ";
//
//    std::string plName = ocl->getSelectedPlatformName();
//    if (OCLUtils::contains(plName, "Portable Computing Language") && (verbose > 1))
//        options += " -D DEBUG ";
//    
//    if (gExtendAligned)
//        options += " -D EXTEND_ALIGNED ";
//    
//    ocl->createProgramFromSource("WFDD2ColsGPU.cl", options);
//    m_kernel = ocl->createKernel("wfdd2cols");
    
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
static long nextMultiple(long value, long multiple)
{
   return ((value + (multiple-1)) / multiple) * multiple; 
}

static long previousMultiple(long value, long multiple)
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
      
    cudaMemcpy(m_buf_P, m_P, m_m * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemcpy(m_buf_T, m_T, m_n * sizeof(char), cudaMemcpyHostToDevice);
    
    // this is the initial height of the pyramid
    long h = 2*m_k+1;
    
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long m_top = max2(m_m,m_n); // this is the maximum possible value of the W pyramid cells

    setCommonArgs();

    m_final_d_r[0] = 0;     // furthest reaching point
    m_final_d_r[1] = m_top; // estimated distance (now, worst case)
    cudaMemcpy(m_buf_final_d_r, m_final_d_r, 2 * sizeof(long), cudaMemcpyHostToDevice );
    

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
            cudaMemcpy(m_final_d_r, m_buf_final_d_r, 2 * sizeof(long), cudaMemcpyDeviceToHost);
            
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

    cudaMemcpy(m_final_d_r, m_buf_final_d_r, 2 * sizeof(long), cudaMemcpyDeviceToHost);
            
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
}

void CUDAWavefrontDynamicDiamond2Cols::invokeKernel(long r, long dstart, long numds)
{
    long k = max2(m_m,m_n);

    wfdd2cols<<<numds, gWorkgroupSize>>>(m_buf_P, m_buf_T, m_m, m_n, r, k, m_buf_W, m_buf_final_d_r, m_tileLen, dstart);

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
