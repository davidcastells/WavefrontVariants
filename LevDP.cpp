#include "LevDP.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <new>

#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))

int dynamic_programming_classic(const char* P, const char* T, int m, int n, int k);

LevDP::LevDP()
{
}

LevDP::~LevDP()
{
	delete [] m_D;
}

void LevDP::setInput(const char* P, const char* T, int k)
{
	m_m = strlen(P);
	m_n = strlen(T);
	m_k = k;

	long size = (m_m+1)*(m_n+1);
	
	printf("create buffer %.2f GB\n", size*sizeof(long)/(1E9));

	try
	{
		m_D = new long[size];
	}
	catch (const std::bad_alloc& e) 
	{
		printf("FAILED to allocate memory\n");
		exit(-1);
	}
	
	m_top = max2(m_m, m_n);
	m_P = P;
	m_T = T;
	
	printf("input set\n");

}

long LevDP::getDistance()
{
	printf("computing\n");
	for (long y = 0; y <= m_m; y++)
		for (long x = 0; x <= m_n; x++)
		{
			if (x == 0)
			{
				m_D[CARTESIAN_TO_INDEX(y, x, m_n+1)] = y;
			}
			else if (y == 0)
			{
				m_D[CARTESIAN_TO_INDEX(y, x, m_n+1)] = x;
			}
			else
			{
				long d = abs(x-y);
				if (d > m_k)
				{
					m_D[CARTESIAN_TO_INDEX(y, x, m_n+1)] = m_top;
				}
				else
				{
					long dif = m_P[y-1] != m_T[x-1];
					long diag = m_D[CARTESIAN_TO_INDEX(y-1, x-1, m_n+1)] + dif;
					long up = m_D[CARTESIAN_TO_INDEX(y-1, x, m_n+1)] + 1;
					long left = m_D[CARTESIAN_TO_INDEX(y, x-1, m_n+1)] + 1;
					long v = min3(up, left, diag);
					m_D[CARTESIAN_TO_INDEX(y, x, m_n+1)] = v;
				}
			}
			
			// printf("D[%d][%d]=%d\n", y, x, D[CARTESIAN_TO_INDEX(y,x, n+1)] );
		}
		
	long ret = m_D[CARTESIAN_TO_INDEX(m_m, m_n, m_n+1)];
	
	return ret;
}
