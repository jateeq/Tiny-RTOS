#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "init.h"
#include "iproc.h"
#include "queue.h"
#include "userProcesses.h"

//#define bufsize 128;

// do any necessary cleanup before exiting
// ( in this case, there is no cleanup to do)
// Basically, we rely on the parent process to cleanup shared memory
void out_die(int signal)
{
	exit(0);
}


int main (int argc, char * argv[]) {
    int buf_size;
    buf_size = 256; 
        int buf_index;
	int parent_pid, fid;

	// if parent tells us to terminate, then clean up first
	sigset(SIGINT,out_die);

	// get id of process to signal when we have input
	// and the file id of the memory mapped file
	// i.e. process input arguments 
	sscanf(argv[1], "%d", &parent_pid );
	sscanf(argv[2], "%d", &fid );  // get the file id

	if (fid == -1) {
	    printf("Bad file for crt");
        }


	// attach to shared memory so we can pass input to 
	// keyboard interrupt handler
	
	mmap_ptr = mmap((caddr_t) 0, buf_size , PROT_READ | PROT_WRITE, MAP_SHARED, fid,(off_t) 0);
        if (mmap_ptr == NULL){
            printf("Child memory map has failed, CRT is aborting!\n");
            out_die(0);
        }
	
	out_mem_ptr = (output_buffer *) mmap_ptr; // now we have a shared memory pointer in input buffer format

	//output to screen
	buf_index = 0;
	out_mem_ptr->flag = 0; //should this be initialized by RTX?
	out_mem_ptr->output_count = 0; //should this be initialized by RTX?

	while (1){ //an infinite loop - exit when parent signals us
		if (out_mem_ptr->flag == 1) {//ready to output
			fflush(stdout);
			printf("CRT is outputting...\n");
			buf_index = 0;
			while (buf_index < out_mem_ptr->output_count) {
					printf("%c", (char) out_mem_ptr->output_data[buf_index]);
				buf_index++;
			}
			printf("\n");
			out_mem_ptr->flag = 0; 
			out_mem_ptr->output_count = 0;
		}
		fflush(stdout);
		//printf("CRT is signalling RTX\n"); 
		fflush(stdout);
		usleep(100000);
		kill(parent_pid, SIGUSR2); 
	}
}
