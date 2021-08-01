#ifndef WAVEFRONT_DYNAMIC_DIAMOND_2_COLS_H_INCLUDED
#define WAVEFRONT_DYNAMIC_DIAMOND_2_COLS_H_INCLUDED

/**
*/
class WavefrontDynamicDiamond2Cols
{
public:
	WavefrontDynamicDiamond2Cols();
	virtual ~WavefrontDynamicDiamond2Cols();

	void setInput(const char* P, const char* T);
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