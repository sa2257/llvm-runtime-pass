//Standard Libraries
#include <stdio.h>
#include <stdlib.h>

//Define compute data type
#define TYPE int

//Specify vector length
#define N 4096

//Define the input range to operate over
#define MIN 0.
#define MAX 1.0

//Set number of iterations to execute
#define MAX_ITERATION 1

//Set dimensions
#define row_size 64
#define col_size 64

////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.
void vvadd( TYPE *m1, TYPE *m2, TYPE *add );

struct bench_args_t {
  TYPE m1[N];
  TYPE m2[N];
  TYPE add[N];
};
