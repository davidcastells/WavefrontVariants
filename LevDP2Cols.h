#ifndef LEV_DP_2_COLS_H_INCLUDED
#define LEV_DP_2_COLS_H_INCLUDED

#include "Aligner.h"

/**
*/
class LevDP2Cols  : public Aligner
{
public:
	LevDP2Cols();
	virtual ~LevDP2Cols();

	void setInput(const char* P, const char* T, long k);
	long getDistance();
	char* getAlignmentPath(long* distance);
	const char* getDescription(); 

protected:
        void progress(PerformanceLap& lap, long x, int& lastpercent, long cellsAllocated, long cellsAlive);
	
	long* m_D;
	long m_top;
	const char* m_P;
	const char* m_T;
	long m_n;
	long m_m;
	long m_k;
};

#endif