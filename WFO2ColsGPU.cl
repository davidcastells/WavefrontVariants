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

#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
//#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)		(((y)>(x))? (x) : (y))

//#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)),w)
// #define POLAR_W_TO_INDEX(d, r)                  ((d)+m_k + (((r)%(2*tileLen)) * (2*m_k+1)))
//#define POLAR_W_TO_INDEX(d, r)		((d)+m_k + (((r)%2) * (2*m_k+1)))

#define POLAR_W_TO_INDEX(d, r)          ((d)+ROW_ZERO_OFFSET + (((r)%2) * COLUMN_HEIGHT))


#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))

//#define POLAR_LOCAL_W_TO_INDEX(d, r, tl) ((r) >= (tl))? (2*((r)-(tl))*(tl)) - ((r)-(tl))*((r)-(tl)) + (2*(tl)-(r)-1) + (d) + (tl)*(tl) : ((r)*(r)+(r)+d)
#define POLAR_LOCAL_W_TO_INDEX(d, r)    ((r) >= TILE_LEN)? (2*((r)-TILE_LEN)*TILE_LEN) - ((r)-TILE_LEN)*((r)-TILE_LEN) + (2*TILE_LEN-(r)-1) + (d) + TILE_LEN*TILE_LEN : ((r)*(r)+(r)+d)

#define max2(a,b) (((a)>(b))?(a):(b))
#define max3(a,b,c) max2(a, max2(b, c))

#ifdef GLOBAL_STORE
    #define LOCAL_TILE_TYPE     __private
    #define LOCAL_TILE_PTR      LOCAL_TILE_TYPE long*
#endif

#ifdef REGISTER_STORE
    #if TILE_LEN == 1
        // we define a tile with len 2
        typedef struct
        {
            long r0_d0;
            long r1_d0;
        } LOCAL_TILE;
        
        #define LOCAL_TILE_PTR      LOCAL_TILE*
        
        void inline __attribute__((always_inline)) 
        writeLocalW(LOCAL_TILE_PTR localW, long d, long r, long v)
        {
            //printf("local_WR(%ld,%ld) = %ld\n", d, r, v);
            // max 8
            switch ((r<<3)|(d+2))
            {
                case (0<<3)|(0+2):  localW->r0_d0 = v; break;
                case (1<<3)|(0+2):  localW->r1_d0 = v; break;
                default:
                    printf("WRITE ERROR r:%d d:%d\n", r, d);

            }
            
            //printf("localW.r0_d0 = %ld\n", localW->r0_d0 );

        }
        
        long inline __attribute__((always_inline)) 
        readLocalW(LOCAL_TILE_PTR localW, long d, long r)
        {
            long v; 
            
            // max 8
            switch ((r<<3)|(d+2))
            {
                case (0<<3)|(0+2):  v = localW->r0_d0; break;
                case (1<<3)|(0+2):  v = localW->r1_d0; break;
                default:
                    printf("READ ERROR r:%d d:%d\n", r, d);
            }
            

            //printf("local_RD(%ld,%ld) = %ld\n", d, r, v);
            return v;
        }
    #endif
    
    #if TILE_LEN == 2
        // we define a tile with len 2
        typedef struct
        {
            long r0_d0;
            long r1_dm1;
            long r1_d0;
            long r1_d1;
            long r2_dm1;
            long r2_d0;
            long r2_d1;
            long r3_d0;
        } LOCAL_TILE;
        
        #define LOCAL_TILE_PTR      LOCAL_TILE*
        
        void inline __attribute__((always_inline)) 
        writeLocalW(LOCAL_TILE_PTR localW, long d, long r, long v)
        {
            //printf("local_WR(%ld,%ld) = %ld\n", d, r, v);
            // max 8
            switch ((r<<3)|(d+2))
            {
                case (0<<3)|(0+2):  localW->r0_d0 = v; break;
                case (1<<3)|(-1+2): localW->r1_dm1 = v; break;
                case (1<<3)|(0+2):  localW->r1_d0 = v; break;
                case (1<<3)|(1+2):  localW->r1_d1 = v; break;
                case (2<<3)|(-1+2): localW->r2_dm1 = v; break;
                case (2<<3)|(0+2):  localW->r2_d0 = v; break;
                case (2<<3)|(1+2):  localW->r2_d1 = v; break;
                case (3<<3)|(0+2):  localW->r3_d0 = v; break;
                default:
                    printf("WRITE ERROR r:%d d:%d\n", r, d);

            }
            
            //printf("localW.r0_d0 = %ld\n", localW->r0_d0 );

        }
        
        long inline __attribute__((always_inline)) 
        readLocalW(LOCAL_TILE_PTR localW, long d, long r)
        {
            long v; 
            
            // max 8
            switch ((r<<3)|(d+2))
            {
                case (0<<3)|(0+2):  v = localW->r0_d0; break;
                case (1<<3)|(-1+2): v = localW->r1_dm1; break;
                case (1<<3)|(0+2):  v = localW->r1_d0; break;
                case (1<<3)|(1+2):  v = localW->r1_d1; break;
                case (2<<3)|(-1+2): v = localW->r2_dm1; break;
                case (2<<3)|(0+2):  v = localW->r2_d0; break;
                case (2<<3)|(1+2):  v = localW->r2_d1; break;
                case (3<<3)|(0+2):  v = localW->r3_d0; break;
                default:
                    printf("READ ERROR r:%d d:%d\n", r, d);
            }
            

            //printf("local_RD(%ld,%ld) = %ld\n", d, r, v);
            return v;
        }
    #endif 
    
    #if TILE_LEN == 3
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
        
        void inline __attribute__((always_inline)) 
        writeLocalW(LOCAL_TILE_PTR localW, long d, long r, long v)
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
        
        long inline __attribute__((always_inline)) 
        readLocalW(LOCAL_TILE_PTR localW, long d, long r)
        {
            long v; 
            
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
        
    #endif
    
    
#endif

#ifdef SHARED_STORE
    #define LOCAL_TILE_TYPE     __local
    #define LOCAL_TILE_PTR      LOCAL_TILE_TYPE long*
#endif

/**
 * 
 * @param ld local diagonal coordinate
 * @param lr local radius coordinate
 * @param tileLen
 * @return 
 */
int inline __attribute__((always_inline)) 
isInLocalBlock(int ld, int lr)
{
    if (lr < 0) return 0;
    if (lr >= 2*TILE_LEN) return 0;
    
    int dec_lr = 2*TILE_LEN -1 - lr;
    int min_lr = min(lr, dec_lr);
    
    if (abs(ld) > min_lr) return 0;
    
    return 1;
}

// We assume we are already in a local block (tile)
int inline __attribute__((always_inline)) 
isInLocalBlockBoundary(int ld, int lr)
{
    int col = lr + abs(ld);
    const int col1 = 2*TILE_LEN - 1;
    const int col2 = col1-1;
    
    return ((col == col1) || (col == col2));
}


long inline __attribute__((always_inline)) 
extendUnaligned(__global const char* P, __global const char* T, long m, long n, long pi, long ti)
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

long inline __attribute__((always_inline)) 
extendAligned(__global const char* P, __global const char* T, long m, long n, long pi, long ti)
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
    int ret =  abs(d) <= r;
    return ret;
}



