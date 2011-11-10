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
	//now enter infinite loop
	while (1) {
		get_console_chars (env); //keyboard input
		env = (msg_envelope *) receive_message();
		while (env == NULL) {
			usleep(tWait);
			env = (msg_envelope *) receive_message();
		}
		//do we have to check if the message type is output envelope?
		send_console_chars(env); //CRT output, wait for ack
		env = (msg_envelope *) receive_message();
		while (env == NULL) {
			usleep (tWait);
			env = (msg_envelope *) receive_message();
		}
		//Yifei Addition:
		release_msg_env(env);
	}

}
