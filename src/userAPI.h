#ifndef KERNELAPI_H
#define	KERNELAPI_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "rtx.h"
   
int change_priority(int new_priority, int target_process_id);
int terminate(); 
int get_console_chars(msg_envelope *message_envelope );
msg_envelope * receive_message();
int request_delay ( int time_delay, int wakeup_code, msg_envelope * message_envelope );
msg_envelope * request_msg_env(); 
int send_console_chars(msg_envelope *env);
int send_message ( int dest_process_id, msg_envelope * message_envelope );
int release_msg_env(msg_envelope * msg);
int release_processor();
int request_process_status(msg_envelope* env);
int get_trace_buffers (msg_envelope* msg);
int change_priority(int new_priority, int target_process_id);
void atomic(int on);
void process_switch();
void context_swtich(jmp_buf *previous, jmp_buf *next);

#ifdef	__cplusplus
}
#endif

#endif	/* KERNELAPI_H */
