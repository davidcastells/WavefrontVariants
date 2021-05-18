#ifndef LEV_DP_H_INCLUDED
#define LEV_DP_H_INCLUDED

/**
*/
class LevDP
{
public:
	LevDP();
	virtual ~LevDP();

	void setInput(const char* P, const char* T, int k);
	int getDistance();
	
	int* m_D;
	long m_top;
	const char* m_P;
	const char* m_T;
	long m_n;
	long m_m;
	long m_k;
};

#endif