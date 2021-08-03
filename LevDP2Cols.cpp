#include "LevDP2Cols.h"
#include "utils.h"
#include "PerformanceLap.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <new>

#define CARTESIAN_TO_INDEX(y, x)		((y) + ((x)%2)*(m_m+1))

int dynamic_programming_classic(const char* P, const char* T, int m, int n, int k);


extern int verbose;

LevDP2Cols::LevDP2Cols()
{
}

LevDP2Cols::~LevDP2Cols()
{
	delete [] m_D;
}

void LevDP2Cols::setInput(const char* P, const char* T, int k)
{
	m_m = strlen(P);
	m_n = strlen(T);
	m_k = k;

	long size = (m_m+1)*2;
	
	printf("create buffer %.2f MB\n", size*sizeof(long)/(1E6));

	try
	{
		m_D = new long[size];
	}
	catch (const std::bad_alloc& e) 
	{
		printf("FAILED to allocate memory\n");
		exit(-1);
	}
	
	assert(m_D);
	m_top = max2(m_m, m_n);
	m_P = P;
	m_T = T;
	
	printf("input set\n");

}

long LevDP2Cols::getDistance()
{
	printf("computing\n");

	PerformanceLap lap;
	lap.start();
	double progress = 0;

	for (long x = 0; x <= m_n; x++)
	{
		if (verbose)
		{
			double elapsed = lap.stop();
			double estimated = (elapsed / x) * m_n;
			printf("\rcol %ld/%ld %.2f%% elapsed: %d s  estimated: %d s    ", x, m_n, (x*100.0/m_n), (int) elapsed, (int) estimated );
		}

		for (long y = 0; y <= m_m; y++)
		{
			if (x == 0)
			{
				m_D[CARTESIAN_TO_INDEX(y, x)] = y;
			}
			else if (y == 0)
			{
				m_D[CARTESIAN_TO_INDEX(y, x)] = x;
			}
			else
			{
				long d = abs(x-y);
				if (d > m_k)
				{
					m_D[CARTESIAN_TO_INDEX(y, x)] = m_top;
				}
				else
				{
					int dif = m_P[y-1] != m_T[x-1];
					int diag = m_D[CARTESIAN_TO_INDEX(y-1, x-1)] + dif;
					int up = m_D[CARTESIAN_TO_INDEX(y-1, x)] + 1;
					int left = m_D[CARTESIAN_TO_INDEX(y, x-1)] + 1;
					int v = min3(up, left, diag);
					m_D[CARTESIAN_TO_INDEX(y, x)] = v;
				}
			}
			
			// printf("D[%d][%d]=%d\n", y, x, D[CARTESIAN_TO_INDEX(y,x, n+1)] );
		}
	}
	
	if (verbose)
		printf("\n");
		
	int ret = m_D[CARTESIAN_TO_INDEX(m_m, m_n)];
	
	return ret;
}
