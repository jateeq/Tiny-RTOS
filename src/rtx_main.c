/* 
 * File:   rtx_main.c
 * Author: Jake
 *
 * Created on November 5, 2011, 1:26 AM
 */

#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include <signal.h>
#include "init.h"
#include "kernelAPI.h"
#include "queue.h"
#include "userAPI.h"
#include "iproc.h"
#include "userProcesses.h"

/*
 * 
 */
int main(int argc, char** argv) {
        
    //Start handling signals
    sigset(SIGUSR1, signal_handler);
    sigset(SIGUSR2, signal_handler);
    sigset(SIGINT, signal_handler);
    sigset(SIGALRM, signal_handler);
    
    int retCode;
    
    /*Initialize*/
    printf("Initializing data...\n");
    initialize_data();

    printf("Initializing user and iprocess...\n");
    initialize_process();
    
    printf("Initializing keyboard process...\n");
    init_keyboard_process();

    printf("Initializing CRT process...\n");
   // init_crt_process();
    /* */
    //Setting up clocktick signals

    printf("Dequeue from rpq...\n");
    current_process = rpq_dequeue();
    printf("Current_process pid: %i", current_process->process_id);
    if (current_process == NULL) {
         printf ("Critical error in dequing from RPQ.");
    }

    ualarm(100000,100000);
    printf("Load the first process to CPU...\n");
    longjmp(*(current_process->context),1);
    while(1);
    return (EXIT_SUCCESS);
}

