#include "diamond.h"

void kernel( TYPE m1[N], TYPE m2[N], TYPE add[N] ){
    int i;
    TYPE temp1, intm1, intm2, temp2;

    loop:for(i=0;i<N;i++) {
      temp1 = m1[i] + m2[i];
      intm1 = temp1 * 10;
      intm2 = temp1 * 0.1;
      temp2  = intm1 + intm2;
      add[i] = temp2;
    }
}
