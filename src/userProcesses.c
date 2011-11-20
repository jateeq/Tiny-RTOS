#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userProcesses.h"
#include "userAPI.h"
#include "rtx.h"
#define MAX_COMMAND_DATA_SIZE 8

void process_CCI() {

	fflush(stdout);
	printf("hello CCI invoked"); 	
	fflush(stdout);
	msg_envelope * msg;
	int error;
	char std_array[5];
	std_array[0] = 'C'; std_array[1] = 'C'; std_array[2] = 'I'; std_array[3] = ':'; std_array[4] = ' ';
	char ps_array[5];

	int invalid; 
	while (1) {
		if (current_process->msg_envelope_q.size == 0) {
			if (msg == NULL) {
				msg = request_msg_env();
			}	
			error = get_console_chars(msg);
		}
		msg = receive_message();
		
		if (msg->msg_type == CONSOLE_INPUT) {

			error = 0;
			invalid = 0;
			int i;

			//check if the beginning of the message starts with "CCI:_"
			for (i=0; i<5; i++) {
				if(msg->msg_text[i] != std_array[i]) {
					invalid = 1; //invalid input
				}
				else {
					invalid = 1;
				}
			}
			if (invalid == 0) {// passes previous test

				//send message to process A
				if (msg->msg_text[5] == 's' && msg->msg_size == 6) {
					error = send_message(PROC_A, msg);
				}

				//display process status
				else if (msg->msg_text[5] == 'p' && msg->msg_text[6] == 's' && msg->msg_size == 7) {
					error = request_process_status(msg);
				}
		
				//Wall clock set time
				//********ATTN: not sure how atoi works******
				/*else if (atoi(msg->msg_text[5])<=9 && atoi(msg->msg_text[6])<=9 && msg_msg_text[7] == ':' && atoi(msg->msg_text[8])<=9 && atoi(msg->msg_text[9])<=9 && msg->msg_text[10]==':' && atoi(msg->msg_text[11])<=9 && atoi(msg->msg_text[12])<=9) {
					msg->msg_size = 6;
					msg->msg_text[0] = msg->msg_text[5];
					msg->msg_text[1] = msg->msg_text[6];
					msg->msg_text[2] = msg->msg_text[8];
					msg->msg_text[3] = msg->msg_text[9];
					msg->msg_text[4] = msg->msg_text[11];
					msg->msg_text[5] = msg->msg_text[12];
					msg->msg_type = CHANGE_CLOCK;
					error = send_message(PROC_CLK, msg);
				}
				*/
				//Turn on wall clock
				else if (msg->msg_text[5]=='c' && msg->msg_text[6]=='d' && msg->msg_size==7) {
					msg->msg_type = SHOW_CLOCK; 
					error = send_message(PROC_CLK, msg);
				}
				//Turn off wall clock
				else if (msg->msg_text[5]=='c' && msg->msg_text[6]=='t' && msg->msg_size==7) {
					msg->msg_type = STOP_CLOCK;
					error = send_message(PROC_CLK, msg); 
				}

				//get trace buffers
				else if (msg->msg_text[5] =='b' && msg->msg_size==6) {
					error = get_trace_buffers(msg);		
				}
				
				//terminate
				else if (msg->msg_text[5]=='t' && msg->msg_size==6) {
					error = terminate();
				}
				//change priority
				//********ATTN: not sure how atoi works******
				/*
				else if (msg->msg_text[5]=='n' && msg->text[6]==' ' && atoi(msg->msg_text[7])<=9 && msg->text[8]==' ' && atoi(msg->msg_text[9])<=9) {
					if (atoi(msg->msg_text[9]) == PROC_NULL)
						//do something to handle this case. For now, just printf
						fflush(stdout);
						printf("cannnot change null process priority");
					}
					else if (atoi(msg->msg_text[7]) > 3 || atoi(msg->msg_text[9]) > NUM_OF_PROCESSES || atoi(msg->msg_text[7]) < 0) {
						//do something to handle this case. Fllor now, just printf
						fflush(stdout);
						printf("cannnot change null process priority");
					}
					else {
						error = change_priority( atoi(msg->msg_text[7]), atoi(msg->msg_text[9]) );
					}
				}
				*/
				
				
				//default case: if all above cases fall through
				else {
					invalid = 1; 
				}

			}
		}
		if (invalid == 1) {
			fflush(stdout);
			printf("CCI receives message not for CONSOLE_INPUT");
		}
		// we should get rid of the envelope in possession
		if (msg != NULL) {
			error = release_msg_env(msg);
		}
	}
}				


