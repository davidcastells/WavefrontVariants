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

	void setInput(const char* P, const char* T, INT_TYPE k);
	INT_TYPE getDistance();
	char* getAlignmentPath(INT_TYPE* distance);
	const char* getDescription(); 
	
	INT_TYPE* m_D;
	INT_TYPE m_top;
	INT_TYPE m_k;
};

#endif
