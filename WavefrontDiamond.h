#ifndef WAVEFRONT_DIAMOND_H_INCLUDED
#define WAVEFRONT_DIAMOND_H_INCLUDED

#include "Aligner.h"

/**
*/
class WavefrontDiamond : public Aligner
{
public:
	WavefrontDiamond();
	virtual ~WavefrontDiamond();

	void setInput(const char* P, const char* T, INT_TYPE k);
	INT_TYPE getDistance();
	char* getAlignmentPath(INT_TYPE* distance);
	const char* getDescription(); 
	
	INT_TYPE* m_W;
	INT_TYPE m_k;
	INT_TYPE m_top;
};

#endif
