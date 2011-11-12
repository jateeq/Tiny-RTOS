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

int k_send_message ( int dest_process_id, msg_envelope * msg_envelope )
{
    if (dest_process_id==0)
        return ERROR_INVALID_PID;
        
    PCB *target_PCB;
    //int n = NUM_OF_IPROC;

    int i;
    
    for (i=0;i<NUM_OF_IPROC;i++) //use dest_id to look up the target process PCB pointer
    {
        if (dest_process_id == pcb_pointer_tracker[i]->process_id)
            target_PCB = pcb_pointer_tracker[i];
    }
    
	msg_queue *temp_msg_q;
    temp_msg_q= &target_PCB->msg_envelope_q;
    
	//enqueue envelope onto the message queue of the target process
    msg_envelope->sender_pid = current_process->process_id;
    msg_envelope->receiver_pid = dest_process_id;
    msg_enqueue(msg_envelope, temp_msg_q);
	fflush(stdout);
	printf("check target pcb queue size: %i\n", target_PCB->msg_envelope_q.size); 
	
	if (target_PCB->msg_envelope_q.head==NULL) {
		printf("send failed: msg_envelope_q is null\n");
	}
	fflush(stdout);        
    
    if(target_PCB->process_state == BLOCKED_ON_RECEIVE)
    {
		target_PCB->process_state = READY;
		rpq_enqueue(target_PCB);//enqueue blocked process to ready queue;
    }
    
    return 0;
}

msg_envelope * k_receive_message()
{	
    while ( current_process->msg_envelope_q.size == 0)
	{			
		return NULL;
	}
    msg_queue *temp_queue;
    temp_queue = &current_process->msg_envelope_q;
    msg_envelope *temp_envelope = (msg_envelope *) msg_dequeue(temp_queue);
	fflush(stdout);
    printf("k_recieve_message: Message received from sender #:");
    printf("%i",temp_envelope->sender_pid);
	fflush(stdout);
    printf("k_recieve_message: to receiver #:");
    printf("%i\n",current_process->process_id);
	printf("k_message_receive: returning message");
	//store the details of this receive transaction on the receive_trace_buffer
	printf("queue dequue: msg env: %d\n", temp_envelope->msg_size);
	printf("msg env envelope sender: %i\n", temp_envelope->sender_pid);
	printf("msg env envelope dest: %i\n", temp_envelope->receiver_pid);
	return temp_envelope;
}

int k_terminate() // GLOBAL VARIABLE FOR: child process id, file id
{
    int retCode=0;
    int i;    
    for (i=0;i<2;i++)
    {
		fflush(stdout);
		printf("childpid: %i\n", childpid[i]);
		fflush(stdout);
        kill(childpid[i],SIGINT);
    }
    // terminate child process(es)    
    int statusfn1,statusfn2,statusfid,status1,status2;
    status1 = munmap(in_mem_ptr, 256); 
    status2 = munmap(out_mem_ptr, 256);
    
    if (status1==-1 || status2==-1) // remove shared memory segment and do some standard error checks
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

    status1 = unlink("in_buf");
    status2 = unlink("out_buf");

    if (status1==-1 || status2==-1)
    // remove shared memory segment and do some standard error checks
    {
        fflush(stdout);
        printf("BAD MMAP UNLINKP\n");
        retCode = ERROR_BAD_FILE_UNLINK;
    }
 
    printf("Deallocating data structures\n");   
	printf("Free free envelope queue\n");
    free(free_env_Q);
    
	printf("Free blocked on resource queue\n");
    free(blocked_on_resource_Q);
    
    printf("Free sorted time list\n");
    free(sorted_timeout_lst);
    
    printf("Free RPQ\n");
    free(rpq);     
    
    printf("Free created PCBs (pcb_pointer_tracker\n");    
    for (i=0; i < NUM_TOTAL_PROC; i++) {//*******8 for full implementation!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        free(pcb_pointer_tracker[i]);
	printf("%i\n",i);
    }     

    kill(getpid(), SIGINT); 
    exit(0);	
    return retCode;
}

int k_send_console_chars(msg_envelope *message_envelope)
{
	int retCode = 0;    
	fflush(stdout);
	printf("Sending message from process P to CRT\n");  
	if (message_envelope != NULL)
    {		
		retCode = k_send_message(IPROC_CRT, message_envelope);
	}
	else 
		retCode = ERROR_INVALID_MID;
		
	return retCode;
}

int k_get_console_chars(msg_envelope *message_envelope )
{
    int retCode = 0;    
    
    if (message_envelope != NULL)
    {
		printf("queue dequue: msg env: %d\n", message_envelope->msg_size);
		retCode = k_send_message(IPROC_KBD, message_envelope);
		fflush(stdout);
		printf("get console chars was here\n");		
    }
	else
		retCode = ERROR_INVALID_MID;
	
    return retCode;
}

int k_request_delay( int time_delay, int wakeup_code, msg_envelope * message_envelope )
{
	if (time_delay && wakeup_code && message_envelope)
    {
		message_envelope->n_clock_ticks = time_delay;
        // popoulate num of clock ticks desired inside message
		message_envelope->msg_type = wakeup_code;
        // write message with code timing service will reply with after delay is over
		int retCode = k_send_message(current_process->process_id, message_envelope);
		
        return retCode;
	}
}

int k_release_msg_env(msg_envelope * rel_msg) {
    rel_msg->sender_pid = -1;
    rel_msg->receiver_pid = -1; 
    rel_msg->msg_type = -1; 
    rel_msg->n_clock_ticks = -1; 

    if (rel_msg->msg_size != 0) {
        rel_msg->msg_size = 0;
    }
    
    int rel_error; 
    rel_error = msg_enqueue(rel_msg, free_env_Q);
     if (blocked_on_resource_Q->head != NULL) {
        blocked_on_resource_Q->head->process_state = READY;
        PCB *temp;
        temp = blocked_on_resource_Q_dequeue(); 
        
        rel_error = rpq_enqueue(temp);
    }
    
    return 0; //should check for error
}

msg_envelope * k_request_msg_env()
{
	while (free_env_Q ==NULL)
    {
		current_process->process_state = BLOCKED_ON_RECEIVE;
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
    {
        return ERROR_INVALID_PID;
    }
    
    else if (target_process_id==0)
    {
        return ERROR_INVALID_PRIORITY;
    }
    else
    {        
        int i;
        for (i = 0;i < NUM_OF_USER_PROC;i++)
        {
            if (target_process_id == pcb_pointer_tracker[i]->process_id)
            {
                temp = pcb_pointer_tracker[i];
            }
        }
	
     temp->priority = new_priority;
     retCode = 0;
		
        //invoke scheduler so that process is enqueued onto appropriate queue		
    }
	return retCode;
}
