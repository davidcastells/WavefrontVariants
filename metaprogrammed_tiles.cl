#if TILE_LEN == 0
typedef struct
{
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
   return v;
}

#endif
#if TILE_LEN == 1
typedef struct
{
   long r0_d0;
   long r1_d0;
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
    if (r == 0)
   {
       if (d == 0) localW->r0_d0 = v; 
   }
   else if (r == 1)
   {
       if (d == 0) localW->r1_d0 = v; 
   }
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
    if (r == 0)
   {
       if (d == 0) v = localW->r0_d0; 
   }
   else if (r == 1)
   {
       if (d == 0) v = localW->r1_d0; 
   }
   return v;
}

#endif
#if TILE_LEN == 2
typedef struct
{
   long r0_d0;
   long r1_dm1;
   long r1_d0;
   long r1_d1;
   long r2_dm1;
   long r2_d0;
   long r2_d1;
   long r3_d0;
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
    if (r == 1)
   {
       if (d == -1) localW->r1_dm1 = v; 
      else if (d == 0) localW->r1_d0 = v; 
      else if (d == 1) localW->r1_d1 = v; 
   }
   else if (r == 2)
   {
       if (d == -1) localW->r2_dm1 = v; 
      else if (d == 0) localW->r2_d0 = v; 
      else if (d == 1) localW->r2_d1 = v; 
   }
   else if (r == 0)
   {
       if (d == 0) localW->r0_d0 = v; 
   }
   else if (r == 3)
   {
       if (d == 0) localW->r3_d0 = v; 
   }
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
    if (r == 1)
   {
       if (d == -1) v = localW->r1_dm1; 
      else if (d == 0) v = localW->r1_d0; 
      else if (d == 1) v = localW->r1_d1; 
   }
   else if (r == 2)
   {
       if (d == -1) v = localW->r2_dm1; 
      else if (d == 0) v = localW->r2_d0; 
      else if (d == 1) v = localW->r2_d1; 
   }
   else if (r == 0)
   {
       if (d == 0) v = localW->r0_d0; 
   }
   else if (r == 3)
   {
       if (d == 0) v = localW->r3_d0; 
   }
   return v;
}

#endif
#if TILE_LEN == 3
typedef struct
{
   long r0_d0;
   long r1_dm1;
   long r1_d0;
   long r1_d1;
   long r2_dm2;
   long r2_dm1;
   long r2_d0;
   long r2_d1;
   long r2_d2;
   long r3_dm2;
   long r3_dm1;
   long r3_d0;
   long r3_d1;
   long r3_d2;
   long r4_dm1;
   long r4_d0;
   long r4_d1;
   long r5_d0;
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
    if (r == 2)
   {
       if (d == -2) localW->r2_dm2 = v; 
      else if (d == -1) localW->r2_dm1 = v; 
      else if (d == 0) localW->r2_d0 = v; 
      else if (d == 1) localW->r2_d1 = v; 
      else if (d == 2) localW->r2_d2 = v; 
   }
   else if (r == 3)
   {
       if (d == -2) localW->r3_dm2 = v; 
      else if (d == -1) localW->r3_dm1 = v; 
      else if (d == 0) localW->r3_d0 = v; 
      else if (d == 1) localW->r3_d1 = v; 
      else if (d == 2) localW->r3_d2 = v; 
   }
   else if (r == 1)
   {
       if (d == -1) localW->r1_dm1 = v; 
      else if (d == 0) localW->r1_d0 = v; 
      else if (d == 1) localW->r1_d1 = v; 
   }
   else if (r == 4)
   {
       if (d == -1) localW->r4_dm1 = v; 
      else if (d == 0) localW->r4_d0 = v; 
      else if (d == 1) localW->r4_d1 = v; 
   }
   else if (r == 0)
   {
       if (d == 0) localW->r0_d0 = v; 
   }
   else if (r == 5)
   {
       if (d == 0) localW->r5_d0 = v; 
   }
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
    if (r == 2)
   {
       if (d == -2) v = localW->r2_dm2; 
      else if (d == -1) v = localW->r2_dm1; 
      else if (d == 0) v = localW->r2_d0; 
      else if (d == 1) v = localW->r2_d1; 
      else if (d == 2) v = localW->r2_d2; 
   }
   else if (r == 3)
   {
       if (d == -2) v = localW->r3_dm2; 
      else if (d == -1) v = localW->r3_dm1; 
      else if (d == 0) v = localW->r3_d0; 
      else if (d == 1) v = localW->r3_d1; 
      else if (d == 2) v = localW->r3_d2; 
   }
   else if (r == 1)
   {
       if (d == -1) v = localW->r1_dm1; 
      else if (d == 0) v = localW->r1_d0; 
      else if (d == 1) v = localW->r1_d1; 
   }
   else if (r == 4)
   {
       if (d == -1) v = localW->r4_dm1; 
      else if (d == 0) v = localW->r4_d0; 
      else if (d == 1) v = localW->r4_d1; 
   }
   else if (r == 0)
   {
       if (d == 0) v = localW->r0_d0; 
   }
   else if (r == 5)
   {
       if (d == 0) v = localW->r5_d0; 
   }
   return v;
}

#endif
#if TILE_LEN == 4
typedef struct
{
   long r0_d0;
   long r1_dm1;
   long r1_d0;
   long r1_d1;
   long r2_dm2;
   long r2_dm1;
   long r2_d0;
   long r2_d1;
   long r2_d2;
   long r3_dm3;
   long r3_dm2;
   long r3_dm1;
   long r3_d0;
   long r3_d1;
   long r3_d2;
   long r3_d3;
   long r4_dm3;
   long r4_dm2;
   long r4_dm1;
   long r4_d0;
   long r4_d1;
   long r4_d2;
   long r4_d3;
   long r5_dm2;
   long r5_dm1;
   long r5_d0;
   long r5_d1;
   long r5_d2;
   long r6_dm1;
   long r6_d0;
   long r6_d1;
   long r7_d0;
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
    if (r == 3)
   {
       if (d == -3) localW->r3_dm3 = v; 
      else if (d == -2) localW->r3_dm2 = v; 
      else if (d == -1) localW->r3_dm1 = v; 
      else if (d == 0) localW->r3_d0 = v; 
      else if (d == 1) localW->r3_d1 = v; 
      else if (d == 2) localW->r3_d2 = v; 
      else if (d == 3) localW->r3_d3 = v; 
   }
   else if (r == 4)
   {
       if (d == -3) localW->r4_dm3 = v; 
      else if (d == -2) localW->r4_dm2 = v; 
      else if (d == -1) localW->r4_dm1 = v; 
      else if (d == 0) localW->r4_d0 = v; 
      else if (d == 1) localW->r4_d1 = v; 
      else if (d == 2) localW->r4_d2 = v; 
      else if (d == 3) localW->r4_d3 = v; 
   }
   else if (r == 2)
   {
       if (d == -2) localW->r2_dm2 = v; 
      else if (d == -1) localW->r2_dm1 = v; 
      else if (d == 0) localW->r2_d0 = v; 
      else if (d == 1) localW->r2_d1 = v; 
      else if (d == 2) localW->r2_d2 = v; 
   }
   else if (r == 5)
   {
       if (d == -2) localW->r5_dm2 = v; 
      else if (d == -1) localW->r5_dm1 = v; 
      else if (d == 0) localW->r5_d0 = v; 
      else if (d == 1) localW->r5_d1 = v; 
      else if (d == 2) localW->r5_d2 = v; 
   }
   else if (r == 1)
   {
       if (d == -1) localW->r1_dm1 = v; 
      else if (d == 0) localW->r1_d0 = v; 
      else if (d == 1) localW->r1_d1 = v; 
   }
   else if (r == 6)
   {
       if (d == -1) localW->r6_dm1 = v; 
      else if (d == 0) localW->r6_d0 = v; 
      else if (d == 1) localW->r6_d1 = v; 
   }
   else if (r == 0)
   {
       if (d == 0) localW->r0_d0 = v; 
   }
   else if (r == 7)
   {
       if (d == 0) localW->r7_d0 = v; 
   }
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
    if (r == 3)
   {
       if (d == -3) v = localW->r3_dm3; 
      else if (d == -2) v = localW->r3_dm2; 
      else if (d == -1) v = localW->r3_dm1; 
      else if (d == 0) v = localW->r3_d0; 
      else if (d == 1) v = localW->r3_d1; 
      else if (d == 2) v = localW->r3_d2; 
      else if (d == 3) v = localW->r3_d3; 
   }
   else if (r == 4)
   {
       if (d == -3) v = localW->r4_dm3; 
      else if (d == -2) v = localW->r4_dm2; 
      else if (d == -1) v = localW->r4_dm1; 
      else if (d == 0) v = localW->r4_d0; 
      else if (d == 1) v = localW->r4_d1; 
      else if (d == 2) v = localW->r4_d2; 
      else if (d == 3) v = localW->r4_d3; 
   }
   else if (r == 2)
   {
       if (d == -2) v = localW->r2_dm2; 
      else if (d == -1) v = localW->r2_dm1; 
      else if (d == 0) v = localW->r2_d0; 
      else if (d == 1) v = localW->r2_d1; 
      else if (d == 2) v = localW->r2_d2; 
   }
   else if (r == 5)
   {
       if (d == -2) v = localW->r5_dm2; 
      else if (d == -1) v = localW->r5_dm1; 
      else if (d == 0) v = localW->r5_d0; 
      else if (d == 1) v = localW->r5_d1; 
      else if (d == 2) v = localW->r5_d2; 
   }
   else if (r == 1)
   {
       if (d == -1) v = localW->r1_dm1; 
      else if (d == 0) v = localW->r1_d0; 
      else if (d == 1) v = localW->r1_d1; 
   }
   else if (r == 6)
   {
       if (d == -1) v = localW->r6_dm1; 
      else if (d == 0) v = localW->r6_d0; 
      else if (d == 1) v = localW->r6_d1; 
   }
   else if (r == 0)
   {
       if (d == 0) v = localW->r0_d0; 
   }
   else if (r == 7)
   {
       if (d == 0) v = localW->r7_d0; 
   }
   return v;
}

