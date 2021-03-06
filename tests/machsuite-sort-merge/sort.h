#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 2048
#define TYPE int32_t
#define TYPE_MAX INT32_MAX

#pragma SDS data zero_copy(a[0:SIZE])
void merge(TYPE a[SIZE], int start, int m, int stop);
void sort(TYPE a[SIZE]);

////////////////////////////////////////////////////////////////////////////////
// Test harness interface code.

struct bench_args_t {
  TYPE a[SIZE];
};
