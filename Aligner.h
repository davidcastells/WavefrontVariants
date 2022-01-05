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