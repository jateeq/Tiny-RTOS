
/*Last updated on Nov 5 2011*/

#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "queue.h"
#include "userProcesses.h"
#include "iproc.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

int PROC_A_PRIORITY = 	MEDIUM;
int PROC_B_PRIORITY = MEDIUM;
int PROC_C_PRIORITY = MEDIUM;
int PROC_P_PRIORITY = MEDIUM;
int PROC_CCI_PRIORITY = HIGH;
int PROC_CLK_PRIORITY = HIGH;
int IPROC_KBD_PRIORITY = IPROCESS;
int IPROC_CRT_PRIORITY = IPROCESS;
int IPROC_TIMER_PRIORITY = IPROCESS;
int PROC_NULL_PRIORITY = LOW; 

int size_of_stack = 4096; 
initialization_table init_table[8]; //Declare an array of initialization_table
int stack_offset = 8;
char *jmpsp;
jmp_buf kernel_buf;

int initialize_data() {
   int i;
   
   for (i = 0; i < 3; i++) {
	pcb_pointer_tracker[i] = NULL;
   }

   blocked_on_resource_Q =  (PCB_queue *) malloc(sizeof(PCB_queue)); //Allocate memory for
   
   blocked_on_resource_Q->head = NULL;
   blocked_on_resource_Q->tail = NULL;

   free_env_Q = (msg_queue *) malloc(sizeof(msg_queue)); //Allocate memory for free_env_q 

   free_env_Q->head = NULL;
   free_env_Q->tail = NULL;
   
   for (i = 0; i < 60; i ++) {
       msg_envelope *temp_envelope = (msg_envelope *) malloc(sizeof(msg_envelope));
       temp_envelope->msg_size = 0;
       msg_enqueue(temp_envelope, free_env_Q);
   }
   
   rpq  = (ready_process_queue *) malloc(sizeof(ready_process_queue)); //Allocate memory for rpq

   for (i = 0; i < 4; i++) {
       rpq->pq_array[i] = (PCB_queue *) malloc(sizeof(PCB_queue));
       rpq->pq_array[i]->head = NULL;
       rpq->pq_array[i]->tail = NULL;
   }

   initialize_IT();
   
   return SUCCESS;
}

void initialize_IT() {
	/*User processes*/ 
        init_table[0].process_id = PROC_P;
	init_table[0].process_priority = PROC_P_PRIORITY;
	init_table[0].stack_size = size_of_stack ;
        init_table[0].initial_pc = (void*) processP;
        
        init_table[1].process_id = IPROC_KBD;
	init_table[1].process_priority = IPROCESS;
	init_table[1].stack_size = size_of_stack;
	init_table[1].initial_pc = (void*) kb_iproc;

	init_table[2].process_id = IPROC_CRT;
	init_table[2].process_priority = IPROCESS;
	init_table[2].stack_size = size_of_stack;
	init_table[2].initial_pc = (void*) crt_iproc;
        
        /*
        init_table[0].process_id = PROC_A;
	init_table[0].process_priority = PROC_A_PRIORITY;
	init_table[0].stack_size = size_of_stack ;
        init_table[0].initial_pc = (void*) process_A;

	init_table[1].process_id = PROC_B;
	init_table[1].process_priority = PROC_B_PRIORITY;
        init_table[1].stack_size = size_of_stack;
	init_table[1].initial_pc = (void*) process_B;
	
	init_table[2].process_id = PROC_C;
	init_table[2].process_priority = PROC_C_PRIORITY;
	init_table[2].stack_size = size_of_stack;
	init_table[2].initial_pc = (void*) process_C;

	init_table[3].process_id = PROC_CCI;
	init_table[3].process_priority = PROC_CCI_PRIORITY;
	init_table[3].stack_size = size_of_stack;
	init_table[3].initial_pc = (void*) process_CCI;

	init_table[4].process_id = PROC_NULL;
	init_table[4].process_priority = PROC_NULL_PRIORITY;
	init_table[4].stack_size = size_of_stack;
	init_table[4].initial_pc = (void*) process_NULL;

	init_table[4].process_id = PROC_CLK;
	init_table[4].process_priority = PROC_NULL_PRIORITY;
	init_table[4].stack_size = size_of_stack;
	init_table[4].initial_pc = (void*) process_CLK;
	/**/

	/*Iprocesses*/
	/*
        init_table[5].process_id = IPROC_TIMER;
	init_table[5].process_priority = IPROCESS;
	init_table[5].stack_size = size_of_stack;
        init_table[5].initial_pc = (void*) timer_iproc;
        *
	init_table[6].process_id = IPROC_KBD;
	init_table[6].process_priority = IPROCESS;
	init_table[6].stack_size = size_of_stack;
	init_table[6].initial_pc = (void*) kb_iproc;

	init_table[7].process_id = IPROC_CRT;
	init_table[7].process_priority = IPROCESS;
	init_table[7].stack_size = size_of_stack;
	init_table[7].initial_pc = (void*) crt_iproc;
	/**/
}

