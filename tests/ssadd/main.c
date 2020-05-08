#include <stdio.h>
#include <stdbool.h>
#include "ssadd.h"

int main() {
    double a, b, c;
    a = 5.0;
    b = 6.0;
    ssadd(&a, &b, &c);
    printf("Output: %f\n", c);
    return 1;
}
