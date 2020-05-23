BENCHMARKS=\
	ssadd \
	vvadd \
	diamond \
	machsuite-aes \
	machsuite-backprop \
	machsuite-bfs-bulk \
	machsuite-bfs-queue \
	machsuite-fft-strided \
	machsuite-fft-transpose \
	machsuite-gemm-blocked \
	machsuite-gemm-ncubed \
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

.PHONY: simple runtime clean

simple:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b simple; done )

runtime:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b runtime; done )

clean:
	@( for b in $(BENCHMARKS); do $(MAKE) -C $$b clean || exit ; done )
