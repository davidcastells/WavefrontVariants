#ifndef WAVEFRONT_DYNAMIC_DIAMOND_2_COLS_H_INCLUDED
#define WAVEFRONT_DYNAMIC_DIAMOND_2_COLS_H_INCLUDED

#include "Aligner.h"

/**
*/
class WavefrontDynamicDiamond2Cols : public Aligner
{
public:
	WavefrontDynamicDiamond2Cols();
	virtual ~WavefrontDynamicDiamond2Cols();

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