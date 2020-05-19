#include <stdio.h>
#include <stdbool.h>
#include "ssadd.h"

int main() {
    TYPE a[1], b[1], c[1];
    a[0] = 5.0;
    b[0] = 6.0;
    ssadd(a, b, c);
    printf("Output: %f\n", c[0]);
    return 1;
}
