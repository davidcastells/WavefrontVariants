/**
 * 
 * Copyright (C) 2021, David Castells-Rufas <david.castells@uab.cat>, 
 * Universitat Autonoma de Barcelona  
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "WavefrontDiamond.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <new>

#define CARTESIAN_TO_INDEX(y, x, w)		((y)*(w) + (x))
#define POLAR_D_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_D_TO_CARTESIAN_Y((d), (r)),POLAR_D_TO_CARTESIAN_X((d), (r)),w)
#define POLAR_D_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? 0: -(d)) + (r))
#define POLAR_D_TO_CARTESIAN_X(d,r)		((((d) >= 0)? (d): 0) + (r))

#define CARTESIAN_TO_POLAR_D_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_D_R(y, x)		(((y)>(x))? (x) : (y))

#define POLAR_W_TO_INDEX(d, r, w)			CARTESIAN_TO_INDEX(POLAR_W_TO_CARTESIAN_Y((d), (r)),POLAR_W_TO_CARTESIAN_X((d), (r)),w)

#define POLAR_W_TO_CARTESIAN_Y(d,r)		((((d) >= 0)? -(d) : 0 ) + (r))
#define POLAR_W_TO_CARTESIAN_X(d,r)		((((d) >= 0)? 0 : (d)) + (r))

#define CARTESIAN_TO_POLAR_W_D(y, x)		((x)-(y))
#define CARTESIAN_TO_POLAR_W_R(y, x)		(((y)>(x))? (y) : (x))

WavefrontDiamond::WavefrontDiamond()
{
}

WavefrontDiamond::~WavefrontDiamond()
{
	delete [] m_W;
}



long WavefrontDiamond_extend(const char* P, const char* T, INT_TYPE m, INT_TYPE n, INT_TYPE pi, INT_TYPE ti)
{
	INT_TYPE e = 0;
	
	while (pi < m && ti < n)
	{
		if (P[pi] != T[ti])
			return e;
		e++;
		pi++;
		ti++;
	}
	
	return e;
}

int WavefrontDiamond_polarExistsInD(INT_TYPE d, INT_TYPE r)
{
	INT_TYPE x = POLAR_D_TO_CARTESIAN_X(d,r);
	INT_TYPE y = POLAR_D_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y>=0));
}

int WavefrontDiamond_polarExistsInW(INT_TYPE d, INT_TYPE r)
{
	INT_TYPE x = POLAR_W_TO_CARTESIAN_X(d,r);
	INT_TYPE y = POLAR_W_TO_CARTESIAN_Y(d,r);
	
	return ((x >= 0) && (y >= 0));
}

void WavefrontDiamond::setInput(const char* P, const char* T, INT_TYPE k)
{
	m_m = strlen(P);
	m_n = strlen(T);
	m_k = k;

	long size = (k)*(k);
	
	printf("create buffer %.2f GB\n", size*sizeof(long)/(1E9));

	try
	{
		m_W = new INT_TYPE[size];
	}
	catch (const std::bad_alloc& e) 
	{
		printf("FAILED to allocate memory\n");
		exit(-1);
	}
	
	assert(m_W);
	m_top = max2(m_m,m_n);
	m_P = P;
	m_T = T;
	
	printf("input set\n");
}

INT_TYPE WavefrontDiamond::getDistance()
{
	INT_TYPE final_d = CARTESIAN_TO_POLAR_D_D(m_m, m_n);
	INT_TYPE ret;
	
	// for the first element, just execute the extend phase
	m_W[0] = WavefrontDiamond_extend(m_P, m_T, m_m, m_n, 0, 0);
		
	INT_TYPE k_odd = m_k % 2;
	INT_TYPE k_half = m_k/2;

	if (m_W[0] >= m_top)
		goto end_loop;

	// opening the diamond
	for (INT_TYPE r=1; r < k_half; r++)
	{
		for (INT_TYPE d=-r; d <= r; d++)
		{			
			INT_TYPE diag_up = (WavefrontDiamond_polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1, m_k)]  : 0;
			INT_TYPE left = (WavefrontDiamond_polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1, m_k)]  : 0;
			INT_TYPE diag_down = (WavefrontDiamond_polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1, m_k)]  : 0;
			
			INT_TYPE compute;
			
			if (d == 0)
				compute = max3(diag_up, left+1, diag_down);
			else if (d > 0)
				compute = max3(diag_up, left+1, diag_down+1);
			else
				compute = max3(diag_up+1, left+1, diag_down);
			
			if ((d == final_d) && compute >= m_top)
			{
				ret = r;
				goto end_loop;
			}
			
			INT_TYPE ex = POLAR_W_TO_CARTESIAN_X(d, compute);
			INT_TYPE ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

			if ((ex < m_n) && (ey < m_m))
			{
				//printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  || ", d, r , diag_up, left, diag_down, compute );
				
				INT_TYPE extendv = WavefrontDiamond_extend(m_P, m_T, m_m, m_n, ey, ex);
				INT_TYPE extended = compute + extendv;
				
				m_W[POLAR_W_TO_INDEX(d, r, m_k)] = extended;
				
				//printf("(Extended)  + %d = %d (P[%d]=%s - T[%d]=%s)\n",  extendv, extended, ey, &m_P[ey], ex, &m_T[ex] );
				//printf("(Extended)  + %d = %d (P[%d] - T[%d])\n",  extendv, extended, ey,  ex );
	
				
				if ((d == final_d) && extended >= m_top)
				{
					//printf("Finishing d=%d r=%d\n", d, r);
					ret = r;
					goto end_loop;
				}
			}
			else
			{
				m_W[POLAR_W_TO_INDEX(d, r, m_k)] = compute;
			}
		}
	}
	
	// closing the diamond
	for (INT_TYPE r=k_half; r < m_k; r++)
	{
		INT_TYPE cr = m_k - (r  + k_odd); // 5 - (2 + 1) = 5 -3 = 2
		
		for (INT_TYPE d=-cr; d <= cr; d++)
		{			
			INT_TYPE diag_up = (WavefrontDiamond_polarExistsInW(d+1, r-1))? m_W[POLAR_W_TO_INDEX(d+1, r-1, m_k)]  : 0;
			INT_TYPE left = (WavefrontDiamond_polarExistsInW(d,r-1))? m_W[POLAR_W_TO_INDEX(d, r-1, m_k)]  : 0;
			INT_TYPE diag_down = (WavefrontDiamond_polarExistsInW(d-1,r-1))? m_W[POLAR_W_TO_INDEX(d-1, r-1, m_k)]  : 0;
			
			INT_TYPE compute;
			
			if (d == 0)
				compute = max3(diag_up, left+1, diag_down);
			else if (d > 0)
				compute = max3(diag_up, left+1, diag_down+1);
			else
				compute = max3(diag_up+1, left+1, diag_down);
			
			if ((d == final_d) && compute >= m_top)
			{
				ret = r;
				goto end_loop;
			}
			
			INT_TYPE ex = POLAR_W_TO_CARTESIAN_X(d, compute);
			INT_TYPE ey = POLAR_W_TO_CARTESIAN_Y(d, compute);

			if ((ex < m_n) && (ey < m_m))
			{
				//printf("Compute (d=%d r=%d) = max(%d,%d,%d) = %d  || ", d, r , diag_up, left, diag_down, compute );
				
				INT_TYPE extendv = WavefrontDiamond_extend(m_P, m_T, m_m, m_n, ey, ex);
				INT_TYPE extended = compute + extendv;
				
				m_W[POLAR_W_TO_INDEX(d, r, m_k)] = extended;
				
				//printf("(Extended)  + %d = %d (P[%d]=%s - T[%d]=%s)\n",  extendv, extended, ey, &m_P[ey], ex, &m_T[ex] );
				//printf("(Extended)  + %d = %d (P[%d] - T[%d])\n",  extendv, extended, ey,  ex );
	
				
				if ((d == final_d) && extended >= m_top)
				{
					//printf("Finishing d=%d r=%d\n", d, r);
					ret = r;
					goto end_loop;
				}
			}
			else
			{
				m_W[POLAR_W_TO_INDEX(d, r, m_k)] = compute;
			}
		}
	}
	
end_loop:	
	return ret;
}


const char* WavefrontDiamond::getDescription()
{
	return "Wavefront Diamond";
}

char* WavefrontDiamond::getAlignmentPath(INT_TYPE* distance)
{
	printf("Alignment Not implemented yet!\n");
	exit(0);
}