void process_A()
{
	msg_envelope* temp;
	while (1) {
		temp = receive_message();
		if (temp != NULL){
			fflush(stdout);
			printf("MESSAGE RECEIVED YES!");
		}	
	}

    temp = receive_message();
    release_msg_env(temp); //deallocate the received message envelope
    int i=0;
    
    while(1)
    {
        msg_envelope* env=request_msg_env();
        env->msg_type=COUNT_REPORT; //set the message_type field to "count_report"
        env->msg_text[1]=(char)i;//set the msg_data[1] field to num
        send_message(PROC_B,env); //send the envelope to process B
        i++;
        release_processor();
    }
    
    return;


}

void process_B()
{

    while(1)
    {
        msg_envelope* env = receive_message(); //receive a message
        send_message(PROC_C,env); //send the message to process C
    }
    
    return;

}

void process_C()
{   
    PCB* pcb;
    msg_envelope* env;
    int i;
    
    for (i = 0;i<NUM_OF_USER_PROC;i++)
    {
        if (pcb_pointer_tracker[i]->process_id==PROC_C)
        {
            pcb = pcb_pointer_tracker[i];
        }
    }//perform any needed initialization
    
    while(1)
    {        
	if (pcb->msg_envelope_q.head==NULL)
	{
            env=receive_message(); //receive a message
        }
	else
	{
            env=(msg_envelope *) msg_dequeue(pcb->msg_envelope_q); //dequeue the first message from the local message queue
        }
    
        if (env->msg_type==COUNT_REPORT)
        {
            if (((int)env->msg_text[1])%20==0) //msg_data[1] is evenly divisible by 20
            {
		char display[8]={'P','r','o','c','e','s','s','C'}; //send "ProcessC" to display using msg envelope
		for (i=0;i<8;i++)
		{
		    env->msg_text[i+2]=display[i];
		    printf("&c",env->msg_text[i+2]);
		}
		printf("\n");

		int t = 500000; //wait for display_ack
		send_console_chars(env);
		env=receive_message();
		while (env==NULL)
		{
		    usleep(t);
		    env=receive_message();
		}
		
		request_delay(1000,PROC_C_WAKEUP_CODE,env); //request a 10 second delay with wakeup_code set to "PROC_C_WAKEUP_CODE"
                
		while (env->msg_type!=PROC_C_WAKEUP_CODE)
		{
		    env=receive_message(); //receive a message (block and let other processes execute)
		    msg_enqueue(env,pcb->msg_envelope_q); //put message on the local message queue for later processing		
		}
            }
        }
        
        release_msg_env(env); //deallocate message envelope
        release_processor();
    }
        
    return;
}

void process_NULL() 
{
    while(1)
        release_processor();
    
    return;
}


