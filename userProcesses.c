#include <stdio.h>
#include <stdlib.h>

#include "userProcesses.h"
#include "userAPI.h"

void process_A() {
    
}

void process_B() {
    
}

void process_C() {
    
}

void process_CCI() {
    
}

void process_NULL() {
    
}

void process_CLK() {
    
}

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
		get_console_chars (env); //keyboard input
		env = (msg_envelope *) k_receive_message();//change this to userAPI call later

		while (env == NULL) {
			//fflush(stdout);
			//printf("ProcessP going to sleep.\n");
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
		printf("\nthe output from procP was: "); 
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
		//Yifei Addition:
		release_msg_env(env);
	}

}