#endif
#if TILE_LEN == 5
typedef struct
{
   long r0_d0;
   long r1_dm1;
   long r1_d0;
   long r1_d1;
   long r2_dm2;
   long r2_dm1;
   long r2_d0;
   long r2_d1;
   long r2_d2;
   long r3_dm3;
   long r3_dm2;
   long r3_dm1;
   long r3_d0;
   long r3_d1;
   long r3_d2;
   long r3_d3;
   long r4_dm4;
   long r4_dm3;
   long r4_dm2;
   long r4_dm1;
   long r4_d0;
   long r4_d1;
   long r4_d2;
   long r4_d3;
   long r4_d4;
   long r5_dm4;
   long r5_dm3;
   long r5_dm2;
   long r5_dm1;
   long r5_d0;
   long r5_d1;
   long r5_d2;
   long r5_d3;
   long r5_d4;
   long r6_dm3;
   long r6_dm2;
   long r6_dm1;
   long r6_d0;
   long r6_d1;
   long r6_d2;
   long r6_d3;
   long r7_dm2;
   long r7_dm1;
   long r7_d0;
   long r7_d1;
   long r7_d2;
   long r8_dm1;
   long r8_d0;
   long r8_d1;
   long r9_d0;
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
    if (r == 4)
   {
       if (d == -4) localW->r4_dm4 = v; 
      else if (d == -3) localW->r4_dm3 = v; 
      else if (d == -2) localW->r4_dm2 = v; 
      else if (d == -1) localW->r4_dm1 = v; 
      else if (d == 0) localW->r4_d0 = v; 
      else if (d == 1) localW->r4_d1 = v; 
      else if (d == 2) localW->r4_d2 = v; 
      else if (d == 3) localW->r4_d3 = v; 
      else if (d == 4) localW->r4_d4 = v; 
   }
   else if (r == 5)
   {
       if (d == -4) localW->r5_dm4 = v; 
      else if (d == -3) localW->r5_dm3 = v; 
      else if (d == -2) localW->r5_dm2 = v; 
      else if (d == -1) localW->r5_dm1 = v; 
      else if (d == 0) localW->r5_d0 = v; 
      else if (d == 1) localW->r5_d1 = v; 
      else if (d == 2) localW->r5_d2 = v; 
      else if (d == 3) localW->r5_d3 = v; 
      else if (d == 4) localW->r5_d4 = v; 
   }
   else if (r == 3)
   {
       if (d == -3) localW->r3_dm3 = v; 
      else if (d == -2) localW->r3_dm2 = v; 
      else if (d == -1) localW->r3_dm1 = v; 
      else if (d == 0) localW->r3_d0 = v; 
      else if (d == 1) localW->r3_d1 = v; 
      else if (d == 2) localW->r3_d2 = v; 
      else if (d == 3) localW->r3_d3 = v; 
   }
   else if (r == 6)
   {
       if (d == -3) localW->r6_dm3 = v; 
      else if (d == -2) localW->r6_dm2 = v; 
      else if (d == -1) localW->r6_dm1 = v; 
      else if (d == 0) localW->r6_d0 = v; 
      else if (d == 1) localW->r6_d1 = v; 
      else if (d == 2) localW->r6_d2 = v; 
      else if (d == 3) localW->r6_d3 = v; 
   }
   else if (r == 2)
   {
       if (d == -2) localW->r2_dm2 = v; 
      else if (d == -1) localW->r2_dm1 = v; 
      else if (d == 0) localW->r2_d0 = v; 
      else if (d == 1) localW->r2_d1 = v; 
      else if (d == 2) localW->r2_d2 = v; 
   }
   else if (r == 7)
   {
       if (d == -2) localW->r7_dm2 = v; 
      else if (d == -1) localW->r7_dm1 = v; 
      else if (d == 0) localW->r7_d0 = v; 
      else if (d == 1) localW->r7_d1 = v; 
      else if (d == 2) localW->r7_d2 = v; 
   }
   else if (r == 1)
   {
       if (d == -1) localW->r1_dm1 = v; 
      else if (d == 0) localW->r1_d0 = v; 
      else if (d == 1) localW->r1_d1 = v; 
   }
   else if (r == 8)
   {
       if (d == -1) localW->r8_dm1 = v; 
      else if (d == 0) localW->r8_d0 = v; 
      else if (d == 1) localW->r8_d1 = v; 
   }
   else if (r == 0)
   {
       if (d == 0) localW->r0_d0 = v; 
   }
   else if (r == 9)
   {
       if (d == 0) localW->r9_d0 = v; 
   }
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
    if (r == 4)
   {
       if (d == -4) v = localW->r4_dm4; 
      else if (d == -3) v = localW->r4_dm3; 
      else if (d == -2) v = localW->r4_dm2; 
      else if (d == -1) v = localW->r4_dm1; 
      else if (d == 0) v = localW->r4_d0; 
      else if (d == 1) v = localW->r4_d1; 
      else if (d == 2) v = localW->r4_d2; 
      else if (d == 3) v = localW->r4_d3; 
      else if (d == 4) v = localW->r4_d4; 
   }
   else if (r == 5)
   {
       if (d == -4) v = localW->r5_dm4; 
      else if (d == -3) v = localW->r5_dm3; 
      else if (d == -2) v = localW->r5_dm2; 
      else if (d == -1) v = localW->r5_dm1; 
      else if (d == 0) v = localW->r5_d0; 
      else if (d == 1) v = localW->r5_d1; 
      else if (d == 2) v = localW->r5_d2; 
      else if (d == 3) v = localW->r5_d3; 
      else if (d == 4) v = localW->r5_d4; 
   }
   else if (r == 3)
   {
       if (d == -3) v = localW->r3_dm3; 
      else if (d == -2) v = localW->r3_dm2; 
      else if (d == -1) v = localW->r3_dm1; 
      else if (d == 0) v = localW->r3_d0; 
      else if (d == 1) v = localW->r3_d1; 
      else if (d == 2) v = localW->r3_d2; 
      else if (d == 3) v = localW->r3_d3; 
   }
   else if (r == 6)
   {
       if (d == -3) v = localW->r6_dm3; 
      else if (d == -2) v = localW->r6_dm2; 
      else if (d == -1) v = localW->r6_dm1; 
      else if (d == 0) v = localW->r6_d0; 
      else if (d == 1) v = localW->r6_d1; 
      else if (d == 2) v = localW->r6_d2; 
      else if (d == 3) v = localW->r6_d3; 
   }
   else if (r == 2)
   {
       if (d == -2) v = localW->r2_dm2; 
      else if (d == -1) v = localW->r2_dm1; 
      else if (d == 0) v = localW->r2_d0; 
      else if (d == 1) v = localW->r2_d1; 
      else if (d == 2) v = localW->r2_d2; 
   }
   else if (r == 7)
   {
       if (d == -2) v = localW->r7_dm2; 
      else if (d == -1) v = localW->r7_dm1; 
      else if (d == 0) v = localW->r7_d0; 
      else if (d == 1) v = localW->r7_d1; 
      else if (d == 2) v = localW->r7_d2; 
   }
   else if (r == 1)
   {
       if (d == -1) v = localW->r1_dm1; 
      else if (d == 0) v = localW->r1_d0; 
      else if (d == 1) v = localW->r1_d1; 
   }
   else if (r == 8)
   {
       if (d == -1) v = localW->r8_dm1; 
      else if (d == 0) v = localW->r8_d0; 
      else if (d == 1) v = localW->r8_d1; 
   }
   else if (r == 0)
   {
       if (d == 0) v = localW->r0_d0; 
   }
   else if (r == 9)
   {
       if (d == 0) v = localW->r9_d0; 
   }
   return v;
}

