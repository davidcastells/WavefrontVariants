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

#include "WavefrontOriginal2Cols.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <new>
#include <omp.h>

#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
//#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)		(((y)>(x))? (x) : (y))

//#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)),w)

#define POLAR_W_TO_INDEX(d, r)		((d)+m_k + (((r)%2) * (2*m_k+1)))

#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))

extern int verbose;
extern int gStats;
extern int gExtendAligned;

#define EXTEND_BIN_ELEMENTS 2

WavefrontOriginal2Cols::WavefrontOriginal2Cols()
{
    for (int i=0; i<10; i++)
        m_statsExtendBins[i] = 0;
    m_statsMaxExtend = 0;
    
    m_statsPTReadBytes = 0;
    m_statsWReadBytes = 0;
    m_statsWReadBytes = 0;
    m_statsTime = 0;
}

WavefrontOriginal2Cols::~WavefrontOriginal2Cols()
{
    delete [] m_W;
}


void WavefrontOriginal2Cols::collectExtendStats(INT_TYPE e)
{
    int bin = e / EXTEND_BIN_ELEMENTS;
    if (bin > 9)
        bin = 9;
    
    m_statsExtendBins[bin]++;
    
    if (e > m_statsMaxExtend)
        m_statsMaxExtend = e;
}

void WavefrontOriginal2Cols::printStats()
{
    printf("\nStats:\n");
    printf("Extend values histogram:\n");
    for (int i=0; i < 10; i++)
    {
        printf("[%2d-%2d] = %ld\n", i*EXTEND_BIN_ELEMENTS , (i+1)*EXTEND_BIN_ELEMENTS-1, m_statsExtendBins[i]);
    }
    
    printf("\nMax Extend: %ld\n\n", m_statsMaxExtend);
    
    printf("P/T Read bytes: %ld\n", m_statsPTReadBytes);
    printf("W Read bytes: %ld\n", m_statsWReadBytes);
    printf("W Write bytes: %ld\n", m_statsWWriteBytes);

    printf("P/T Read Bandwidth: %f Bps\n", (double)m_statsPTReadBytes/m_statsTime);
    printf("W Read Bandwidth: %f Bps\n", (double)m_statsWReadBytes/m_statsTime);
    printf("W Write Bandwidth: %f Bps\n", (double)m_statsWWriteBytes/m_statsTime);
}

#define ALIGN_MASK 0xFFFFFFFFFFFFFFF8

void WavefrontOriginal2Cols::collectTime(double lap)
{
    m_statsTime = lap;
}

void WavefrontOriginal2Cols::collectPTReadBytes(int inc)
{
    m_statsPTReadBytes += inc;
}

void WavefrontOriginal2Cols::collectWWriteBytes(int inc)
{
    m_statsWWriteBytes += inc;
}

void WavefrontOriginal2Cols::collectWReadBytes(int inc)
{
    m_statsWReadBytes += inc;
}

INT_TYPE WavefrontOriginal2Cols::extendAligned(const char* P, const char* T, INT_TYPE m, INT_TYPE n, INT_TYPE pi, INT_TYPE ti)
{
    int pbv; // P valid bytes
    int tbv; // T valid bytes
    
    INT_TYPE pai;
    INT_TYPE tai;
    
    int pbidx;
    int tbidx;
    
    INT_TYPE PV;    // P value
    INT_TYPE TV;    // T value

    int mbv;
    unsigned long mask;
    int neq;
    
    INT_TYPE e = 0;
    
    //long gt = extend(P, T, m, n, pi, ti);
    
loop:
    pai = pi & ALIGN_MASK;
    tai = ti & ALIGN_MASK;
    
    PV = *(long*)(&P[pai]);
    TV = *(long*)(&T[tai]);
    
    if (gStats) collectPTReadBytes(sizeof(long)*2);
    
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

        neq = __builtin_ctzl((PV ^ TV) | mask) / 8;

//        printf("neq = %d\n", neq);

        e += neq;

        if (neq == mbv)
        {
            ti += neq;
            pi += neq;
            goto loop;
        }
    }
    
    if (gStats) collectExtendStats(e);

