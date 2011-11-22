#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "queue.h"
#include "iproc.h"
#include "userProcesses.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include "rtx.h"
#include <string.h>

int k_send_message ( int dest_process_id, msg_envelope * msg_envelope )
{
    if (dest_process_id==0)
        return ERROR_INVALID_PID;

    PCB *target_PCB;
    //int n = NUM_OF_IPROC;

    int i;
    int retCode = SUCCESS;
	
    for (i=0;i<NUM_TOTAL_PROC;i++) //use dest_id to look up the target process PCB pointer
    {
	if (dest_process_id == pcb_pointer_tracker[i]->process_id) 
	    target_PCB = pcb_pointer_tracker[i];
    }

    msg_queue *temp_msg_q;
    temp_msg_q= &target_PCB->msg_envelope_q;
    printf("k_send_message: Target process: %i, current process: %i\n",target_PCB->process_id, current_process->process_id);

    //enqueue envelope onto the message queue of the target process
    msg_envelope->sender_pid = current_process->process_id;
    msg_envelope->receiver_pid = dest_process_id;
    retCode = msg_enqueue(msg_envelope, temp_msg_q);

    //printf("k_send_message: target pcb queue size: %i\n", target_PCB->msg_envelope_q.size); 

    if (target_PCB->msg_envelope_q.head==NULL)
	printf("send failed: msg_envelope_q is null\n");

    if(target_PCB->process_state == BLOCKED_ON_RECEIVE)
    {
		target_PCB->process_state = READY;
		printf("k_send_message:Enquing process %i to rpq\n", target_PCB->process_id); 
		retCode = rpq_enqueue(target_PCB);//enqueue blocked process to ready queue;
    }

	//Record send on the trace buffer
	if (send_tr_buf->index == BUFFER_SIZE) {
		//If the trace buffer is full
		int i;
		//Shift the stored data by 1 unit
		for (i = 0; i <send_tr_buf->index-1; i++){
			send_tr_buf->send_trace_buffer_array[i].sender_pid = send_tr_buf->send_trace_buffer_array[i+1].sender_pid;
			send_tr_buf->send_trace_buffer_array[i].receiver_pid = send_tr_buf->send_trace_buffer_array[i+1].receiver_pid;
			send_tr_buf->send_trace_buffer_array[i].time = send_tr_buf->send_trace_buffer_array[i+1].time;
		}
		send_tr_buf->send_trace_buffer_array[BUFFER_SIZE].sender_pid = msg_envelope->sender_pid;
		send_tr_buf->send_trace_buffer_array[BUFFER_SIZE].receiver_pid = msg_envelope->receiver_pid;
		send_tr_buf->send_trace_buffer_array[BUFFER_SIZE].time = kernel_clock;
	} else {
		send_tr_buf->send_trace_buffer_array[send_tr_buf->index].sender_pid = msg_envelope->sender_pid;
		send_tr_buf->send_trace_buffer_array[send_tr_buf->index].receiver_pid = msg_envelope->receiver_pid;
		send_tr_buf->send_trace_buffer_array[send_tr_buf->index].time = kernel_clock;
		send_tr_buf->index++;
	}

	fflush(stdout);        

	return SUCCESS;
}

