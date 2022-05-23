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
 * File:   OCLGPUWavefrontDynamicDiamond2Cols.cpp
 * Author: dcr
 * 
 * Created on January 9, 2022, 1:24 PM
 */

#include "OCLGPUWavefrontDynamicDiamond2Cols.h"
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

//#define NUMBER_OF_INVOCATIONS_PER_READ 100
#define NUMBER_OF_INVOCATIONS_PER_READ gEnqueuedInvocations

extern int verbose;
extern int gPid;
extern int gDid;
extern int gTileLen;
extern int gWorkgroupSize;
extern int gExtendAligned;
extern int gEnqueuedInvocations;
extern double gPrintPeriod;
extern char* gLocalTileMemory;


OCLGPUWavefrontDynamicDiamond2Cols::OCLGPUWavefrontDynamicDiamond2Cols()
{
    m_buf_P = NULL;
    m_buf_T = NULL;
    m_buf_W = NULL;
    
    auto ocl = OCLUtils::getInstance();
    //ocl->selectPlatform(gPid);
    ocl->selectDevice(gDid);
    
    m_context = ocl->createContext();
    m_queue = ocl->createQueue();
    
    m_W = NULL;
}


OCLGPUWavefrontDynamicDiamond2Cols::~OCLGPUWavefrontDynamicDiamond2Cols()
{
    auto ocl = OCLUtils::getInstance();
    ocl->releaseMemObject(m_buf_P);
    ocl->releaseMemObject(m_buf_T);
            
    ocl->releaseContext(m_context);
    
    if (m_W != NULL)
        delete [] m_W;
}


