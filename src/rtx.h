/* 
 * File:   rtx.h
 * Author: Jake
 *
 * Created on November 4, 2011, 12:52 PM
 */

#ifndef RTX_H
#define	RTX_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <sys/types.h>

/*Constants*/
//RTX Processes
#define NUM_OF_PROC 8
#define NUM_OF_USER_PROC 1
#define NUM_OF_IPROC 3
#define NUM_TOTAL_PROC 4 //change this to 8 for full implementation

//Process IDs
#define PROC_A 0
#define PROC_B 1
#define PROC_C 2
#define PROC_CCI 3
#define PROC_CLK 4
#define IPROC_KBD 5
#define IPROC_CRT 6
#define IPROC_TIMER 7
#define PROC_NULL 8

//PROCP
#define PROC_P 9
    
//wakeup codes (unique for each process)
#define WALL_CLK_WAKEUPCODE 0	
	
//Process priorities
#define IPROCESS 0 //used for iprocesses
#define High 1
#define MEDIUM 2
#define LOW 3

//Process state
#define READY 0
#define EXECUTING 1
#define INTERRUPTED 2
#define BLOCKED_ON_ENVELOPE 3
#define BLOCKED_ON_RECEIVE 4

//IPC
#define MAX_NUM_MSG_ENV 150
#define MAX_CHAR = 128

//Message types
#define DISPLAY_ACK 0
#define CONSOLE_INPUT 1
#define TERMINATED 2
#define INCREMENT_CLOCK 3
#define CHANGE_CLOCK 4
#define STOP_CLOCK 5
#define COUNT_REPORT 6

//Success Code
#define SUCCESS 0
#define FAIL -1

//Error Codes
#define ERROR_BAD_MEMORY_UNMAP -1
#define ERROR_BAD_FILE_CLOSE -2	
#define ERROR_INVALID_PID -3
#define ERROR_INVALID_MID -4 //invalid message id
#define ERROR_FAIL_TO_MALLOC -5
#define ERROR_INVALID_PRIORITY -6
#define ERROR_BAD_FILE_UNLINK -7

//Priority
#define IPROCESS 0 //used for iprocesses
#define HIGH 1
#define MEDIUM 2
#define LOW 3
#define NULLPROCESS 4

#define ON 1
#define OFF 0
    
//Buffer Size for TX and RX:
#define BUFFER_SIZE 128 //bytes

#define NUM_MSG_ENV 60
#define PCB_QUEUE_COUNT 4
#define STACK_SIZE 4096
#define STACK_OFFSET 8
/**/

/*RTX objects*/
typedef struct msg_envelope{
   struct msg_envelope *previous;
   struct msg_envelope *next;
   int sender_pid;
   int receiver_pid;
   int msg_type; 	
   int n_clock_ticks; 
   char msg_text[128];  
   int msg_size;
} msg_envelope;

typedef struct {
    int process_id;
    int process_priority;
    int stack_size;
    void (*initial_pc)();
} initialization_table;

//this structure is used as a record in the send and receive trace buffers
typedef struct {
      int time;
      int sender_pid;
      int receiver_pid;
} msg_trace;

typedef struct {
	struct msg_envelope *head;
	struct msg_envelope *tail;
	int size;
} msg_queue;

typedef struct PCB{
   struct PCB *previous;
   struct PCB *next;
   int process_id;
   int process_priority;
   int process_state;
   void (*initial_pc)();
   int priority;
   char *process_stack;
   jmp_buf *context;
   msg_queue msg_envelope_q;
} PCB;
	
typedef struct {
     PCB *head;
     PCB *tail;
     int size;
} PCB_queue;

//struct of input buffer of RX memory mapped space:
typedef struct { 
     char input_data[128]; // 128 byte buffer stream
     int flag; //flag to indicate that input has been buffered. 1 indicates input is ready; 0 otherwise.
     int input_count; //counter of buffered input
} input_buffer;

//struct of output buffer of TX memory mapped space:
typedef struct {
      char output_data[128]; // 128 byte buffer stream
      int flag;//flag to indicate that output has been buffered. 
      int output_count; //counter of buffered output
} output_buffer;

//the send trace buffer
typedef struct {
	msg_trace send_trace_buffer_array[16];
}send_trace_buffer;

//the receive trace buffer
typedef struct {
	msg_trace receive_trace_buffer_array[16];
} receive_trace_buffer;

// Where is the iprocess timer queue?

typedef struct {
      PCB_queue *pq_array[4];  
} ready_process_queue;
/**/

/*Kernel data field*/
msg_queue *free_env_Q; //A queue for free envelopes
PCB_queue *blocked_on_resource_Q; //Blocked on resource queue for PCBs
msg_queue *sorted_timeout_list;//used by timer services to store timeout requests
ready_process_queue *rpq; //rpq
input_buffer *in_mem_ptr; // Pointer to input buffer
output_buffer *out_mem_ptr; // Pointer to  output buffer
PCB* pcb_pointer_tracker[8]; //Process list
char* input_filename; //file address of shared RX mapped file
char* output_filename; //file address of shared TX mapped file
caddr_t *mmap_ptr;
PCB* current_process;
int kernel_clock;
int childpid[2];
int fileid[2];
char* KBfilename[8];
char* CRTfilename[3];
/**/


#ifdef	__cplusplus
}
#endif

#endif	/* RTX_H */


