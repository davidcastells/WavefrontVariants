#ifndef WAVEFRONT_DIAMOND_2_COLS_H_INCLUDED
#define WAVEFRONT_DIAMOND_2_COLS_H_INCLUDED

#include "Aligner.h"

/**
*/
class WavefrontDiamond2Cols : public Aligner
{
public:
	WavefrontDiamond2Cols();
	virtual ~WavefrontDiamond2Cols();

	void setInput(const char* P, const char* T, long k);
	long getDistance();
	char* getAlignmentPath(long* distance);
	const char* getDescription(); 

	
	long* m_W;
	const char* m_P;
	const char* m_T;
	long m_n;
	long m_m;
	long m_k;
	long m_top;
};

#endif