#endif
#if TILE_LEN == 6
typedef struct
{
   long r0_d0;
   long r1_dm1;
   long r1_d0;
   long r1_d1;
   long r2_dm2;
   long r2_dm1;
   long r2_d0;
   long r2_d1;
   long r2_d2;
   long r3_dm3;
   long r3_dm2;
   long r3_dm1;
   long r3_d0;
   long r3_d1;
   long r3_d2;
   long r3_d3;
   long r4_dm4;
   long r4_dm3;
   long r4_dm2;
   long r4_dm1;
   long r4_d0;
   long r4_d1;
   long r4_d2;
   long r4_d3;
   long r4_d4;
   long r5_dm5;
   long r5_dm4;
   long r5_dm3;
   long r5_dm2;
   long r5_dm1;
   long r5_d0;
   long r5_d1;
   long r5_d2;
   long r5_d3;
   long r5_d4;
   long r5_d5;
   long r6_dm5;
   long r6_dm4;
   long r6_dm3;
   long r6_dm2;
   long r6_dm1;
   long r6_d0;
   long r6_d1;
   long r6_d2;
   long r6_d3;
   long r6_d4;
   long r6_d5;
   long r7_dm4;
   long r7_dm3;
   long r7_dm2;
   long r7_dm1;
   long r7_d0;
   long r7_d1;
   long r7_d2;
   long r7_d3;
   long r7_d4;
   long r8_dm3;
   long r8_dm2;
   long r8_dm1;
   long r8_d0;
   long r8_d1;
   long r8_d2;
   long r8_d3;
   long r9_dm2;
   long r9_dm1;
   long r9_d0;
   long r9_d1;
   long r9_d2;
   long r10_dm1;
   long r10_d0;
   long r10_d1;
   long r11_d0;
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
    if (r == 5)
   {
       if (d == -5) localW->r5_dm5 = v; 
      else if (d == -4) localW->r5_dm4 = v; 
      else if (d == -3) localW->r5_dm3 = v; 
      else if (d == -2) localW->r5_dm2 = v; 
      else if (d == -1) localW->r5_dm1 = v; 
      else if (d == 0) localW->r5_d0 = v; 
      else if (d == 1) localW->r5_d1 = v; 
      else if (d == 2) localW->r5_d2 = v; 
      else if (d == 3) localW->r5_d3 = v; 
      else if (d == 4) localW->r5_d4 = v; 
      else if (d == 5) localW->r5_d5 = v; 
   }
   else if (r == 6)
   {
       if (d == -5) localW->r6_dm5 = v; 
      else if (d == -4) localW->r6_dm4 = v; 
      else if (d == -3) localW->r6_dm3 = v; 
      else if (d == -2) localW->r6_dm2 = v; 
      else if (d == -1) localW->r6_dm1 = v; 
      else if (d == 0) localW->r6_d0 = v; 
      else if (d == 1) localW->r6_d1 = v; 
      else if (d == 2) localW->r6_d2 = v; 
      else if (d == 3) localW->r6_d3 = v; 
      else if (d == 4) localW->r6_d4 = v; 
      else if (d == 5) localW->r6_d5 = v; 
   }
   else if (r == 4)
   {
       if (d == -4) localW->r4_dm4 = v; 
      else if (d == -3) localW->r4_dm3 = v; 
      else if (d == -2) localW->r4_dm2 = v; 
      else if (d == -1) localW->r4_dm1 = v; 
      else if (d == 0) localW->r4_d0 = v; 
      else if (d == 1) localW->r4_d1 = v; 
      else if (d == 2) localW->r4_d2 = v; 
      else if (d == 3) localW->r4_d3 = v; 
      else if (d == 4) localW->r4_d4 = v; 
   }
   else if (r == 7)
   {
       if (d == -4) localW->r7_dm4 = v; 
      else if (d == -3) localW->r7_dm3 = v; 
      else if (d == -2) localW->r7_dm2 = v; 
      else if (d == -1) localW->r7_dm1 = v; 
      else if (d == 0) localW->r7_d0 = v; 
      else if (d == 1) localW->r7_d1 = v; 
      else if (d == 2) localW->r7_d2 = v; 
      else if (d == 3) localW->r7_d3 = v; 
      else if (d == 4) localW->r7_d4 = v; 
   }
   else if (r == 3)
   {
       if (d == -3) localW->r3_dm3 = v; 
      else if (d == -2) localW->r3_dm2 = v; 
      else if (d == -1) localW->r3_dm1 = v; 
      else if (d == 0) localW->r3_d0 = v; 
      else if (d == 1) localW->r3_d1 = v; 
      else if (d == 2) localW->r3_d2 = v; 
      else if (d == 3) localW->r3_d3 = v; 
   }
   else if (r == 8)
   {
       if (d == -3) localW->r8_dm3 = v; 
      else if (d == -2) localW->r8_dm2 = v; 
      else if (d == -1) localW->r8_dm1 = v; 
      else if (d == 0) localW->r8_d0 = v; 
      else if (d == 1) localW->r8_d1 = v; 
      else if (d == 2) localW->r8_d2 = v; 
      else if (d == 3) localW->r8_d3 = v; 
   }
   else if (r == 2)
   {
       if (d == -2) localW->r2_dm2 = v; 
      else if (d == -1) localW->r2_dm1 = v; 
      else if (d == 0) localW->r2_d0 = v; 
      else if (d == 1) localW->r2_d1 = v; 
      else if (d == 2) localW->r2_d2 = v; 
   }
   else if (r == 9)
   {
       if (d == -2) localW->r9_dm2 = v; 
      else if (d == -1) localW->r9_dm1 = v; 
      else if (d == 0) localW->r9_d0 = v; 
      else if (d == 1) localW->r9_d1 = v; 
      else if (d == 2) localW->r9_d2 = v; 
   }
   else if (r == 1)
   {
       if (d == -1) localW->r1_dm1 = v; 
      else if (d == 0) localW->r1_d0 = v; 
      else if (d == 1) localW->r1_d1 = v; 
   }
   else if (r == 10)
   {
       if (d == -1) localW->r10_dm1 = v; 
      else if (d == 0) localW->r10_d0 = v; 
      else if (d == 1) localW->r10_d1 = v; 
   }
   else if (r == 0)
   {
       if (d == 0) localW->r0_d0 = v; 
   }
   else if (r == 11)
   {
       if (d == 0) localW->r11_d0 = v; 
   }
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
    if (r == 5)
   {
       if (d == -5) v = localW->r5_dm5; 
      else if (d == -4) v = localW->r5_dm4; 
      else if (d == -3) v = localW->r5_dm3; 
      else if (d == -2) v = localW->r5_dm2; 
      else if (d == -1) v = localW->r5_dm1; 
      else if (d == 0) v = localW->r5_d0; 
      else if (d == 1) v = localW->r5_d1; 
      else if (d == 2) v = localW->r5_d2; 
      else if (d == 3) v = localW->r5_d3; 
      else if (d == 4) v = localW->r5_d4; 
      else if (d == 5) v = localW->r5_d5; 
   }
   else if (r == 6)
   {
       if (d == -5) v = localW->r6_dm5; 
      else if (d == -4) v = localW->r6_dm4; 
      else if (d == -3) v = localW->r6_dm3; 
      else if (d == -2) v = localW->r6_dm2; 
      else if (d == -1) v = localW->r6_dm1; 
      else if (d == 0) v = localW->r6_d0; 
      else if (d == 1) v = localW->r6_d1; 
      else if (d == 2) v = localW->r6_d2; 
      else if (d == 3) v = localW->r6_d3; 
      else if (d == 4) v = localW->r6_d4; 
      else if (d == 5) v = localW->r6_d5; 
   }
   else if (r == 4)
   {
       if (d == -4) v = localW->r4_dm4; 
      else if (d == -3) v = localW->r4_dm3; 
      else if (d == -2) v = localW->r4_dm2; 
      else if (d == -1) v = localW->r4_dm1; 
      else if (d == 0) v = localW->r4_d0; 
      else if (d == 1) v = localW->r4_d1; 
      else if (d == 2) v = localW->r4_d2; 
      else if (d == 3) v = localW->r4_d3; 
      else if (d == 4) v = localW->r4_d4; 
   }
   else if (r == 7)
   {
       if (d == -4) v = localW->r7_dm4; 
      else if (d == -3) v = localW->r7_dm3; 
      else if (d == -2) v = localW->r7_dm2; 
      else if (d == -1) v = localW->r7_dm1; 
      else if (d == 0) v = localW->r7_d0; 
      else if (d == 1) v = localW->r7_d1; 
      else if (d == 2) v = localW->r7_d2; 
      else if (d == 3) v = localW->r7_d3; 
      else if (d == 4) v = localW->r7_d4; 
   }
   else if (r == 3)
   {
       if (d == -3) v = localW->r3_dm3; 
      else if (d == -2) v = localW->r3_dm2; 
      else if (d == -1) v = localW->r3_dm1; 
      else if (d == 0) v = localW->r3_d0; 
      else if (d == 1) v = localW->r3_d1; 
      else if (d == 2) v = localW->r3_d2; 
      else if (d == 3) v = localW->r3_d3; 
   }
   else if (r == 8)
   {
       if (d == -3) v = localW->r8_dm3; 
      else if (d == -2) v = localW->r8_dm2; 
      else if (d == -1) v = localW->r8_dm1; 
      else if (d == 0) v = localW->r8_d0; 
      else if (d == 1) v = localW->r8_d1; 
      else if (d == 2) v = localW->r8_d2; 
      else if (d == 3) v = localW->r8_d3; 
   }
   else if (r == 2)
   {
       if (d == -2) v = localW->r2_dm2; 
      else if (d == -1) v = localW->r2_dm1; 
      else if (d == 0) v = localW->r2_d0; 
      else if (d == 1) v = localW->r2_d1; 
      else if (d == 2) v = localW->r2_d2; 
   }
   else if (r == 9)
   {
       if (d == -2) v = localW->r9_dm2; 
      else if (d == -1) v = localW->r9_dm1; 
      else if (d == 0) v = localW->r9_d0; 
      else if (d == 1) v = localW->r9_d1; 
      else if (d == 2) v = localW->r9_d2; 
   }
   else if (r == 1)
   {
       if (d == -1) v = localW->r1_dm1; 
      else if (d == 0) v = localW->r1_d0; 
      else if (d == 1) v = localW->r1_d1; 
   }
   else if (r == 10)
   {
       if (d == -1) v = localW->r10_dm1; 
      else if (d == 0) v = localW->r10_d0; 
      else if (d == 1) v = localW->r10_d1; 
   }
   else if (r == 0)
   {
       if (d == 0) v = localW->r0_d0; 
   }
   else if (r == 11)
   {
       if (d == 0) v = localW->r11_d0; 
   }
   return v;
}

