#ifndef LEV_DP_2_COLS_H_INCLUDED
#define LEV_DP_2_COLS_H_INCLUDED

/**
*/
class LevDP2Cols
{
public:
	LevDP2Cols();
	virtual ~LevDP2Cols();

	void setInput(const char* P, const char* T, int k);
	long getDistance();
	
	long* m_D;
	long m_top;
	const char* m_P;
	const char* m_T;
	long m_n;
	long m_m;
	long m_k;
};

#endif