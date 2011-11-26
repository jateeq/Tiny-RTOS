#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userProcesses.h"
#include "userAPI.h"
#include "rtx.h"
#define MAX_COMMAND_DATA_SIZE 8
#define TIME_SIZE 8

void process_CCI() {

	printf("CCI invoked\n"); 	
	fflush(stdout);
	msg_envelope * msg;
	int error;
	char std_array[5];
	std_array[0] = 'C'; std_array[1] = 'C'; std_array[2] = 'I'; std_array[3] = ':'; std_array[4] = ' ';
	char ps_array[5];
	int i;//used for for loop count
	int invalid; 
	while (1) {
		printf("CCI starting loop\n");
		fflush(stdout);
		if (current_process->msg_envelope_q.size == 0) {
			msg = request_msg_env();
			error = get_console_chars(msg);
		}
		msg = receive_message();

		if (msg == NULL) {
		   printf("process_CCI: Received a NULL message!!");
		}
		printf("CCI: msg_type: %i\n", msg->msg_type);
		fflush(stdout);
		if (msg->msg_type == CONSOLE_INPUT) {
									
			printf("process_CCI: Message from keyboard has been received. Message type: %i Message content: %s\n", msg->msg_type, msg->msg_text);
			fflush(stdout);
			error = 0;
			invalid = 0;
			int i;

			//check if the beginning of the message starts with "CCI:_"
			/*for (i=0; i<5; i++) {
				if(msg->msg_text[i] != std_array[i]) {
					invalid = 1; //invalid input
				}
				else {
					invalid = 1;
				}
			}*/
			if (invalid == 0) {// passes previous test

				//send message to process A
				if (msg->msg_text[0] == 's') {
					printf("CCI sends message to PROC_A\n"); 
					fflush(stdout); 
					error = send_message(PROC_A, msg);
					//msg->msg_text[0] = 'Z'; msg->msg_text[1] = 'Z'; msg->msg_text[2] = 'Z'; msg->msg_text[3] = 'Z'; 
					//msg->msg_size = 4; 
					//msg->msg_type = OUTPUT_REQUEST; 
					//error = send_console_chars(msg);
				}

				//display process status
				else if (msg->msg_text[0] == 'p' && msg->msg_text[1] == 's') {
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
				}*/
				
				else if (msg->msg_text[0] == 'c' && msg->msg_text[1] == ' ')
				{
					msg->msg_type = CHANGE_CLOCK; 
					for (i =0;i<10; i++)
					{
						msg->msg_text[i] = msg->msg_text[i+2];
					}
					error = send_message(PROC_CLK, msg);
					/*msg->msg_text[0] = msg->msg_text[2];
					msg->msg_text[1] = msg->msg_text[3];
					msg->msg_text[2] = msg->msg_text[4];
					msg->msg_text[3] = msg->msg_text[5];
					msg->msg_text[4] = msg->msg_text[6];
					msg->msg_text[5] = msg->msg_text[7];
					msg->msg_text[6] = msg->msg_text[8];
					msg->msg_text[7] = msg->msg_text[9];*/
					//msg->msg_type = CHANGE_CLOCK;
					//error = send_message(PROC_CLK, msg);
				}
				//Turn on wall clock
				else if (msg->msg_text[0]=='c' && msg->msg_text[1]=='d') {
					msg->msg_type = SHOW_CLOCK; 
					error = send_message(PROC_CLK, msg);
				}
				//Turn off wall clock
				else if (msg->msg_text[0]=='c' && msg->msg_text[1]=='t') {
					msg->msg_type = STOP_CLOCK;
					error = send_message(PROC_CLK, msg); 
				}

				//get trace buffers
				else if (msg->msg_text[0] =='b') {
					error = get_trace_buffers(msg);		
				}
				
				//terminate
				else if (msg->msg_text[0] =='t') {
					error = terminate();
				}

				//change priority
				else if (msg->msg_text[0]=='n' && msg->msg_text[1]==' ' && msg->msg_text[3]==' ' && msg->msg_size == 5 ) {
					int priority;
					int pid;
					sscanf(&msg->msg_text[2], "%i", &priority);
					sscanf(&msg->msg_text[4], "%i", &pid);
					printf("CCI: Process %i is receiving new priority %i\n", pid, priority);
					error = change_priority( priority,pid);
				}
				
				//default case: if all above cases fall through
				else {
					invalid = 1; 
				}

			}
		}
		if (invalid == 1) {
			fflush(stdout);
			printf("CCI receives message not for CONSOLE_INPUT\n");
		}
		// we should get rid of the envelope in possession
		if (msg != NULL) {
			//error = release_msg_env(msg);
		}
	}
}				


