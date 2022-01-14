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

#ifndef ALIGNER_H_INCLUDED
#define ALIGNER_H_INCLUDED

#include "PerformanceLap.h"

#include <stdio.h>

class Aligner
{
public:
    virtual void setInput(const char* P, const char* T, long k) = 0;
    virtual long getDistance() = 0;
    virtual char* getAlignmentPath(long* distance) = 0;
    virtual const char* getDescription() = 0; 

    void freePath(char* p)
    {
            delete [] p;
    }


    void execute(const char* P, const char* T, int k, int doAlignmentPath)
    {
            setInput(P, T, k);

            PerformanceLap lap;
            if (doAlignmentPath)
            {
                    long ed;
                    char* path = getAlignmentPath(&ed);
                    lap.stop();

                    printf("Alignment path:\n");
                    //printf(gT);
                    //printf("\n");
                    printf(path);
                    printf("\n");
                    //printf(gP);
                    //printf("\n");
                    printf("%s Distance=%d Time=%0.5f seconds\n", getDescription(),  ed, lap.lap());
                    freePath(path);
            }
            else
            {
                    long ed = getDistance();
                    lap.stop();
                    printf("%s Distance=%d Time=%0.5f seconds\n", getDescription(), ed, lap.lap());
                    printf("m x n = %ld x %ld\n", m_m, m_n);
                    printf("Equivalent GCUPS: %f\n", (((double)m_m*m_n)/(lap.lap() * 1E9)));
            }
    }
        

protected:        
    const char* m_P;
    const char* m_T;
    long m_n;
    long m_m;

};

#endif