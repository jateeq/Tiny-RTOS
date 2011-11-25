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
			if (msg_env == NULL) {
			   printf("KBD_IPROC: message env is null\n");
			}
			else
			{			
				//printf("KBD IPROC: msg env: %d\n", msg_env->msg_size);
				fflush(stdout);
				in_mem_ptr->flag = 0;
				fflush(stdout);				
				int count;
				for(count = 0;count<in_mem_ptr->input_count;count++)
				{
					msg_env->msg_text[count] = in_mem_ptr->input_data[count];
				}
				msg_env->msg_size = in_mem_ptr->input_count;
				in_mem_ptr->input_count = 0;								
				printf("KBD_IPROC: sending message to process CCI\n"); 
				fflush(stdout);		
				msg_env->msg_type = CONSOLE_INPUT;		
				k_send_message(PROC_CCI, msg_env);
			}		
		}
	}               
}

void crt_iproc()
{
	printf("=========CRT IPROC is invoked========\n");
	fflush(stdout);
	int error_code; 
	msg_envelope * temp; 
	temp = current_process->msg_envelope_q.head; 
	printf("crt_iproc: CRT message queue size: %i\n", current_process->msg_envelope_q.size);
	int error; 
	if (temp != NULL) {
		if (temp->msg_type == OUTPUT_REQUEST) {
			fflush(stdout);
			printf("crt_iproc: Message from sender process was: %c\n", temp->msg_text[0]);

			//fill buffer		
			int count;
			printf("CRT IPROC: Copying %i characters.\n", temp->msg_size);
			out_mem_ptr->output_count = 0;

			for(count = 0; count < temp->msg_size; count++)
		            {
		                out_mem_ptr->output_data[count] = temp->msg_text[count];
						out_mem_ptr->output_count++;
		            }
			temp->msg_type = OUTPUT_CONFIRM; 
			//get rid of this dangerous code
			out_mem_ptr->flag = 1;
		}

		else if (temp->msg_type == OUTPUT_CONFIRM) {
			fflush(stdout);
			printf("crt_iproc: trying to send confirmation\n");
			fflush(stdout);
			//waiting for flag to become 0 to indicate CRT has finished output
			while (out_mem_ptr->flag == 1)
			{
				usleep(100000);
			}
			//send and receive confirmation message to invoking process		
			temp = k_receive_message(); 
			temp->receiver_pid = temp->sender_pid;
			temp->sender_pid = current_process->process_id; 
			temp->msg_type = DISPLAY_ACK;
			error_code = k_send_message(PROC_CCI, temp);
			fflush(stdout);
			printf("crt_iproc: Signal from CRT has been received. Acknowledgment sent back to process P\n");
			fflush(stdout);
		}
	} else {		
		printf("crt_iproc: head is NULL");
	}
    return; 
}


void timer_iproc() {
	//Increment internel kernel clock (in ms)
	kernel_clock++;
	
	//printf("Timer iprocess is invoked\n");

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
		//	printf("timer_iproc: Clock tick:%i\n", msg->n_clock_ticks);
			while (msg->next) {
				msg->n_clock_ticks--;
			}
		} else {
		    	//Something went wrong...
		}

	//	printf("timer_iproc: Checking for expired message envelope\n");
		while (sorted_timeout_list->tail != NULL)
		{
			if (sorted_timeout_list->tail->n_clock_ticks == 0) {
				msg = msg_dequeue(sorted_timeout_list);//Dequeue the expired message from	 the sender
				//printf("timer_iproc: Returning message back to the sender\n");
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
	PCB* temp;
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
				fflush(stdout); 
				current_process->process_state = INTERRUPTED;
				previous_process = current_process;
				current_process = pcb_pointer_tracker[IPROC_CRT];
				crt_iproc();
	            current_process = previous_process;
	            current_process->process_state = EXECUTING;									
				break;
	}	
}
