
#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)	((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)	(((y)>(x))? (x) : (y))

#define POLAR_W_TO_INDEX(d, r)          ((d)+ROW_ZERO_OFFSET + (((r)%2) * COLUMN_HEIGHT))

#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)    ((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)	(((y)>(x))? (y) : (x))


#define max2(a,b) (((a)>(b))?(a):(b))
#define max3(a,b,c) max2(a, max2(b, c))

INT_TYPE extend(__global const char* P, __global const char* T, INT_TYPE m, INT_TYPE n, INT_TYPE pi, INT_TYPE ti)
{
    INT_TYPE e = 0;

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
    return abs(d) <= r;
}

#define LOCAL_R(r)  ((2+r)%2)
#define LOCAL_D(d)  ((d)+TILE_LEN)

//#define TRACE_WRITES

void  processCell(__global char* P,     
        __global char* T,   
        INT_TYPE m_m,           
        INT_TYPE m_n, 
        __local INT_TYPE localW[2][TILE_LEN*2+1], 
        size_t gid, 
        size_t lid, 
        INT_TYPE r,
        INT_TYPE d, 
        int lr, int ld, 
        int* pdone,
        __global INT_TYPE* p_final_d_r, 
        INT_TYPE m_top, 
        INT_TYPE final_d,
        INT_TYPE tile,
	INT_TYPE r0)
{
    INT_TYPE diag_up;
    INT_TYPE left;
    INT_TYPE diag_down; 
    
    int lup;
    int ldown;
    
    
    int done = *pdone;
    
    if (done)
        return;

    if (lr < TILE_LEN)
    {
        lup = lr;
        ldown = -lr;
    }
    else
    {
        lup   = 2*TILE_LEN - lr -1;
        ldown = -(2*TILE_LEN - lr -1);
    }
    
    // printf("r:%ld lr:%d - d:%ld ld: %d [%d, %d]\n", r, lr, d, ld, lup, ldown);

    if ((ld >= ldown) && (ld <= lup))
    {
        //printf("up: %d, %d\n", LOCAL_R(lr-1), LOCAL_D(ld+1));
        
        // this cell must be computed
        diag_up = localW[LOCAL_R(lr-1)][LOCAL_D(ld+1)];
        left = localW[LOCAL_R(lr-1)][LOCAL_D(ld)];
        diag_down = localW[LOCAL_R(lr-1)][LOCAL_D(ld-1)];
                
        diag_up = (polarExistsInW(d+1, r-1))?  diag_up  : 0; 
        left = (polarExistsInW(d,r-1))? left : 0;
        diag_down = (polarExistsInW(d-1,r-1))?  diag_down  : 0; 
        
        INT_TYPE compute;

        if (d == 0)
            compute = max(diag_up, diag_down);
        else if (d > 0)
            compute = max(diag_up, diag_down+1);
        else
            compute = max(diag_up+1, diag_down);
         
            
        if (r == 0)
        {
            compute = 0;
            //printf("compute = 0\n");
        }
        else
            compute = max(left+1, compute);
            

        if ((done == 0) && (d == final_d) )
        {
        
            //printf("tile: %d LWR(%d, %d) GWR(%d,%d) c: %d \n", tile, lr, ld, r,d ,compute);

            localW[LOCAL_R(lr)][LOCAL_D(ld)] = compute;
            p_final_d_r[0] = compute;   // furthest reaching point
            p_final_d_r[1] = r;         // at edit distance = r
            
            if (compute >= m_top)
            {
#ifdef TRACE_WRITES
                printf("tile: (%d,%d) LWR(%d, %d) GWR(%d,%d) c: %d \n", r0/TILE_LEN, tile, lr, ld, r,d ,compute);
                //printf("COMPLETE\n");
#endif

                *pdone = 1;
                return;
            }
        }
        
        INT_TYPE ex = POLAR_W_TO_CARTESIAN_X(d, compute);
        INT_TYPE ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

        if ((ex < m_n) && (ey < m_m))
        {
            INT_TYPE extendv = extend(P, T, m_m, m_n, ey, ex);
            INT_TYPE extended = compute + extendv;

            // printf("WR %ld\n", extended);

            localW[LOCAL_R(lr)][LOCAL_D(ld)] = extended;
#ifdef TRACE_WRITES
            printf("tile: (%d,%d) LWR(%d, %d) GWR(%d,%d) e: %d \n", r0/TILE_LEN, tile, lr, ld, r, d, extended);
#endif

            if ((done == 0) && (d == final_d) )
            {
                p_final_d_r[0] = extended;  // furthest reaching point
                p_final_d_r[1] = r;         // at edit distance = r
                
                if (extended >= m_top)
                {
                    //printf("COMPLETE\n");
                    *pdone = 1;
                    return;
                }
            }
        }
        else
        {
#ifdef TRACE_WRITES
            printf("tile: (%d,%d) LWR(%d, %d) GWR(%d,%d) e: %d \n", r0/TILE_LEN, tile, lr, ld, r,d ,compute);
#endif            
            localW[LOCAL_R(lr)][LOCAL_D(ld)] = compute;
            // it is impossible to assign the final result here, because it would
            // have been in the previous compute check
        }    
    }      
}        

