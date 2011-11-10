#include "queue.h"

int rpq_enqueue(PCB *PCB_to_enqueue) {
	if (PCB_to_enqueue == NULL) {
	    //WTF
            printf("Error the PCB to be enqueued is NULL!");
	} 

	int index = PCB_to_enqueue->priority - 1; 

	if (rpq->pq_array[index]->size == 0) {
		PCB_to_enqueue->next = NULL; //?	
		PCB_to_enqueue->previous = NULL;
		rpq->pq_array[index]->head = PCB_to_enqueue;
		rpq->pq_array[index]->tail = PCB_to_enqueue;
		rpq->pq_array[index]->size++;
	} else {
		PCB_to_enqueue->next = NULL;
		PCB_to_enqueue->previous = rpq->pq_array[index]->tail;
		rpq->pq_array[index]->tail = PCB_to_enqueue;
		rpq->pq_array[index]->size++;
	}

	return SUCCESS;
}

PCB *rpq_dequeue() {
	int index = HIGH - 1;
	
	while (rpq->pq_array[index]->size == 0 && index < 4) {
		if (index == 3 && rpq->pq_array[index]->size ==0) {
		    //WTF??
                    printf("Error: Cannot find the PCB to deqeue!");
		    return NULL;
		}
		index++;
	}

	PCB *temp = rpq->pq_array[index]->tail; // Maybe we can just use the pointers we have in the pcb list?
	if (temp->previous != NULL) {
		temp->previous = NULL;
	}

	rpq->pq_array[index]->tail = NULL;
	rpq->pq_array[index]->size--;
	if (rpq->pq_array[index]->size == 0) {
		rpq->pq_array[index]->head = NULL;
	}

	return temp;
}

int msg_enqueue(msg_envelope *msg_env, msg_queue *queue) {
	fflush(stdout);
	printf("msg_enqueue invoked\n"); 
	fflush(stdout);
	if (queue->size == 0) {
		msg_env->previous = NULL;
		msg_env->next = NULL;
		queue->head= msg_env;
		queue->tail = msg_env;
		queue->size++;
	} else {
		msg_env->previous = queue->tail;
		msg_env->next = NULL;
		queue->tail = msg_env;
		queue->size++;
	}

	return SUCCESS;
}

msg_envelope *msg_dequeue(msg_queue *queue) {
	if (queue->size == 0) {
		return NULL;
	}
	
	msg_envelope *temp;
	temp = queue->tail;
	if (temp->previous != NULL) {
		queue->tail = temp->previous; 
		temp->next = NULL;
		temp->previous = NULL;
		queue->tail->next = NULL;
	}
	queue->size--;
	if (queue->size == 0){
		queue->head = NULL;
	}
	
	return temp;
}

int blocked_on_resource_Q_enqueue(PCB *PCB_to_enqueue){
    if (blocked_on_resource_Q->size == 0) {
            blocked_on_resource_Q->head = PCB_to_enqueue;
            blocked_on_resource_Q->tail =  PCB_to_enqueue;
            PCB_to_enqueue->previous = NULL;
            PCB_to_enqueue->next = NULL;
            blocked_on_resource_Q->size++;
    } else {
            PCB_to_enqueue->previous = blocked_on_resource_Q->tail;
            blocked_on_resource_Q->tail = PCB_to_enqueue;
            PCB_to_enqueue->next = NULL;
            blocked_on_resource_Q->size++;
    }

    return SUCCESS;
}


PCB *blocked_on_resource_Q_dequeue(){
	if (blocked_on_resource_Q->size ==0) {
		return NULL;
	}
	
	PCB *temp = blocked_on_resource_Q->tail;
	if (temp->previous != NULL) {
		temp->previous = NULL;
	}

	blocked_on_resource_Q->tail = NULL;
	blocked_on_resource_Q->size--;
	if (blocked_on_resource_Q->size == 0) {
            blocked_on_resource_Q->head = NULL;
	}
        
        return temp;
}

PCB *remove_PCB_from_rpq(int target_pid) {
    int priority = current_process->process_priority;
    PCB *removed_pcb = rpq->pq_array[priority]->head;
    
    if (removed_pcb->process_id == target_pid) {
        rpq->pq_array[priority]->head = rpq->pq_array[priority]->head->next;
        return removed_pcb;
    }
    
    while (removed_pcb->next) {
        removed_pcb = removed_pcb->next;
        if (removed_pcb->process_id == target_pid) {
            removed_pcb->previous->next = removed_pcb->next;
            removed_pcb->next->previous = removed_pcb->previous;
            removed_pcb->previous = NULL;
            removed_pcb->next = NULL;
            return removed_pcb;
        }        
    } 
    
    if (removed_pcb->next == NULL) {
        //wtf?
        printf("Error: Cannot find the PCB to dequeue!");
    }
}
