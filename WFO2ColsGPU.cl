
#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
//#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)		(((y)>(x))? (x) : (y))

//#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)),w)

#define POLAR_W_TO_INDEX(d, r)                  ((d)+m_k + (((r)%2) * (2*m_k+1)))

#define POLAR_LAST_R_TO_INDEX(d)                ((d)+m_k + (2 * (2*m_k+1)))

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

#define WRITE_W(d,r,v)      m_W[POLAR_W_TO_INDEX((d), (r))] = (v)
#define READ_W(d, r)        m_W[POLAR_W_TO_INDEX((d), (r))] 

//#define WRITE_LAST_R(d,v)   ((volatile long*) m_W)[POLAR_LAST_R_TO_INDEX(d)] = (v)
//#define READ_LAST_R(d)      ((volatile long*) m_W)[POLAR_LAST_R_TO_INDEX(d)]



int processCell(__global char* P,  __global char* T, long m_m, long m_n, 
    __global long* m_W, 
    __local long* localW, 
    __global long* p_final_d_r,
     long d, long r, long final_d, long m_k, long m_top)
{
    size_t gid = get_global_id(0);
    size_t lid = get_local_id(0);
    
    //printf("t%ld - r0:%ld executing r:%ld d:%ld\n", (long) gid, r0, r, d);
    //printf("d:%ld last r: %ld < %ld\n", d, READ_LAST_R(d), (r-1) );

    
    // early exit for useless work items
    if (!polarExistsInW(d,r))
    {
        //printf("skip r:%ld d:%ld\n", r, d);
        return 0;
    }
       
    if (r == 0)
    {
        if (d == 0)
        {
            // initial case
            long extended = extend(P, T, m_m, m_n, 0, 0);
            WRITE_W(d, r, extended);
            //WRITE_LAST_R(d, r);
            
            //printf("W(%ld, %ld) = %ld\n", d, r, extended);
            
            if ((d == final_d) && extended >= m_top)
            {
                printf("COMPLETE: distance %ld t%ld\n", r, gid);
                printf("W(%ld, %ld) = %ld\n", d, r, extended);
                p_final_d_r[0] = extended;  // furthest reaching point
                p_final_d_r[1] = r;         // at edit distance = r
                return 1;
            }
        }
        else
        {
            //printf("W(%ld, %ld) = %ld\n", d, r, 0);
            WRITE_W(d, r, 0);
            //WRITE_LAST_R(d, r);
        }  
    }
    else
    {
        /*
        long ldm1 = READ_LAST_R(d-1);
        long ld = READ_LAST_R(d);
        long ldp1 = READ_LAST_R(d+1);
        
        // last computed row must reach r-1 before continuing
        while ((ldm1 <(r-1)) || (ld <(r-1)) || (ldp1 <(r-1)))
        {
            if (ldm1 <(r-1))
            {
                //printf("#,t,%ld,%ld,d,%ld, Diagonal d:%ld reached r: %ld instead of  %ld -> Compute (%ld, %ld)\n", gid, r, d, d-1, ldm1, (r-1), d-1, r-1 );
                
                //processCellNoRecursive(P, T, m_m, m_n, m_W, p_final_d_r, d-1, r-1, final_d, m_k, m_top);
            }
            
            if (ld <(r-1))
            {
                //printf("#,t,%ld,%ld,d,%ld, Diagonal d:%ld reached r: %ld instead of  %ld -> Compute (%ld, %ld)\n", gid, r, d, d, ld, (r-1), d, r-1 );
                //processCellNoRecursive(P, T, m_m, m_n, m_W, p_final_d_r, d, r-1, final_d, m_k, m_top);
            }
            
            if (ldp1 <(r-1))
            {
                //printf("#,t,%ld,%ld,d,%ld, Diagonal d:%ld reached r: %ld instead of  %ld -> Compute (%ld, %ld)\n", gid, r, d, d+1, ldp1, (r-1), d+1, r-1 );
                //processCellNoRecursive(P, T, m_m, m_n, m_W, p_final_d_r, d+1, r-1, final_d, m_k, m_top);
            }
            
            while ((ldm1 <(r-1)) || (ld <(r-1)) || (ldp1 <(r-1)))
            {
                ldm1 = READ_LAST_R(d-1);
                ld = READ_LAST_R(d);
                ldp1 = READ_LAST_R(d+1);
            
            
            // return;
//                    printf(".");
            // stop if we already finished
                if (p_final_d_r[0] >= m_top)
                {
                    //printf("Completed in another thread: %ld >= %ld\n", p_final_d_r[0] , m_top);
                    return 1;
                }
            }
        }*/

        
        long diag_up = (polarExistsInW(d+1, r-1))? ((lid < 96)? localW[lid+1] : READ_W(d+1, r-1)) : 0;
        long left = (polarExistsInW(d,r-1))? localW[lid] /*READ_W(d, r-1)*/ : 0;
        long diag_down = (polarExistsInW(d-1,r-1))? ((lid > 0)? localW[lid-1] : READ_W(d-1, r-1)) : 0;

        long compute;

        if (d == 0)
            compute = max3(diag_up, left+1, diag_down);
        else if (d > 0)
            compute = max3(diag_up, left+1, diag_down+1);
        else
            compute = max3(diag_up+1, left+1, diag_down);

        if ((d == final_d) && compute >= m_top)
        {
            printf("COMPLETE: distance %ld t%ld\n", r, gid);
            printf("W(%ld, %ld) = %ld\n", d, r, compute);
            WRITE_W(d, r, compute);
            //WRITE_LAST_R(d, r);
            p_final_d_r[0] = compute;
            p_final_d_r[1] = r;
            return 1;
            // ret = r;
            // goto end_loop;
        }

        long ex = POLAR_W_TO_CARTESIAN_X(d, compute);
        long ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

        if ((ex < m_n) && (ey < m_m))
        {
            long extendv = extend(P, T, m_m, m_n, ey, ex);
            long extended = compute + extendv;

            //printf("W(%ld, %ld) = %ld\n", d, r, extended);
            WRITE_W(d, r, extended);
            //WRITE_LAST_R(d, r);

            if ((d == final_d) && extended >= m_top)
            {
                printf("COMPLETE: distance %ld t%ld\n", r, gid);
                printf("W(%ld, %ld) = %ld\n", d, r, extended);
                p_final_d_r[0] = extended;
                p_final_d_r[1] = r;
                return 1;
                // printf("Finishing d=%d r=%d\n", d, r);
                //ret = r;
                // goto end_loop;
            }
        }
        else
        {
            //printf("W(%ld, %ld) = %ld\n", d, r, compute);
            WRITE_W(d, r, compute);
            //WRITE_LAST_R(d, r);
        }
    }
    
    return 0;
}

