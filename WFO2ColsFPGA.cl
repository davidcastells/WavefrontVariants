
#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
//#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)		(((y)>(x))? (x) : (y))

//#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)),w)

#define POLAR_W_TO_INDEX(d, r)                  ((d)+m_k + (((r)%2) * (2*m_k+1)))

#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))


#define max2(a,b) (((a)>(b))?(a):(b))
#define max3(a,b,c) max2(a, max2(b, c))

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
        __global char* restrict P, 
        __global char* restrict T, 
        long m_m, 
        long m_n, 
        long r, 
        long m_k,  
        __global long* m_W,
        __global long* p_final_d_r)
{
    size_t gid = get_global_id(0);
    
    //long d = gid - (r-1);
    long d = r - gid; 
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long m_top = max2(m_m,m_n);
    
    // we already reached the final point in previous invocations
    if (p_final_d_r[0] >= m_top)
        return;
    
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
        long diag_up = (polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1)]  : 0;
        long left = (polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1)]  : 0;
        long diag_down = (polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1)]  : 0;

        long compute;

        if (d == 0)
            compute = max3(diag_up, left+1, diag_down);
        else if (d > 0)
            compute = max3(diag_up, left+1, diag_down+1);
        else
            compute = max3(diag_up+1, left+1, diag_down);

        if ((d == final_d) && compute >= m_top)
        {
            m_W[POLAR_W_TO_INDEX(d, r)] = compute;
            p_final_d_r[0] = compute;   // furthest reaching point
            p_final_d_r[1] = r;         // at edit distance = r
            return;
        }

        long ex = POLAR_W_TO_CARTESIAN_X(d, compute);
        long ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

        if ((ex < m_n) && (ey < m_m))
        {
            long extendv = extend(P, T, m_m, m_n, ey, ex);
            long extended = compute + extendv;

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
