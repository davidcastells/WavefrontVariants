#include "LevDP.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <new>

#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))


LevDP::LevDP()
{
}

LevDP::~LevDP()
{
	delete [] m_D;
}

void LevDP::setInput(const char* P, const char* T, INT_TYPE k)
{
	m_m = strlen(P);
	m_n = strlen(T);
	m_k = k;

	long size = (m_m+1)*(m_n+1);
	
	printf("create buffer %.2f GB\n", size*sizeof(long)/(1E9));

	try
	{
		m_D = new INT_TYPE[size];
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

INT_TYPE LevDP::getDistance()
{
	printf("computing\n");
	for (INT_TYPE y = 0; y <= m_m; y++)
		for (INT_TYPE x = 0; x <= m_n; x++)
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
				INT_TYPE d = abs(x-y);
				if (d > m_k)
				{
					m_D[CARTESIAN_TO_INDEX(y, x, m_n+1)] = m_top;
				}
				else
				{
					INT_TYPE dif = m_P[y-1] != m_T[x-1];
					INT_TYPE diag = m_D[CARTESIAN_TO_INDEX(y-1, x-1, m_n+1)] + dif;
					INT_TYPE up = m_D[CARTESIAN_TO_INDEX(y-1, x, m_n+1)] + 1;
					INT_TYPE left = m_D[CARTESIAN_TO_INDEX(y, x-1, m_n+1)] + 1;
					INT_TYPE v = min3(up, left, diag);
					m_D[CARTESIAN_TO_INDEX(y, x, m_n+1)] = v;
				}
			}
			
			// printf("D[%d][%d]=%d\n", y, x, D[CARTESIAN_TO_INDEX(y,x, n+1)] );
		}
		
	INT_TYPE ret = m_D[CARTESIAN_TO_INDEX(m_m, m_n, m_n+1)];
	
	return ret;
}

char* LevDP::getAlignmentPath(INT_TYPE* distance)
{
    *distance = getDistance();

    // longest worst case path goes through the table boundary 
    char* path = new char[m_m + m_n];
    // not implemented
    long i = 0;

    long y = m_m;
    long x = m_n;

    while (y > 0 || x > 0)
    {
        INT_TYPE ed = m_D[CARTESIAN_TO_INDEX(y, x, m_n + 1)];
        INT_TYPE up = (y > 0) ? m_D[CARTESIAN_TO_INDEX(y - 1, x, m_n + 1)] : ed;
        INT_TYPE left = (x > 0) ? m_D[CARTESIAN_TO_INDEX(y, x - 1, m_n + 1)] : ed;
        INT_TYPE diag = (x > 0 && y > 0) ? m_D[CARTESIAN_TO_INDEX(y - 1, x - 1, m_n + 1)] : ed;
        INT_TYPE m = min3(up, left, diag);
        if (diag == m)
        {
            if (ed == diag)
            {
                path[i++] = '|'; // match
            } else
            {
                path[i++] = 'X'; // unmatch
            }
            x--;
            y--;
        } else if (left == m)
        {
            path[i++] = 'D'; // delete
            x--;
        } else
        {
            path[i++] = 'I'; // insert
            y--;
        }

        // printf("%c", path[i-1]); fflush(stdout);
    }

    path[i] = 0;

    // now reverse
    INT_TYPE k = i - 1;

    for (int i = 0; i < k / 2; i++)
    {
        long aux = path[i];
        path[i] = path[k - i];
        path[k - i] = aux;
    }

    return path;
}

const char* LevDP::getDescription()
{
	return "Dynamic Programming classic";
}
