#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include "vvadd.h"
#include <unistd.h>
#include <sys/stat.h>

//#define WRITE_OUTPUT
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

char *readfile(int fd) {
  char *p; 
  struct stat s;
  off_t len;
  ssize_t bytes_read, status;

  assert(fd>1 && "Invalid file descriptor");
  assert(0==fstat(fd, &s) && "Couldn't determine file size");
  len = s.st_size;
  assert(len>0 && "File is empty");
  p = (char *)malloc(len+1);
  bytes_read = 0;
  while( bytes_read<len ) {
    status = read(fd, &p[bytes_read], len-bytes_read);
    assert(status>=0 && "read() failed");
    bytes_read+=status;
  }
  p[len] = (char)0; // Add NULL terminator
  close(fd);
  return p;
}

char *find_section_start(char *s, int n) {
  int i=0;

  assert(n>=0 && "Invalid section number");
  if(n==0)
    return s;

  // Find the nth "%%\n" substring (if *s==0, there wasn't one)
  while(i<n && (*s)!=(char)0) {
    // This comparison will short-circuit before overrunning the string, so no length check.
    if( s[0]=='%' && s[1]=='%' && s[2]=='\n' ) {
      i++;
    }
    s++;
  }
  if(*s!=(char)0)
    return s+2; // Skip the section header itself, return pointer to the content
  return s; // Hit the end, return an empty string
}

int write_section_header(int fd) {
    assert(fd>1 && "Invalid file descriptor");
    dprintf(fd, "%%%%\n"); // Just prints %%
    return 0;
}

int parse_int_array(char *s, TYPE *arr, int n) { 
  char *line, *endptr; 
  int i=0; 
  TYPE v; 
  
  assert(s!=NULL && "Invalid input string"); 
  
  line = strtok(s,"\n"); 
  while( line!=NULL && i<n ) { 
    endptr = line; 
    v = (TYPE)(strtol(line, &endptr, 10)); 
    if( (*endptr)!=(char)0 ) { 
      fprintf(stderr, "Invalid input: line %d of section\n", i); 
    } 
    arr[i] = v; 
    i++; 
    line[strlen(line)] = '\n'; /* Undo the strtok replacement.*/ 
    line = strtok(NULL,"\n"); 
  } 
  if(line!=NULL) { /* stopped because we read all the things */ 
    line[strlen(line)] = '\n'; /* Undo the strtok replacement.*/ 
  } 
  
  return 0; 
}

int parse_double_array(char *s, TYPE *arr, int n) { 
  char *line, *endptr; 
  int i=0; 
  TYPE v; 
  
  assert(s!=NULL && "Invalid input string"); 
  
  line = strtok(s,"\n"); 
  while( line!=NULL && i<n ) { 
    endptr = line; 
    v = (TYPE)(strtod(line, &endptr)); 
    if( (*endptr)!=(char)0 ) { 
      fprintf(stderr, "Invalid input: line %d of section\n", i); 
    } 
    arr[i] = v; 
    i++; 
    line[strlen(line)] = '\n'; /* Undo the strtok replacement.*/ 
    line = strtok(NULL,"\n"); 
  } 
  if(line!=NULL) { /* stopped because we read all the things */ 
    line[strlen(line)] = '\n'; /* Undo the strtok replacement.*/ 
  } 
  
  return 0; 
}

int write_int_array(int fd, TYPE *arr, int n) {
    int i;
    assert(fd>1 && "Invalid file descriptor");
    for( i=0; i<n; i++ ) {
        //dprintf(fd, "%d\n", arr[i]);
    }
    return 0;
}

int write_double_array(int fd, TYPE *arr, int n) {
    int i;
    assert(fd>1 && "Invalid file descriptor");
    for( i=0; i<n; i++ ) {
        dprintf(fd, "%" ".16f" "\n", arr[i]);
    }
    return 0;
}

int run_benchmark() {
    struct bench_args_t args;
    char const *in_file;
    in_file = "input_double.data";

    int in_fd;
    in_fd = open( in_file, O_RDONLY );
    assert( in_fd>0 && "Couldn't open input data file");

    char *p, *s;
    p = readfile(in_fd);
    
    s = find_section_start(p,1);
    parse_double_array(s, args.m1, N);
    
    s = find_section_start(p,2);
    parse_double_array(s, args.m2, N);
    free(p);

    for (int i = 0; i < row_size; i++) {
        for (int j = 0; j < col_size; j++) {
            args.add[i * row_size + j] = 0;
        }
    }

    struct perf_counter_t perf_ctr;
    perf_reset(&perf_ctr);
    perf_start(&perf_ctr);
    kernel( args.m1, args.m2, args.add );
    perf_stop(&perf_ctr);
    uint64_t perf_cycles = perf_avg_cycles(&perf_ctr);
    printf("Average execution time for benchmark: %llu \n", perf_cycles );

#ifdef WRITE_OUTPUT
    char const *out_file;
    out_file = "output.data";
    
    int out_fd;
    out_fd = open( out_file, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    assert( out_fd>0 && "Couldn't open output data file");
    write_section_header(out_fd);
    write_double_array(out_fd, args.add, N);
#endif

#ifdef CHECK_OUTPUT
    struct bench_args_t refs;
    char const *chk_file;
    chk_file = "check.data";
	
	int chk_fd;
	chk_fd = open( chk_file, O_RDONLY );
	assert( chk_fd>0 && "Couldn't open check data file");
	
    p = readfile(chk_fd);
    
    s = find_section_start(p,1);
    parse_double_array(s, refs.add, N);

	for(int i = 0;  i < row_size; i++) {
		for(int j = 0; j < col_size; j++) {
			TYPE diff = args.add[i * row_size + j] - refs.add[ i * row_size + j];
			if ((diff<-EPSILON) || (EPSILON<diff)) {
	  			fprintf(stderr, "Benchmark results are incorrect\n");
				return -1;
			}
		}
	}
	free(p);
#endif
	
	printf("Success.\n");
	return 0;
}

int main () {
    return run_benchmark();
}
