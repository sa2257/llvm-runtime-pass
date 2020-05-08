#include "vvadd.h"

void vvadd( TYPE m1[N], TYPE m2[N], TYPE add[N] ){
    int i;
    TYPE temp;

    loop:for(i=0;i<N;i++) {
      temp = m1[i] + m2[i];
      add[i]  = temp;
    }
}
