#include "ssadd.h"

void kernel( TYPE m1[1], TYPE m2[1], TYPE add[1] ){
    TYPE temp;

    temp = m1[0] + m2[0];
    add[0]  = temp;
}
