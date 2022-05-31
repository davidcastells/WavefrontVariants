#ifndef ALIGNER_H_INCLUDED
#define ALIGNER_H_INCLUDED

#include "PerformanceLap.h"

#include <stdio.h>

extern int gMeasureIterationTime;

#define INT_TYPE int
#define INT_TYPE_FMT d
#define STR(x)  #x
#define STR_INT_TYPE STR(int)
#define FMT_INT_TYPE STR(INT_TYPE_FMT)
#define CL_INT_TYPE cl_int

class Aligner
{
public:
	virtual void setInput(const char* P, const char* T, INT_TYPE k) = 0;
	virtual INT_TYPE getDistance() = 0;
	virtual char* getAlignmentPath(INT_TYPE* distance) = 0;
	virtual const char* getDescription() = 0; 
	
	void freePath(char* p)
	{
		delete [] p;
	}

	
	void execute(const char* P, const char* T, INT_TYPE k, int doAlignmentPath)
	{
		setInput(P, T, k);
		
		PerformanceLap lap;
		if (doAlignmentPath)
		{
			INT_TYPE ed;
			char* path = getAlignmentPath(&ed);
			lap.stop();

			printf("Alignment path:\n");
			//printf(gT);
			//printf("\n");
			printf("%s\n", path);
			//printf(gP);
			//printf("\n");
			printf("%s Distance=%ld Time=%0.5f seconds\n", getDescription(),  ed, lap.lap());
			freePath(path);
		}
		else
		{
			INT_TYPE ed = getDistance();
			lap.stop();
			
			if (!gMeasureIterationTime)
            {
                printf("%s Distance=%d Time=%0.5f seconds\n", getDescription(), ed, lap.lap());
                printf("m x n = %d x %d\n", m_m, m_n);
                printf("Equivalent GCUPS: %f\n", (((double)m_m*m_n)/(lap.lap() * 1E9)));
            }
		}
	}
	
protected:        
    const char* m_P;
    const char* m_T;
    INT_TYPE m_n;
    INT_TYPE m_m;
};

#endif
