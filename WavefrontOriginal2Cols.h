#ifndef WAVEFRONT_ORIGINAL_2_COLS_H_INCLUDED
#define WAVEFRONT_ORIGINAL_2_COLS_H_INCLUDED

/**
*/
class WavefrontOriginal2Cols
{
public:
	WavefrontOriginal2Cols();
	virtual ~WavefrontOriginal2Cols();

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