//    printf("e = %ld\n", e);
    
//    if (e != gt)
//    {
//        printf("ERROR at pi: %ld ti: %ld\n", pi, ti);
//        exit(0);
//    }
    return e;
}


INT_TYPE WavefrontOriginal2Cols::extend(const char* P, const char* T, INT_TYPE m, INT_TYPE n, INT_TYPE pi, INT_TYPE ti)
{
    INT_TYPE e = 0;

    while (pi < m && ti < n)
    {
        if (gStats) collectPTReadBytes(sizeof(char)*2);
        
        if (P[pi] != T[ti])
        {
            if (gStats)
                collectExtendStats(e);
            
            return e;
        }
        
        e++;
        pi++;
        ti++;
    }

    return e;
}

static INT_TYPE polarExistsInD(INT_TYPE d, INT_TYPE r)
{
    INT_TYPE x = POLAR_D_TO_CARTESIAN_X(d,r);
    INT_TYPE y = POLAR_D_TO_CARTESIAN_Y(d,r);

    return ((x >= 0) && (y>=0));
}

static INT_TYPE polarExistsInW(INT_TYPE d, INT_TYPE r)
{
    INT_TYPE x = POLAR_W_TO_CARTESIAN_X(d,r);
    INT_TYPE y = POLAR_W_TO_CARTESIAN_Y(d,r);

    return ((x >= 0) && (y >= 0));
}

void WavefrontOriginal2Cols::setInput(const char* P, const char* T, INT_TYPE k)
{
    m_m = strlen(P);
    m_n = strlen(T);
    m_k = k;

    INT_TYPE size = 2*(2*k+1);

    printf("create buffer %.2f GB\n", size*sizeof(long)/(1E9));

    try
    {
        m_W = new INT_TYPE[size];
    }
    catch (const std::bad_alloc& e) 
    {
        printf("FAILED to allocate memory\n");
        exit(-1);
    }

    assert(m_W);
    m_top = max2(m_m,m_n);
    m_P = P;
    m_T = T;

    printf("input set\n");

}

void WavefrontOriginal2Cols::progress(PerformanceLap& lap, INT_TYPE r, int& lastpercent, long cellsAllocated, long cellsAlive)
{
    static double lastPrintLap = -1;
    
    if (!verbose)
        return;
    
    double elapsed = lap.stop();
    double estimated = (elapsed / (r*r)) * (m_k*m_k);
    int percent = (r*r*100.0/(m_k*m_k));
    
    
    if (elapsed > (lastPrintLap + 0.5))
    {
        //printf("\rcol %ld/%ld %.2f%% cells allocated: %ld alive: %ld elapsed: %d s  estimated: %d s    ", x, m_n, ((double)percent/DECIMALS_PERCENT), cellsAllocated, cellsAlive, (int) elapsed, (int) estimated );
        printf("\rr %ld/%ld %.2f%% elapsed: %d s  estimated: %d s", r, m_k, ((double)percent) , (int) elapsed, (int) estimated );
    
        fflush(stdout);
        lastpercent = percent;
        lastPrintLap = elapsed;
    }
}

