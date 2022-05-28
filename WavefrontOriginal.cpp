#include "WavefrontOriginal.h"
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

#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)),w)

#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))

WavefrontOriginal::WavefrontOriginal()
{
}

WavefrontOriginal::~WavefrontOriginal()
{
	delete [] m_W;
}



long extend(const char* P, const char* T, long m, long n, long pi, long ti)
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

long polarExistsInD(long d, long r)
{
	long  x = POLAR_D_TO_CARTESIAN_X(d,r);
	long y = POLAR_D_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y>=0));
}

long polarExistsInW(long d, long r)
{
	long x = POLAR_W_TO_CARTESIAN_X(d,r);
	long y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y >= 0));
}

void WavefrontOriginal::setInput(const char* P, const char* T, INT_TYPE k)
{
    m_m = strlen(P);
    m_n = strlen(T);
    m_k = k;

    long size = (k)*(k);

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
    m_top = max2(m_m,m_n);
    m_P = P;
    m_T = T;

    printf("input set\n");

}

INT_TYPE WavefrontOriginal::getDistance()
{
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    long ret;

    // for the first element, just execute the extend phase
    m_W[0] = extend(m_P, m_T, m_m, m_n, 0, 0);

    if (m_W[0] >= m_top)
        goto end_loop;

    for (long r = 1; r < m_k; r++)
    {
        for (long d = -r; d <= r; d++)
        {
            long diag_up = (polarExistsInW(d + 1, r - 1)) ? m_W[POLAR_W_TO_INDEX(d + 1, r - 1, m_k)] : 0;
            long left = (polarExistsInW(d, r - 1)) ? m_W[POLAR_W_TO_INDEX(d, r - 1, m_k)] : 0;
            long diag_down = (polarExistsInW(d - 1, r - 1)) ? m_W[POLAR_W_TO_INDEX(d - 1, r - 1, m_k)] : 0;

            long compute;

            if (d == 0)
                compute = max3(diag_up, left + 1, diag_down);
            else if (d > 0)
                compute = max3(diag_up, left + 1, diag_down + 1);
            else // d < 0
                compute = max3(diag_up + 1, left + 1, diag_down);

            if ((d == final_d) && compute >= m_top)
            {
                m_W[POLAR_W_TO_INDEX(d, r, m_k)] = compute;
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

                m_W[POLAR_W_TO_INDEX(d, r, m_k)] = extended;

                //printf("(Extended)  + %d = %d (P[%d]=%s - T[%d]=%s)\n",  extendv, extended, ey, &m_P[ey], ex, &m_T[ex] );
                //printf("(Extended)  + %d = %d (P[%d] - T[%d])\n",  extendv, extended, ey,  ex );

                if ((d == final_d) && (extended >= m_top))
                {
                    //printf("Finishing d=%d r=%d\n", d, r);
                    m_W[POLAR_W_TO_INDEX(d, r, m_k)] = extended;
                    ret = r;
                    goto end_loop;
                }
            } 
            else
            {
                m_W[POLAR_W_TO_INDEX(d, r, m_k)] = compute;
            }
        }
    }

end_loop:

    return ret;
}

void test_primitives()
{
    int d = 0;
    int r = 1;
    int x = POLAR_D_TO_CARTESIAN_X(d, r);
    int y = POLAR_D_TO_CARTESIAN_Y(d, r);


    printf("Cell: d=%d, r=%d -> x=%d, y=%d\n", d, r, x, y);

    for (int tr = 0; tr < 3; tr++)
    {
        int td = d + 1;

        printf("Cell: d=%d, r=%d -> ", td, tr);

        x = POLAR_D_TO_CARTESIAN_X(td, tr);
        y = POLAR_D_TO_CARTESIAN_Y(td, tr);

        printf("x=%d, y=%d ", x, y);
        printf("%s\n", (polarExistsInD(td, tr)) ? "EXIST" : "DON'T EXIST");
    }

    exit(0);
}

const char* WavefrontOriginal::getDescription()
{
	return "Wavefront Original";
}

/**
 * @todo verify, some bugs still
 * 
 * @param distance
 * @return 
 */
char* WavefrontOriginal::getAlignmentPath(INT_TYPE* distance)
{
    long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
    
    *distance = getDistance();

    // longest worst case path goes through the table boundary 
    char* path = new char[m_m + m_n];
    
    long i=0;
    
    long d = final_d;
    long r = *distance;
    
    
    while (r > 0)
    {
        int incr = 0;
        
        long ed = m_W[POLAR_W_TO_INDEX(d, r, m_k)];
        long diag = (polarExistsInW(d, r - 1)) ? m_W[POLAR_W_TO_INDEX(d, r-1, m_k)] : 0;
        long up = (polarExistsInW(d+1, r - 1)) ? m_W[POLAR_W_TO_INDEX(d+1, r-1, m_k)] : 0;
        long down = (polarExistsInW(d-1, r - 1)) ? m_W[POLAR_W_TO_INDEX(d-1, r-1, m_k)] : 0;
        
        if (d >= 0)
            up--;
        if (d <= 0)
            down--;
        
        long m = max3(diag, up , down);
        
        if (m == diag)
        {
            // unroll extend, then change
            //printf("ed: %d diag: %d\n", ed, diag);
            for (int k=0; k < (ed - (diag + 1)); k++)
                path[i++] = '|';
            path[i++] = 'X';
        }
        else if (m == up)
        {
            for (int k=0; k < (ed - (up + 1)); k++)
                path[i++] = '|';
            path[i++] = 'I';
            d++;
        }
        else    // down
        {
            for (int k=0; k < (ed - (down + 1)); k++)
                path[i++] = '|';
            path[i++] = 'D';
            d--;
        }
        
        r--;
    }
    
    path[i] = 0;
    
    // now reverse
    long k = i - 1;

    for (int i = 0; i < k / 2; i++)
    {
        long aux = path[i];
        path[i] = path[k - i];
        path[k - i] = aux;
    }
    return path;
}

