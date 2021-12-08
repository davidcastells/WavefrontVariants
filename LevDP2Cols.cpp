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

void LevDP2Cols::setInput(const char* P, const char* T, long k)
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


const char* LevDP2Cols::getDescription()
{
	return "Dynamic Programming classic using 2 columns";
}

class CellPointer
{
public:
	CellPointer(long x, long y, CellPointer* d)
	{
		m_x = x;
		m_y = y;
		m_p = d;
                count = 0;
                
                if (m_p != NULL)
                    m_p->count++;
	}
	
        static void deleteChain(CellPointer* target, long& alive)
        {
            CellPointer* p = target;
            CellPointer* pp;
            
        loop:
            pp = p->m_p;
            delete p;
            alive--;
            
            if (pp == NULL)
                return;
            
            pp->count--;
            
            if (pp->count <= 0)
            {
                p = pp;
                goto loop;
            }
        }
        
	long m_x;
	long m_y;
	CellPointer* m_p;
        int count;
};

char* LevDP2Cols::getAlignmentPath(long* distance)
{
	char* path = new char[m_m+m_n];
	
	printf("computing\n");

	PerformanceLap lap;
	lap.start();
	double progress = 0;

	CellPointer** prev = new CellPointer*[m_m+1];
	CellPointer** selected = new CellPointer*[m_m+1];
        
        long cellsAllocated = 0;
        long cellsAlive = 0;
        
	for (long y = 0; y <= m_m; y++)
	{
            prev[y] = NULL;
	}

        int lastpercent = -1;
        
	for (long x = 0; x <= m_n; x++)
	{		
		if (verbose)
		{
			double elapsed = lap.stop();
			double estimated = (elapsed / x) * m_n;
                        int percent = (x*100.0*100/m_n);
                        
                        if (percent != lastpercent)
                        {
                            printf("\rcol %ld/%ld %.2f%% cells allocated: %ld alive: %ld elapsed: %d s  estimated: %d s    ", x, m_n, (double) (percent/100.0), cellsAllocated, cellsAlive, (int) elapsed, (int) estimated );
                            fflush(stdout);
                            lastpercent = percent;
                        }
		}

		for (long y = 0; y <= m_m; y++)
		{
			if (x == 0)
			{
				m_D[CARTESIAN_TO_INDEX(y, x)] = y;
				selected[y] = NULL;
			}
			else if (y == 0)
			{
				m_D[CARTESIAN_TO_INDEX(y, x)] = x;
				selected[y] = NULL;
			}
			else
			{
				long d = abs(x-y);
				if (d > m_k)
				{
					m_D[CARTESIAN_TO_INDEX(y, x)] = m_top;
					selected[y] = NULL;
					printf("d>k\n");
					exit(0);
				}
				else
				{
					long dif = m_P[y-1] != m_T[x-1];
					long diag = m_D[CARTESIAN_TO_INDEX(y-1, x-1)] + dif;
					long up = m_D[CARTESIAN_TO_INDEX(y-1, x)] + 1;
					long left = m_D[CARTESIAN_TO_INDEX(y, x-1)] + 1;
					long v = min3(up, left, diag);
					
					if (diag == v)
					{
						selected[y] = new CellPointer(x,y, prev[y-1]);
                                                cellsAllocated++;
                                                cellsAlive++;
					}
					else if (up == v)
					{
						selected[y] = new CellPointer(x,y, selected[y-1]);
                                                cellsAllocated++;
                                                cellsAlive++;
					}
					else	// left
					{
						selected[y] = new CellPointer(x,y, prev[y]);
                                                cellsAllocated++;
                                                cellsAlive++;
					}
					
					m_D[CARTESIAN_TO_INDEX(y, x)] = v;
				}
			}
			
			// printf("D[%d][%d]=%d\n", y, x, D[CARTESIAN_TO_INDEX(y,x, n+1)] );
		}
		
		// now delete cells (from prev) that are not referenced
		for (long y=0; y <= m_m; y++)
		{
			if (prev[y] == NULL)
				continue;
			
			if (y == m_m) 
				if (x == m_n)
				{
					// this is a special case, do not remove this cell
					continue;
				}
			
			// the 3 possibilities is that either down, diag, or right depend on them
			// so when we do see any of this dependency we remove that cell
			int right = (x < m_n && (selected[y] != NULL))? (prev[y] == selected[y]->m_p) :  0;
			int diag = (y < m_m && (selected[y+1] != NULL))? (prev[y] == selected[y+1]->m_p) : 0;
			int down = (y < m_m && (prev[y+1] != NULL))? (prev[y] == prev[y+1]->m_p) : 0;
			
			if (!(right || diag || down))
			{
                            // we should delete the cell, and their preceders
                            CellPointer::deleteChain(prev[y], cellsAlive);
                            prev[y] = NULL;
			}
		}
		
		// now copy current column to the prev
		for (long y=0; y <= m_m; y++)
		{
			prev[y] = selected[y];
		}
	}
	
	if (verbose)
		printf("\n");
		
	*distance = m_D[CARTESIAN_TO_INDEX(m_m, m_n)];
	
	delete [] selected;
        delete [] prev;
        
	return path;
}
