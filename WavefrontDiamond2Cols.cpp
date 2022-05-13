#include "WavefrontDiamond2Cols.h"
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

WavefrontDiamond2Cols::WavefrontDiamond2Cols()
{
}

WavefrontDiamond2Cols::~WavefrontDiamond2Cols()
{
	delete [] m_W;
}


static INT_TYPE extend(const char* P, const char* T, INT_TYPE m, INT_TYPE n, INT_TYPE pi, INT_TYPE ti)
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

static INT_TYPE polarExistsInD(INT_TYPE d, INT_TYPE r)
{
	INT_TYPE x = POLAR_D_TO_CARTESIAN_X(d,r);
	INT_TYPE y = POLAR_D_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y>=0));
}

static INT_TYPE polarExistsInW(INT_TYPE d, INT_TYPE r)
{
	INT_TYPE x = POLAR_W_TO_CARTESIAN_X(d,r);
	INT_TYPE y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y >= 0));
}

void WavefrontDiamond2Cols::setInput(const char* P, const char* T, INT_TYPE k)
{
	m_m = strlen(P);
	m_n = strlen(T);
	m_k = k;

	long size = 2*(2*k+1);
	
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

INT_TYPE WavefrontDiamond2Cols::getDistance()
{
	INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
	INT_TYPE ret;
	
	// for the first element, just execute the extend phase
	m_W[0] = extend(m_P, m_T, m_m, m_n, 0, 0);
		
	INT_TYPE k_odd = m_k % 2;
	INT_TYPE k_half = m_k/2;

	if (m_W[0] >= m_top)
		goto end_loop;

	// opening the diamond
	for (INT_TYPE r=1; r < k_half; r++)
	{
		if (verbose)
			printf("\rr %ld/%ld %.2f%%", r, m_k, (r*100.0/m_k) );

		for (INT_TYPE d=-r; d <= r; d++)
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
				ret = r;
				goto end_loop;
			}
			
			INT_TYPE ex = POLAR_W_TO_CARTESIAN_X(d, compute);
			INT_TYPE ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

			if ((ex < m_n) && (ey < m_m))
			{
				//printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  || ", d, r , diag_up, left, diag_down, compute );
				
				INT_TYPE extendv = extend(m_P, m_T, m_m, m_n, ey, ex);
				INT_TYPE extended = compute + extendv;
				
				m_W[POLAR_W_TO_INDEX(d, r)] = extended;
				
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
			}
		}
	}
	
	// closing the diamond
	for (INT_TYPE r=k_half; r < m_k; r++)
	{
		if (verbose)
			printf("\rr %ld/%ld %.2f%%", r, m_k, (r*100.0/m_k) );

		long cr = m_k - (r  + k_odd); // 5 - (2 + 1) = 5 -3 = 2
		
		for (INT_TYPE d=-cr; d <= cr; d++)
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
				ret = r;
				goto end_loop;
			}
			
			INT_TYPE ex = POLAR_W_TO_CARTESIAN_X(d, compute);
			INT_TYPE ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

			if ((ex < m_n) && (ey < m_m))
			{
				//printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  || ", d, r , diag_up, left, diag_down, compute );
				
				INT_TYPE extendv = extend(m_P, m_T, m_m, m_n, ey, ex);
				INT_TYPE extended = compute + extendv;
				
				m_W[POLAR_W_TO_INDEX(d, r)] = extended;
				
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
			}
		}
	}
	
end_loop:	
	if (verbose)
		printf("\n");
	
	return ret;
}



const char* WavefrontDiamond2Cols::getDescription()
{
	return "Wavefront Diamond 2 columns";
}

char* WavefrontDiamond2Cols::getAlignmentPath(INT_TYPE* distance)
{
	printf("Alignment Not implemented yet!\n");
	exit(0);
}
