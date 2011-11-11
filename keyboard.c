/*MAX FILE SIZE: 79 lines*/
/* CONTENT: header file for KBD linux process*/

#include <stdio.h> /*for printf - may need to remove if no stdoutput required*/
#include <stdlib.h>/*for malloc and free*/
#include <sys/mman.h>/*for PROT_READ, MAP_SHARED etc*/
#include <fcntl.h>
#include <unistd.h> /* for usleep */
#include <signal.h> /*kill*/
#include "rtx.h"
#include "queue.h"
#define BUFFER_SIZE 128
#define DEBUG 1

/*will need to lock file or use semaphores to regulate access to memory mapped file for concurrency reasons*/

int debug(FILE* fid, char* error_msg[])
{
	if (DEBUG)
	{		
		fprintf(fid, "Keyboard Proc: %s", error_msg);
	}
}

int main(int argc, char* argv[])
{		
	int retCode = 0;/*success*/
	int pid;
	int i;
	int fid; /*parent process id and file id of RX shared map memory*/
	FILE* fid2; /*file id for debug output*/
	char*  mmap_ptr; /*pointer to the shared map memory*/
	input_buffer *in_mem_ptr; /*C standard pointer to the shared map memory*/
	int loop_index;
	char kbd_input; //stores the current char entered on keyboard		

	if (DEBUG)
	{
		fid2 = fopen("keyboardOutput", "w+");
		if (fid2 == NULL)
		{
			fprintf(fid2, "Keyboard Proc: could not open keyboardOutput file");
		}
	}	
    
	sscanf(argv[1], "%d", &pid); 
	sscanf(argv[2], "%d", &fid); 
	
	printf("pid: %i\n", pid);
	printf("fid: %i\n", fid); 

	/*establish connection to RX shared memory map*/

	mmap_ptr = mmap((void *)0, BUFFER_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fid, (off_t) 0);
	
	if (mmap_ptr == NULL)
	{
		if (DEBUG) debug(fid2, "Keyboard Process: Memory map could not be created\n");
	//	die(0);//what should the parameter be?
	}
       	
	in_mem_ptr = (input_buffer *) mmap_ptr;
	in_mem_ptr->flag = 0; //fprintf(fid2,"flag is: %d\n", in_mem_ptr->flag);//set ready flag to indicate not ready
	in_mem_ptr->input_count = 0; //fprintf(fid2,"input count of memory is: %d\n", in_mem_ptr->input_count);
	loop_index = 0;//fprintf(fid2,"loop count is: %d\n", loop_index);

	do
	{
		kbd_input = getchar();
		fprintf(fid2, "the input was: %c\n", kbd_input);
		fflush(fid2);
		if (kbd_input != '\n')
		{
			if (loop_index < BUFFER_SIZE)			
			{
				in_mem_ptr->input_data[loop_index] = kbd_input; //fprintf(fid2, "loop index: %d, memory input: %c\n", loop_index, in_mem_ptr->input_data[loop_index]);
				in_mem_ptr->input_count++; //fprintf(fid2, "input count: %d\n", in_mem_ptr->input_count);
				loop_index++; //fprintf(fid2, "loop index: %d\n", loop_index);
			}
		}
		else
		{
			//set flag to done, and signal rtx to start reading
			//in_mem_ptr->input_data[loop_index] = '\0'; //fprintf(fid2, "loop index: %d, memory input: %c\n", loop_index, in_mem_ptr->input_data[loop_index]);
			in_mem_ptr->flag = 1;                     
			/*for (i = 0; i < loop_index; i++) {
				printf("%c", in_mem_ptr->input_data[i]);
		    }*/			
			loop_index = 0;
			kill(pid, SIGUSR1);					
			//in_mem_ptr->input_count = 0;
			/*while(in_mem_ptr->flag == 1)
			{
				usleep(1000);                              
			}*/
		}
	}while(1);  		
	return retCode;
}
	