INT_TYPE WavefrontOriginal2Cols::getDistance()
{
    PerformanceLap lap;
    int lastpercent = -1;
    
    long cellsAllocated = 0;
    long cellsAlive = 0;
    
    INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    INT_TYPE ret;

    // for the first element, just execute the extend phase
    m_W[POLAR_W_TO_INDEX(0, 0)] = (gExtendAligned) ? extendAligned(m_P, m_T, m_m, m_n, 0, 0) : extend(m_P, m_T, m_m, m_n, 0, 0);

    if (gStats) collectWWriteBytes(sizeof(long));
    
    if (m_W[POLAR_W_TO_INDEX(0, 0)] >= m_top)
            goto end_loop;

    for (INT_TYPE r=1; r < m_k; r++)
    {
        progress(lap, r, lastpercent, cellsAllocated, cellsAlive);

        for (INT_TYPE d=-r; d <= r; d++)
        {			
            // printf("d=%d r=%d idx=%d\n", d+1, r-1, POLAR_W_TO_INDEX(d+1, r-1));
            INT_TYPE diag_up = (polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1)]  : 0;
            INT_TYPE left = (polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1)]  : 0;
            INT_TYPE diag_down = (polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1)]  : 0;

            if (gStats) collectWReadBytes(sizeof(long)*3);

            INT_TYPE compute;

            if (d == 0)
                compute = max3(diag_up, left+1, diag_down);
            else if (d > 0)
                compute = max3(diag_up, left+1, diag_down+1);
            else
                compute = max3(diag_up+1, left+1, diag_down);

            if ((d == final_d) && compute >= m_top)
            {
                ret = r;
                goto end_loop;
            }

            INT_TYPE ex = POLAR_W_TO_CARTESIAN_X(d, compute);
            INT_TYPE ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

            if ((ex < m_n) && (ey < m_m))
            {
                //printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  || ", d, r , diag_up, left, diag_down, compute );

                INT_TYPE extendv = (gExtendAligned) ? extendAligned(m_P, m_T, m_m, m_n, ey, ex) : extend(m_P, m_T, m_m, m_n, ey, ex);
                INT_TYPE extended = compute + extendv;

                m_W[POLAR_W_TO_INDEX(d, r)] = extended;
                
                if (gStats) collectWWriteBytes(sizeof(long));


                //printf("(Extended)  + %d = %d (P[%d]=%s - T[%d]=%s)\n",  extendv, extended, ey, &m_P[ey], ex, &m_T[ex] );
                //printf("(Extended)  + %d = %d (P[%d] - T[%d])\n",  extendv, extended, ey,  ex );


                if ((d == final_d) && extended >= m_top)
                {
                        printf("Finishing d=%ld r=%ld\n", d, r);
                        ret = r;
                        goto end_loop;
                }
            }
            else
            {
                m_W[POLAR_W_TO_INDEX(d, r)] = compute;
                
                if (gStats) collectWWriteBytes(sizeof(long));
            }
        }
    }
    
    lastpercent--;
    progress(lap, m_k, lastpercent, cellsAllocated, cellsAlive);

end_loop:
                    
    collectTime(lap.lap());

    if (verbose)
        printf("\n");

    if (gStats)
        printStats();

    return ret;
}


const char* WavefrontOriginal2Cols::getDescription()
{
    return "Wavefront Original 2 columns";
}

class WCellPointer
{
public:
	WCellPointer(INT_TYPE d, INT_TYPE r, INT_TYPE extend, WCellPointer* p, char type)
	{
		m_d = d;
		m_r = r;
        m_extend = extend;
		m_p = p;
        count = 0;
        m_type = type;
                
        //printf("\nallocated d:%ld r:%ld\n", d, r);
        
        if (m_p != NULL)
        {
            #pragma omp critical
            {
            m_p->count++;
            }
            //printf("\ncount d:%ld r:%ld %d\n", m_p->m_d, m_p->m_r, m_p->count);
        }
	}
        
        static void deleteChain(WCellPointer* target, long& alive)
        {
            WCellPointer* p = target;
            WCellPointer* pp;
            
        loop:
            if (p->count > 0)
                // do not delete cells with dependencies
                return;
        
            pp = p->m_p;
            
            //printf("\ndeleting d:%ld r:%ld\n", p->m_d, p->m_r);
            delete p;
            alive--;
            
            if (pp == NULL)
                return;
            
            pp->count--;
            //printf("\ncount d:%ld r:%ld %d\n", pp->m_d, pp->m_r, pp->count);
            
            assert(pp->count >= 0);
            
            if (pp->count <= 0)
            {
                p = pp;
                goto loop;
            }
        }
        
