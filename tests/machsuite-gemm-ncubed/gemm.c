#include "gemm.h"

int foo(TYPE m1[N], TYPE m2[N], TYPE prod[N], int row, int col) {
    int __attribute__((annotate("matrix multiplication"))) matmul = row;
    outer:for(int i=0;i<row_size;i++) {
        middle:for(int j=0;j<col_size;j++) {
            int i_col = i * col_size;
            TYPE sum = 0;
            inner:for(int k=0;k<row_size;k++) {
                int k_col = k * col_size;
                TYPE mult = m1[i_col + k] * m2[k_col + j];
                sum += mult;
            }
            prod[i_col + j]  = sum;
        }
    }
    return matmul;
}

void kernel( TYPE m1[N], TYPE m2[N], TYPE prod[N] ){
#pragma HLS INTERFACE s_axilite port=m1
#pragma HLS INTERFACE s_axilite port=m2
#pragma HLS INTERFACE s_axilite port=prod

    int val = foo(m1, m2, prod, row_size, col_size);
    fprintf(stdout, "val is %d\n", val);
}
