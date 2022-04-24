#ifndef ALIGNER_H_INCLUDED
#define ALIGNER_H_INCLUDED

#include "PerformanceLap.h"

#include <stdio.h>

extern int gMeasureIterationTime;

class Aligner
{
public:
	virtual void setInput(const char* P, const char* T, long k) = 0;
	virtual long getDistance() = 0;
	virtual char* getAlignmentPath(long* distance) = 0;
	virtual const char* getDescription() = 0; 
	
	void freePath(char* p)
	{
		delete [] p;
	}

	
	void execute(const char* P, const char* T, int k, int doAlignmentPath)
	{
		setInput(P, T, k);
		
		PerformanceLap lap;
		if (doAlignmentPath)
		{
			long ed;
			char* path = getAlignmentPath(&ed);
			lap.stop();

			printf("Alignment path:\n");
			//printf(gT);
			//printf("\n");
			printf(path);
			printf("\n");
			//printf(gP);
			//printf("\n");
			printf("%s Distance=%ld Time=%0.5f seconds\n", getDescription(),  ed, lap.lap());
			freePath(path);
		}
		else
		{
			long ed = getDistance();
			lap.stop();
			if (!gMeasureIterationTime)
    			printf("%s Distance=%ld Time=%0.5f seconds\n", getDescription(), ed, lap.lap());
		}
	}
};

#endif
