#include "WavefrontDynamicDiamond.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

WavefrontDynamicDiamond::WavefrontDynamicDiamond()
{
}

WavefrontDynamicDiamond::~WavefrontDynamicDiamond()
{
	delete [] m_W;
}



int WavefrontDynamicDiamond_extend(const char* P, const char* T, int m, int n, int pi, int ti)
{
	int e = 0;
	
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

int WavefrontDynamicDiamond_polarExistsInD(int d, int r)
{
	int x = POLAR_D_TO_CARTESIAN_X(d,r);
	int y = POLAR_D_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y>=0));
}

int WavefrontDynamicDiamond_polarExistsInW(int d, int r)
{
	int x = POLAR_W_TO_CARTESIAN_X(d,r);
	int y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y >= 0));
}

void WavefrontDynamicDiamond::setInput(const char* P, const char* T)
{                       
	m_m = strlen(P);
	m_n = strlen(T);
	m_top = max2(m_m,m_n);	// Maximum distance value

	m_k = m_top;

	long size = (m_k)*(m_k);
	
	printf("create buffer %.2f GB\n", size/(1E9));

	m_W = new long[size];
	
	assert(m_W);
	m_P = P;
	m_T = T;
	
	printf("input set\n");
	
	printf("Initial limit: %ld\n", m_k);

}

int WavefrontDynamicDiamond::getDistance()
{
	int final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
	int ret;
	
	long kt = m_k;
	
	// for the first element, just execute the extend phase
	m_W[0] = WavefrontDynamicDiamond_extend(m_P, m_T, m_m, m_n, 0, 0);
	
	
	// opening the diamond
	for (long r=1; r < kt; r++)
	{
		long k_odd = kt % 2;
		long k_half = kt/2;

		//printf("[%ld] half: %ld \n", r, k_half);

		if (r  < k_half)
		{
			
			for (long d=-r; d <= r; d++)
			{			
				int diag_up = (WavefrontDynamicDiamond_polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1, m_k)]  : 0;
				int left = (WavefrontDynamicDiamond_polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1, m_k)]  : 0;
				int diag_down = (WavefrontDynamicDiamond_polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1, m_k)]  : 0;
				
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
					
					int extendv = WavefrontDynamicDiamond_extend(m_P, m_T, m_m, m_n, ey, ex);
					int extended = compute + extendv;
					
					m_W[POLAR_W_TO_INDEX(d, r, m_k)] = extended;
					
					//if (d==0) 
						if ((r + d + m_top - extended)  < kt)
					{
						//printf("[%ld] Updating (previous k %ld) k = %ld (top) - %ld (extended) = %ld\n", r, kt,  m_top, extended, m_top-extended+r  );
						kt = r + d + m_top - extended;
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
					m_W[POLAR_W_TO_INDEX(d, r, m_k)] = compute;
					
					if ((r + d + m_top - compute)  < kt)
					{
						//printf("[%ld] Updating (previous k %ld) k = %ld (top) - %ld (extended) = %ld\n", r, kt,  m_top, extended, m_top-extended+r  );
						kt = r + d + m_top - compute;
					}
				}
			}
		}
		
		// closing the diamond
		if ( r >= k_half)
		{
			//printf("[%ld]\n", r);
			
			int cr = kt - (r  + k_odd); // 5 - (2 + 1) = 5 -3 = 2
			
			for (int d=-cr; d <= cr; d++)
			{			
				int diag_up = (WavefrontDynamicDiamond_polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1, m_k)]  : 0;
				int left = (WavefrontDynamicDiamond_polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1, m_k)]  : 0;
				int diag_down = (WavefrontDynamicDiamond_polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1, m_k)]  : 0;
				
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
					
					int extendv = WavefrontDynamicDiamond_extend(m_P, m_T, m_m, m_n, ey, ex);
					int extended = compute + extendv;
					
					m_W[POLAR_W_TO_INDEX(d, r, m_k)] = extended;
					
					if (d==0)
						if ((r + d + m_top - extended)  < kt)
					{
						//printf("[%ld] Updating (previous k %ld) k = %ld (top) - %ld (extended) = %ld\n", r, kt,  m_top, extended, m_top-extended+r  );
						kt = r + d + m_top - extended ;
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
					m_W[POLAR_W_TO_INDEX(d, r, m_k)] = compute;
					
					if (d==0) if ((r + d + m_top - compute)  < kt)
					{
						//printf("[%ld] Updating (previous k %ld) k = %ld (top) - %ld (extended) = %ld\n", r, kt,  m_top, extended, m_top-extended+r  );
						kt = r + d + m_top - compute;
					}
				}
			}
		}
	}
	
	ret = kt;
	
end_loop:
	
	return ret;
}

