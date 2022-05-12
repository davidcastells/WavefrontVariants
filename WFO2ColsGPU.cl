
#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
//#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)		(((y)>(x))? (x) : (y))

//#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)),w)

//#define POLAR_W_TO_INDEX(d, r)                  ((d)+m_k + (((r)%2) * (2*m_k+1)))
#define POLAR_W_TO_INDEX(d, r)          ((d)+ROW_ZERO_OFFSET + (((r)%2) * COLUMN_HEIGHT))

#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))


#define max2(a,b) (((a)>(b))?(a):(b))
#define max3(a,b,c) max2(a, max2(b, c))

#define INT_TYPE int

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


int polarExistsInW(INT_TYPE d, INT_TYPE r)
{
    int ret =  abs(d) <= r;
    return ret;
}


/**
 * This is the initial kernel version.
 * The host will create as many work items as the height of the column W
 * most of them will die soon with nothing useful to do
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
    
    //long d = gid - (r-1);
    INT_TYPE d = m_k - gid; 
    INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    INT_TYPE m_top = max2(m_m,m_n);
            
    // early exit for useless work items
    if (!polarExistsInW(d,r))
        return;
            
    if (r == 0)
    {
        if (d == 0)
            // initial case
            m_W[POLAR_W_TO_INDEX(d, r)] = extend(P, T, m_m, m_n, 0, 0);
        else
            m_W[POLAR_W_TO_INDEX(d, r)]  = 0;
    }
    else
    {
        INT_TYPE diag_up = (polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1)]  : 0;
        INT_TYPE left = (polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1)]  : 0;
        INT_TYPE diag_down = (polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1)]  : 0;

        INT_TYPE compute;

        if (d == 0)
            compute = max3(diag_up, left+1, diag_down);
        else if (d > 0)
            compute = max3(diag_up, left+1, diag_down+1);
        else
            compute = max3(diag_up+1, left+1, diag_down);

        if ((d == final_d) && compute >= m_top)
        {
            m_W[POLAR_W_TO_INDEX(d, r)] = compute;
            *p_final_d_r = compute;
            return;
            // ret = r;
            // goto end_loop;
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
                *p_final_d_r = extended;
                return;
                // printf("Finishing d=%d r=%d\n", d, r);
                //ret = r;
                // goto end_loop;
            }
        }
        else
        {
            m_W[POLAR_W_TO_INDEX(d, r)] = compute;
        }
    }

}