#define WRITE_W(d,r, v)         writeToW(m_W, localW, (d), (r), (v), m_k, tileLen, ld, lr)
#define READ_W(d,r, ld, lr)     (readFromW(m_W, localW, (d), (r), m_k, tileLen, ld, lr))


#ifdef REGISTER_STORE
    

    void inline __attribute__((always_inline)) 
    writeToW(__global long* m_W, LOCAL_TILE_PTR localW, long d, long r, long v, long m_k, int tileLen, int ld, int lr)
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
    void inline __attribute__((always_inline)) 
    writeToW(__global long* m_W, LOCAL_TILE_PTR localW, long d, long r, long v, long m_k, int tileLen, int ld, int lr)
    {    
        int lidx = POLAR_LOCAL_W_TO_INDEX(ld, lr);

        localW[lidx] = v;

        int inBoundary = isInLocalBlockBoundary(ld, lr);
        
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


#ifdef REGISTER_STORE
    
    
    long inline __attribute__((always_inline)) 
    readFromW(__global long* m_W, LOCAL_TILE_PTR localW, long d, long r, long m_k, int tileLen, int ld, int lr)
    {
        int isInLocal = isInLocalBlock(ld, lr);
        
        if (isInLocal)
        {
            long lv = readLocalW(localW, ld, lr);
            

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
#else
    long inline __attribute__((always_inline)) 
    readFromW(__global long* m_W, LOCAL_TILE_PTR localW, long d, long r, long m_k, int tileLen, int ld, int lr)
    {
        int isInLocal = isInLocalBlock(ld, lr);
        
        if (isInLocal)
        {
            int lidx = POLAR_LOCAL_W_TO_INDEX(ld, lr);
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
#endif


void inline __attribute__((always_inline)) 
processCell(__global char* P, 
        __global char* T, 
        long m_m, 
        long m_n,
        long m_k, 
        __global long* m_W,
        __global long* p_final_d_r,
        long d,
        long r,
        int tileLen,
        LOCAL_TILE_PTR localW,
        int ld,
        int lr,
        int* doRun,
        long m_top,
        long final_d)
{
    // printf("%ld  >= %ld ? \n", p_final_d_r[0], m_top);

    if (p_final_d_r[0] >= m_top)
    {
        //printf("%ld is >= TOP", p_final_d_r[0]);
        *doRun = 0;
        return;
    }


    // early exit for useless work items
    if (!polarExistsInW(d,r))
        return;
        
    long diag_up;
    long left;
    long diag_down;
    
    if (r > 0)
    {
        diag_up = READ_W(d+1, r-1, ld+1, lr-1);
        left = READ_W(d, r-1, ld, lr-1);
        diag_down = READ_W(d-1, r-1, ld-1, lr-1) ;
    }
            
    if (r == 0)
    {
        if (d == 0)
        {
            // initial case
            long extended = extend(P, T, m_m, m_n, 0, 0);
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
        diag_up = (polarExistsInW(d+1, r-1))? diag_up : 0;
        left = (polarExistsInW(d,r-1))? left : 0;
        diag_down = (polarExistsInW(d-1,r-1))? diag_down : 0;

        //printf("u|l|r = %ld|%ld|%ld\n",  diag_up, left, diag_down);

        long compute;

        if (d == 0)
            compute = max(diag_up, diag_down);
        else if (d > 0)
            compute = max(diag_up, diag_down+1);
        else
            compute = max(diag_up+1, diag_down);
            
        compute = max(left+1, compute);
            
        //printf("compute: %ld, u|l|r = %ld|%ld|%ld\n", compute, diag_up, left, diag_down);

        if ((d == final_d) && compute >= m_top)
        {
            //printf("COMPLETE\n");
            WRITE_W(d, r, compute);
            p_final_d_r[0] = compute;   // furthest reaching point
            p_final_d_r[1] = r;         // at edit distance = r
            *doRun = 0;
            return;
        }

        long ex = POLAR_W_TO_CARTESIAN_X(d, compute);
        long ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

        if ((ex < m_n) && (ey < m_m))
        {
            long extendv = extend(P, T, m_m, m_n, ey, ex);
            long extended = compute + extendv;

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
        {
            WRITE_W(d, r, compute);
            // it is impossible to assign the final result here, because it would
            // have been in the previous compute check
        }
    }

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
__kernel void wfo2cols(
        __global char* P, 
        __global char* T, 
        long m_m, 
        long m_n, 
        long r0, 
        long m_k,  
        __global long* m_W,
        __global long* p_final_d_r,
        int tileLen)
{
#ifdef GLOBAL_STORE
    LOCAL_TILE_TYPE long localW[2*TILE_LEN*TILE_LEN];    
    #define GET_LOCAL_TILE_REF
#endif

#ifdef REGISTER_STORE
    LOCAL_TILE localW;
    #define GET_LOCAL_TILE_REF &
#endif
    
#ifdef SHARED_STORE
    LOCAL_TILE_TYPE long shared_localW[WORKGROUP_SIZE][2*TILE_LEN*TILE_LEN];
    size_t lid = get_local_id(0);
    LOCAL_TILE_TYPE long* localW = &shared_localW[lid][0];
    #define GET_LOCAL_TILE_REF
#endif

    size_t gid = get_global_id(0);

    //long d = gid - (r-1);
    long d0 = r0 - gid*2*TILE_LEN; 
    long m_top = max2(m_m,m_n);
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    int doRun = 1;
    
    if (abs(d0) > r0)
        doRun = 0;

#ifdef DEBUG
    printf("\ngid: %ld - final_d: %ld  d0: %ld run: %d\n", gid, final_d, d0, doRun);
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
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+i, tileLen, GET_LOCAL_TILE_REF localW, j, i, &doRun, m_top, final_d);
    
    // Decrease
    for (int i=0 ; ((i < tileLen) && (doRun)); i++)
    {
        int ii = tileLen - 1 -i;
        
        for (int j=-ii; ((j <= ii) && (doRun)); j++)
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+tileLen+i, tileLen, GET_LOCAL_TILE_REF localW, j, tileLen+i, &doRun, m_top, final_d);
    }
}