void process_A()
{
	int error; 
	msg_envelope* temp = NULL;

	while (temp==NULL) {
		temp = receive_message();
	}	
    error = release_msg_env(temp); //deallocate the received message envelope

    int  num =0;
    while(1)
    {
        msg_envelope* env = request_msg_env();
        env->msg_type=COUNT_REPORT; //set the message_type field to "count_report"
        env->msg_text[0]=(char)num;//set the msg_data[1] field to num
        send_message(PROC_B,env); //send the envelope to process B
        num++;
        release_processor();
    }
    
    return;

}

void process_B()
{
	int error; 
	printf("process B started\n"); 
	fflush(stdout);

    while(1)
    {
		//test123();
        msg_envelope* env;
		env = receive_message();
        error = send_message(PROC_C,env);
    }
    return;
}

void process_C()
{   

	printf("process B started\n"); 
	fflush(stdout);
    PCB* pcb;
    msg_envelope* env;
	char temp[2]; 
	int repcount;
    int i;
	int error;
    
	pcb = current_process; 
    
    while(1)
    {        
		if (pcb->msg_envelope_q.head==NULL) {
			printf("proc c: msg env q size: %i\n" ,pcb->msg_envelope_q.size);
			fflush(stdout);
			env = receive_message(); //receive a message
		}
		else {
		   	env = (msg_envelope *) msg_dequeue(&current_process->msg_envelope_q); 
		}

		if (env->msg_type==COUNT_REPORT) {
			//double digit number takes up the first 2 spaces in msg_text
			temp[0] = env->msg_text[0]; temp[1] = env->msg_text[1]; 
			repcount = atoi(temp); 
			
			printf("Proc C: env msg text: %i\n", repcount); 
			fflush(stdout); 
		    if ((repcount%20)==0) {//msg_data[1] is evenly divisible by 20
				char display[9]={'P','r','o','c','e','s','s',' ','C'}; //send "ProcessC" to display using msg envelope
				env->msg_size = 0;
				for (i=0;i<10;i++) {
					env->msg_text[i]=display[i];
					env->msg_size++;
				}
				env->msg_type = OUTPUT_REQUEST;
				error = send_console_chars(env);

				while (1) {
					//test123();
					env = receive_message();
					if (env->msg_type == DISPLAY_ACK) {
						printf("Proc C: receives acknowledgment!\n");
						fflush(stdout);
						break; 
					}
					else {
						release_msg_env(env); 
					    //msg_queue *temp_msg_q;
    					//temp_msg_q= &current_process->msg_envelope_q;
						//msg_enqueue(env,temp_msg_q); //put message on the local message queue for later processing	
					}
					usleep(10000000); 
				} 
/*				request_delay(10000000,PROC_C_WAKEUP_CODE,env); //request a 10 second delay with wakeup_code set to "PROC_C_WAKEUP_CODE"

				while (1) {
					printf("proc c: i am stuck in the second loop\n"); 
					env = receive_message();
					if (env->msg_type == PROC_C_WAKEUP_CODE) {
						break; 
					}
					else {
						msg_enqueue(env,pcb->msg_envelope_q); //put message on the local message queue for later processing	
					}
				}
*/
			printf("Proc P: iteration successful\n"); 
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
    //char time[TIME_SIZE] = "00:00:00";
	int time = 0; //if you comment this the wall clock does not run in a loop...
	int nSeconds = 0;
	int nMinutes = 0;
	int nHours = 0;
	int clock_status = OFF;	
	int awake = 1;
	int time_set = 0; //determines whether the user set the time or not - used while displaying the time
	int command;
	int temp; //stores return values from user APIs	
	char command_data[MAX_COMMAND_DATA_SIZE];
	msg_envelope * msg = NULL;	
	msg_envelope * timer_msg;	
	while(timer_msg==NULL)		
		timer_msg = request_msg_env();		
	timer_msg->receiver_pid = IPROC_TIMER;
	timer_msg->sender_pid = PROC_CLK;
	request_delay(10, WALL_CLK_WAKEUPCODE, timer_msg);
	int i =0;
	int confirmation_sent;	
	int clock_status2;
	
	//this section should run forever once all variables have been initialized
	do {		
		msg = receive_message();		 
		//expecting messages only from the CCI at this pointu
		
		//parse the message text to get the command and other data out
		
		if (msg != NULL)
		{	
			//get the command entered by user
			
			if(msg->sender_pid == IPROC_TIMER)
			{				
				//increment your clock		
				printf("WALL CLOCK - incrementing time\n");fflush(stdout);
				//time ++;							
				nSeconds ++;
				if (nSeconds == 60)
				{
					nMinutes++;
					nSeconds = 0;
				}
				if (nMinutes == 60)
				{
					nHours++;
					nMinutes = 0;
				}
				
				if (nHours == 24 && nMinutes == 60 && nSeconds == 60)
				{
					printf("WALL CLOCK: The rtx has been running for a day. Resetting time to zero");
					nSeconds = nMinutes = nHours = 0;
				}
				
				printf("WALL CLOCK: should the clock be displayed? clock_status2=%i\n", clock_status2);fflush(stdout);
				
				//display wall clock
				//if (clock_status == ON)
				if (clock_status2 == ON)
				{			
					printf("WALL CLOCK: displaying time\n");
					//for(i =0;i<TIME_SIZE;i++)
					/*crt_msg->msg_text[0] = time[0];
					crt_msg->msg_text[1] = time[1];
					crt_msg->msg_text[2] = '';
					crt_msg->msg_text[3] = time[2];
					crt_msg->msg_text[4] = time[3];
					crt_msg->msg_text[5] = '';
					crt_msg->msg_text[6] = time[4];
					crt_msg->msg_text[7] = time[5];*/
										
					/*int nSeconds = time%(3600*60); 
					int nMinutes = time/60;
					int nHours = time/3600;
					if (nSeconds <10)
					{
						msg->msg_text[0] = '0';	
						//strcat(msg->msg_text, (char*)nSeconds);
						//sprintf(msg->text[1], "%i", nSeconds);
					}
					else
					{
						//sSeconds = (char)nSeconds
						//msg->text[0] = sSeconds[0];
						//msg->text[1] = sSeconds[1];*/
						//strcat(msg->msg_text, (char*)nSeconds);						
						sprintf(msg->msg_text, "%i:%i:%i", nHours, nMinutes, nSeconds);						
						strcat(msg->msg_text,"\n");
						msg->msg_size+=2;
						
					//}	/**/
					//strcat(msg->msg_text, '\0');
					//printf("WALL CLOCK: the time right now is : %i\n", time);fflush(stdout);
					//sprintf(msg->msg_text, "%s", sSeconds);		
					//msg->msg_text[0] = (char)time;
					msg->msg_type = OUTPUT_REQUEST;
					printf("WALL CLOCK: msg text is: %s", msg->msg_text);
					msg->msg_size = strlen(msg->msg_text);
					printf("WALL CLOCK: the message size is : %i\n", msg->msg_size);
					//msg->msg_size = 1;
					msg->sender_pid = PROC_CLK;
					msg->receiver_pid = IPROC_CRT;
					temp = send_console_chars(msg); //send to the crt the updated time					
				}
				
				//return the msg back to the timer as a request delay
				msg->receiver_pid = IPROC_TIMER;
				msg->sender_pid = PROC_CLK;
				request_delay(10, WALL_CLK_WAKEUPCODE, msg);
			}
			else if(msg->sender_pid == IPROC_CRT)
			{
				release_msg_env(msg);
			}
			else if(msg->sender_pid == PROC_CCI)
			{
				command = msg->msg_type;			
				
				switch(command)
				{
					case CHANGE_CLOCK: 	//set wall clock time					
						/*time[0] = (int) msg->msg_text[0];
						time[1] = (int) msg->msg_text[1];
						//time[2] = (int) msg->msg_text[2];
						time[3] = (int) msg->msg_text[3];
						time[4] = (int) msg->msg_text[4];
						//time[5] = (int) msg->msg_text[5];
						time[6] = (int) msg->msg_text[6];
						time[7] = (int) msg->msg_text[7];	*/
						printf("changing time");fflush(stdout);
						time_set = 1;					
						break;
					case STOP_CLOCK: 	//turn wall clock display off
						printf("turning off clock\n");fflush(stdout);
						//clock_status =  OFF;
						clock_status2 =  OFF;
						release_msg_env(msg);
						break;
					case SHOW_CLOCK: 	//turn wall clock display on	
						printf("wALL CLOCK: turning on clock\n");fflush(stdout);
						//clock_status = ON;
						clock_status2 = ON;
						break;					
				}
			}
		}
		
			
					
		//request a message envelope to send message to timing service
		//what if it takes a very long time - approx 1s - to get a message? our clock will not be up to date
		/*
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
			if (msg == NULL)		//make sure you have received the message
				release_processor();										
			else if(msg->sender_pid == IPROC_TIMER) //make sure you awake only if message is from timer iproc
				awake = 1;			
			else if(msg->msg_type!= WALL_CLK_WAKEUPCODE) //make sure the message received is from the timer process
				release_processor();
		}*/
		
		/*		
		//since it is critical that the wall clock have a message envelope on time, don't release the envelope
		//requested and keep it reserved*/
		
	}while(awake); //at this line of code the process is always awake...so it always runs
	
	
}
