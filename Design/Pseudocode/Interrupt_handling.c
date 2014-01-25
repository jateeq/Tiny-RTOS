//NOTES
- KB process and CRT process need to be forked during initialization
- State field in PCB of kb, crt and timer process = 'i_process' permanently.

//constants
//the numbers can be changed based on other constants!
SIGALARM 20 //clock signal
SIGINT 21 // used for CNTRL C on CCI
SIGUSR1 22 // CRT signal
SIGUSR2 23 // KB signal

//Data structures required:
'in_mem_p' (Input shared memory) : memory-mapped file for keyboard input
'out_mem_p' (Output shared memory) : memory-mapped file for CRT output
inputbuf and outpuf data structures, to hold contents to be received and transmitted to UART buffers (see KB and CRT handler processes)

Design criteria: 
-file size?
-buffer size?
-need flag to inform appropriate processes about status of buffer

initialize() //the code in this is part of the main initialize primitive, this only defines the portion relevant to signals
{
	...
	sigset(SIGALARM, tick_handler);
	sigset(SIGINT, die());
	sigset(SIGUSR1, crt_handler());
	sigset(SIGUSR2, kb_handler());
	
	fork the KB process
	if the PID of the the process is 0 
	{
		launch_kb_process();
	}	
	//if control reaches here, parent process code is running	
	fork the CRT process
	if the PID of the process is 0
	{
		launch_crt_process();
	}
	//parent process code continues here
	...
}

// This funciton is given in the project
// Requires '<signal.h>'
void atomic (bool on) //'on' is be defined as 0 for true
{
	static sigset_t oldmas;
	sigset_t newmask;
	if (on)
	{
		sigemptyset(&newmask); //initialize the signal set pointed to by set, such that signals are excluded
		sigaddset(&newmask, SIGALARM); //the alarm signal
		sigaddset(&newmask, SIGINT); //the CNTRL-C
		sigaddset(&newmask, SIGUSR1); //the CRT signal
		sigaddset*&newmask, SIGUSR2); //the KB signal
		sigprocmask(SIG_BLOCK, &newmask, &oldmask);
	}
	else
	{
		//unblock the signals
		sigprocmask(SIG_SETMASK, &oldmask, NULL);
	}
}

//routine to clean up things before terminating main program
/* this stuff must be cleaned up or we have child processes and shared
/memory hanging around after the main process terminates*/

void cleanup()
{
	kill(in_pid, SIGINT); // termiante child proceses
	status = munmap(mmap_ptr, bufsize); //remove shared memory segement and do some standard error checks
	if (status == -1)
	{
		printf ("Bad munmap during cleanup\n");		
	}
	stats = close(fid_; // close teh temporary mmap file
	if (status == -1)
	{
		printf("Bad close of temporary mmap file during cleanup'n");
	}
	status = unlink(sfilename); // unlink (delte) the mmap file
	if (status == -1)
	{ 
		print f("Band unlin during cleanup.\n");
	}
}

//routine to call before exiting
//this routine is called when SIGINT signal is received
void die(int signal)
{
	cleanup();
	printf("\n\n Signal Received. Leving demo..\n");
	exit(0);
}

//this i_process runs when 
//what is signum required for?
void kb_handler()
{
	MsgEnv * msg = receive_message(); // ensure iproc does not block	
	if (msg != NULL)
		enqueue the message onto local message queue

	MsgEnv * current_msg = first message in queue	
	inputbuf command; //the command that was entered by user at kb
	
	if (message queue is not empty)
	{
		if (in_mem_p->indata[0] != '\0') //if shared memory is not empty
		{
			strcpy (command.indata, in_mem_p->indata);  //copy buffer contents		
			in_mem_p->input_count = 0; //set number of chars to read to 0, so receiver knows micro is done reading
			current_msg->msg_text = command.indata;		
			store current pid in msg, and insert KB_HANDLER _ID into msg as source id
			return the message to the invoking process
		}
	}
	else 
		set process state to idle
}

void crt_handler()
{		
	//check for any new messages
	MsgEnv * msg = receive_message(); //ensure the service does not block i_process
	if (msg != NULL)	
		enqueue the message onto the local message queue		
	
	if (there are still messages on queue) //there are still messages waiting for service
	{	
		if (out_mem_p.flag == 1)
		{
			MsgEnv * current_msg = dequeue the current message from local msg queue
			strcpy(out_mem_p.output_data, current_msg->msg_text);			
			out_mem_p.output_count = sizeof(current_msg->msg_text);		
			store current pid in msg, and insert CRT_HANDLER _ID into msg as source id
			return envelope to invoking proc as acknowledgement once message has been displayed
		}
	}	
	else
		set state variable to idle	
}

void tick_handler()
{
	timeout_iprocess();
}

//given in code - MODIFY according to our constants and primitives!
//this is launched everytime a signal is intercepted by the RTX
void exception handler(int signum)
{
	//pre-handling code
	pcb* temp;
	pcb* savePCB = RTX.current_process;
	
	switch(signum)
	{
		case SIGINT: 
			die()
			break;
		case SIGALARM:
			tick_handler(signum);
			break;
		case SIGUSR1: // CRT handler
			crt_handler(signum);
			break;
		case SIGUSR2: // KB handler
			kb_handler(signum)
			break;
	}	
	RTX.current_process = savePCB;
	//post handling code
	
	return;
}

void timeout_iprocess()
{
	static int kernel_counter = 0; 
	kernel_counter ++; //increment counter everytime process is launched
	MsgEnv * msg = k_receive_message(); //receive pending messages but do not block
	
	while(env is not null)
	{
		sort_list(msg);		
		msg = k_receive_message(); //see if any more msgs left
	}
	if (timeout_list is not empty)
	{		
		decrement the tick count of 1st message in the timeout_list
		while(timeout_list.head->n_clock_ticks is zero)
		{
			dequeue the first timeout request
			save it's current source id
			set TSERVICE_ID as source id
			k_send_message(envelope,destination id); //return envelope
		}
	}	
	
	void sort_list(Msg *msg)
	{
		int delay_time = msg->n_clock_ticks;
		int temp = timeout_list.head->n_clock_ticks;
		if delay_time is greater than temp
		{
			subtract delay_time and temp
			store result in msg->n_clock_ticks
			enqueue the new msg in the correct pos in list
		}
		else
			enqueue the new msg as head of list
	}
	
}