int initialize_process() {
    int i;

    for (i = 0; i < 1; i++) {
            pcb_pointer_tracker[i] = (PCB *)malloc(sizeof(PCB));

            if (pcb_pointer_tracker[i] == NULL) {
                    return ERROR_FAIL_TO_MALLOC;
            }

            pcb_pointer_tracker[i]->process_id = init_table[i].process_id;
            pcb_pointer_tracker[i]->priority = init_table[i].process_priority;

            char *stack_temp = ((char*)malloc(init_table[i].stack_size)) + size_of_stack - stack_offset;
            if (stack_temp == NULL) {
                    return ERROR_FAIL_TO_MALLOC;
            }

	    //printf("sp ok");

            pcb_pointer_tracker[i]->process_stack = stack_temp;
            pcb_pointer_tracker[i]->initial_pc = init_table[i].initial_pc;
            pcb_pointer_tracker[i]->process_state = READY;
            pcb_pointer_tracker[i]->previous = NULL;
            pcb_pointer_tracker[i]->next = NULL;
            pcb_pointer_tracker[i]->msg_envelope_q.head = NULL;
            pcb_pointer_tracker[i]->msg_envelope_q.tail = NULL;

            jmp_buf *temp_context = (jmp_buf *) malloc(sizeof(jmp_buf));
            if (temp_context == NULL) {
                    return ERROR_FAIL_TO_MALLOC;
            }

	    //printf("jmp_buf ok");

            pcb_pointer_tracker[i]->context = temp_context;
            rpq_enqueue(pcb_pointer_tracker[i]);

            current_process = pcb_pointer_tracker[i];

            if  (setjmp (kernel_buf) == 0) {
                    jmpsp = pcb_pointer_tracker[i]->process_stack;
                    __asm__ ("movl %0,%%esp" :"=m" (jmpsp));

                    //_set_sp(jmpsp);

                    if (setjmp (*pcb_pointer_tracker[i]->context) ==0){
                            longjmp (kernel_buf, 1);
                    } else {
                            current_process->process_state = EXECUTING;
                            current_process->initial_pc();
                    }
            }

	    //printf("setjmp ok");
    }

    for (i = 1; i < 3; i++) {
            pcb_pointer_tracker[i] = (PCB *) malloc(sizeof(PCB));

            if (pcb_pointer_tracker[i] == NULL) {
                    return ERROR_FAIL_TO_MALLOC;
            }

            pcb_pointer_tracker[i]->process_id = init_table[i].process_id;
            pcb_pointer_tracker[i]->priority = init_table[i].process_priority;

            char *stack_temp = ((char*)malloc(init_table[i].stack_size)) + stack_offset;
            if (stack_temp == NULL) {
                    return ERROR_FAIL_TO_MALLOC;
            }

            pcb_pointer_tracker[i]->process_stack = stack_temp;
            pcb_pointer_tracker[i]->initial_pc = init_table[i].initial_pc;
            pcb_pointer_tracker[i]->process_priority = READY;
            pcb_pointer_tracker[i]->previous = NULL;
            pcb_pointer_tracker[i]->next = NULL;
            pcb_pointer_tracker[i]->msg_envelope_q.head = NULL;
            pcb_pointer_tracker[i]->msg_envelope_q.tail = NULL;

            jmp_buf *temp_context = (jmp_buf *) malloc(sizeof(jmp_buf));
            if (temp_context == NULL) {
                    return ERROR_FAIL_TO_MALLOC;
            }

            pcb_pointer_tracker[i]->context = temp_context;
    }

    return SUCCESS;
}

int init_keyboard_process() {
    input_filename = "in_buf";

    int fid = open(input_filename, O_RDWR | O_CREAT | O_EXCL,  (mode_t) 0755); //Create input buffer file
    ftruncate(fid, 256); //Making the size of the file the same as the buffer
    int pid = getpid(); // parent id to pass on to keyboard process
     
     char arg_for_child1 [20];
     char arg_for_child2 [20]; // Two char array to store pid and fid
     
     sprintf(arg_for_child1, "%d", pid); 
     sprintf(arg_for_child2, "%d", fid); //Convert the id into character array
    
     int current_id = fork(); //Create kbd child process 	 	 
     if (current_id == 0) { //Check if this is child process
         
         if (execl("./keyboard", "keyboard" ,arg_for_child1, arg_for_child2, NULL ) == -1) { // execute keyboard process and pass in the pid and fid. 
             printf("Error in creating keyboard child process\n");
             exit(1);
         }
     } else {
        //Parent process continues to create input memory map
        mmap_ptr = mmap((caddr_t) 0, 512, PROT_READ | PROT_WRITE, MAP_SHARED, fid, (off_t) 0); //Create shared map region between RTX and keyboard

        if (mmap_ptr == NULL) {
            printf("Error: Failed at creating memory map. Exiting...\n");
            // k_terminate();
        }

        in_mem_ptr = (input_buffer *) mmap_ptr; //Standard C RX memory map pointer
        in_mem_ptr->flag = 0;
        in_mem_ptr->input_count = 0 ;

        return SUCCESS; 
     }

}

int init_crt_process() { 
    output_filename = "out_buf";
    int fid = open(output_filename, O_RDWR | O_CREAT | O_EXCL,  (mode_t) 0755); //Create out  buffer file
    ftruncate(fid, 128); //Making the size of the file the same as the buffer
    int pid = getpid(); // parent id to pass on to keyboard process
       
     char arg_for_child1 [20];
     char arg_for_child2 [20]; // Two char array to store pid and fid
     
     sprintf(arg_for_child1, "%d", pid); 
     sprintf(arg_for_child2, "%d", fid); //Convert the id into character array
    
     int current_id = fork(); //Create kbd child process 	 	 
     if (current_id ==0) { //Check if this is child process
         execl("./crt", "crt", arg_for_child1, arg_for_child2, NULL); // execute keyboard pocess and pass in the pid and fid.
	 printf("Error in creating crt child process"); 
	}
    
    //Parent process continues to create output memory map
    mmap_ptr = mmap((caddr_t) 0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fid, (off_t) 0); //Create shared map region between RTX and keyboard
	
	if (mmap_ptr == NULL) {
      	printf("Error: Failed at creating memory map. Exiting...\n");
      //  k_terminate(); //Clean up and terminate
    }
	
    out_mem_ptr = (output_buffer *) mmap_ptr; //Standard C TX memory map pointer
    out_mem_ptr->flag = 0;
    out_mem_ptr->output_count= 0;
    
    return SUCCESS;
}





