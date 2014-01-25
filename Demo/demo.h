// demo.h

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include <string.h>
#include <stdlib.h>

#define BUFFERSIZE 100;

// structures for shared memory
#define MAXCHAR 80	

typedef struct {
	int ok_flag;
	char indata[MAXCHAR];
	int length;
} inputbuf;
