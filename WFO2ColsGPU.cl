
#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
//#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)		(((y)>(x))? (x) : (y))

//#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)),w)

#define POLAR_W_TO_INDEX(d, r)                  ((d)+m_k + (((r)%(2*tileLen)) * (2*m_k+1)))

#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))

#define POLAR_LOCAL_W_TO_INDEX(d, r, tl) ((r) >= (tl))? (2*((r)-(tl))*(tl)) - ((r)-(tl))*((r)-(tl)) + (2*(tl)-(r)-1) + (d) + (tl)*(tl) : ((r)*(r)+(r)+d)

#define max2(a,b) (((a)>(b))?(a):(b))
#define max3(a,b,c) max2(a, max2(b, c))

#define LOCAL_STORE 

/**
 * 
 * @param ld local diagonal coordinate
 * @param lr local radius coordinate
 * @param tileLen
 * @return 
 */
int inline __attribute__((always_inline)) isInLocalBlock(int ld, int lr, int tileLen)
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

int inline __attribute__((always_inline)) isInLocalBlockBoundary(int ld, int lr, int tileLen)
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



long inline __attribute__((always_inline)) extend(__global const char* P, __global const char* T, long m, long n, long pi, long ti)
{    
#ifdef EXTEND_ALIGNED
        return extendAligned(P, T, m, n, pi, ti);
#else
        return extendUnaligned(P, T, m, n, pi, ti);
#endif
}

int inline __attribute__((always_inline)) polarExistsInW(long d, long r)
{
    long x = POLAR_W_TO_CARTESIAN_X(d,r);
    long y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
    return ((x >= 0) && (y >= 0));
}


#define WRITE_W(d,r, v) writeToW(m_W, localW, (d), (r), (v), m_k, tileLen, ld, lr)
#define READ_W(d,r, ld, lr)     (readFromW(m_W, localW, (d), (r), m_k, tileLen, ld, lr))


void inline __attribute__((always_inline)) writeToW(__global long* m_W, LOCAL_STORE long* localW, long d, long r, long v, long m_k, int tileLen, int ld, int lr)
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

long inline __attribute__((always_inline)) readFromW(__global long* m_W, LOCAL_STORE long* localW, long d, long r, long m_k, int tileLen, int ld, int lr)
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

void inline __attribute__((always_inline)) processCell(__global char* P, 
        __global char* T, 
        long m_m, 
        long m_n,
        long m_k, 
        __global long* m_W,
        __global long* p_final_d_r,
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
    if (!polarExistsInW(d,r))
        return;
            
    if (r == 0)
    {
        if (d == 0)
            // initial case
            WRITE_W(d, r, extend(P, T, m_m, m_n, 0, 0));
        else
            WRITE_W(d, r, 0);
    }
    else
    {
        long diag_up = (polarExistsInW(d+1, r-1))? READ_W(d+1, r-1, ld+1, lr-1) : 0;
        long left = (polarExistsInW(d,r-1))? READ_W(d, r-1, ld, lr-1) : 0;
        long diag_down = (polarExistsInW(d-1,r-1))? READ_W(d-1, r-1, ld-1, lr-1) : 0;

        long compute;

        if (d == 0)
            compute = max3(diag_up, left+1, diag_down);
        else if (d > 0)
            compute = max3(diag_up, left+1, diag_down+1);
        else
            compute = max3(diag_up+1, left+1, diag_down);

        if ((d == final_d) && compute >= m_top)
        {
//            printf("COMPLETE\n");
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
//                printf("COMPLETE\n");
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
    LOCAL_STORE long localW[2*TILE_LEN*TILE_LEN];

    size_t gid = get_global_id(0);

    //long d = gid - (r-1);
    long d0 = r0 - gid*2*tileLen; 
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
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+i, tileLen, localW, j, i, &doRun);
    
    // Decrease
    for (int i=0 ; ((i < tileLen) && (doRun)); i++)
    {
        int ii = tileLen - 1 -i;
        
        for (int j=-ii; ((j <= ii) && (doRun)); j++)
            processCell(P, T, m_m, m_n, m_k, m_W, p_final_d_r, d0+j, r0+tileLen+i, tileLen, localW, j, tileLen+i, &doRun);
    }
}
