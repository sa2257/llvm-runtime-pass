void log_val(double i) {
     printf("Log: %f\n", i);
}

// To replace functions
void matmul_accel(double* m1, double* m2, double* prod, int row_size, int col_size) {
    int row_arr = 16; int col_arr = 16;
    tilei:for(int ii = 0; ii < row_size; ii += row_arr) {
    tilej:for(int jj = 0; jj < col_size; jj += col_arr) {
        outer:for(int i=0;i<row_arr;i++) {
            middle:for(int j=0;j<col_arr;j++) {
                int i_col = (ii + i) * col_size;
                double sum = 0;
                inner:for(int k=0;k<row_size;k++) {
                    int k_col = k * col_size;
                    double mult = m1[i_col + k] * m2[k_col + jj + j];
                    sum += mult;
                }
                prod[i_col + jj + j]  = sum;
            }
        }
    }}
    printf("Sample: %f\n", prod[0]);
}
