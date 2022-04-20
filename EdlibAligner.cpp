

#include "EdlibAligner.h"

#include "edlib.h"
#include <string.h>
#include <stdlib.h>


EdlibAligner::EdlibAligner()
{
}

EdlibAligner::~EdlibAligner()
{
}

void EdlibAligner::setInput(const char* P, const char* T, long k)
{
	m_m = strlen(P);
	m_n = strlen(T);
	m_k = k;

	m_P = P;
	m_T = T;
	
	// printf("input set\n");
}

long EdlibAligner::getDistance()
{   
    EdlibAlignResult ret;
	EdlibAlignConfig alignConfig = edlibNewAlignConfig(m_k, EDLIB_MODE_NW, EDLIB_TASK_DISTANCE, NULL, 0);
    ret = edlibAlign(m_P, m_m, m_T, m_n, alignConfig);
    return ret.editDistance;        
}

char* EdlibAligner::getAlignmentPath(long* distance)
{
    printf("Not implemented\n");
    exit(-1);

    return "";
}

const char* EdlibAligner::getDescription()
{
	return "Edlib Global Alignment";
}