#endif
#if TILE_LEN == 7
typedef struct
{
   long r0_d0;
   long r1_dm1;
   long r1_d0;
   long r1_d1;
   long r2_dm2;
   long r2_dm1;
   long r2_d0;
   long r2_d1;
   long r2_d2;
   long r3_dm3;
   long r3_dm2;
   long r3_dm1;
   long r3_d0;
   long r3_d1;
   long r3_d2;
   long r3_d3;
   long r4_dm4;
   long r4_dm3;
   long r4_dm2;
   long r4_dm1;
   long r4_d0;
   long r4_d1;
   long r4_d2;
   long r4_d3;
   long r4_d4;
   long r5_dm5;
   long r5_dm4;
   long r5_dm3;
   long r5_dm2;
   long r5_dm1;
   long r5_d0;
   long r5_d1;
   long r5_d2;
   long r5_d3;
   long r5_d4;
   long r5_d5;
   long r6_dm6;
   long r6_dm5;
   long r6_dm4;
   long r6_dm3;
   long r6_dm2;
   long r6_dm1;
   long r6_d0;
   long r6_d1;
   long r6_d2;
   long r6_d3;
   long r6_d4;
   long r6_d5;
   long r6_d6;
   long r7_dm6;
   long r7_dm5;
   long r7_dm4;
   long r7_dm3;
   long r7_dm2;
   long r7_dm1;
   long r7_d0;
   long r7_d1;
   long r7_d2;
   long r7_d3;
   long r7_d4;
   long r7_d5;
   long r7_d6;
   long r8_dm5;
   long r8_dm4;
   long r8_dm3;
   long r8_dm2;
   long r8_dm1;
   long r8_d0;
   long r8_d1;
   long r8_d2;
   long r8_d3;
   long r8_d4;
   long r8_d5;
   long r9_dm4;
   long r9_dm3;
   long r9_dm2;
   long r9_dm1;
   long r9_d0;
   long r9_d1;
   long r9_d2;
   long r9_d3;
   long r9_d4;
   long r10_dm3;
   long r10_dm2;
   long r10_dm1;
   long r10_d0;
   long r10_d1;
   long r10_d2;
   long r10_d3;
   long r11_dm2;
   long r11_dm1;
   long r11_d0;
   long r11_d1;
   long r11_d2;
   long r12_dm1;
   long r12_d0;
   long r12_d1;
   long r13_d0;
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
    if (r == 6)
   {
       if (d == -6) localW->r6_dm6 = v; 
      else if (d == -5) localW->r6_dm5 = v; 
      else if (d == -4) localW->r6_dm4 = v; 
      else if (d == -3) localW->r6_dm3 = v; 
      else if (d == -2) localW->r6_dm2 = v; 
      else if (d == -1) localW->r6_dm1 = v; 
      else if (d == 0) localW->r6_d0 = v; 
      else if (d == 1) localW->r6_d1 = v; 
      else if (d == 2) localW->r6_d2 = v; 
      else if (d == 3) localW->r6_d3 = v; 
      else if (d == 4) localW->r6_d4 = v; 
      else if (d == 5) localW->r6_d5 = v; 
      else if (d == 6) localW->r6_d6 = v; 
   }
   else if (r == 7)
   {
       if (d == -6) localW->r7_dm6 = v; 
      else if (d == -5) localW->r7_dm5 = v; 
      else if (d == -4) localW->r7_dm4 = v; 
      else if (d == -3) localW->r7_dm3 = v; 
      else if (d == -2) localW->r7_dm2 = v; 
      else if (d == -1) localW->r7_dm1 = v; 
      else if (d == 0) localW->r7_d0 = v; 
      else if (d == 1) localW->r7_d1 = v; 
      else if (d == 2) localW->r7_d2 = v; 
      else if (d == 3) localW->r7_d3 = v; 
      else if (d == 4) localW->r7_d4 = v; 
      else if (d == 5) localW->r7_d5 = v; 
      else if (d == 6) localW->r7_d6 = v; 
   }
   else if (r == 5)
   {
       if (d == -5) localW->r5_dm5 = v; 
      else if (d == -4) localW->r5_dm4 = v; 
      else if (d == -3) localW->r5_dm3 = v; 
      else if (d == -2) localW->r5_dm2 = v; 
      else if (d == -1) localW->r5_dm1 = v; 
      else if (d == 0) localW->r5_d0 = v; 
      else if (d == 1) localW->r5_d1 = v; 
      else if (d == 2) localW->r5_d2 = v; 
      else if (d == 3) localW->r5_d3 = v; 
      else if (d == 4) localW->r5_d4 = v; 
      else if (d == 5) localW->r5_d5 = v; 
   }
   else if (r == 8)
   {
       if (d == -5) localW->r8_dm5 = v; 
      else if (d == -4) localW->r8_dm4 = v; 
      else if (d == -3) localW->r8_dm3 = v; 
      else if (d == -2) localW->r8_dm2 = v; 
      else if (d == -1) localW->r8_dm1 = v; 
      else if (d == 0) localW->r8_d0 = v; 
      else if (d == 1) localW->r8_d1 = v; 
      else if (d == 2) localW->r8_d2 = v; 
      else if (d == 3) localW->r8_d3 = v; 
      else if (d == 4) localW->r8_d4 = v; 
      else if (d == 5) localW->r8_d5 = v; 
   }
   else if (r == 4)
   {
       if (d == -4) localW->r4_dm4 = v; 
      else if (d == -3) localW->r4_dm3 = v; 
      else if (d == -2) localW->r4_dm2 = v; 
      else if (d == -1) localW->r4_dm1 = v; 
      else if (d == 0) localW->r4_d0 = v; 
      else if (d == 1) localW->r4_d1 = v; 
      else if (d == 2) localW->r4_d2 = v; 
      else if (d == 3) localW->r4_d3 = v; 
      else if (d == 4) localW->r4_d4 = v; 
   }
   else if (r == 9)
   {
       if (d == -4) localW->r9_dm4 = v; 
      else if (d == -3) localW->r9_dm3 = v; 
      else if (d == -2) localW->r9_dm2 = v; 
      else if (d == -1) localW->r9_dm1 = v; 
      else if (d == 0) localW->r9_d0 = v; 
      else if (d == 1) localW->r9_d1 = v; 
      else if (d == 2) localW->r9_d2 = v; 
      else if (d == 3) localW->r9_d3 = v; 
      else if (d == 4) localW->r9_d4 = v; 
   }
   else if (r == 3)
   {
       if (d == -3) localW->r3_dm3 = v; 
      else if (d == -2) localW->r3_dm2 = v; 
      else if (d == -1) localW->r3_dm1 = v; 
      else if (d == 0) localW->r3_d0 = v; 
      else if (d == 1) localW->r3_d1 = v; 
      else if (d == 2) localW->r3_d2 = v; 
      else if (d == 3) localW->r3_d3 = v; 
   }
   else if (r == 10)
   {
       if (d == -3) localW->r10_dm3 = v; 
      else if (d == -2) localW->r10_dm2 = v; 
      else if (d == -1) localW->r10_dm1 = v; 
      else if (d == 0) localW->r10_d0 = v; 
      else if (d == 1) localW->r10_d1 = v; 
      else if (d == 2) localW->r10_d2 = v; 
      else if (d == 3) localW->r10_d3 = v; 
   }
   else if (r == 2)
   {
       if (d == -2) localW->r2_dm2 = v; 
      else if (d == -1) localW->r2_dm1 = v; 
      else if (d == 0) localW->r2_d0 = v; 
      else if (d == 1) localW->r2_d1 = v; 
      else if (d == 2) localW->r2_d2 = v; 
   }
   else if (r == 11)
   {
       if (d == -2) localW->r11_dm2 = v; 
      else if (d == -1) localW->r11_dm1 = v; 
      else if (d == 0) localW->r11_d0 = v; 
      else if (d == 1) localW->r11_d1 = v; 
      else if (d == 2) localW->r11_d2 = v; 
   }
   else if (r == 1)
   {
       if (d == -1) localW->r1_dm1 = v; 
      else if (d == 0) localW->r1_d0 = v; 
      else if (d == 1) localW->r1_d1 = v; 
   }
   else if (r == 12)
   {
       if (d == -1) localW->r12_dm1 = v; 
      else if (d == 0) localW->r12_d0 = v; 
      else if (d == 1) localW->r12_d1 = v; 
   }
   else if (r == 0)
   {
       if (d == 0) localW->r0_d0 = v; 
   }
   else if (r == 13)
   {
       if (d == 0) localW->r13_d0 = v; 
   }
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
    if (r == 6)
   {
       if (d == -6) v = localW->r6_dm6; 
      else if (d == -5) v = localW->r6_dm5; 
      else if (d == -4) v = localW->r6_dm4; 
      else if (d == -3) v = localW->r6_dm3; 
      else if (d == -2) v = localW->r6_dm2; 
      else if (d == -1) v = localW->r6_dm1; 
      else if (d == 0) v = localW->r6_d0; 
      else if (d == 1) v = localW->r6_d1; 
      else if (d == 2) v = localW->r6_d2; 
      else if (d == 3) v = localW->r6_d3; 
      else if (d == 4) v = localW->r6_d4; 
      else if (d == 5) v = localW->r6_d5; 
      else if (d == 6) v = localW->r6_d6; 
   }
   else if (r == 7)
   {
       if (d == -6) v = localW->r7_dm6; 
      else if (d == -5) v = localW->r7_dm5; 
      else if (d == -4) v = localW->r7_dm4; 
      else if (d == -3) v = localW->r7_dm3; 
      else if (d == -2) v = localW->r7_dm2; 
      else if (d == -1) v = localW->r7_dm1; 
      else if (d == 0) v = localW->r7_d0; 
      else if (d == 1) v = localW->r7_d1; 
      else if (d == 2) v = localW->r7_d2; 
      else if (d == 3) v = localW->r7_d3; 
      else if (d == 4) v = localW->r7_d4; 
      else if (d == 5) v = localW->r7_d5; 
      else if (d == 6) v = localW->r7_d6; 
   }
   else if (r == 5)
   {
       if (d == -5) v = localW->r5_dm5; 
      else if (d == -4) v = localW->r5_dm4; 
      else if (d == -3) v = localW->r5_dm3; 
      else if (d == -2) v = localW->r5_dm2; 
      else if (d == -1) v = localW->r5_dm1; 
      else if (d == 0) v = localW->r5_d0; 
      else if (d == 1) v = localW->r5_d1; 
      else if (d == 2) v = localW->r5_d2; 
      else if (d == 3) v = localW->r5_d3; 
      else if (d == 4) v = localW->r5_d4; 
      else if (d == 5) v = localW->r5_d5; 
   }
   else if (r == 8)
   {
       if (d == -5) v = localW->r8_dm5; 
      else if (d == -4) v = localW->r8_dm4; 
      else if (d == -3) v = localW->r8_dm3; 
      else if (d == -2) v = localW->r8_dm2; 
      else if (d == -1) v = localW->r8_dm1; 
      else if (d == 0) v = localW->r8_d0; 
      else if (d == 1) v = localW->r8_d1; 
      else if (d == 2) v = localW->r8_d2; 
      else if (d == 3) v = localW->r8_d3; 
      else if (d == 4) v = localW->r8_d4; 
      else if (d == 5) v = localW->r8_d5; 
   }
   else if (r == 4)
   {
       if (d == -4) v = localW->r4_dm4; 
      else if (d == -3) v = localW->r4_dm3; 
      else if (d == -2) v = localW->r4_dm2; 
      else if (d == -1) v = localW->r4_dm1; 
      else if (d == 0) v = localW->r4_d0; 
      else if (d == 1) v = localW->r4_d1; 
      else if (d == 2) v = localW->r4_d2; 
      else if (d == 3) v = localW->r4_d3; 
      else if (d == 4) v = localW->r4_d4; 
   }
   else if (r == 9)
   {
       if (d == -4) v = localW->r9_dm4; 
      else if (d == -3) v = localW->r9_dm3; 
      else if (d == -2) v = localW->r9_dm2; 
      else if (d == -1) v = localW->r9_dm1; 
      else if (d == 0) v = localW->r9_d0; 
      else if (d == 1) v = localW->r9_d1; 
      else if (d == 2) v = localW->r9_d2; 
      else if (d == 3) v = localW->r9_d3; 
      else if (d == 4) v = localW->r9_d4; 
   }
   else if (r == 3)
   {
       if (d == -3) v = localW->r3_dm3; 
      else if (d == -2) v = localW->r3_dm2; 
      else if (d == -1) v = localW->r3_dm1; 
      else if (d == 0) v = localW->r3_d0; 
      else if (d == 1) v = localW->r3_d1; 
      else if (d == 2) v = localW->r3_d2; 
      else if (d == 3) v = localW->r3_d3; 
   }
   else if (r == 10)
   {
       if (d == -3) v = localW->r10_dm3; 
      else if (d == -2) v = localW->r10_dm2; 
      else if (d == -1) v = localW->r10_dm1; 
      else if (d == 0) v = localW->r10_d0; 
      else if (d == 1) v = localW->r10_d1; 
      else if (d == 2) v = localW->r10_d2; 
      else if (d == 3) v = localW->r10_d3; 
   }
   else if (r == 2)
   {
       if (d == -2) v = localW->r2_dm2; 
      else if (d == -1) v = localW->r2_dm1; 
      else if (d == 0) v = localW->r2_d0; 
      else if (d == 1) v = localW->r2_d1; 
      else if (d == 2) v = localW->r2_d2; 
   }
   else if (r == 11)
   {
       if (d == -2) v = localW->r11_dm2; 
      else if (d == -1) v = localW->r11_dm1; 
      else if (d == 0) v = localW->r11_d0; 
      else if (d == 1) v = localW->r11_d1; 
      else if (d == 2) v = localW->r11_d2; 
   }
   else if (r == 1)
   {
       if (d == -1) v = localW->r1_dm1; 
      else if (d == 0) v = localW->r1_d0; 
      else if (d == 1) v = localW->r1_d1; 
   }
   else if (r == 12)
   {
       if (d == -1) v = localW->r12_dm1; 
      else if (d == 0) v = localW->r12_d0; 
      else if (d == 1) v = localW->r12_d1; 
   }
   else if (r == 0)
   {
       if (d == 0) v = localW->r0_d0; 
   }
   else if (r == 13)
   {
       if (d == 0) v = localW->r13_d0; 
   }
   return v;
}