msg_envelope * k_receive_message()
{	
	while ( current_process->msg_envelope_q.size == 0)
	{
		if (current_process->process_priority == IPROCESS)
			return NULL;
		else {
			current_process->process_state = BLOCKED_ON_RECEIVE;
			process_switch();
		}
	}
	msg_queue *temp_queue;
	temp_queue = &current_process->msg_envelope_q;
	msg_envelope *temp_envelope = (msg_envelope *) msg_dequeue(temp_queue);
	//printf("k_receive_message: Sender id is %i",temp_envelope->sender_pid);
	//store the details of this receive transaction on the receive_trace_buffer
	if (send_tr_buf->index == BUFFER_SIZE) {
		//If the trace buffer is full
		int i;
		for (i = 0; i <send_tr_buf->index-1; i++){
			//Shift the stored data by 1 unit
			receive_tr_buf->receive_trace_buffer_array[i].sender_pid = send_tr_buf->send_trace_buffer_array[i+1].sender_pid;
			receive_tr_buf->receive_trace_buffer_array[i].receiver_pid = send_tr_buf->send_trace_buffer_array[i+1].receiver_pid;
			receive_tr_buf->receive_trace_buffer_array[i].time = send_tr_buf->send_trace_buffer_array[i+1].time;
		}

		receive_tr_buf->receive_trace_buffer_array[BUFFER_SIZE].sender_pid = temp_envelope->sender_pid;
		receive_tr_buf->receive_trace_buffer_array[BUFFER_SIZE].receiver_pid = temp_envelope->receiver_pid;
		receive_tr_buf->receive_trace_buffer_array[BUFFER_SIZE].time = kernel_clock;
	} else {
		receive_tr_buf->receive_trace_buffer_array[receive_tr_buf->index].sender_pid = temp_envelope->sender_pid;
		receive_tr_buf->receive_trace_buffer_array[receive_tr_buf->index].receiver_pid = temp_envelope->receiver_pid;
		receive_tr_buf->receive_trace_buffer_array[receive_tr_buf->index].time = kernel_clock;
		receive_tr_buf->index++;
	}
	return temp_envelope;
}

int k_terminate() // GLOBAL VARIABLE FOR: child process id, file id
{
    int retCode=SUCCESS;
    int i;    
    for (i=0;i<2;i++)
    {
	fflush(stdout);
	printf("childpid: %i\n", childpid[i]);
	fflush(stdout);
        kill(childpid[i],SIGINT);
    } // terminate child process(es)    
    
    int statusfn1,statusfn2,statusfid,status1,status2;
    status1 = munmap(in_mem_ptr, 256); 
    status2 = munmap(out_mem_ptr, 256);
    
    if (status1==-1 || status2==-1) // remodve shared memory segment and do some standard error checks
    {
	fflush(stdout);
        printf("BAD UNMAP\n");
        retCode = ERROR_BAD_MEMORY_UNMAP;
    }
    
    for (i=0;i<2;i++)
    {
        fflush(stdout);
	printf("fid: %i\n", fileid[i]);
        statusfid = close(fileid[i]);        
        
        if (statusfid == -1)
        {
	    fflush(stdout);
            printf("BAD FILE CLOSE\n");
            retCode = ERROR_BAD_FILE_CLOSE;
        }
    }

    status1 = unlink("./build/in_buf");
    status2 = unlink("./build/out_buf");

    if (status1==-1 || status2==-1)
    // remove shared memory segment and do some standard error checks
    {
        fflush(stdout);
        printf("BAD MMAP UNLINKP\n");
        retCode = ERROR_BAD_FILE_UNLINK;
    }
 
    printf("K_terminate: Deallocating data structures\n");   
    printf("K_terminate: Free free envelope queue\n");
    free(free_env_Q);
    
    printf("K_terminate: Free blocked on resource queue\n");
    free(blocked_on_resource_Q);
    
    printf("K_terminate: Free sorted time list\n");
    free(sorted_timeout_list);
    
    printf("K_terminate: Free RPQ\n");
    free(rpq);     
    
    printf("K_terminate: Free created PCBs (pcb_pointer_tracker\n");    
    for (i=0; i < NUM_TOTAL_PROC; i++) //*******8 for full implementation!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    {
        free(pcb_pointer_tracker[i]);
	printf("%i\n",i);
    }     

    kill(getpid(), SIGINT); 
    exit(0);	
    return retCode;
}

