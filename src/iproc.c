/*this project is part of the uwaterloo - rtos mte 241 - fall 2011course project for group 19
authors:
jake park, yifei cheng. zhuojun li, jawad ateeq

updated october 23, 2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "rtx.h"
#include "queue.h"
#include "iproc.h"
#include "kernelAPI.h"

void kb_iproc(){

//	fflush(stdout);
//	printf("kb_iproc msg_queue size: %i\n", current_process->msg_envelope_q.size); 
//	printf("current executing proc id (should be KB): %i\n", current_process->process_id); 
//	fflush(stdout);
		
	msg_envelope * msg_env = (msg_envelope *)k_receive_message(); 

	while (msg_env == NULL) {
	    usleep(1000000);		    
	}	

	if( msg_env != NULL)
	{
		//check if the buffer is ready for transmitting
                if (in_mem_ptr->flag == 1)
		{
			printf("IPROCKBD: calling receive...\n");	

			if (msg_env == NULL) {
			   printf("KBD_IPROC: message env is null\n");
			}
			else
			{			
			  	printf("KBD_IPROC: message reveived!\n");
				printf("KBD IPROC: msg env: %d\n", msg_env->msg_size);
				printf("KBD_IPROC: Setting the kb flag to zero!\n");
				fflush(stdout);
				in_mem_ptr->flag = 0;
				printf("KBD_IPROC: copying contents into message to be sent to process P\n"); 
				fflush(stdout);				
				int count;
				for(count = 0;count<in_mem_ptr->input_count;count++)
				{
					msg_env->msg_text[count] = in_mem_ptr->input_data[count];
				}
				msg_env->msg_size = in_mem_ptr->input_count;
				in_mem_ptr->input_count = 0;								
				printf("KBD_IPROC: sending message to process p"); fflush(stdout);				
				k_send_message(PROC_P, msg_env);
			}		
		}
	}               
}

void crt_iproc()
{
	msg_envelope * msg; 
	int error_code;
	msg = (msg_envelope *) k_receive_message();   

	if (msg != NULL) {

		fflush(stdout);
		printf("Message from process P was: %c\n", msg->msg_text[0]);
		//waiting for flag to become 0 so that the buffer is ready to be filled
		while (out_mem_ptr->flag == 1)
		{
			usleep(100000);
		}

		//fill buffer		
		int count;
		printf("CRT IPROC: Copying %i characters.\n", msg->msg_size);
		for(count = 0; count < msg->msg_size; count++)
                {
                    out_mem_ptr->output_data[count] = msg->msg_text[count];
                }
		out_mem_ptr->flag = 1;
		out_mem_ptr->output_count = msg->msg_size;
		//waiting for flag to become 0 to indicate CRT has finished output
		while (out_mem_ptr->flag == 1)
		{
			usleep(100000);
		}
		//send confirmation message to invoking process		
		msg->receiver_pid = msg->sender_pid;
		msg->sender_pid = current_process->process_id; 
		msg->msg_type = DISPLAY_ACK;
		error_code = k_send_message(PROC_P, msg);
		fflush(stdout);
		printf("Signal from CRT has been received. Acknowledgment sent back to process P\n");
		fflush(stdout);
	} else {		
	
	}
    return; 
}

void timer_iproc() {
	//Increment internel kernel clock (in ms)
	kernel_clock++;
	
//	printf("Timer iprocess is invoked\n");

	//Now check for any message sent by other processes
//	printf("Recieving time out request from other processes\n");
	msg_envelope *msg = (msg_envelope *) k_receive_message();
	if (msg == NULL) {
//		printf("Message is not received\n");
	}

	while (msg != NULL) {
		time_out_request_enqueue(msg); //code to enqueue and sort the message envelope into the timeout_list
		msg = (msg_envelope *) k_receive_message(); //see if any more msgs left
	}

	//Check if the timeout list is empty
	if (sorted_timeout_list->size > 0)
	{
		//code to decrement the tick count of 1st in the timeout_list
//		printf("Decrementing clock ticks for message envelopes in timeout list\n");
		msg = sorted_timeout_list->head;
		if (msg != NULL) {
		    	msg->n_clock_ticks--;
			printf("Clock tick:%i\n", msg->n_clock_ticks);
		}
		while (msg->next) {
			msg->n_clock_ticks--;
		}

		printf("Checking for expired message envelope\n");
		while (sorted_timeout_list->tail != NULL)
		{
			if (sorted_timeout_list->tail->n_clock_ticks == 0) {
				msg = msg_dequeue(sorted_timeout_list);//Dequeue the expired message from the sender
				printf("Returning message back to the sender\n");
			    k_send_message(msg->sender_pid, msg);//return envelope to the sender
			} else {
				break;
			}
		}
	} else {
//		printf("time out list is empty\n");
	}
}


void signal_handler(int signum)
{    
	PCB* previous_process;
	switch(signum)
	{
			case SIGINT: 
				k_terminate();
				break;
			case SIGALRM:
				current_process->process_state = INTERRUPTED;
				previous_process = current_process;
				current_process = pcb_pointer_tracker[IPROC_TIMER];
	                	timer_iproc();
	                	current_process = previous_process;
	               		current_process->process_state = EXECUTING;
				break;
			case SIGUSR1: // kb handler
				current_process->process_state = INTERRUPTED;
				previous_process = current_process;
			    	current_process = pcb_pointer_tracker[IPROC_KBD];
		 		kb_iproc();
	                	current_process = previous_process;
	               		current_process->process_state = EXECUTING;
				break;
			case SIGUSR2: // crt handler
				current_process->process_state = INTERRUPTED;
				previous_process = current_process;
				current_process = pcb_pointer_tracker[IPROC_CRT];
				crt_iproc();
	                	current_process = previous_process;
	                	current_process->process_state = EXECUTING;
				break;
	}	
} 	


