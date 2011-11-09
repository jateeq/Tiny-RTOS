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
    
    printf("Initializing keyboard process...");
    init_keyboard_process();
    
 //   if (retCode == 0) {
        
 //   }
    
     printf("Initializing CRT process...");
     init_crt_process();
    
////    if (retCode == 0 ) {
 //       
 //   }
    /**/
    
    //Setting up clocktick signals

     current_process = rpq_dequeue();
             if (current_process == NULL)
             {
                 printf ("Critical error in dequing from RPQ.");
             }
             ualarm(10000,10000);
             longjmp(*(current_process->context),1);



    ualarm(10000, 10000);
    
    return (EXIT_SUCCESS);
}