int k_send_console_chars(msg_envelope *message_envelope)
{
    int retCode = SUCCESS;
    printf("Send_console_chars: invoked\n");
    fflush(stdout);
    if (message_envelope != NULL)
    {	
		if (message_envelope->msg_type == OUTPUT_REQUEST) {
		    retCode = k_send_message(IPROC_CRT, message_envelope);
			kill(getpid(), SIGUSR2);
		}
		else {
			printf("send_console_chars: trying to output msg with invalid msg type\n"); 
			retCode = ERROR_INVALID_MID; 
		}
    }
    else
    	retCode = ERROR_INVALID_MID;
		
    return retCode;
}

int k_get_console_chars(msg_envelope *message_envelope )
{
    int retCode = SUCCESS;
    
    if (message_envelope != NULL)
    {
	//printf("Get_console_chars: queue dequue: msg env: %d\n", message_envelope->msg_size);
	fflush(stdout);
	retCode = k_send_message(IPROC_KBD, message_envelope);
	printf("Get console chars was called by %i\n", current_process->process_id);
	fflush(stdout);
    }
    else
	retCode = ERROR_INVALID_MID;
	
    return retCode;
}

int k_request_delay( int time_delay, int wakeup_code, msg_envelope * message_envelope )
{
    if (message_envelope)
    {
	message_envelope->n_clock_ticks = time_delay;
        // popoulate num of clock ticks desired inside message
	message_envelope->msg_type = wakeup_code;
        // write message with code timing service will reply with after delay is over
	int retCode = k_send_message(message_envelope->receiver_pid, message_envelope);
		
        return retCode;
    }
    return FAIL;
}

int k_release_msg_env(msg_envelope * rel_msg)
{
    rel_msg->sender_pid = -1;
    rel_msg->receiver_pid = -1; 
    rel_msg->msg_type = -1; 
    rel_msg->n_clock_ticks = -1; 

    if (rel_msg->msg_size != 0)
        rel_msg->msg_size = 0;
    
    int retCode = SUCCESS;
    retCode = msg_enqueue(rel_msg, free_env_Q);
    if (blocked_on_resource_Q->head != NULL)
    {
        blocked_on_resource_Q->head->process_state = READY;
        PCB *temp;
        temp = blocked_on_resource_Q_dequeue(); 
        
        retCode = rpq_enqueue(temp);
    }
    
    return retCode; //should check for error
}

msg_envelope * k_request_msg_env()
{
    while (free_env_Q ==NULL)
    {
	current_process->process_state = BLOCKED_ON_RECEIVE;
	blocked_on_resource_Q_enqueue(current_process);
	process_switch();
    }
    
    msg_envelope *temp = msg_dequeue(free_env_Q);
    // NO DEQUEUE OR ENQUEUE FUNCTION FOR FREE_ENV_Q???
    return temp;
}

int k_change_priority(int new_priority, int target_process_id)
{
    PCB *temp;
    int retCode;
    
    if (new_priority==0)
        return ERROR_INVALID_PID;
    
    else if (target_process_id==0)
        return ERROR_INVALID_PRIORITY;
    else
    {        
        int i;
        for (i = 0;i < NUM_OF_USER_PROC;i++)
        {
            if (target_process_id == pcb_pointer_tracker[i]->process_id)
                temp = pcb_pointer_tracker[i];
        }
	
        temp->process_priority = new_priority;
        retCode = 0;
		
        //invoke scheduler so that process is enqueued onto appropriate queue		
    }
	return retCode;
}

int k_release_processor()
{
    current_process->process_state=READY; //set state of process to ready;
    int retCode=rpq_enqueue(current_process);//enqueue onto ready queue;
    process_switch();
    return retCode;
}

