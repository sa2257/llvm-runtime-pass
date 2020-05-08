BENCHMARKS=\
	machsuite-aes \
	machsuite-backprop \
	machsuite-bfs-bulk \
	machsuite-bfs-queue \
	machsuite-fft-strided \
	machsuite-fft-transpose \
	machsuite-gemm-blocked \
	machsuite-gemm-ncubed \
	machsuite-gemm-ncubed-int \
	machsuite-kmp \
	machsuite-md-grid \
	machsuite-md-knn \
	machsuite-nw \
	machsuite-sort-merge \
	machsuite-sort-radix \
	machsuite-spmv-crs \
	machsuite-spmv-ellpack \
	machsuite-stencil-stencil2d \
	machsuite-stencil-stencil3d \
	machsuite-viterbi

.PHONY: simple profile time allocate clean

simple:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b simple; done )

profile:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b profile; done )

time:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b time; done )

allocate:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b allocate; done )

depgen:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b depgen; done )

clean:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b clean || exit ; done )
