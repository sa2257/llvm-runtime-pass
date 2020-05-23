#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include "bfs.h"
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

int parse_uint64_t_array(char *s, uint64_t *arr, int n) { 
  char *line, *endptr; 
  int i=0; 
  uint64_t v; 
  
  assert(s!=NULL && "Invalid input string"); 
  
  line = strtok(s,"\n"); 
  while( line!=NULL && i<n ) { 
    endptr = line; 
    v = (uint64_t)(strtol(line, &endptr, 10)); 
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
    parse_uint64_t_array(s, &args.starting_node, 1);
    
    s = find_section_start(p,2);
    uint64_t* nodes = (uint64_t *)malloc(N_NODES*2*sizeof(uint64_t));
    parse_uint64_t_array(s, nodes, N_NODES*2);
    for( int i = 0; i < N_NODES; i++) {
      args.nodes[i].edge_begin = nodes[2*i];
      args.nodes[i].edge_end = nodes[2*i+1];
    }
    free(nodes);

    s = find_section_start(p,3);
    uint64_t* edges = (uint64_t *)malloc(N_EDGES*1*sizeof(uint64_t));
    parse_uint64_t_array(s, edges, N_EDGES*1);
    for( int i = 0; i < N_EDGES; i++) {
      args.edges[i].src = 0;
      args.edges[i].dst = edges[i];
    }
    free(edges);
    free(p);

    bfs( args.nodes, args.edges, args.starting_node, args.level, args.level_counts );
    printf("One example output is %llu \n", args.level_counts[N_LEVELS-1]);
}

int main () {
    run_benchmark();
    return 1;
}
