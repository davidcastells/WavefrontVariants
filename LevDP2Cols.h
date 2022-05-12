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

	void setInput(const char* P, const char* T, INT_TYPE k);
	INT_TYPE getDistance();
	char* getAlignmentPath(INT_TYPE* distance);
	const char* getDescription(); 

protected:
        void progress(PerformanceLap& lap, INT_TYPE x, int& lastpercent, long cellsAllocated, long cellsAlive);
	
	INT_TYPE* m_D;
	INT_TYPE m_top;
	INT_TYPE m_k;
};

#endif
