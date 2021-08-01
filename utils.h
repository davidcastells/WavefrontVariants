#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#define min2(a,b) (((a)<(b))?(a):(b))
#define max2(a,b) (((a)>(b))?(a):(b))
#define min3(a,b,c) min2(a, min2(b, c))
#define max3(a,b,c) max2(a, max2(b, c))


#endif