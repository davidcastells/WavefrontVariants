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

	void setInput(const char* P, const char* T, INT_TYPE k);
	INT_TYPE getDistance();
	char* getAlignmentPath(INT_TYPE* distance);
	const char* getDescription(); 

	INT_TYPE* m_W;
	INT_TYPE m_k;
	INT_TYPE m_top;
};

#endif
