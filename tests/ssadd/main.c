#include <stdio.h>
#include <stdbool.h>
#include "ssadd.h"

int main() {
    TYPE a, b, c;
    a = 5.0;
    b = 6.0;
    TYPE d = ssadd(&a, &b, &c);
    printf("Output: %f\n", d);
    return 1;
}