#endif
#if TILE_LEN == 8
typedef struct
{
   long r0_d0;
   long r1_dm1;
   long r1_d0;
   long r1_d1;
   long r2_dm2;
   long r2_dm1;
   long r2_d0;
   long r2_d1;
   long r2_d2;
   long r3_dm3;
   long r3_dm2;
   long r3_dm1;
   long r3_d0;
   long r3_d1;
   long r3_d2;
   long r3_d3;
   long r4_dm4;
   long r4_dm3;
   long r4_dm2;
   long r4_dm1;
   long r4_d0;
   long r4_d1;
   long r4_d2;
   long r4_d3;
   long r4_d4;
   long r5_dm5;
   long r5_dm4;
   long r5_dm3;
   long r5_dm2;
   long r5_dm1;
   long r5_d0;
   long r5_d1;
   long r5_d2;
   long r5_d3;
   long r5_d4;
   long r5_d5;
   long r6_dm6;
   long r6_dm5;
   long r6_dm4;
   long r6_dm3;
   long r6_dm2;
   long r6_dm1;
   long r6_d0;
   long r6_d1;
   long r6_d2;
   long r6_d3;
   long r6_d4;
   long r6_d5;
   long r6_d6;
   long r7_dm7;
   long r7_dm6;
   long r7_dm5;
   long r7_dm4;
   long r7_dm3;
   long r7_dm2;
   long r7_dm1;
   long r7_d0;
   long r7_d1;
   long r7_d2;
   long r7_d3;
   long r7_d4;
   long r7_d5;
   long r7_d6;
   long r7_d7;
   long r8_dm7;
   long r8_dm6;
   long r8_dm5;
   long r8_dm4;
   long r8_dm3;
   long r8_dm2;
   long r8_dm1;
   long r8_d0;
   long r8_d1;
   long r8_d2;
   long r8_d3;
   long r8_d4;
   long r8_d5;
   long r8_d6;
   long r8_d7;
   long r9_dm6;
   long r9_dm5;
   long r9_dm4;
   long r9_dm3;
   long r9_dm2;
   long r9_dm1;
   long r9_d0;
   long r9_d1;
   long r9_d2;
   long r9_d3;
   long r9_d4;
   long r9_d5;
   long r9_d6;
   long r10_dm5;
   long r10_dm4;
   long r10_dm3;
   long r10_dm2;
   long r10_dm1;
   long r10_d0;
   long r10_d1;
   long r10_d2;
   long r10_d3;
   long r10_d4;
   long r10_d5;
   long r11_dm4;
   long r11_dm3;
   long r11_dm2;
   long r11_dm1;
   long r11_d0;
   long r11_d1;
   long r11_d2;
   long r11_d3;
   long r11_d4;
   long r12_dm3;
   long r12_dm2;
   long r12_dm1;
   long r12_d0;
   long r12_d1;
   long r12_d2;
   long r12_d3;
   long r13_dm2;
   long r13_dm1;
   long r13_d0;
   long r13_d1;
   long r13_d2;
   long r14_dm1;
   long r14_d0;
   long r14_d1;
   long r15_d0;
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
    if (r == 7)
   {
       if (d == -7) localW->r7_dm7 = v; 
      else if (d == -6) localW->r7_dm6 = v; 
      else if (d == -5) localW->r7_dm5 = v; 
      else if (d == -4) localW->r7_dm4 = v; 
      else if (d == -3) localW->r7_dm3 = v; 
      else if (d == -2) localW->r7_dm2 = v; 
      else if (d == -1) localW->r7_dm1 = v; 
      else if (d == 0) localW->r7_d0 = v; 
      else if (d == 1) localW->r7_d1 = v; 
      else if (d == 2) localW->r7_d2 = v; 
      else if (d == 3) localW->r7_d3 = v; 
      else if (d == 4) localW->r7_d4 = v; 
      else if (d == 5) localW->r7_d5 = v; 
      else if (d == 6) localW->r7_d6 = v; 
      else if (d == 7) localW->r7_d7 = v; 
   }
   else if (r == 8)
   {
       if (d == -7) localW->r8_dm7 = v; 
      else if (d == -6) localW->r8_dm6 = v; 
      else if (d == -5) localW->r8_dm5 = v; 
      else if (d == -4) localW->r8_dm4 = v; 
      else if (d == -3) localW->r8_dm3 = v; 
      else if (d == -2) localW->r8_dm2 = v; 
      else if (d == -1) localW->r8_dm1 = v; 
      else if (d == 0) localW->r8_d0 = v; 
      else if (d == 1) localW->r8_d1 = v; 
      else if (d == 2) localW->r8_d2 = v; 
      else if (d == 3) localW->r8_d3 = v; 
      else if (d == 4) localW->r8_d4 = v; 
      else if (d == 5) localW->r8_d5 = v; 
      else if (d == 6) localW->r8_d6 = v; 
      else if (d == 7) localW->r8_d7 = v; 
   }
   else if (r == 6)
   {
       if (d == -6) localW->r6_dm6 = v; 
      else if (d == -5) localW->r6_dm5 = v; 
      else if (d == -4) localW->r6_dm4 = v; 
      else if (d == -3) localW->r6_dm3 = v; 
      else if (d == -2) localW->r6_dm2 = v; 
      else if (d == -1) localW->r6_dm1 = v; 
      else if (d == 0) localW->r6_d0 = v; 
      else if (d == 1) localW->r6_d1 = v; 
      else if (d == 2) localW->r6_d2 = v; 
      else if (d == 3) localW->r6_d3 = v; 
      else if (d == 4) localW->r6_d4 = v; 
      else if (d == 5) localW->r6_d5 = v; 
      else if (d == 6) localW->r6_d6 = v; 
   }
   else if (r == 9)
   {
       if (d == -6) localW->r9_dm6 = v; 
      else if (d == -5) localW->r9_dm5 = v; 
      else if (d == -4) localW->r9_dm4 = v; 
      else if (d == -3) localW->r9_dm3 = v; 
      else if (d == -2) localW->r9_dm2 = v; 
      else if (d == -1) localW->r9_dm1 = v; 
      else if (d == 0) localW->r9_d0 = v; 
      else if (d == 1) localW->r9_d1 = v; 
      else if (d == 2) localW->r9_d2 = v; 
      else if (d == 3) localW->r9_d3 = v; 
      else if (d == 4) localW->r9_d4 = v; 
      else if (d == 5) localW->r9_d5 = v; 
      else if (d == 6) localW->r9_d6 = v; 
   }
   else if (r == 5)
   {
       if (d == -5) localW->r5_dm5 = v; 
      else if (d == -4) localW->r5_dm4 = v; 
      else if (d == -3) localW->r5_dm3 = v; 
      else if (d == -2) localW->r5_dm2 = v; 
      else if (d == -1) localW->r5_dm1 = v; 
      else if (d == 0) localW->r5_d0 = v; 
      else if (d == 1) localW->r5_d1 = v; 
      else if (d == 2) localW->r5_d2 = v; 
      else if (d == 3) localW->r5_d3 = v; 
      else if (d == 4) localW->r5_d4 = v; 
      else if (d == 5) localW->r5_d5 = v; 
   }
   else if (r == 10)
   {
       if (d == -5) localW->r10_dm5 = v; 
      else if (d == -4) localW->r10_dm4 = v; 
      else if (d == -3) localW->r10_dm3 = v; 
      else if (d == -2) localW->r10_dm2 = v; 
      else if (d == -1) localW->r10_dm1 = v; 
      else if (d == 0) localW->r10_d0 = v; 
      else if (d == 1) localW->r10_d1 = v; 
      else if (d == 2) localW->r10_d2 = v; 
      else if (d == 3) localW->r10_d3 = v; 
      else if (d == 4) localW->r10_d4 = v; 
      else if (d == 5) localW->r10_d5 = v; 
   }
   else if (r == 4)
   {
       if (d == -4) localW->r4_dm4 = v; 
      else if (d == -3) localW->r4_dm3 = v; 
      else if (d == -2) localW->r4_dm2 = v; 
      else if (d == -1) localW->r4_dm1 = v; 
      else if (d == 0) localW->r4_d0 = v; 
      else if (d == 1) localW->r4_d1 = v; 
      else if (d == 2) localW->r4_d2 = v; 
      else if (d == 3) localW->r4_d3 = v; 
      else if (d == 4) localW->r4_d4 = v; 
   }
   else if (r == 11)
   {
       if (d == -4) localW->r11_dm4 = v; 
      else if (d == -3) localW->r11_dm3 = v; 
      else if (d == -2) localW->r11_dm2 = v; 
      else if (d == -1) localW->r11_dm1 = v; 
      else if (d == 0) localW->r11_d0 = v; 
      else if (d == 1) localW->r11_d1 = v; 
      else if (d == 2) localW->r11_d2 = v; 
      else if (d == 3) localW->r11_d3 = v; 
      else if (d == 4) localW->r11_d4 = v; 
   }
   else if (r == 3)
   {
       if (d == -3) localW->r3_dm3 = v; 
      else if (d == -2) localW->r3_dm2 = v; 
      else if (d == -1) localW->r3_dm1 = v; 
      else if (d == 0) localW->r3_d0 = v; 
      else if (d == 1) localW->r3_d1 = v; 
      else if (d == 2) localW->r3_d2 = v; 
      else if (d == 3) localW->r3_d3 = v; 
   }
   else if (r == 12)
   {
       if (d == -3) localW->r12_dm3 = v; 
      else if (d == -2) localW->r12_dm2 = v; 
      else if (d == -1) localW->r12_dm1 = v; 
      else if (d == 0) localW->r12_d0 = v; 
      else if (d == 1) localW->r12_d1 = v; 
      else if (d == 2) localW->r12_d2 = v; 
      else if (d == 3) localW->r12_d3 = v; 
   }
   else if (r == 2)
   {
       if (d == -2) localW->r2_dm2 = v; 
      else if (d == -1) localW->r2_dm1 = v; 
      else if (d == 0) localW->r2_d0 = v; 
      else if (d == 1) localW->r2_d1 = v; 
      else if (d == 2) localW->r2_d2 = v; 
   }
   else if (r == 13)
   {
       if (d == -2) localW->r13_dm2 = v; 
      else if (d == -1) localW->r13_dm1 = v; 
      else if (d == 0) localW->r13_d0 = v; 
      else if (d == 1) localW->r13_d1 = v; 
      else if (d == 2) localW->r13_d2 = v; 
   }
   else if (r == 1)
   {
       if (d == -1) localW->r1_dm1 = v; 
      else if (d == 0) localW->r1_d0 = v; 
      else if (d == 1) localW->r1_d1 = v; 
   }
   else if (r == 14)
   {
       if (d == -1) localW->r14_dm1 = v; 
      else if (d == 0) localW->r14_d0 = v; 
      else if (d == 1) localW->r14_d1 = v; 
   }
   else if (r == 0)
   {
       if (d == 0) localW->r0_d0 = v; 
   }
   else if (r == 15)
   {
       if (d == 0) localW->r15_d0 = v; 
   }
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
    if (r == 7)
   {
       if (d == -7) v = localW->r7_dm7; 
      else if (d == -6) v = localW->r7_dm6; 
      else if (d == -5) v = localW->r7_dm5; 
      else if (d == -4) v = localW->r7_dm4; 
      else if (d == -3) v = localW->r7_dm3; 
      else if (d == -2) v = localW->r7_dm2; 
      else if (d == -1) v = localW->r7_dm1; 
      else if (d == 0) v = localW->r7_d0; 
      else if (d == 1) v = localW->r7_d1; 
      else if (d == 2) v = localW->r7_d2; 
      else if (d == 3) v = localW->r7_d3; 
      else if (d == 4) v = localW->r7_d4; 
      else if (d == 5) v = localW->r7_d5; 
      else if (d == 6) v = localW->r7_d6; 
      else if (d == 7) v = localW->r7_d7; 
   }
   else if (r == 8)
   {
       if (d == -7) v = localW->r8_dm7; 
      else if (d == -6) v = localW->r8_dm6; 
      else if (d == -5) v = localW->r8_dm5; 
      else if (d == -4) v = localW->r8_dm4; 
      else if (d == -3) v = localW->r8_dm3; 
      else if (d == -2) v = localW->r8_dm2; 
      else if (d == -1) v = localW->r8_dm1; 
      else if (d == 0) v = localW->r8_d0; 
      else if (d == 1) v = localW->r8_d1; 
      else if (d == 2) v = localW->r8_d2; 
      else if (d == 3) v = localW->r8_d3; 
      else if (d == 4) v = localW->r8_d4; 
      else if (d == 5) v = localW->r8_d5; 
      else if (d == 6) v = localW->r8_d6; 
      else if (d == 7) v = localW->r8_d7; 
   }
   else if (r == 6)
   {
       if (d == -6) v = localW->r6_dm6; 
      else if (d == -5) v = localW->r6_dm5; 
      else if (d == -4) v = localW->r6_dm4; 
      else if (d == -3) v = localW->r6_dm3; 
      else if (d == -2) v = localW->r6_dm2; 
      else if (d == -1) v = localW->r6_dm1; 
      else if (d == 0) v = localW->r6_d0; 
      else if (d == 1) v = localW->r6_d1; 
      else if (d == 2) v = localW->r6_d2; 
      else if (d == 3) v = localW->r6_d3; 
      else if (d == 4) v = localW->r6_d4; 
      else if (d == 5) v = localW->r6_d5; 
      else if (d == 6) v = localW->r6_d6; 
   }
   else if (r == 9)
   {
       if (d == -6) v = localW->r9_dm6; 
      else if (d == -5) v = localW->r9_dm5; 
      else if (d == -4) v = localW->r9_dm4; 
      else if (d == -3) v = localW->r9_dm3; 
      else if (d == -2) v = localW->r9_dm2; 
      else if (d == -1) v = localW->r9_dm1; 
      else if (d == 0) v = localW->r9_d0; 
      else if (d == 1) v = localW->r9_d1; 
      else if (d == 2) v = localW->r9_d2; 
      else if (d == 3) v = localW->r9_d3; 
      else if (d == 4) v = localW->r9_d4; 
      else if (d == 5) v = localW->r9_d5; 
      else if (d == 6) v = localW->r9_d6; 
   }
   else if (r == 5)
   {
       if (d == -5) v = localW->r5_dm5; 
      else if (d == -4) v = localW->r5_dm4; 
      else if (d == -3) v = localW->r5_dm3; 
      else if (d == -2) v = localW->r5_dm2; 
      else if (d == -1) v = localW->r5_dm1; 
      else if (d == 0) v = localW->r5_d0; 
      else if (d == 1) v = localW->r5_d1; 
      else if (d == 2) v = localW->r5_d2; 
      else if (d == 3) v = localW->r5_d3; 
      else if (d == 4) v = localW->r5_d4; 
      else if (d == 5) v = localW->r5_d5; 
   }
   else if (r == 10)
   {
       if (d == -5) v = localW->r10_dm5; 
      else if (d == -4) v = localW->r10_dm4; 
      else if (d == -3) v = localW->r10_dm3; 
      else if (d == -2) v = localW->r10_dm2; 
      else if (d == -1) v = localW->r10_dm1; 
      else if (d == 0) v = localW->r10_d0; 
      else if (d == 1) v = localW->r10_d1; 
      else if (d == 2) v = localW->r10_d2; 
      else if (d == 3) v = localW->r10_d3; 
      else if (d == 4) v = localW->r10_d4; 
      else if (d == 5) v = localW->r10_d5; 
   }
   else if (r == 4)
   {
       if (d == -4) v = localW->r4_dm4; 
      else if (d == -3) v = localW->r4_dm3; 
      else if (d == -2) v = localW->r4_dm2; 
      else if (d == -1) v = localW->r4_dm1; 
      else if (d == 0) v = localW->r4_d0; 
      else if (d == 1) v = localW->r4_d1; 
      else if (d == 2) v = localW->r4_d2; 
      else if (d == 3) v = localW->r4_d3; 
      else if (d == 4) v = localW->r4_d4; 
   }
   else if (r == 11)
   {
       if (d == -4) v = localW->r11_dm4; 
      else if (d == -3) v = localW->r11_dm3; 
      else if (d == -2) v = localW->r11_dm2; 
      else if (d == -1) v = localW->r11_dm1; 
      else if (d == 0) v = localW->r11_d0; 
      else if (d == 1) v = localW->r11_d1; 
      else if (d == 2) v = localW->r11_d2; 
      else if (d == 3) v = localW->r11_d3; 
      else if (d == 4) v = localW->r11_d4; 
   }
   else if (r == 3)
   {
       if (d == -3) v = localW->r3_dm3; 
      else if (d == -2) v = localW->r3_dm2; 
      else if (d == -1) v = localW->r3_dm1; 
      else if (d == 0) v = localW->r3_d0; 
      else if (d == 1) v = localW->r3_d1; 
      else if (d == 2) v = localW->r3_d2; 
      else if (d == 3) v = localW->r3_d3; 
   }
   else if (r == 12)
   {
       if (d == -3) v = localW->r12_dm3; 
      else if (d == -2) v = localW->r12_dm2; 
      else if (d == -1) v = localW->r12_dm1; 
      else if (d == 0) v = localW->r12_d0; 
      else if (d == 1) v = localW->r12_d1; 
      else if (d == 2) v = localW->r12_d2; 
      else if (d == 3) v = localW->r12_d3; 
   }
   else if (r == 2)
   {
       if (d == -2) v = localW->r2_dm2; 
      else if (d == -1) v = localW->r2_dm1; 
      else if (d == 0) v = localW->r2_d0; 
      else if (d == 1) v = localW->r2_d1; 
      else if (d == 2) v = localW->r2_d2; 
   }
   else if (r == 13)
   {
       if (d == -2) v = localW->r13_dm2; 
      else if (d == -1) v = localW->r13_dm1; 
      else if (d == 0) v = localW->r13_d0; 
      else if (d == 1) v = localW->r13_d1; 
      else if (d == 2) v = localW->r13_d2; 
   }
   else if (r == 1)
   {
       if (d == -1) v = localW->r1_dm1; 
      else if (d == 0) v = localW->r1_d0; 
      else if (d == 1) v = localW->r1_d1; 
   }
   else if (r == 14)
   {
       if (d == -1) v = localW->r14_dm1; 
      else if (d == 0) v = localW->r14_d0; 
      else if (d == 1) v = localW->r14_d1; 
   }
   else if (r == 0)
   {
       if (d == 0) v = localW->r0_d0; 
   }
   else if (r == 15)
   {
       if (d == 0) v = localW->r15_d0; 
   }
   return v;
}