        INT_TYPE m_d;   // diagonal (y axis of the W pyramid)
        INT_TYPE m_r;   // distance (x axis of the W pyramid)
        INT_TYPE m_extend;   // extension
        WCellPointer* m_p;  // dependent
        char m_type;
        char count; // number of dependant cells
};

#define LEFT_EDIT       'X'
#define DIAG_UP_EDIT    'I'
#define DIAG_DOWN_EDIT  'D'

/**
 * 
 * @param distance
 * @return 
 */
char* WavefrontOriginal2Cols::getAlignmentPath(INT_TYPE* distance)
{
    // longest worst case path goes through the table boundary 
    char* path = new char[m_m + m_n];
    
    long cellsAllocated = 0;
    long cellsAlive = 0;

    WCellPointer** selected = new WCellPointer*[2*(2*m_k+1)];
    
    for (INT_TYPE k = 0; k < 2*(2*m_k+1); k++ )
        selected[k] = NULL;
    
    PerformanceLap lap;
    int lastpercent = -1;
    
    INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    INT_TYPE ret;

    // for the first element, just execute the extend phase
    m_W[POLAR_W_TO_INDEX(0, 0)] = extend(m_P, m_T, m_m, m_n, 0, 0);

    selected[POLAR_W_TO_INDEX(0, 0)] = selected[POLAR_W_TO_INDEX(0, 1)] = new WCellPointer(0, 0, m_W[POLAR_W_TO_INDEX(0, 0)], NULL, '|');
    cellsAllocated++;
    cellsAlive++;
    
    
    if (m_W[POLAR_W_TO_INDEX(0, 0)] >= m_top)
        goto end_loop;

    for (INT_TYPE r=1; r < m_k; r++)
    {
        progress(lap, r, lastpercent, cellsAllocated, cellsAlive);

        // #pragma omp parallel for schedule(static, 100)
        for (INT_TYPE d=-r; d <= r; d++)
        {			
            // printf("d=%d r=%d idx=%d\n", d+1, r-1, POLAR_W_TO_INDEX(d+1, r-1));
            int diagUpExist = polarExistsInW(d+1, r-1);
            int leftExist = polarExistsInW(d,r-1);
            int diagDownExist = polarExistsInW(d-1,r-1);
            
            INT_TYPE diag_up = (diagUpExist) ? m_W[POLAR_W_TO_INDEX(d+1, r-1)]  : 0;
            INT_TYPE left = (leftExist) ? m_W[POLAR_W_TO_INDEX(d, r-1)]  : 0;
            INT_TYPE diag_down = (diagDownExist)? m_W[POLAR_W_TO_INDEX(d-1, r-1)]  : 0;

            WCellPointer* prev;
            char type;
            
            INT_TYPE compute;

            if (d == 0)
            {
                compute = max3(diag_up, left+1, diag_down);
                if (compute == (left+1) && leftExist)
                {
                    prev = selected[POLAR_W_TO_INDEX(d, 1)];
                    type = LEFT_EDIT;
                }
                else if (compute == diag_up && diagUpExist)
                {
                    prev = selected[POLAR_W_TO_INDEX(d+1, 1)];
                    type = DIAG_UP_EDIT;
                }
                else // if (compute == diag_down)
                {
                    prev = selected[POLAR_W_TO_INDEX(d-1, 1)];
                    type = DIAG_DOWN_EDIT;
                }
            }
            else if (d > 0)
            {
                compute = max3(diag_up, left+1, diag_down+1);
                if (compute == (left+1) && leftExist)
                {
                    prev = selected[POLAR_W_TO_INDEX(d, 1)];
                    type = LEFT_EDIT;
                }
                else if (compute == diag_up && diagUpExist)
                {
                    prev = selected[POLAR_W_TO_INDEX(d+1, 1)];
                    type = DIAG_UP_EDIT;
                }
                else // if (compute == diag_down)
                {
                    prev = selected[POLAR_W_TO_INDEX(d-1, 1)];
                    type = DIAG_DOWN_EDIT;
                }
            }
            else
            {
                compute = max3(diag_up+1, left+1, diag_down);
                if (compute == (left+1) && leftExist)
                {
                    prev = selected[POLAR_W_TO_INDEX(d, 1)];
                    type = LEFT_EDIT;
                }
                else if (compute == (diag_up+1) && diagUpExist)
                {
                    prev = selected[POLAR_W_TO_INDEX(d+1, 1)];
                    type = DIAG_UP_EDIT;
                }
                else // if (compute == diag_down)
                {
                    prev = selected[POLAR_W_TO_INDEX(d-1, 1)];
                    type = DIAG_DOWN_EDIT;
                }
            }

            if ((d == final_d) && compute >= m_top)
            {
                m_W[POLAR_W_TO_INDEX(d, r)] = compute;
                selected[POLAR_W_TO_INDEX(d, 0)] = new WCellPointer(d, r, 0, prev, type);
                cellsAllocated++;
                cellsAlive++;
                ret = r;
                continue;
                //goto end_loop;
            }

            INT_TYPE ex = POLAR_W_TO_CARTESIAN_X(d, compute);
            INT_TYPE ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

            if ((ex < m_n) && (ey < m_m))
            {
                //printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  || ", d, r , diag_up, left, diag_down, compute );

                INT_TYPE extendv = extend(m_P, m_T, m_m, m_n, ey, ex);
                INT_TYPE extended = compute + extendv;

                m_W[POLAR_W_TO_INDEX(d, r)] = extended;
                selected[POLAR_W_TO_INDEX(d, 0)] = new WCellPointer(d, r, extendv, prev, type);
                cellsAllocated++;
                cellsAlive++;

                //printf("(Extended)  + %d = %d (P[%d]=%s - T[%d]=%s)\n",  extendv, extended, ey, &m_P[ey], ex, &m_T[ex] );
                //printf("(Extended)  + %d = %d (P[%d] - T[%d])\n",  extendv, extended, ey,  ex );

                if ((d == final_d) && extended >= m_top)
                {
                    printf("Finishing d=%ld r=%ld\n", d, r);
                    ret = r;
                    continue;
                    //goto end_loop;
                }
            }
            else
            {
                m_W[POLAR_W_TO_INDEX(d, r)] = compute;
                selected[POLAR_W_TO_INDEX(d, 0)] = new WCellPointer(d, r, 0, prev, type);
                cellsAllocated++;
                cellsAlive++;
            }
        }
        
        if (m_W[POLAR_W_TO_INDEX(final_d, r)] >= m_top)
            goto end_loop;
        
        // now purge unused links
        for (INT_TYPE d=-r; d <= r; d++)
        {
            if (selected[POLAR_W_TO_INDEX(d, 1)] == NULL)
                continue;
            
            WCellPointer::deleteChain(selected[POLAR_W_TO_INDEX(d, 1)], cellsAlive);
            selected[POLAR_W_TO_INDEX(d, 1)] = NULL;
        }
        
        // now copy from current to previous
        for (INT_TYPE d=-r; d <= r; d++)
        {
            selected[POLAR_W_TO_INDEX(d, 1)] = selected[POLAR_W_TO_INDEX(d, 0)];
        }
    }
    
end_loop:
    lastpercent--;
    progress(lap, m_k, lastpercent, cellsAllocated, cellsAlive);

    if (verbose)
        printf("\n");

    WCellPointer* p=selected[POLAR_W_TO_INDEX(final_d, 0)] ;
    long i=0;
    
    while (p != NULL)
    {
        WCellPointer* pp = p->m_p;
        
        for (INT_TYPE j = 0; j < p->m_extend; j++)
            path[i++] = '|';
        
        if (pp != NULL)
            path[i++] = p->m_type;        
        p = pp;
    }
    
    path[i] = 0;
    
    // now reverse
    INT_TYPE k=i-1;

    for (int i = 0; i < k/2; i++)
    {
        long aux = path[i];
        path[i] = path[k-i];
        path[k-i] = aux;
    }
    
    
    *distance = ret;
    
    delete [] selected;
    
    return path;
}
