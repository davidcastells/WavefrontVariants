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

#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
//#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)		(((y)>(x))? (x) : (y))

#define POLAR_W_TO_INDEX(d, r)          ((d)+ROW_ZERO_OFFSET + (((r)%2) * COLUMN_HEIGHT))

#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))


#define max2(a,b) (((a)>(b))?(a):(b))
#define max3(a,b,c) max2(a, max2(b, c))



long inline __attribute__((always_inline)) extendUnaligned(__global const char* P, __global const char* T, long m, long n, long pi, long ti)
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

#ifdef __ENDIAN_LITTLE__
#define ctz(x) popcount(~(x | -x))
#endif

#define ALIGN_MASK 0xFFFFFFFFFFFFFFF8

long inline __attribute__((always_inline)) extendAligned(__global const char* P, __global const char* T, long m, long n, long pi, long ti)
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
    
    PV = *(__global long*)(&P[pai]);
    TV = *(__global long*)(&T[tai]);
    
//    printf("pi: %ld ti: %ld pai: %ld tai: %ld \n", pi, ti, pai, tai);
//    printf("PV = 0x%016lX\n", PV);
//    printf("TV = 0x%016lX\n", TV);
    
    pbidx = (pi%8);
    tbidx = (ti%8);
    
    pbv = 8 - pbidx;
    tbv = 8 - tbidx;
    
    if (pbv > (m-pi)) pbv = m-pi;
    if (tbv > (n-ti)) tbv = n-pi;

    mbv = min(pbv, tbv);    // minimum valid bytes
    
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



long inline __attribute__((always_inline)) 
extend(__global const char* P, __global const char* T, long m, long n, long pi, long ti)
{    
#ifdef EXTEND_ALIGNED
        return extendAligned(P, T, m, n, pi, ti);
#else
        return extendUnaligned(P, T, m, n, pi, ti);
#endif
}

int inline __attribute__((always_inline)) 
polarExistsInW(long d, long r)
{
    return abs(d) <= r;
}



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
__kernel void wfdd2cols(
        __global char* P, 
        __global char* T, 
        long m_m, 
        long m_n, 
        long r, 
        long m_k,  
        __global long* m_W,
        __global long* p_final_d_r,
        long dstart)
{
    size_t gid = get_global_id(0);
    size_t lid = get_local_id(0);
    
#ifdef SHARED_STORE
    __local long localW[WORKGROUP_SIZE];
#endif
    
    //long d = gid - (r-1);
    long d = dstart + gid; 
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long m_top = max2(m_m,m_n);
    
    long diag_up;
    long left;
    long diag_down; 
        
#ifdef SHARED_STORE
    if (r > 0)
    {
        left = m_W[POLAR_W_TO_INDEX(d, r-1)];
        localW[lid] = left;
        // printf("lid=%ld d=%ld r=%ld\n ", lid, d, r);
    }       
    barrier(CLK_LOCAL_MEM_FENCE);
    
    diag_up = localW[(lid-1) % WORKGROUP_SIZE];
    diag_down = localW[(lid+1) % WORKGROUP_SIZE];
    
#else
        
    if (r > 0)
    {
        diag_up = m_W[POLAR_W_TO_INDEX(d+1, r-1)]  ; 
        left = m_W[POLAR_W_TO_INDEX(d, r-1)] ;
        diag_down = m_W[POLAR_W_TO_INDEX(d-1, r-1)]  ; 
    }
#endif

    // we already reached the final point in previous invocations
    if (p_final_d_r[0] >= m_top)
        return;
    
    // early exit for useless work items
    if (!polarExistsInW(d,r))
        return;
    
    if (r == 0)
    {
        if (d == 0)
        {
            long extended = extend(P, T, m_m, m_n, 0, 0); 
            // initial case
            m_W[POLAR_W_TO_INDEX(d, r)] = extended; 
            if ((d == final_d) && extended >= m_top)
            {
                m_W[POLAR_W_TO_INDEX(d, r)] = extended;
                p_final_d_r[0] = extended;   // furthest reaching point
                p_final_d_r[1] = r;         // at edit distance = r
                return;
            }
        }
        else
            m_W[POLAR_W_TO_INDEX(d, r)]  = 0;
            
        
    }
    else
    {
#ifdef SHARED_STORE
        diag_up = (polarExistsInW(d+1, r-1))?  ((lid > 0)? diag_up : m_W[POLAR_W_TO_INDEX(d+1, r-1)])  : 0; 
        left = (polarExistsInW(d,r-1))? left  : 0;
        diag_down = (polarExistsInW(d-1,r-1))? ((lid < (WORKGROUP_SIZE-1)) ? diag_down : m_W[POLAR_W_TO_INDEX(d-1, r-1)])  : 0; 
#else
        diag_up = (polarExistsInW(d+1, r-1))?  diag_up  : 0; 
        left = (polarExistsInW(d,r-1))? left : 0;
        diag_down = (polarExistsInW(d-1,r-1))?  diag_down  : 0; 
#endif
        long compute;

        if (d == 0)
            compute = max(diag_up, diag_down);
        else if (d > 0)
            compute = max(diag_up, diag_down+1);
        else
            compute = max(diag_up+1, diag_down);
            
        compute = max(left+1, compute);

        if (d == final_d) 
        {
            m_W[POLAR_W_TO_INDEX(d, r)] = compute;
            p_final_d_r[0] = compute;   // furthest reaching point
            p_final_d_r[1] = r;         // at edit distance = r
            
            if (compute >= m_top)
                return;
        }

        long ex = POLAR_W_TO_CARTESIAN_X(d, compute);
        long ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

        if ((ex < m_n) && (ey < m_m))
        {
            long extendv = extend(P, T, m_m, m_n, ey, ex);
            long extended = compute + extendv;

            m_W[POLAR_W_TO_INDEX(d, r)] = extended;

            if (d == final_d) 
            {
                p_final_d_r[0] = extended;  // furthest reaching point
                p_final_d_r[1] = r;         // at edit distance = r
                
                if (extended >= m_top)
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