#endif
#if TILE_LEN == 9
typedef struct
{
   long r0_d0;
   long r1_dm1;
   long r1_d0;
   long r1_d1;
   long r2_dm2;
   long r2_dm1;
   long r2_d0;
   long r2_d1;
   long r2_d2;
   long r3_dm3;
   long r3_dm2;
   long r3_dm1;
   long r3_d0;
   long r3_d1;
   long r3_d2;
   long r3_d3;
   long r4_dm4;
   long r4_dm3;
   long r4_dm2;
   long r4_dm1;
   long r4_d0;
   long r4_d1;
   long r4_d2;
   long r4_d3;
   long r4_d4;
   long r5_dm5;
   long r5_dm4;
   long r5_dm3;
   long r5_dm2;
   long r5_dm1;
   long r5_d0;
   long r5_d1;
   long r5_d2;
   long r5_d3;
   long r5_d4;
   long r5_d5;
   long r6_dm6;
   long r6_dm5;
   long r6_dm4;
   long r6_dm3;
   long r6_dm2;
   long r6_dm1;
   long r6_d0;
   long r6_d1;
   long r6_d2;
   long r6_d3;
   long r6_d4;
   long r6_d5;
   long r6_d6;
   long r7_dm7;
   long r7_dm6;
   long r7_dm5;
   long r7_dm4;
   long r7_dm3;
   long r7_dm2;
   long r7_dm1;
   long r7_d0;
   long r7_d1;
   long r7_d2;
   long r7_d3;
   long r7_d4;
   long r7_d5;
   long r7_d6;
   long r7_d7;
   long r8_dm8;
   long r8_dm7;
   long r8_dm6;
   long r8_dm5;
   long r8_dm4;
   long r8_dm3;
   long r8_dm2;
   long r8_dm1;
   long r8_d0;
   long r8_d1;
   long r8_d2;
   long r8_d3;
   long r8_d4;
   long r8_d5;
   long r8_d6;
   long r8_d7;
   long r8_d8;
   long r9_dm8;
   long r9_dm7;
   long r9_dm6;
   long r9_dm5;
   long r9_dm4;
   long r9_dm3;
   long r9_dm2;
   long r9_dm1;
   long r9_d0;
   long r9_d1;
   long r9_d2;
   long r9_d3;
   long r9_d4;
   long r9_d5;
   long r9_d6;
   long r9_d7;
   long r9_d8;
   long r10_dm7;
   long r10_dm6;
   long r10_dm5;
   long r10_dm4;
   long r10_dm3;
   long r10_dm2;
   long r10_dm1;
   long r10_d0;
   long r10_d1;
   long r10_d2;
   long r10_d3;
   long r10_d4;
   long r10_d5;
   long r10_d6;
   long r10_d7;
   long r11_dm6;
   long r11_dm5;
   long r11_dm4;
   long r11_dm3;
   long r11_dm2;
   long r11_dm1;
   long r11_d0;
   long r11_d1;
   long r11_d2;
   long r11_d3;
   long r11_d4;
   long r11_d5;
   long r11_d6;
   long r12_dm5;
   long r12_dm4;
   long r12_dm3;
   long r12_dm2;
   long r12_dm1;
   long r12_d0;
   long r12_d1;
   long r12_d2;
   long r12_d3;
   long r12_d4;
   long r12_d5;
   long r13_dm4;
   long r13_dm3;
   long r13_dm2;
   long r13_dm1;
   long r13_d0;
   long r13_d1;
   long r13_d2;
   long r13_d3;
   long r13_d4;
   long r14_dm3;
   long r14_dm2;
   long r14_dm1;
   long r14_d0;
   long r14_d1;
   long r14_d2;
   long r14_d3;
   long r15_dm2;
   long r15_dm1;
   long r15_d0;
   long r15_d1;
   long r15_d2;
   long r16_dm1;
   long r16_d0;
   long r16_d1;
   long r17_d0;
} LOCAL_TILE;
#define LOCAL_TILE_PTR      LOCAL_TILE*

