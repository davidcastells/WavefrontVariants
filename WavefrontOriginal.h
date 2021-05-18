#ifndef WAVEFRONT_ORIGINAL_H_INCLUDED
#define WAVEFRONT_ORIGINAL_H_INCLUDED

/**
*/
class WavefrontOriginal
{
public:
	WavefrontOriginal();
	virtual ~WavefrontOriginal();

	void setInput(const char* P, const char* T, long k);
	long getDistance();
	
	long* m_W;
	const char* m_P;
	const char* m_T;
	long m_n;
	long m_m;
	long m_k;
	long m_top;
};

#endif