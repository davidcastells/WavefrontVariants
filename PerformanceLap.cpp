/*
 * Copyright (C) 2018 Universitat Autonoma de Barcelona 
 * David Castells-Rufas <david.castells@uab.cat>
 * 
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PerformanceLap.h"

#include <stdlib.h>

#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

struct timeval tnow;

/* time in seconds */

double PerformanceLap::dtime()
{
#ifdef WIN32
    	LARGE_INTEGER val;
	BOOL bRet = QueryPerformanceCounter((LARGE_INTEGER*)&val);

	double ret = val.QuadPart;

	ret = ret / m_freq.QuadPart;

    return ret;
#else
    double q;

    gettimeofday(&tnow, NULL);
    q = (double)tnow.tv_sec + (double)tnow.tv_usec * 1.0e-6;

    return q;
#endif
}

PerformanceLap::PerformanceLap() 
{
#ifdef WIN32
    BOOL bRet = QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
#endif
    
    start();
}


PerformanceLap::~PerformanceLap() {
}


void PerformanceLap::start()
{
    d0 = dtime();
}

double PerformanceLap::stop()
{
    df = dtime();
    
    return (df-d0);
}

double PerformanceLap::lap()
{
    if (df < d0)
        stop();
    
    return (df-d0);
}