void inline __attribute__((always_inline))
writeLocalW(LOCAL_TILE_PTR localW, int d, int r, long v)
{
    if (r == 8)
   {
       if (d == -8) localW->r8_dm8 = v; 
      else if (d == -7) localW->r8_dm7 = v; 
      else if (d == -6) localW->r8_dm6 = v; 
      else if (d == -5) localW->r8_dm5 = v; 
      else if (d == -4) localW->r8_dm4 = v; 
      else if (d == -3) localW->r8_dm3 = v; 
      else if (d == -2) localW->r8_dm2 = v; 
      else if (d == -1) localW->r8_dm1 = v; 
      else if (d == 0) localW->r8_d0 = v; 
      else if (d == 1) localW->r8_d1 = v; 
      else if (d == 2) localW->r8_d2 = v; 
      else if (d == 3) localW->r8_d3 = v; 
      else if (d == 4) localW->r8_d4 = v; 
      else if (d == 5) localW->r8_d5 = v; 
      else if (d == 6) localW->r8_d6 = v; 
      else if (d == 7) localW->r8_d7 = v; 
      else if (d == 8) localW->r8_d8 = v; 
   }
   else if (r == 9)
   {
       if (d == -8) localW->r9_dm8 = v; 
      else if (d == -7) localW->r9_dm7 = v; 
      else if (d == -6) localW->r9_dm6 = v; 
      else if (d == -5) localW->r9_dm5 = v; 
      else if (d == -4) localW->r9_dm4 = v; 
      else if (d == -3) localW->r9_dm3 = v; 
      else if (d == -2) localW->r9_dm2 = v; 
      else if (d == -1) localW->r9_dm1 = v; 
      else if (d == 0) localW->r9_d0 = v; 
      else if (d == 1) localW->r9_d1 = v; 
      else if (d == 2) localW->r9_d2 = v; 
      else if (d == 3) localW->r9_d3 = v; 
      else if (d == 4) localW->r9_d4 = v; 
      else if (d == 5) localW->r9_d5 = v; 
      else if (d == 6) localW->r9_d6 = v; 
      else if (d == 7) localW->r9_d7 = v; 
      else if (d == 8) localW->r9_d8 = v; 
   }
   else if (r == 7)
   {
       if (d == -7) localW->r7_dm7 = v; 
      else if (d == -6) localW->r7_dm6 = v; 
      else if (d == -5) localW->r7_dm5 = v; 
      else if (d == -4) localW->r7_dm4 = v; 
      else if (d == -3) localW->r7_dm3 = v; 
      else if (d == -2) localW->r7_dm2 = v; 
      else if (d == -1) localW->r7_dm1 = v; 
      else if (d == 0) localW->r7_d0 = v; 
      else if (d == 1) localW->r7_d1 = v; 
      else if (d == 2) localW->r7_d2 = v; 
      else if (d == 3) localW->r7_d3 = v; 
      else if (d == 4) localW->r7_d4 = v; 
      else if (d == 5) localW->r7_d5 = v; 
      else if (d == 6) localW->r7_d6 = v; 
      else if (d == 7) localW->r7_d7 = v; 
   }
   else if (r == 10)
   {
       if (d == -7) localW->r10_dm7 = v; 
      else if (d == -6) localW->r10_dm6 = v; 
      else if (d == -5) localW->r10_dm5 = v; 
      else if (d == -4) localW->r10_dm4 = v; 
      else if (d == -3) localW->r10_dm3 = v; 
      else if (d == -2) localW->r10_dm2 = v; 
      else if (d == -1) localW->r10_dm1 = v; 
      else if (d == 0) localW->r10_d0 = v; 
      else if (d == 1) localW->r10_d1 = v; 
      else if (d == 2) localW->r10_d2 = v; 
      else if (d == 3) localW->r10_d3 = v; 
      else if (d == 4) localW->r10_d4 = v; 
      else if (d == 5) localW->r10_d5 = v; 
      else if (d == 6) localW->r10_d6 = v; 
      else if (d == 7) localW->r10_d7 = v; 
   }
   else if (r == 6)
   {
       if (d == -6) localW->r6_dm6 = v; 
      else if (d == -5) localW->r6_dm5 = v; 
      else if (d == -4) localW->r6_dm4 = v; 
      else if (d == -3) localW->r6_dm3 = v; 
      else if (d == -2) localW->r6_dm2 = v; 
      else if (d == -1) localW->r6_dm1 = v; 
      else if (d == 0) localW->r6_d0 = v; 
      else if (d == 1) localW->r6_d1 = v; 
      else if (d == 2) localW->r6_d2 = v; 
      else if (d == 3) localW->r6_d3 = v; 
      else if (d == 4) localW->r6_d4 = v; 
      else if (d == 5) localW->r6_d5 = v; 
      else if (d == 6) localW->r6_d6 = v; 
   }
   else if (r == 11)
   {
       if (d == -6) localW->r11_dm6 = v; 
      else if (d == -5) localW->r11_dm5 = v; 
      else if (d == -4) localW->r11_dm4 = v; 
      else if (d == -3) localW->r11_dm3 = v; 
      else if (d == -2) localW->r11_dm2 = v; 
      else if (d == -1) localW->r11_dm1 = v; 
      else if (d == 0) localW->r11_d0 = v; 
      else if (d == 1) localW->r11_d1 = v; 
      else if (d == 2) localW->r11_d2 = v; 
      else if (d == 3) localW->r11_d3 = v; 
      else if (d == 4) localW->r11_d4 = v; 
      else if (d == 5) localW->r11_d5 = v; 
      else if (d == 6) localW->r11_d6 = v; 
   }
   else if (r == 5)
   {
       if (d == -5) localW->r5_dm5 = v; 
      else if (d == -4) localW->r5_dm4 = v; 
      else if (d == -3) localW->r5_dm3 = v; 
      else if (d == -2) localW->r5_dm2 = v; 
      else if (d == -1) localW->r5_dm1 = v; 
      else if (d == 0) localW->r5_d0 = v; 
      else if (d == 1) localW->r5_d1 = v; 
      else if (d == 2) localW->r5_d2 = v; 
      else if (d == 3) localW->r5_d3 = v; 
      else if (d == 4) localW->r5_d4 = v; 
      else if (d == 5) localW->r5_d5 = v; 
   }
   else if (r == 12)
   {
       if (d == -5) localW->r12_dm5 = v; 
      else if (d == -4) localW->r12_dm4 = v; 
      else if (d == -3) localW->r12_dm3 = v; 
      else if (d == -2) localW->r12_dm2 = v; 
      else if (d == -1) localW->r12_dm1 = v; 
      else if (d == 0) localW->r12_d0 = v; 
      else if (d == 1) localW->r12_d1 = v; 
      else if (d == 2) localW->r12_d2 = v; 
      else if (d == 3) localW->r12_d3 = v; 
      else if (d == 4) localW->r12_d4 = v; 
      else if (d == 5) localW->r12_d5 = v; 
   }
   else if (r == 4)
   {
       if (d == -4) localW->r4_dm4 = v; 
      else if (d == -3) localW->r4_dm3 = v; 
      else if (d == -2) localW->r4_dm2 = v; 
      else if (d == -1) localW->r4_dm1 = v; 
      else if (d == 0) localW->r4_d0 = v; 
      else if (d == 1) localW->r4_d1 = v; 
      else if (d == 2) localW->r4_d2 = v; 
      else if (d == 3) localW->r4_d3 = v; 
      else if (d == 4) localW->r4_d4 = v; 
   }
   else if (r == 13)
   {
       if (d == -4) localW->r13_dm4 = v; 
      else if (d == -3) localW->r13_dm3 = v; 
      else if (d == -2) localW->r13_dm2 = v; 
      else if (d == -1) localW->r13_dm1 = v; 
      else if (d == 0) localW->r13_d0 = v; 
      else if (d == 1) localW->r13_d1 = v; 
      else if (d == 2) localW->r13_d2 = v; 
      else if (d == 3) localW->r13_d3 = v; 
      else if (d == 4) localW->r13_d4 = v; 
   }
   else if (r == 3)
   {
       if (d == -3) localW->r3_dm3 = v; 
      else if (d == -2) localW->r3_dm2 = v; 
      else if (d == -1) localW->r3_dm1 = v; 
      else if (d == 0) localW->r3_d0 = v; 
      else if (d == 1) localW->r3_d1 = v; 
      else if (d == 2) localW->r3_d2 = v; 
      else if (d == 3) localW->r3_d3 = v; 
   }
   else if (r == 14)
   {
       if (d == -3) localW->r14_dm3 = v; 
      else if (d == -2) localW->r14_dm2 = v; 
      else if (d == -1) localW->r14_dm1 = v; 
      else if (d == 0) localW->r14_d0 = v; 
      else if (d == 1) localW->r14_d1 = v; 
      else if (d == 2) localW->r14_d2 = v; 
      else if (d == 3) localW->r14_d3 = v; 
   }
   else if (r == 2)
   {
       if (d == -2) localW->r2_dm2 = v; 
      else if (d == -1) localW->r2_dm1 = v; 
      else if (d == 0) localW->r2_d0 = v; 
      else if (d == 1) localW->r2_d1 = v; 
      else if (d == 2) localW->r2_d2 = v; 
   }
   else if (r == 15)
   {
       if (d == -2) localW->r15_dm2 = v; 
      else if (d == -1) localW->r15_dm1 = v; 
      else if (d == 0) localW->r15_d0 = v; 
      else if (d == 1) localW->r15_d1 = v; 
      else if (d == 2) localW->r15_d2 = v; 
   }
   else if (r == 1)
   {
       if (d == -1) localW->r1_dm1 = v; 
      else if (d == 0) localW->r1_d0 = v; 
      else if (d == 1) localW->r1_d1 = v; 
   }
   else if (r == 16)
   {
       if (d == -1) localW->r16_dm1 = v; 
      else if (d == 0) localW->r16_d0 = v; 
      else if (d == 1) localW->r16_d1 = v; 
   }
   else if (r == 0)
   {
       if (d == 0) localW->r0_d0 = v; 
   }
   else if (r == 17)
   {
       if (d == 0) localW->r17_d0 = v; 
   }
}

