#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userProcesses.h"
#include "userAPI.h"
#include "rtx.h"

#define MAX_COMMAND_DATA_SIZE 8

void process_A() {
    
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

void process_C() {
    
}

void process_CCI() {
    
}

void process_NULL() 
{
    //while(1) 
        //release_processor();
    
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
					msg = message_receive(); //wait for a confirmation that the message was sent	
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
			if (msg != NULL && msg->type!= WALL_CLK_WAKEUPCODE)		//make sure the message received is from the timer process	
				release_processor();										
			else if(msg->sender_pid == IPROC_TIMER) //make sure you awake only if message is from timer iproc
				awake = 1;			
		}
		
		/*		
		//since it is critical that the wall clock have a message envelope on time, don't release the envelope
		//requested and keep it reserved*/
		
	}while(awake); //at this line of code the process is always awake...so it always runs
	
	
}

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
		printf("P: Sending timeout request to timer iprocess\n");
		env->sender_pid = current_process->process_id;
		env->receiver_pid = IPROC_TIMER;
		k_request_delay(10, WALL_CLK_WAKEUPCODE, env);
		printf("Receiving envelope from timer iprocess\n");
		msg_envelope *temp_ptr;
		while (temp_ptr == NULL) {
			//usleep (tWait);
			temp_ptr = (msg_envelope *) receive_message();
		}
		printf("Recieved envelope from timer_iprocess:%i\n",env->sender_pid);
		/*printf("P: Send message to kbd..\n");
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
>>>>>>> Stashed changes

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
		   printf("Waiting for acknowledgment message from CRT\n");
		}

		while (env == NULL) {
			usleep (tWait);
			env = (msg_envelope *) receive_message();

		}		
		release_msg_env(env);
	}*/

}