void wall_clock() {

	//this section is only going to run once during the lifetime of the rtx
    int time = 0;
	int clock_status = OFF;	
	int awake = 1;
	int time_set = 0; //determines whether the user set the time or not - used while displaying the time
	int command;
	int temp; //stores return values from user APIs	
	char command_data[MAX_COMMAND_DATA_SIZE];
	msg_envelope * msg = NULL;
	msg_envelope * crt_msg = NULL;	
	
	//this section should run forever once all variables have been initialized
	do {			 
		msg = receive_message();
		
		//expecting messages only from the CCI at this point
		
		//parse the message text to get the command and other data out
		
		if (msg != NULL)
		{
			int i = 0;
			
			//get the command entered by user
			command = msg->msg_type;			
			
			switch(command)
			{
				case 4: 	//set wall clock time
					command_data[0] = msg->msg_text[0];
					command_data[1] = msg->msg_text[1];
					command_data[2] = ' ';
					command_data[3] = msg->msg_text[2];
					command_data[4] = msg->msg_text[3];
					command_data[5] = ' ';
					command_data[6] = msg->msg_text[4];
					command_data[7] = msg->msg_text[5];	
					
					time_set = 1;
					break;
				case 5: 	//turn wall clock display off
					clock_status =  OFF;
					break;
				case 6: 	//turn wall clock display on					
					clock_status = ON;
					break;					
			}
		}
		
		//display wall clock
		if (clock_status == ON)
		{
			if (time_set == 1) //if the user set the time then a message with the time should be available			
				temp = send_console_chars(msg); //send to the crt the message with the new user-defined time			
			else
			{
				while(crt_msg == NULL)
					crt_msg = request_msg_env(); //request envelope to send message to crt
				temp = send_console_chars(crt_msg); //send to the crt the updated time
			}
			
			if (temp == SUCCESS)
			{
				msg = NULL;			
				while(msg==NULL) //need to change this so that only message from iproc_crt are accepted				
					msg = receive_message(); //wait for a confirmation that the message was sent	
				//what if confirmation doesn't come?
			}
		}
					
		//request a message envelope to send message to timing service
		//what if it takes a very long time - approx 1s - to get a message? our clock will not be up to date
		
		msg = NULL;
		
		while(msg == NULL)
			msg = request_msg_env();
		
		msg->next = NULL;
		msg->previous = NULL;
		msg->sender_pid = PROC_CLK;
		msg->receiver_pid = IPROC_TIMER;
		
		//need to change the time delay according to implementation in the timing service
		int code = request_delay(10, WALL_CLK_WAKEUPCODE, msg);
		
		if (code == FAIL)
			printf("WALL CLOCK: could not request delay");
		
		awake = 0;
		int sec = 0;
		while(!awake)
		{
			msg = receive_message();
			if (msg != NULL && msg->msg_type!= WALL_CLK_WAKEUPCODE)		//make sure the message received is from the timer process	
				release_processor();										
			else if(msg->sender_pid == IPROC_TIMER) //make sure you awake only if message is from timer iproc
				awake = 1;			
		}
		
		/*		
		//since it is critical that the wall clock have a message envelope on time, don't release the envelope
		//requested and keep it reserved*/
		
	}while(awake); //at this line of code the process is always awake...so it always runs
	
	
}
/*
void processP()
{
	//get msg envelope and send message to wall clock process to simulate CCI
	msg_envelope * msg = NULL;
    
	while(msg == NULL)
	{
		msg = (msg_envelope *) request_msg_env();
	}
	
	//msg->msg_type = CHANGE_CLOCK;
	msg->msg_type = SHOW_CLOCK;	
	
	msg->msg_text[0] = '1';
	msg->msg_text[1] = '2';
	msg->msg_text[2] = '1';
	msg->msg_text[3] = '2';	
	msg->msg_text[4] = '3';
	msg->msg_text[5] = '4';
	
	msg->sender_pid = PROC_P;
	msg->receiver_pid = PROC_CLK;
	
	send_message(PROC_CLK, msg);
	
    current_process = pcb_pointer_tracker[1];
	wall_clock();
	/*const int tWait = 500000; //rcv loop wait time in usec, approx value
        msg_envelope * env;
        env = (msg_envelope *) request_msg_env(); //or just malloc memory for one envelope
	if (env == NULL) {
		fflush(stdout);
		printf("\nenvelope allocation failed\n");
	}
	//now enter infinite loop
	while (1) {
		printf("P: Send message to kbd..\n");
		get_console_chars (env); //keyboard input
		fflush(stdout);
		printf("P: waiting for message from kb..\n");		
		env = (msg_envelope *) receive_message();//change this to userAPI call later

		while (env == NULL) {
			usleep(tWait);					
			env = (msg_envelope *) receive_message();
			if (env != NULL) {
			   fflush(stdout);
			   printf("Message received from the KBD\n");		
			}
		}
		
		// for testing only
        int i = 0;

		fflush(stdout);
		printf("\nthe message from keyboard was: "); 
                for (i=0; i<sizeof(env->msg_text); i++)
                {
                    printf("%c", env->msg_text[i]); 
                }
		fflush(stdout);
		//do we have to check if the message type is output envelope?
		printf("\nNow sending message to CRT iproc\n");
		send_console_chars(env); //CRT output, wait for ack
		env = (msg_envelope *) receive_message();
		if (env == NULL) {
		   printf("Waiting for acknowledgement message from CRT\n");
		}
		while (env == NULL) {
			usleep (tWait);
			env = (msg_envelope *) receive_message();

		}		
		release_msg_env(env);
	}
}*/