int k_get_trace_buffers(msg_envelope *env) {
	// Check for null envelope pointer

	if (env != NULL) {
		//Get all 16 elements of send and receive trace buffers and copy them into the message envelope
		int size; // Size of buffer array
		char output_format[100]; // output format for the trace buffer
		char sid[1];
		char rid[1];
		char t[20];
		char * temp_str;
		int i, j;
		int retCode;

		if(env->msg_size > 0) 
		    env->msg_size = 0; //Make this free envelope

		size = send_tr_buf->index;
		printf("k_get_trace_buffer: Copying send trace content of size %i\n", size);
		while (size > 0 && env->msg_size < BUFFER_SIZE) {
			sprintf(sid, "%i", send_tr_buf->send_trace_buffer_array[size-1].sender_pid);
			sprintf(rid, "%i", send_tr_buf->send_trace_buffer_array[size-1].receiver_pid);
			sprintf(t, "%i", send_tr_buf->send_trace_buffer_array[size-1].time);

			strcat(output_format, "Sender ID: ");
			strncat(output_format, sid, 1);
			
			strcat(output_format, " Receiver ID: ");
			strncat(output_format, rid, 1);

			strcat(output_format, " Time: ");
			strncat(output_format, t, 20);

			strcat(output_format, "\n\n");

			int str_size = strlen(output_format);
			char output[str_size - 1];
			strcpy(output, output_format);
			i = env->msg_size + str_size - 1;
			j = 0;
			while (env->msg_size < i){
				if (env->msg_size >= BUFFER_SIZE) {
				    break;
				} else {
				    env->msg_text[env->msg_size] = output[j];
				    env->msg_size++;
				}
				j++;				
			}
			size--;
		}	

		size = receive_tr_buf->index;
		printf("k_get_trace_buffer: Copying rceive trace content of size %i\n", size);
		while (size > 0 && env->msg_size < BUFFER_SIZE) {
			sprintf(sid, "%i", send_tr_buf->send_trace_buffer_array[size-1].sender_pid);
			sprintf(rid, "%i", send_tr_buf->send_trace_buffer_array[size-1].receiver_pid);
			sprintf(t, "%i", send_tr_buf->send_trace_buffer_array[size-1].time);

			strcat(output_format, "Receiver ID: ");
			strncat(output_format, rid, 1);

			strcat(output_format, " Sender ID: ");
			strncat(output_format, sid, 1);

			strcat(output_format, " Time: ");
			strncat(output_format, t, 20);

			strcat(output_format, "\n\n");

			//output_format =" Receiver ID: " + *rid + "Sender ID: " + *sid + "Time: " + *t + "\n";

			int str_size = strlen(output_format);
			char output[str_size - 1];
			strcpy(output, output_format);
			i = env->msg_size + str_size - 1;
			j = 0;
			while (env->msg_size < i){
				if (env->msg_size >= BUFFER_SIZE) {
				    break;
				} else {
				    env->msg_text[env->msg_size] = output[j];
				    env->msg_size++;
				}
				j++;				
			}
			size--;
		}	

		//Send the filled message envelope to the CRT so that it can output to screen
		env->msg_type = OUTPUT_REQUEST;
		retCode = k_send_console_chars(env);
        return retCode;
	} else {
		return FAIL;
	}
}

int k_request_process_status(msg_envelope * msg) {
	int i;
	char *outputformat;
	int retCode;
	retCode = 0;

	outputformat = "Process_ID,Priority,Status\n";
	for (i=0; i<NUM_OF_PROC; i++) {
		strcat(outputformat, (char *) pcb_pointer_tracker[i]->process_id);
		strcat(outputformat, ",");
		strcat(outputformat, (char *) pcb_pointer_tracker[i]->process_priority);
		strcat(outputformat, ",");
		strcat(outputformat, (char *) pcb_pointer_tracker[i]->process_state);
		strcat(outputformat, "\n");
	}

	int str_size = strlen(outputformat);
	char output[str_size];
	strcpy(output, outputformat);
	msg->msg_size = 0;
	for (i=0; i<str_size; i++) {
		msg->msg_text[i] = output[i];
		msg->msg_size++;
	}
	retCode = send_console_chars(msg);
	return retCode;
}
