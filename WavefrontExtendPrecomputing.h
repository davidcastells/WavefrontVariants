#ifndef WAVEFRONT_EXTEND_H_INCLUDED
#define WAVEFRONT_EXTEND_H_INCLUDED

#include "Aligner.h"

/**
*/
class WavefrontExtendPrecomputing : public Aligner
{
public:
	WavefrontExtendPrecomputing();
	virtual ~WavefrontExtendPrecomputing();

	void setInput(const char* P, const char* T, INT_TYPE k);
	void precomputeExtend();
	INT_TYPE getDistance();
	char* getAlignmentPath(INT_TYPE* distance);
	const char* getDescription(); 

	
	int polarExistsInD(INT_TYPE d, INT_TYPE r);
	int polarExistsInW(INT_TYPE d, INT_TYPE r);

	INT_TYPE* m_W;
	INT_TYPE m_size_W;
	INT_TYPE* m_E;
	INT_TYPE m_size_E;
	INT_TYPE m_k;
	INT_TYPE m_top;
};

#endif
