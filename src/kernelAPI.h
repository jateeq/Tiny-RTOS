#ifndef KERNELAPI_H
#define	KERNELAPI_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "rtx.h"    
    
int k_change_priority(int new_priority, int target_process_id);
int k_terminate(); 
int get_console_chars(msg_envelope *message_envelope );
msg_envelope *k_receive_message();
int k_request_delay ( int time_delay, int wakeup_code, msg_envelope * msg_envelope );
msg_envelope * k_request_msg_env(); 
int k_send_console_chars(msg_envelope *env);
int k_send_message ( int dest_process_id, msg_envelope * msg_envelope );
int k_release_msg_env(msg_envelope * msg);
int k_release_processor();
int k_request_process_status(msg_envelope* env);

#ifdef	__cplusplus
}
#endif

#endif	/* KERNELAPI_H */