__kernel void resetW(__global INT_TYPE* m_W)
{
    size_t gid = get_global_id(0);
    
    if (gid >= COLUMN_HEIGHT*2)
        return; 
    
    m_W[gid] = 0;
    // m_W[POLAR_W_TO_INDEX(gid, 1)] = 0;
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
 * @param r radius of the W column to compute
 * @param m_k max number of errors we are going to cover
 * @param m_W memory for the 2 columns of the W pyramid
 * @param furthest reaching radius 
 */
__kernel void wfdd2cols(
        __global char* P,   // 0  
        __global char* T,   // 1
        INT_TYPE m_m,           // 2
        INT_TYPE m_n, 
        INT_TYPE r0, 
        INT_TYPE m_k,  
        __global INT_TYPE* m_W,
        __global INT_TYPE* p_final_d_r,
        INT_TYPE dstart
        )
{
    size_t gid = get_global_id(0);
    size_t lid = get_local_id(0);
    
#ifdef SHARED_STORE
    //__local long localW[WORKGROUP_SIZE];
    
    __local INT_TYPE localW[2][TILE_LEN*2+1];   //  column, diagonal
#endif
    
    //long d = gid - (r-1);
    INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    INT_TYPE m_top = max2(m_m,m_n);
    
    int ld = -TILE_LEN + lid;
    INT_TYPE tile = gid / (TILE_LEN*2 + 1);
    INT_TYPE d = dstart + 2 * TILE_LEN * tile + ld;


    if (gid == 0)
    {
        // printf("m_top = %ld final_d= %ld\n", m_top, final_d);
    }
    
    if (p_final_d_r[0] >= m_top)
        return;

    // first, fetch the local values
    localW[LOCAL_R(0)][LOCAL_D(ld)] = m_W[POLAR_W_TO_INDEX(d, r0%2)];
    localW[LOCAL_R(1)][LOCAL_D(ld)] = m_W[POLAR_W_TO_INDEX(d, (r0+1)%2)];
    
    //printf("t[%ld][%ld] - r:%ld tile:%ld fetching d:%ld ld:%d\n", gid, lid, r0, tile,  d, ld);

    barrier(CLK_LOCAL_MEM_FENCE);

    //printf("t[%ld][%ld] - r0: %ld   d:%ld\n", gid, lid, r0, d);


    
    int done = 0;

    // Now compute the tile using only shared memory    
    for (int lr = 0; lr < 2*TILE_LEN; lr++)
    {
        INT_TYPE r = r0 + lr;

        //printf("t[%ld][%ld] - r: %ld   d:%ld  lr: %d ld: %d\n", gid, lid, r, d, lr, ld);

        processCell(P, T, m_m, m_n, localW, gid, lid, r, d, lr, ld, &done, p_final_d_r, m_top, final_d, tile, r0);
        
        barrier(CLK_LOCAL_MEM_FENCE);

    }
    
    
    if (abs(ld) < TILE_LEN)
    {
        // printf("global WR(%ld, %d) = %ld\n", d, 0, localW[LOCAL_R(0)][LOCAL_D(ld)]);
        // printf("global WR(%ld, %d) = %ld\n", d, 1, localW[LOCAL_R(1)][LOCAL_D(ld)]);
    
        m_W[POLAR_W_TO_INDEX(d, r0%2)] = localW[LOCAL_R(0)][LOCAL_D(ld)]; 
        m_W[POLAR_W_TO_INDEX(d, (r0+1)%2)] = localW[LOCAL_R(1)][LOCAL_D(ld)];
    }   
    
}
