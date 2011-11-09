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

#define buffer_size 128 

/*will need to lock file or use semaphores to regulate access to memory mapped file for concurrency reasons*/

int main(int argc, char* argv[])
{	
	printf("Hi");
	int retCode = 0;/*success*/
	int pid;
	int fid; /*parent process id and file id of RX shared map memory*/
	char*  mmap_ptr; /*pointer to the shared map memory*/
	input_buffer *in_mem_ptr; /*C standard pointer to the shared map memory*/
	int loop_index;
	char kbd_input; //stores the current char entered on keyboard	

	fid = open("mmapfile", O_RDWR);
	if (fid == -1)
	{
		printf("Bad file");
	}	
        
	sscanf(argv[1], "%d", &pid);
	sscanf(argv[2], "%d", &fid);
	
	/*establish connection to RX shared memory map*/
	mmap_ptr = mmap((caddr_t *)0, buffer_size, PROT_READ|PROT_WRITE, MAP_SHARED, fid, (off_t) 0);
	
	if (mmap_ptr == NULL)
	{
		printf("Keyboard Process: Memory map could not be created\n");
	//	die(0);//what should the parameter be?
	}
                        
	in_mem_ptr = (input_buffer *) mmap_ptr;
	in_mem_ptr->flag = 0; /*set ready flag to indicate not ready*/
	in_mem_ptr->input_count = 0; 
	loop_index = 0;

	do
	{
		kbd_input = getchar();
		if (kbd_input != '\n')
		{
			if (loop_index < 50)
			{
				in_mem_ptr->input_data[loop_index] = kbd_input;
				in_mem_ptr->input_count++;
				loop_index++;
			}
		}
                else{
                    //set flag to done, and signal rtx to start reading
                    in_mem_ptr->flag = 1;                     
                    pid_t x = IPROC_KBD;
                    kill(x, SIGUSR1);
                }
                loop_index = 0;
                in_mem_ptr->input_count = 0;
                while(in_mem_ptr->flag == 1)
                    usleep(1000);                               
	}while(1); 
		
	return retCode;
}
