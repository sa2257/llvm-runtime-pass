#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TYPE double

// Problem Constants
#define nAtoms        256
#define domainEdge    20.0
#define blockSide     4
//#define blockSide     1
#define nBlocks       (blockSide*blockSide*blockSide)
#define blockEdge     (domainEdge/((TYPE)blockSide))
// Memory Bound
// This is an artifact of using statically-allocated arrays. We'll pretend that
// it doesn't exist and instead track the actual number of points.
#define densityFactor 10
// LJ coefficients
#define lj1           1.5
#define lj2           2.0

typedef struct {
  TYPE x, y, z;
} dvector_t;
typedef struct {
  int32_t x, y, z;
} ivector_t;

#pragma SDS data copy(n_points[0:nBlocks])
void md( int32_t n_points[blockSide][blockSide][blockSide],
         TYPE force_x[blockSide][blockSide][blockSide][densityFactor],
         TYPE force_y[blockSide][blockSide][blockSide][densityFactor],
         TYPE force_z[blockSide][blockSide][blockSide][densityFactor],
         TYPE position_x[blockSide][blockSide][blockSide][densityFactor],
         TYPE position_y[blockSide][blockSide][blockSide][densityFactor],
         TYPE position_z[blockSide][blockSide][blockSide][densityFactor]
       );
////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
  int32_t n_points[blockSide][blockSide][blockSide];
  TYPE force_x[blockSide][blockSide][blockSide][densityFactor];
  TYPE force_y[blockSide][blockSide][blockSide][densityFactor];
  TYPE force_z[blockSide][blockSide][blockSide][densityFactor];
  TYPE position_x[blockSide][blockSide][blockSide][densityFactor];
  TYPE position_y[blockSide][blockSide][blockSide][densityFactor];
  TYPE position_z[blockSide][blockSide][blockSide][densityFactor];
};
