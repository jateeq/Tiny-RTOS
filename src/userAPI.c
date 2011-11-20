/*Updated on November 05 2011*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtx.h"
#include <signal.h>
#include "kernelAPI.h"
#include "userAPI.h"

void atomic(int on) {
    static sigset_t oldmask;
    sigset_t newmask;
    if (on) {		
		sigemptyset(&newmask);
		sigaddset(&newmask, SIGALRM); //the alarm signal
		sigaddset(&newmask, SIGINT); // the CNTRL-C
		sigaddset(&newmask, SIGUSR1); // the KB signal
		sigaddset(&newmask, SIGUSR2); // the CRT signal
		sigprocmask(SIG_BLOCK, &newmask, &oldmask);			
    }  
	else 
	{		
		//unblock the signals
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
    }
}

int request_process_status(msg_envelope msg) {
	int retCode = 0;
	return retCode;
}

int get_trace_buffers (msg_envelope msg) {
	int retCode = 0;
	return retCode;
}

int terminate()
{
	int retCode = 0;	
	atomic(ON);	
	retCode = k_terminate();	
	atomic(OFF);	
	return retCode;
}

int send_message(int dest_process_id, msg_envelope *msg_envelope)
{
    int retCode = 0;
    atomic(ON);
    retCode = k_send_message(dest_process_id, msg_envelope);
    atomic(OFF);
    return retCode;
}

msg_envelope * receive_message()
{
    msg_envelope *temp;
    atomic(ON);
    msg_envelope * msgEnv = k_receive_message();
    atomic(OFF);
    return msgEnv;
}

void context_switch (jmp_buf *previous, jmp_buf *next)
{
    int return_code; 
    return_code = setjmp(*previous); // obtain return_code, save previous process context
    if (return_code == 0) // if setjmp is successful
    {
        longjmp(*next,1); // restore next process context
    }
}

void process_switch( )
{
    int error_code;
    //suspend 'current' process to release CPU
    PCB * next_process; //hold pointer to the next process pcb
    next_process = (PCB *) rpq_dequeue( ); //pointer points to the next highest priority ready process 

    context_switch( current_process->context, next_process->context ); //switch the context of 'current' process to 'next' process

    if (current_process->process_state==1) {
        current_process->process_state = 2;
        error_code = rpq_enqueue(current_process);
    }
    else if(current_process->process_state==3) {
        error_code = blocked_on_resource_Q_enqueue(current_process);
    }
    current_process = next_process;// set 'current_process' to point to next's PCB

}

int send_console_chars(msg_envelope *message_envelope)
{
    int retCode;
    atomic(ON);
    retCode=k_send_console_chars(message_envelope);
    atomic(OFF);
    
    return retCode;
}

int get_console_chars(msg_envelope *message_envelope )
{
    int retCode;
    atomic(ON);
    retCode=k_get_console_chars(message_envelope);
    atomic(OFF);
    
    return retCode;
}

int release_msg_env(msg_envelope *rel_msg)
{
    int retCode;
    atomic(ON);
    retCode=k_release_msg_env(rel_msg);
    atomic(OFF);
    
    return retCode;
}

msg_envelope *request_msg_env()
{
    msg_envelope *temp;
    atomic(ON);
    temp=k_request_msg_env();
    atomic(OFF);
    
    return temp;
}

int request_delay(int time_delay,int wakeup_code,msg_envelope* message_envelope)
{
    int retCode;
    atomic(ON);
    retCode=k_request_delay(time_delay,wakeup_code,message_envelope);
    atomic(OFF);

    return retCode;
}