void OCLGPUWavefrontDynamicDiamond2Cols::setInput(const char* P, const char* T, INT_TYPE k)
{
    // this should not be allocated, we only expect a single call
    assert(m_W == NULL);
    
    m_m = strlen(P);
    m_n = strlen(T);
    m_k = k;
    m_tileLen = gTileLen;

    if (strcmp(gLocalTileMemory, "shared") == 0)
    {
        m_localStore = "SHARED_STORE";
    }
    else
    {
        m_localStore = "GLOBAL_STORE";
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
    
    m_buf_final_d_r = clCreateBuffer(m_context, CL_MEM_READ_WRITE, 2 * sizeof(INT_TYPE), NULL, &err);
    CHECK_CL_ERRORS(err);
    
    auto ocl = OCLUtils::getInstance();
        
    std::string options ="";
    options += " -D TILE_LEN=" + std::to_string(m_tileLen);
    options += " -D " + m_localStore;
    options += " -D WORKGROUP_SIZE=" + std::to_string(gWorkgroupSize);
    options += " -D COLUMN_HEIGHT=" + std::to_string(2*k+1);
    options += " -D ROW_ZERO_OFFSET=" + std::to_string(k);
    options += " -D INT_TYPE="  STR_INT_TYPE "";
    options += " ";

    std::string plName = ocl->getSelectedPlatformName();
    if (OCLUtils::contains(plName, "Portable Computing Language") && (verbose > 1))
        options += " -D DEBUG ";
    
    if (gExtendAligned)
        options += " -D EXTEND_ALIGNED ";
    
    ocl->createProgramFromSource("WFDD2ColsGPU.cl", options);
    m_kernel = ocl->createKernel("wfdd2cols");
    m_resetKernel = ocl->createKernel("resetW");
    
    printf("input set\n");
}

void OCLGPUWavefrontDynamicDiamond2Cols::progress(PerformanceLap& lap, INT_TYPE r, INT_TYPE rsol, int& lastpercent, long cellsAllocated, long cellsAlive, INT_TYPE numds)
{
    static double lastPrintLap = -1;
    
    double printPeriod = (gPrintPeriod > 0)? gPrintPeriod : 0.5;
    
#define DECIMALS_PERCENT    1000
    if (!verbose)
        return;
    
    double elapsed = lap.stop();

    // square model
    double rd = r;
    double kd = m_k;
    double estimated = (elapsed / (rd*rd)) * (kd*kd);
    int percent = (rd*rd*100.0*DECIMALS_PERCENT/(kd*kd));
    
    if (elapsed > (lastPrintLap + printPeriod))
    {
        printf((gPrintPeriod > 0)?"\n":"\r");
        //printf("\rcol %ld/%ld %.2f%% cells allocated: %ld alive: %ld elapsed: %d s  estimated: %d s    ", x, m_n, ((double)percent/DECIMALS_PERCENT), cellsAllocated, cellsAlive, (int) elapsed, (int) estimated );
        printf("r %d/%d %.2f%% (ds: %ld) elapsed: %d s  estimated: %d s  ", r, rsol, ((double)percent/DECIMALS_PERCENT), numds , (int) elapsed, (int) estimated );
    
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
INT_TYPE nextMultiple(INT_TYPE value, INT_TYPE multiple)
{
   return ((value + (multiple-1)) / multiple) * multiple; 
}

INT_TYPE previousMultiple(INT_TYPE value, INT_TYPE multiple)
{
   return ((value) / multiple) * multiple; 
}

/*
def previousMultiple(value,  multiple):
   return ((value ) / multiple) * multiple 

*/



INT_TYPE OCLGPUWavefrontDynamicDiamond2Cols::getDistance()
{
    PerformanceLap lap;
    int lastpercent = -1;
    long cellsAllocated = 0;
    long cellsAlive = 0;
    
    // this is the initial height of the pyramid
    INT_TYPE h = 2*m_k+1;
    
    resetW(h);
    
    m_queue->writeBuffer(m_buf_P, (void*) m_P, m_m);
    m_queue->writeBuffer(m_buf_T, (void*) m_T, m_n);
    
    
    INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    INT_TYPE m_top = max2(m_m,m_n); // this is the maximum possible value of the W pyramid cells

    printf("Final d=%d\n", final_d);
    setCommonArgs();

    // Values for the solution diagonal
    m_final_d_r[0] = 0;     // furthest reaching point in the solution diagonal
    m_final_d_r[1] = 0;     // edit distance of the current value in the solution diagonal 
    m_queue->writeBuffer(m_buf_final_d_r, m_final_d_r, 2 * sizeof(INT_TYPE) );

    INT_TYPE dsol = final_d;
    INT_TYPE rsol = m_top;      // radius where the solution is, this is the worst case, 
    INT_TYPE last_dstart = m_tileLen;
     
#ifdef TRACE_BOUNDARY
    printf("diamond_boundary,ld,lu,rd,ru,start,numds,numtiles,workitems,workgroupsize\n");
#endif
    for (INT_TYPE r=0; r <= rsol; r+= m_tileLen)
    {
        INT_TYPE dup_left = r;      
        INT_TYPE ddown_left = -r;   
        
        INT_TYPE dup_right = nextMultiple(dsol + (rsol-r), m_tileLen) ;        
        INT_TYPE ddown_right = previousMultiple(dsol - (rsol-r) , m_tileLen);
        
        //INT_TYPE dup_right_min = nextMultiple(dsol+m_tileLen, m_tileLen);
        
        // find the next multiple of tilelen
        
        INT_TYPE dup = min2(dup_left, dup_right);
        INT_TYPE ddown = max2(ddown_left, ddown_right);
        
        //INT_TYPE dup = dup_left;
        //INT_TYPE ddown = ddown_left;
        
        INT_TYPE dstart = ddown;
        INT_TYPE numds = (dup - ddown)+(m_tileLen*2)-1;             // number of ds 
        
        if ((dstart % (2*m_tileLen)) == (last_dstart % (2*m_tileLen)))
        {
            dstart -= m_tileLen;
            numds += m_tileLen;
        }

        
        // round numds to 
        //numds = (numds + gWorkgroupSize -1 ) / gWorkgroupSize  * gWorkgroupSize ; 
        
        // numds are not rounded, we should round them to number workgroupsize
        //long k = max2(m_m,m_n);
        INT_TYPE tileSmallSize = 2*m_tileLen - 1;
        INT_TYPE tileBigSize = 2*(m_tileLen+1) - 1;
        INT_TYPE numTiles = numds/tileSmallSize;
        
        INT_TYPE numWorkItems = numTiles * tileBigSize;

#ifdef TRACE_BOUNDARY
            printf("diamond_boundary,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", ddown_left, dup_left, ddown_right, dup_right, dstart, numds, numTiles, numWorkItems, tileBigSize);
#endif
        
        invokeKernel(r, dstart, numWorkItems, tileBigSize);
        
        last_dstart = dstart;
        
        progress(lap, r, rsol, lastpercent, cellsAllocated, cellsAlive, numds);
        
        if ((r % NUMBER_OF_INVOCATIONS_PER_READ) == 0)
        {            
            m_queue->readBuffer(m_buf_final_d_r, m_final_d_r, 2 * sizeof(INT_TYPE));
            
            if (verbose > 1)
            {
                printf("Check to continue\nTop: %d\n", m_top);
                printf("Fur. reaching point: %d\n", m_final_d_r[0]);
                printf("Edit distance: %d\n", m_final_d_r[1]);
            }
            
            if (m_final_d_r[0] >= m_top)
                return m_final_d_r[1];  // return the distance
            else
            {
                // update the maximum error to consider, if it is lower than current one
                //printf("w value: %d - ed: %d\n", m_final_d_r[0], m_final_d_r[1]);
                //rsol = m_top - (m_final_d_r[0] - m_final_d_r[1]);
                INT_TYPE dtosol = m_top - m_final_d_r[0];   // this is max distance the distance to the solution
                rsol = nextMultiple(m_final_d_r[1] + dtosol, 2*m_tileLen);             // solution must be, at most, in current radius + distance to soltuion 
                
                //printf("rsol: %d   top: %d  further reached radius: %d   with error: %d\n", rsol, m_top, m_final_d_r[0], m_final_d_r[1]);
                //if (newk < m_k)
                //    m_k = newk;
            }
        }
    }
    
    printf("WARNING did not complete! r: %d rsol: %d\n", m_final_d_r[1], rsol);
    
    lastpercent--;
    progress(lap, rsol, rsol, lastpercent, cellsAllocated, cellsAlive, 0);

    m_queue->readBuffer(m_buf_final_d_r, m_final_d_r, 2 * sizeof(INT_TYPE));
            
    if (verbose > 1)
    {
        printf("Check to continue\nTop: %d\n", m_top);
        printf("Fur. reaching point: %d\n", m_final_d_r[0]);
        printf("Edit distance: %d\n", m_final_d_r[1]);
    }

    if (m_final_d_r[0] >= m_top)
        return m_final_d_r[1];

    return m_top;
}
    
void OCLGPUWavefrontDynamicDiamond2Cols::setCommonArgs()
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

void OCLGPUWavefrontDynamicDiamond2Cols::resetW(INT_TYPE h)
{
    cl_int ret;
    
    ret = clSetKernelArg(m_resetKernel, 0, sizeof(cl_mem), (void *)&m_buf_W);
    CHECK_CL_ERRORS(ret);
    
    //printf("r=%d dstart=%d numds=%d rsol=%d workitems=%d workgroup size=%d\n", r, dstart, numds, rsol, numWorkItems, numWorkItemsPerTile);
    m_queue->invokeKernel1D(m_resetKernel, h*2, 32);
}


void OCLGPUWavefrontDynamicDiamond2Cols::invokeKernel(INT_TYPE r, INT_TYPE dstart, INT_TYPE numWorkItems, INT_TYPE workgroupSize)
{
    cl_int ret;
    
    ret = clSetKernelArg(m_kernel, 4, sizeof(CL_INT_TYPE), (void *)&r);
    CHECK_CL_ERRORS(ret);
    
    ret = clSetKernelArg(m_kernel, 8, sizeof(CL_INT_TYPE), (void *)&dstart);
    CHECK_CL_ERRORS(ret);

    
    //printf("r=%d dstart=%d numds=%d rsol=%d workitems=%d workgroup size=%d\n", r, dstart, numds, rsol, numWorkItems, tileBigSize);
    m_queue->invokeKernel1D(m_kernel, numWorkItems, workgroupSize);
}



char* OCLGPUWavefrontDynamicDiamond2Cols::getAlignmentPath(INT_TYPE* distance)
{
    printf("Not implemented yet\n");
    exit(-1);
}
    
const char* OCLGPUWavefrontDynamicDiamond2Cols::getDescription()
{
    return "Wavefront Dynamic Diamond 2 columns [ocl_local_tiles] in OpenCL";
}