long inline __attribute__((always_inline))
readLocalW(LOCAL_TILE_PTR localW, int d, int r)
{
   long v; 
    if (r == 8)
   {
       if (d == -8) v = localW->r8_dm8; 
      else if (d == -7) v = localW->r8_dm7; 
      else if (d == -6) v = localW->r8_dm6; 
      else if (d == -5) v = localW->r8_dm5; 
      else if (d == -4) v = localW->r8_dm4; 
      else if (d == -3) v = localW->r8_dm3; 
      else if (d == -2) v = localW->r8_dm2; 
      else if (d == -1) v = localW->r8_dm1; 
      else if (d == 0) v = localW->r8_d0; 
      else if (d == 1) v = localW->r8_d1; 
      else if (d == 2) v = localW->r8_d2; 
      else if (d == 3) v = localW->r8_d3; 
      else if (d == 4) v = localW->r8_d4; 
      else if (d == 5) v = localW->r8_d5; 
      else if (d == 6) v = localW->r8_d6; 
      else if (d == 7) v = localW->r8_d7; 
      else if (d == 8) v = localW->r8_d8; 
   }
   else if (r == 9)
   {
       if (d == -8) v = localW->r9_dm8; 
      else if (d == -7) v = localW->r9_dm7; 
      else if (d == -6) v = localW->r9_dm6; 
      else if (d == -5) v = localW->r9_dm5; 
      else if (d == -4) v = localW->r9_dm4; 
      else if (d == -3) v = localW->r9_dm3; 
      else if (d == -2) v = localW->r9_dm2; 
      else if (d == -1) v = localW->r9_dm1; 
      else if (d == 0) v = localW->r9_d0; 
      else if (d == 1) v = localW->r9_d1; 
      else if (d == 2) v = localW->r9_d2; 
      else if (d == 3) v = localW->r9_d3; 
      else if (d == 4) v = localW->r9_d4; 
      else if (d == 5) v = localW->r9_d5; 
      else if (d == 6) v = localW->r9_d6; 
      else if (d == 7) v = localW->r9_d7; 
      else if (d == 8) v = localW->r9_d8; 
   }
   else if (r == 7)
   {
       if (d == -7) v = localW->r7_dm7; 
      else if (d == -6) v = localW->r7_dm6; 
      else if (d == -5) v = localW->r7_dm5; 
      else if (d == -4) v = localW->r7_dm4; 
      else if (d == -3) v = localW->r7_dm3; 
      else if (d == -2) v = localW->r7_dm2; 
      else if (d == -1) v = localW->r7_dm1; 
      else if (d == 0) v = localW->r7_d0; 
      else if (d == 1) v = localW->r7_d1; 
      else if (d == 2) v = localW->r7_d2; 
      else if (d == 3) v = localW->r7_d3; 
      else if (d == 4) v = localW->r7_d4; 
      else if (d == 5) v = localW->r7_d5; 
      else if (d == 6) v = localW->r7_d6; 
      else if (d == 7) v = localW->r7_d7; 
   }
   else if (r == 10)
   {
       if (d == -7) v = localW->r10_dm7; 
      else if (d == -6) v = localW->r10_dm6; 
      else if (d == -5) v = localW->r10_dm5; 
      else if (d == -4) v = localW->r10_dm4; 
      else if (d == -3) v = localW->r10_dm3; 
      else if (d == -2) v = localW->r10_dm2; 
      else if (d == -1) v = localW->r10_dm1; 
      else if (d == 0) v = localW->r10_d0; 
      else if (d == 1) v = localW->r10_d1; 
      else if (d == 2) v = localW->r10_d2; 
      else if (d == 3) v = localW->r10_d3; 
      else if (d == 4) v = localW->r10_d4; 
      else if (d == 5) v = localW->r10_d5; 
      else if (d == 6) v = localW->r10_d6; 
      else if (d == 7) v = localW->r10_d7; 
   }
   else if (r == 6)
   {
       if (d == -6) v = localW->r6_dm6; 
      else if (d == -5) v = localW->r6_dm5; 
      else if (d == -4) v = localW->r6_dm4; 
      else if (d == -3) v = localW->r6_dm3; 
      else if (d == -2) v = localW->r6_dm2; 
      else if (d == -1) v = localW->r6_dm1; 
      else if (d == 0) v = localW->r6_d0; 
      else if (d == 1) v = localW->r6_d1; 
      else if (d == 2) v = localW->r6_d2; 
      else if (d == 3) v = localW->r6_d3; 
      else if (d == 4) v = localW->r6_d4; 
      else if (d == 5) v = localW->r6_d5; 
      else if (d == 6) v = localW->r6_d6; 
   }
   else if (r == 11)
   {
       if (d == -6) v = localW->r11_dm6; 
      else if (d == -5) v = localW->r11_dm5; 
      else if (d == -4) v = localW->r11_dm4; 
      else if (d == -3) v = localW->r11_dm3; 
      else if (d == -2) v = localW->r11_dm2; 
      else if (d == -1) v = localW->r11_dm1; 
      else if (d == 0) v = localW->r11_d0; 
      else if (d == 1) v = localW->r11_d1; 
      else if (d == 2) v = localW->r11_d2; 
      else if (d == 3) v = localW->r11_d3; 
      else if (d == 4) v = localW->r11_d4; 
      else if (d == 5) v = localW->r11_d5; 
      else if (d == 6) v = localW->r11_d6; 
   }
   else if (r == 5)
   {
       if (d == -5) v = localW->r5_dm5; 
      else if (d == -4) v = localW->r5_dm4; 
      else if (d == -3) v = localW->r5_dm3; 
      else if (d == -2) v = localW->r5_dm2; 
      else if (d == -1) v = localW->r5_dm1; 
      else if (d == 0) v = localW->r5_d0; 
      else if (d == 1) v = localW->r5_d1; 
      else if (d == 2) v = localW->r5_d2; 
      else if (d == 3) v = localW->r5_d3; 
      else if (d == 4) v = localW->r5_d4; 
      else if (d == 5) v = localW->r5_d5; 
   }
   else if (r == 12)
   {
       if (d == -5) v = localW->r12_dm5; 
      else if (d == -4) v = localW->r12_dm4; 
      else if (d == -3) v = localW->r12_dm3; 
      else if (d == -2) v = localW->r12_dm2; 
      else if (d == -1) v = localW->r12_dm1; 
      else if (d == 0) v = localW->r12_d0; 
      else if (d == 1) v = localW->r12_d1; 
      else if (d == 2) v = localW->r12_d2; 
      else if (d == 3) v = localW->r12_d3; 
      else if (d == 4) v = localW->r12_d4; 
      else if (d == 5) v = localW->r12_d5; 
   }
   else if (r == 4)
   {
       if (d == -4) v = localW->r4_dm4; 
      else if (d == -3) v = localW->r4_dm3; 
      else if (d == -2) v = localW->r4_dm2; 
      else if (d == -1) v = localW->r4_dm1; 
      else if (d == 0) v = localW->r4_d0; 
      else if (d == 1) v = localW->r4_d1; 
      else if (d == 2) v = localW->r4_d2; 
      else if (d == 3) v = localW->r4_d3; 
      else if (d == 4) v = localW->r4_d4; 
   }
   else if (r == 13)
   {
       if (d == -4) v = localW->r13_dm4; 
      else if (d == -3) v = localW->r13_dm3; 
      else if (d == -2) v = localW->r13_dm2; 
      else if (d == -1) v = localW->r13_dm1; 
      else if (d == 0) v = localW->r13_d0; 
      else if (d == 1) v = localW->r13_d1; 
      else if (d == 2) v = localW->r13_d2; 
      else if (d == 3) v = localW->r13_d3; 
      else if (d == 4) v = localW->r13_d4; 
   }
   else if (r == 3)
   {
       if (d == -3) v = localW->r3_dm3; 
      else if (d == -2) v = localW->r3_dm2; 
      else if (d == -1) v = localW->r3_dm1; 
      else if (d == 0) v = localW->r3_d0; 
      else if (d == 1) v = localW->r3_d1; 
      else if (d == 2) v = localW->r3_d2; 
      else if (d == 3) v = localW->r3_d3; 
   }
   else if (r == 14)
   {
       if (d == -3) v = localW->r14_dm3; 
      else if (d == -2) v = localW->r14_dm2; 
      else if (d == -1) v = localW->r14_dm1; 
      else if (d == 0) v = localW->r14_d0; 
      else if (d == 1) v = localW->r14_d1; 
      else if (d == 2) v = localW->r14_d2; 
      else if (d == 3) v = localW->r14_d3; 
   }
   else if (r == 2)
   {
       if (d == -2) v = localW->r2_dm2; 
      else if (d == -1) v = localW->r2_dm1; 
      else if (d == 0) v = localW->r2_d0; 
      else if (d == 1) v = localW->r2_d1; 
      else if (d == 2) v = localW->r2_d2; 
   }
   else if (r == 15)
   {
       if (d == -2) v = localW->r15_dm2; 
      else if (d == -1) v = localW->r15_dm1; 
      else if (d == 0) v = localW->r15_d0; 
      else if (d == 1) v = localW->r15_d1; 
      else if (d == 2) v = localW->r15_d2; 
   }
   else if (r == 1)
   {
       if (d == -1) v = localW->r1_dm1; 
      else if (d == 0) v = localW->r1_d0; 
      else if (d == 1) v = localW->r1_d1; 
   }
   else if (r == 16)
   {
       if (d == -1) v = localW->r16_dm1; 
      else if (d == 0) v = localW->r16_d0; 
      else if (d == 1) v = localW->r16_d1; 
   }
   else if (r == 0)
   {
       if (d == 0) v = localW->r0_d0; 
   }
   else if (r == 17)
   {
       if (d == 0) v = localW->r17_d0; 
   }
   return v;
}

#endif
