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
/*wat the hell? dont think this is needed. 
    msg_envelope * msg = (msg_envelope *)k_receive_message();
    msg_queue * cur_msg_queue = &(current_process->msg_envelope_q);
    if (msg!= NULL)
    {
        //enqueue the message onto the local queue     
        msg_enqueue( msg , cur_msg_queue);
    }
*/	
	FILE* fid2;
	fid2 = fopen("keyboardOutput", "w+"); //just for debugging		
	
	//THIS IS VERY BAD BECAUSE IT CHANGES CURRENT PROCESS MANUALLY AND DOES, MUST BE CHANGED
	    int n = NUM_OF_IPROC;
	    int t = IPROC_KBD; 
	    int i;
	    
	    for (i=0;i<n;i++)
	    //use dest_id to look up the target process PCB pointer
	    {
		if (t == pcb_pointer_tracker[i]->process_id)
		    current_process = pcb_pointer_tracker[i];
	    }
		
	fflush(stdout);
	printf("kb_iproc msg_queue size: %i\n", current_process->msg_envelope_q.size); 
	printf("current executing proc id (should be KB): %i\n", current_process->process_id); 
	fflush(stdout);
	
    if(current_process->msg_envelope_q.size != 0)
    {
	//check if the buffer is ready for transmitting
		printf("KB_IPROC: mem ptr: %d\n",in_mem_ptr->flag);fflush(stdout);
        if (in_mem_ptr->flag == 1)
        {
			msg_queue * cur_msg_queue = &(current_process->msg_envelope_q); 			
			if (cur_msg_queue!= NULL)
			{
				msg_envelope * current_msg = cur_msg_queue->head; 							
				if (current_msg != NULL)
				{
				printf(fid2, "KB_IPROC: message queue pointer: %d\n", current_msg);
				}
			}
			
			
			
			/*input_buffer command; //the command that was entered by the user
			strcpy(command.input_data, in_mem_ptr->input_data);

			set number of chars read to zero so receiver knows micro is done reading*/

			/*in_mem_ptr->input_count = 0;
			in_mem_ptr->flag = 0;
			int i = 0;
			for (i; i<sizeof(command.input_data); i++)
			{
					current_msg->msg_text[i] = command.input_data[i]; //Working?
			}
			current_msg->msg_type = CONSOLE_INPUT;        
			int dest_pid = PROC_P;
			current_msg->sender_pid = current_process->process_id;
			printf("Sending message back to Process P\n");
			k_send_message(dest_pid, current_msg);    */            
        }
	}
}

void crt_iproc()
{
	//set current process
	int i;
	int n = 3;
	int t = IPROC_CRT;
	for (i=0;i<n;i++)
	{
	if (t == pcb_pointer_tracker[i]->process_id)
	    current_process = pcb_pointer_tracker[i];
	}

	msg_envelope * msg;
	int proc_p_pid; 
	int error_code;

	msg = (msg_envelope *) k_receive_message();
   
	/* we shouldn't try to keep getting message. instead, CRT process will signal and invoke repeatedly
	while (msg == NULL)
	{
		usleep(100000); 
		msg = (msg_envelope *) k_receive_message(); 
	} */

	if (msg != NULL) {

		fflush(stdout);
		printf("Message from process P was: %c\n", msg->msg_text[0]);
		//waiting for flag to become 0 so that the buffer is ready to be filled
		while (out_mem_ptr->flag == 1)
		{
			usleep(100000);
		}

		//fill buffer
		strcpy(out_mem_ptr->output_data, msg->msg_text);
		out_mem_ptr->output_count = sizeof(msg->msg_text);
		out_mem_ptr->flag = 1;
		//waiting for flag to become 0 to indicate CRT has finished output
		while (out_mem_ptr->flag == 1)
		{
			usleep(100000);
		}
		//send confirmation message to invoking process
		proc_p_pid = PROC_P;
		msg->receiver_pid = msg->sender_pid;
		msg->sender_pid = current_process->process_id; 
		msg->msg_type = DISPLAY_ACK;
		error_code = k_send_message(proc_p_pid, msg);
		fflush(stdout);
		printf("Signal from CRT has been received. Acknowledgment sent back to process P\n");
		fflush(stdout);
	} else {
		//fflush(stdout);
		//printf("Waiting for message from process P\n");
		//fflush(stdout);
	}

	//change current process back to Proc_P
	n = 3;
	t = PROC_P;
	for (i=0;i<n;i++)
	{
	if (t == pcb_pointer_tracker[i]->process_id)
	    current_process = pcb_pointer_tracker[i];
	}
    return; 
}

void timer_iproc()
{
    printf("I'm not doing naythign");
}

void signal_handler(int signum)
{
    //save the context of the currently running process
    PCB* tempPCB;
/*
    RTX * tempRTX = (RTX *)malloc(sizeof(RTX));
*/			
    tempPCB = (PCB *)malloc(sizeof(PCB));
    if (tempPCB != NULL)
    {
            tempPCB = current_process;
            switch(signum)
            {
/*
                    case SIGINT: 
                            die();
                            break;
                    case SIGALRM:
                            timer_iproc(signum);
                            break;
*/
                    case SIGUSR1: // kb handler
                            kb_iproc();
                            break;

                    case SIGUSR2: // crt handler
                            crt_iproc();
                            break;
            }	
            //current_process = tempPCB;
            free(tempPCB);
/*
            process_switch();//call the scheduler
*/
    }
}

