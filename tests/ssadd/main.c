#include <stdio.h>
#include <stdbool.h>
#include "ssadd.h"

int main() {
    TYPE a, b, c;
    a = 5;
    b = 6;
    ssadd(&a, &b, &c);
    printf("Output: %d\n", c);
    return 1;
}
