/* 
 * File:   queue.h
 * Author: Jake
 *
 * Created on November 4, 2011, 12:55 PM
 */

#ifndef QUEUE_H
#define	QUEUE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "rtx.h"

int rpq_enqueue(PCB *PCB_to_enqueue);
PCB *rpq_dequeue();
int msg_enqueue(msg_envelope *msg_env, msg_queue *queue);
msg_envelope *msg_dequeue(msg_queue *queue);
int blocked_on_resource_Q_enqueue(PCB *PCB_to_enqueue);
PCB *blocked_on_resource_Q_dequeue();
PCB *remove_PCB_from_rpq(int target_pid);

#ifdef	__cplusplus
}
#endif

#endif	/* QUEUE_H */

