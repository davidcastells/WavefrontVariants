#include "WavefrontExtendPrecomputing.h"
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

#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)), (w))

#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))

WavefrontExtendPrecomputing::WavefrontExtendPrecomputing()
{
}

WavefrontExtendPrecomputing::~WavefrontExtendPrecomputing()
{
	delete [] m_W;
	delete [] m_E;
}



int WavefrontExtendPrecomputing::polarExistsInD(int d, int r)
{
	int x = POLAR_D_TO_CARTESIAN_X(d,r);
	int y = POLAR_D_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y>=0));
}

int WavefrontExtendPrecomputing::polarExistsInW(int d, int r)
{
	int x = POLAR_W_TO_CARTESIAN_X(d,r);
	int y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y >= 0));
}
/*
int extend(const char* P, const char* T, int m, int n, int pi, int ti)
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
*/


void WavefrontExtendPrecomputing::setInput(const char* P, const char* T, long k)
{
	m_m = strlen(P);
	m_n = strlen(T);
	m_k = k;

	m_size_W = (k)*(k);
	
	printf("create buffer %.2f GB\n", m_size_W/(1E9));

	printf("Size = %ld\n", m_size_W);

	m_W = new int[m_size_W];
	
	assert(m_W);
	m_top = max2(m_m,m_n);
	m_P = P;
	m_T = T;
	
	m_size_E = m_m * m_n;
	m_E = new int[m_size_E];
	
	printf("SizeE = %ld\n", m_size_E);
	
	for (long i=0; i< m_size_E; i++)
		m_E[i] = 0;
	
	printf("input set\n");

	precomputeExtend();
}

void WavefrontExtendPrecomputing::precomputeExtend()
{
	for (long y = m_m-1; y >= 0; y--)
		//for (long x = m_n-1; x >= 0; x--)
		for (long x = y+m_k; x >= y-m_k ; x--)
		{
			long d = abs(x-y);
			if (d > m_k || x < 0  )
			{
				// ignore distant cells
			}
			else
			{
				int v = (m_P[y] == m_T[x]) ? 1:0;
				
				if (v)
					if ((x < m_n-1) && (y < m_m-1))
						v += m_E[CARTESIAN_TO_INDEX(y+1, x+1, m_n)];
				
				m_E[CARTESIAN_TO_INDEX(y, x, m_n)] = v;
			}
		}

	/*
	printf("EXTEND\n");
	
	for (long y = 0; y < m_m; y++)
	{
		for (long x = 0; x < m_n; x++)
		{
			printf("%d ", m_E[CARTESIAN_TO_INDEX(y, x, m_n)]);
		}
		printf("\n");
	}*/
}

long WavefrontExtendPrecomputing::getDistance()
{
	long final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
	long ret;
	
	// for the first element, just execute the extend phase
	m_W[0] = m_E[CARTESIAN_TO_INDEX(0, 0, m_n)];
	
	for (long r=1; r < m_k; r++)
	{
		for (long d=-r; d <= r; d++)
		{			
			long diag_up = (polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1, m_k)]  : 0;
			long left = (polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1, m_k)]  : 0;
			long diag_down = (polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1, m_k)]  : 0;
			
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
			
			//printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  \n ", d, r , diag_up, left, diag_down, compute );

			long ex = POLAR_W_TO_CARTESIAN_X(d, compute);
			long ey = POLAR_W_TO_CARTESIAN_Y(d, compute);
			
			if ((ex < m_n) && (ey < m_m))
			{
					
				//printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  || ", d, r , diag_up, left, diag_down, compute );
				long extendv = m_E[CARTESIAN_TO_INDEX(ey, ex, m_n)]; //extend(m_P, m_T, m_m, m_n, ey, ex);
				long extended = compute + extendv;
				
				//printf("Extend (ey=%d ex=%d) = %d\n ", ey, ex, extendv);
				//printf("index: %d\n", POLAR_W_TO_INDEX(d, r, m_k)),
				
				assert(POLAR_W_TO_INDEX(d, r, m_k) < m_size_W);
				
				m_W[POLAR_W_TO_INDEX(d, r, m_k)] = extended;
				
				//printf("(Extended)  + %d = %d (P[%d]=%s - T[%d]=%s)\n",  extendv, extended, ey, &P[ey], ex, &T[ex] );
				//printf("(Extended)  + %d = %d (P[%d] - T[%d])\n",  extendv, extended, ey,  ex );
	
				
				if ((d == final_d) && extended >= m_top)
				{
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



const char* WavefrontExtendPrecomputing::getDescription()
{
	return "Wavefront Extend Precomputing";
}

char* WavefrontExtendPrecomputing::getAlignmentPath(long* distance)
{
	printf("Alignment Not implemented yet!\n");
	exit(0);
}
