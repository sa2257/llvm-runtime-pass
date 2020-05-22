#include <stdio.h>
#include <stdbool.h>
#include "ssadd.h"
#define CHECK_OUTPUT
#define EPSILON ((TYPE)1.0e-6)

int main() {
    TYPE a[1], b[1], c[1], refs[1];
    a[0] = 5.0;
    b[0] = 6.0;
    refs[0] = 11.0;
    ssadd(a, b, c);

#ifdef CHECK_OUTPUT
    TYPE diff = c[0] - refs[0];
	if ((diff<-EPSILON) || (EPSILON<diff)) {
	    fprintf(stderr, "Benchmark results are incorrect\n");
	    return -1;
	}
#endif
	
	printf("Success.\n");
	return 0;
}
