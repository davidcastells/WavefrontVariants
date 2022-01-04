
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

int isInLocalBlock(int ld, int lr, int tileLen)
{
    // return whether the cell is in the local memory 
    int idx = POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen);
    return ((idx >= 0) && (idx <= 2*tileLen*tileLen));
}

int isInLocalBlockBoundary(int ld, int lr, int tileLen)
{
    int lrp = (lr >= tileLen) ? 2*tileLen-lr-1 : lr;
    return (abs(ld) == lrp);
}

long extend(__global const char* P, __global const char* T, long m, long n, long pi, long ti)
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


int polarExistsInW(long d, long r)
{
    long x = POLAR_W_TO_CARTESIAN_X(d,r);
    long y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
    return ((x >= 0) && (y >= 0));
}

void processCell(__global char* P, __global char* T, long m_m,  long m_n, long m_k, __global long* m_W, __global long* p_final_d_r, long d, long r, int tileLen, __local long* localW, int ld, int lr, int* doRun);

/**
 * This is the initial kernel version.
 * The host will create as many work items as the height of the column W
 * most of them will die soon with nothing useful to do
 * 
 * @param P the pattern
 * @param T the text
 * @param m_m the length of the pattern
 * @param m_n the length of the text
 * @param r radius of the W column to compute
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
    __local long localW[2*TILE_LEN*TILE_LEN];

    size_t gid = get_global_id(0);

    //long d = gid - (r-1);
    long d0 = r0 - gid*2*tileLen; 
    long m_top = max2(m_m,m_n);
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    int doRun = 1;

#ifdef DEBUG
    printf("\ngid: %ld - final_d: %ld \n", gid, final_d);
#endif
    // printf("\n[POCL] d0=%ld r0=%ld  cv=%ld\n", d0, r0, p_final_d_r[0]);
    
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

#define WRITE_W(d,r, v) writeToW(m_W, localW, (d), (r), (v), m_k, tileLen, ld, lr)
#define READ_W(d,r, ld, lr)     (readFromW(m_W, localW, (d), (r), m_k, tileLen, ld, lr))


void writeToW(__global long* m_W, __local long* localW, long d, long r, long v, long m_k, int tileLen, int ld, int lr)
{
    m_W[POLAR_W_TO_INDEX(d, r)] = v;
    
    int idx = POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen);
    
#ifdef DEBUG
    printf("WR(%ld, %ld, %d) -> local WR(%d, %d, %d) -> WR idx(%d) = %ld\n", 
            d, r, tileLen, 
            ld, lr, tileLen, idx, v);
#endif
    
    //if (isInLocalBlockBoundary(ld, lr, tileLen))
//    {
//        localW[POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen)] = v;
//    }
}

long readFromW(__global long* m_W, __local long* localW, long d, long r, long m_k, int tileLen, int ld, int lr)
{
    int idx = POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen);

    long v = m_W[POLAR_W_TO_INDEX(d, r)];
    
#ifdef DEBUG
//    printf("RD(%ld, %ld, %d) -> local WR(%d, %d, %d) -> WR idx(%d) = %ld\n", 
//            d, r, tileLen, 
//            ld, lr, tileLen, idx, v);
#endif
    
//    if (isInLocalBlock(ld, lr, tileLen))
//    {
//        long v = localW[POLAR_LOCAL_W_TO_INDEX(ld, lr, tileLen)];
//
////        if (v != m_W[POLAR_W_TO_INDEX(d, r)])
////            printf("ERROR! in %ld, %ld = %ld\n", d, r, v);
//        
//        return v;
//    }
//    else
        return m_W[POLAR_W_TO_INDEX(d, r)];
}

void processCell(__global char* P, 
        __global char* T, 
        long m_m, 
        long m_n,
        long m_k, 
        __global long* m_W,
        __global long* p_final_d_r,
        long d,
        long r,
        int tileLen,
        __local long* localW,
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