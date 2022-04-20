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

	void setInput(const char* P, const char* T, long k);
	void precomputeExtend();
	long getDistance();
	char* getAlignmentPath(long* distance);
	const char* getDescription(); 

	
	int polarExistsInD(int d, int r);
	int polarExistsInW(int d, int r);

	int* m_W;
	long m_size_W;
	int* m_E;
	long m_size_E;
	const char* m_P;
	const char* m_T;
	long m_n;
	long m_m;
	long m_k;
	long m_top;
};

#endif