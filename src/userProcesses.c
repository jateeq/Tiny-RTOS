#include <stdio.h>
#include <stdlib.h>

#include "userProcesses.h"
#include "userAPI.h"
#include "rtx.h"

void process_A()
{
    msg_envelope* temp=receive_message();
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
        msg_envelope* env=receive_message(); //receive a message
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
            env=msg_dequeue(pcb->msg_envelope_q); //dequeue the first message from the local message queue
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
		
		request_delay(1000,WAKEUP10,env); //request a 10 second delay with wakeup_code set to "wakeup10"
                
		while (env->msg_type!=WAKEUP10)
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

void process_CCI() {
    
}

void process_NULL() 
{
    while(1) 
        release_processor();
    
    return;
}
/*
void wall_clock() {

	//this section is only going to run once during the lifetime of the rtx
    int time = 0;
	int clock_status = OFF;
	int count = 0;	
	int awake = 1;
	char* command;
	char* command_data;
	
	//this section should run forever once all variables have been initialized
	do {	
		//message_envelope * msg = malloc(sizeof(message_envelope));
		//msg = receive_message();
		
		//expecting messages only from the CCI at this point
		
		//parse the message text to get the command and other data out
		
		if (msg != NULL)
		{
			int i = 0;
			
			//get the command entered by user
			for(i;i<3;i++)
				command = msg->text[i];
			
			//get the time specified by the user
			if (command == " c")
			{
				for(i;i<5;i++)
					command_data = msg->msg_text[i];
					
				//parse the data to extract hours, minutes and seconds
			}
			
			if ()
			switch(msg->msg_text)
			{
				case 'c': 	//set wall clock time
					time = command_data;
					break;
				case 'cd': 	//turn wall clock display on
					clock_status = ON;
					break;
				case 'ct': 	//turn wall clock display off
					clock_status =  OFF;
					break;
				default:
					//printf("WALL_CLOCK: unrecognized message text"); fflush(stdout);
					break;
			}
		}
		
		if (clock_status == ON)
			//display wall clock
		
		//request a message envelope to send message to timing service
		//what if it takes a very long time - approx 1s - to get a message? our clock will not be up to date
		msg = NULL;
		while(msg == NULL)
			msg = request_msg_env();

		//need to change the time delay according to implementation in the timing service
		request_delay(1000ms, WALL_CLK_WAKEUPCODE, msg);
		awake = 0;
		
		while(!awake)
			release_processor();
		
		//if we reach this point the process is awake again and one second has passed
		awake = 1;
		
		//since it is critical that the wall clock have a message envelope on time, don't release the envelope
		//requested and keep it reserved
		
	}while(awake); //at this line of code the process is always awake...so it always runs
	
	
}
*/
void processP()
{
	const int tWait = 500000; //rcv loop wait time in usec, approx value
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
		
		/* for testing only*/
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

}
