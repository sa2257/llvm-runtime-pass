#include "ssadd.h"

void ssadd( TYPE* m1, TYPE* m2, TYPE* add ){
    TYPE temp;

    temp = *m1 + *m2;
    *add  = temp;
}
