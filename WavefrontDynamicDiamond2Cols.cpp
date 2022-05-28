#include "WavefrontDynamicDiamond2Cols.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <new>

#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
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

WavefrontDynamicDiamond2Cols::WavefrontDynamicDiamond2Cols()
{
}

WavefrontDynamicDiamond2Cols::~WavefrontDynamicDiamond2Cols()
{
	delete [] m_W;
}



static long extend(const char* P, const char* T, long m, long n, long pi, long ti)
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

static long polarExistsInD(long d, long r)
{
	long x = POLAR_D_TO_CARTESIAN_X(d,r);
	long y = POLAR_D_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y>=0));
}

static long polarExistsInW(long d, long r)
{
	long x = POLAR_W_TO_CARTESIAN_X(d,r);
	long y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y >= 0));
}

void WavefrontDynamicDiamond2Cols::setInput(const char* P, const char* T, INT_TYPE k)
{                       
	m_m = strlen(P);
	m_n = strlen(T);
	m_top = max2(m_m,m_n);	// Maximum distance value

	m_k = m_top;

	long size = 2*(2*m_k+1);
	
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
	m_P = P;
	m_T = T;
	
	printf("input set\n");
	
	printf("Initial limit: %ld\n", m_k);

}

INT_TYPE WavefrontDynamicDiamond2Cols::getDistance()
{
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long ret;
	
    long kt = m_k;
	
    // for the first element, just execute the extend phase
    m_W[0] = extend(m_P, m_T, m_m, m_n, 0, 0);
	
    if (m_W[0] >= m_top)
        goto end_loop;
	
    // opening the diamond
    for (long r=1; r < kt; r++)
    {
            if (verbose)
                    printf("\rr %ld/%ld %.2f%%", r, kt, (r*100.0/kt) );

            long k_odd = kt % 2;
            long k_half = kt/2;

            //printf("[%ld] half: %ld \n", r, k_half);

            if (r  <= k_half)
            {

                    for (long d=-r; d <= r; d++)
                    {			
                            long abs_d = max2(final_d-d,d-final_d);

                            long diag_up = (polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1)]  : 0;
                            long left = (polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1)]  : 0;
                            long diag_down = (polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1)]  : 0;

                            int compute;

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

                            int ex = POLAR_W_TO_CARTESIAN_X(d, compute);
                            int ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

                            if ((ex < m_n) && (ey < m_m))
                            {
                                    //printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  || ", d, r , diag_up, left, diag_down, compute );

                                    long extendv = extend(m_P, m_T, m_m, m_n, ey, ex);
                                    long extended = compute + extendv;

                                    m_W[POLAR_W_TO_INDEX(d, r)] = extended;

                                    if ((r + m_top + abs_d - extended)  < kt)
                                    {
                                            kt = r + m_top + abs_d - extended;
                                    }


                                    //printf("(Extended)  + %d = %d (P[%d]=%s - T[%d]=%s)\n",  extendv, extended, ey, &m_P[ey], ex, &m_T[ex] );
                                    //printf("(Extended)  + %d = %d (P[%d] - T[%d])\n",  extendv, extended, ey,  ex );


                                    if ((d == final_d) && extended >= m_top)
                                    {
                                            //printf("Finishing d=%d r=%d\n", d, r);
                                            ret = r;
                                            goto end_loop;
                                    }
                            }
                            else
                            {
                                    m_W[POLAR_W_TO_INDEX(d, r)] = compute;

                                    /*if ((r + m_top + abs_d - compute)  < kt)
                                    {
                                            kt = r + m_top + abs_d - compute;
                                    }*/
                            }
                    }
            }

            // closing the diamond
            //if ( r >= k_half)
            else
            {
                    //printf("[%ld]\n", r);

                    long cr = kt - (r  + k_odd); // 5 - (2 + 1) = 5 -3 = 2

                    for (long d=-cr; d <= cr; d++)
                    {			
                            long abs_d = max2(final_d-d,d-final_d);

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
                                    ret = r;
                                    goto end_loop;
                            }

                            long ex = POLAR_W_TO_CARTESIAN_X(d, compute);
                            long ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

                            if ((ex < m_n) && (ey < m_m))
                            {
                                    //printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  || ", d, r , diag_up, left, diag_down, compute );

                                    long extendv = extend(m_P, m_T, m_m, m_n, ey, ex);
                                    long extended = compute + extendv;

                                    m_W[POLAR_W_TO_INDEX(d, r)] = extended;

                                    if ((r + m_top + abs_d - extended)  < kt)
                                    {
                                            kt = r + m_top + abs_d - extended;
                                    }

                                    //printf("(Extended)  + %d = %d (P[%d]=%s - T[%d]=%s)\n",  extendv, extended, ey, &m_P[ey], ex, &m_T[ex] );
                                    //printf("(Extended)  + %d = %d (P[%d] - T[%d])\n",  extendv, extended, ey,  ex );


                                    if ((d == final_d) && extended >= m_top)
                                    {
                                            //printf("Finishing d=%d r=%d\n", d, r);
                                            ret = r;
                                            goto end_loop;
                                    }
                            }
                            else
                            {
                                    m_W[POLAR_W_TO_INDEX(d, r)] = compute;

                                    /*
                                    if ((r + m_top + abs_d - compute)  < kt)
                                    {
                                            kt = r + m_top + abs_d - compute;
                                    }*/

                            }
                    }
            }
    }

    ret = kt;

end_loop:
    if (verbose)
            printf("\n");

    return ret;
}



const char* WavefrontDynamicDiamond2Cols::getDescription()
{
	return "Wavefront Dynamic Diamond 2 columns";
}

char* WavefrontDynamicDiamond2Cols::getAlignmentPath(INT_TYPE* distance)
{
	printf("Alignment Not implemented yet!\n");
	exit(0);
}