/**
 * This is the initial kernel version.
 * The host will create as many work items as the height of the column W
 * most of them will die soon with nothing useful to do
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
        long rf,
        int stride)
{
    size_t gid = get_global_id(0);
    size_t lid = get_local_id(0);
    
    // we launch 96000 threads using workgroups of 96, if we copy the W value in local memory, the threads of the workgroup can
    // access that value instead of going to local memory. 
    __local long localW[96];  
    
    //printf("t%ld - kernel r0:%ld rf:%ld stride: %d\n", gid, r0, rf, stride);
    
//    long d = gid - r0; 
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long m_top = max2(m_m,m_n);

    for (long r=r0; r < rf; r++)
    {
        //printf("t%ld - r:%ld/%ld\n", (long) gid, r, rf);
                
        long roundedr = ((r + stride - 1 ) / stride) * stride;
                
        for (long d=gid-roundedr; d <= r; d += stride)
        {
            // printf("#,t,%ld,%ld,d,%ld\n", gid, r, d);
            
            if (r > 0)
                localW[lid] = READ_W(d ,r-1);
        
            barrier(CLK_LOCAL_MEM_FENCE);
            
            if (processCell(P, T, m_m, m_n,  m_W, localW, p_final_d_r, d, r, final_d, m_k, m_top))
                return;
                
            barrier(CLK_LOCAL_MEM_FENCE);
        }
        
        
    }
    
}
