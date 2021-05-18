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

#ifndef PERFORMANCELAP_H
#define	PERFORMANCELAP_H

#ifdef WIN32
    #include <windows.h>
#endif

class PerformanceLap {
public:
    PerformanceLap();
    virtual ~PerformanceLap();
    
    void start();
    double stop();
    double lap();
    double dtime();
    
private:
    

    double d0;
    double df;
    
#ifdef WIN32
    LARGE_INTEGER m_freq;
#endif
};

#endif	/* PERFORMANCELAP_H */

