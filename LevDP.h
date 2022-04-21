#ifndef LEV_DP_H_INCLUDED
#define LEV_DP_H_INCLUDED

#include "Aligner.h"

/**
*/
class LevDP : public Aligner
{
public:
	LevDP();
	virtual ~LevDP();

	void setInput(const char* P, const char* T, long k);
	long getDistance();
	char* getAlignmentPath(long* distance);
	const char* getDescription(); 
	
	long* m_D;
	long m_top;
	const char* m_P;
	const char* m_T;
	long m_n;
	long m_m;
	long m_k;
};

#endif