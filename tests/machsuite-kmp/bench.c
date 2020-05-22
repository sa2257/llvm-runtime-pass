#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include "kmp.h"
#include <unistd.h>
#include <sys/stat.h>

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

int parse_string(char *s, char *arr, int n) {
  int k;
  assert(s!=NULL && "Invalid input string");

  if( n<0 ) { // terminated string
    k = 0;
    while( s[k]!=(char)0 && s[k+1]!=(char)0 && s[k+2]!=(char)0
        && !(s[k]=='\n'  && s[k+1]=='%'     && s[k+2]=='%') ) {
      k++;
    }
  } else { // fixed-length string
    k = n;
  }

  memcpy( arr, s, k );
  if( n<0 )
    arr[k] = 0;

  return 0;
}

void run_benchmark() {
    struct bench_args_t args;
    char const *in_file;
    in_file = "input.data";

    int in_fd;
    in_fd = open( in_file, O_RDONLY );
    assert( in_fd>0 && "Couldn't open input data file");

    char *p, *s;
    p = readfile(in_fd);
    
    s = find_section_start(p,1);
    parse_string(s, args.pattern, PATTERN_SIZE);
    
    s = find_section_start(p,2);
    parse_string(s, args.input, STRING_SIZE);
    free(p);

    kmp( args.pattern, args.input, args.kmpNext, args.n_matches );
    printf("One example output is %d \n", args.n_matches[0]);
}

int main () {
    run_benchmark();
    return 1;
}
