#ifndef WAVEFRONT_DYNAMIC_DIAMOND_H_INCLUDED
#define WAVEFRONT_DYNAMIC_DIAMOND_H_INCLUDED

/**
*/
class WavefrontDynamicDiamond
{
public:
	WavefrontDynamicDiamond();
	virtual ~WavefrontDynamicDiamond();

	void setInput(const char* P, const char* T);
	int getDistance();
	
	long* m_W;
	const char* m_P;
	const char* m_T;
	long m_n;
	long m_m;
	long m_k;
	long m_top;
};

#endif