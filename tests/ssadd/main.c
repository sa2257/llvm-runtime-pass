#include <stdio.h>
#include <stdbool.h>
#include "ssadd.h"
#define WRITE_TIME
#define CHECK_OUTPUT
#define EPSILON ((TYPE)1.0e-6)

#include <time.h>
// from https://www.programiz.com/cpp-programming/library-function/ctime/clock

struct perf_counter_t {
     uint64_t tot, cnt, calls;
};

static inline void perf_reset(struct perf_counter_t *ctr) { ctr->tot = ctr->cnt = ctr->calls = 0; }
static inline void perf_start(struct perf_counter_t *ctr) { ctr->cnt = clock(); ctr->calls++; };
//static inline void perf_stop(struct perf_counter_t *ctr) { ctr->tot += (clock() - ctr->cnt); };
static inline void perf_stop(struct perf_counter_t *ctr) { ctr->tot += (clock() - ctr->cnt)*1000000/CLOCKS_PER_SEC; };
static inline uint64_t perf_avg_cycles(struct perf_counter_t *ctr) { return ((ctr->tot+(ctr->calls>>1)) / ctr->calls); };

int main() {
    TYPE a[1], b[1], c[1], refs[1];
    a[0] = 5.0;
    b[0] = 6.0;
    refs[0] = 11.0;
    
    struct perf_counter_t perf_ctr;
    perf_reset(&perf_ctr);
    perf_start(&perf_ctr);
    kernel(a, b, c);
    perf_stop(&perf_ctr);
    uint64_t perf_cycles = perf_avg_cycles(&perf_ctr);
    printf("Average execution time for benchmark: %llu \n", perf_cycles );
#ifdef WRITE_TIME
    char const *time_file;
    time_file = "time.data";
    
    int time_fd;
    time_fd = open( time_file, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    assert( time_fd>0 && "Couldn't open timing data file");
    dprintf(time_fd, "%llu \n", perf_cycles);
#endif

#ifdef CHECK_OUTPUT
    TYPE diff = c[0] - refs[0];
	if ((diff<-EPSILON) || (EPSILON<diff)) {
	    fprintf(stderr, "Benchmark results are incorrect\n");
	    return -1;
	}
#endif
	
	printf("Success.\n");
	return 0;
}
