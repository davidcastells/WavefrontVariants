
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

long extend(__global const char* P, __global const char* T, INT_TYPE m, INT_TYPE n, INT_TYPE pi, INT_TYPE ti)
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


int polarExistsInW(INT_TYPE d, INT_TYPE r)
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
 * @param r radius of the W column to compute
 * @param m_k max number of errors we are going to cover
 * @param m_W memory for the 2 columns of the W pyramid
 * @param furthest reaching radius 
 */
__kernel void wfo2cols(
        __global char* P, 
        __global char* T, 
        INT_TYPE m_m, 
        INT_TYPE m_n, 
        INT_TYPE r, 
        INT_TYPE m_k,  
        __global INT_TYPE* m_W,
        __global INT_TYPE* p_final_d_r)
{
    size_t gid = get_global_id(0);
    size_t lid = get_local_id(0);
    
#ifdef SHARED_STORE
    __local INT_TYPE localW[WORKGROUP_SIZE];
#endif
    
    //long d = gid - (r-1);
    INT_TYPE d = r - gid; 
    INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    INT_TYPE m_top = max2(m_m,m_n);
    
    INT_TYPE diag_up;
    INT_TYPE left;
    INT_TYPE diag_down; 
        
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
            INT_TYPE extended = extend(P, T, m_m, m_n, 0, 0); 
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
        INT_TYPE compute;

        if (d == 0)
            compute = max(diag_up, diag_down);
        else if (d > 0)
            compute = max(diag_up, diag_down+1);
        else
            compute = max(diag_up+1, diag_down);
            
        compute = max(left+1, compute);

        if ((d == final_d) && compute >= m_top)
        {
            m_W[POLAR_W_TO_INDEX(d, r)] = compute;
            p_final_d_r[0] = compute;   // furthest reaching point
            p_final_d_r[1] = r;         // at edit distance = r
            return;
        }

        INT_TYPE ex = POLAR_W_TO_CARTESIAN_X(d, compute);
        INT_TYPE ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

        if ((ex < m_n) && (ey < m_m))
        {
            INT_TYPE extendv = extend(P, T, m_m, m_n, ey, ex);
            INT_TYPE extended = compute + extendv;

            m_W[POLAR_W_TO_INDEX(d, r)] = extended;

            if ((d == final_d) && extended >= m_top)
            {
                p_final_d_r[0] = extended;  // furthest reaching point
                p_final_d_r[1] = r;         // at edit distance = r
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
