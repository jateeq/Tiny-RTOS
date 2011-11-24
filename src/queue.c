#include "queue.h"

int rpq_enqueue(PCB *PCB_to_enqueue) {
	if (PCB_to_enqueue == NULL) {
	    return FAIL;
	} 

	int index = PCB_to_enqueue->process_priority - 1; 

	if (rpq->pq_array[index]->size == 0) {
		PCB_to_enqueue->next = NULL; //?	
		PCB_to_enqueue->previous = NULL;
		rpq->pq_array[index]->head = PCB_to_enqueue;
		rpq->pq_array[index]->tail = PCB_to_enqueue;
		rpq->pq_array[index]->size++;
	} else {
		PCB_to_enqueue->previous = NULL;
		PCB_to_enqueue->next = rpq->pq_array[index]->head;
		rpq->pq_array[index]->head->previous = PCB_to_enqueue;
		rpq->pq_array[index]->head = PCB_to_enqueue;
		rpq->pq_array[index]->size++;
		/*
		rpq->pq_array[index]->tail->next = PCB_to_enqueue;
		PCB_to_enqueue->previous = rpq->pq_array[index]->tail;
		rpq->pq_array[index]->tail = PCB_to_enqueue;
		rpq->pq_array[index]->tail->next = NULL;
		rpq->pq_array[index]->size++;
		*/
	}

	return SUCCESS;
}

PCB *rpq_dequeue() {
	int index = HIGH - 1;
	
	while (rpq->pq_array[index]->size == 0 && index < 4) {
		if (index == 3 && rpq->pq_array[index]->size ==0) {	
                    printf("Error: Cannot find the PCB to deqeue!");
		    return NULL;
		}
		index++;
	}

	PCB *temp = rpq->pq_array[index]->tail; // Maybe we can just use the pointers we have in the pcb list?

	rpq->pq_array[index]->tail = temp->previous;
	rpq->pq_array[index]->size--;
	if (rpq->pq_array[index]->size == 0) {
		rpq->pq_array[index]->head = NULL;
		rpq->pq_array[index]->tail = NULL;
	} else {
		rpq->pq_array[index]->tail->next = NULL;	
	}

	if (temp->previous != NULL) {
		temp->previous = NULL;
	}

	return temp;
}

int msg_enqueue(msg_envelope *msg_env, msg_queue *queue) {
	if (msg_env == NULL || queue == NULL) {
		return FAIL;
	}

	if (queue->size == 0) {
		msg_env->previous = NULL;
		msg_env->next = NULL;
		queue->head= msg_env;
		queue->tail = msg_env;
		queue->size++;
	} else {
		msg_env->previous = NULL;
		msg_env->next = queue->head;
		queue->head->previous = msg_env;
		queue->head = msg_env;
		queue->size++;		
		/*
		msg_env->previous = queue->tail;
		msg_env->next = NULL;
		queue->tail = msg_env;
		queue->size++;
		*/
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
		queue->tail = NULL;
	}
	
	return temp;
}

int blocked_on_resource_Q_enqueue(PCB *PCB_to_enqueue){
	if (PCB_to_enqueue) {
		return FAIL;
	}

    if (blocked_on_resource_Q->size == 0) {
            blocked_on_resource_Q->head = PCB_to_enqueue;
            blocked_on_resource_Q->tail =  PCB_to_enqueue;
            PCB_to_enqueue->previous = NULL;
            PCB_to_enqueue->next = NULL;
            blocked_on_resource_Q->size++;
    } else {
	    PCB_to_enqueue->previous = NULL;
	    PCB_to_enqueue->next = blocked_on_resource_Q->head;
	    blocked_on_resource_Q->head->previous = PCB_to_enqueue;
	    blocked_on_resource_Q->head = PCB_to_enqueue;
	    blocked_on_resource_Q->size++;
	    /*
            PCB_to_enqueue->previous = blocked_on_resource_Q->tail;
            blocked_on_resource_Q->tail->next = PCB_to_enqueue;
	    blocked_on_resource_Q->tail = PCB_to_enqueue;
            blocked_on_resource_Q->tail->next = NULL;
            blocked_on_resource_Q->size++;
	    */
    }

    return SUCCESS;
}


PCB *blocked_on_resource_Q_dequeue(){
	if (blocked_on_resource_Q->size ==0) {
		return NULL;
	}

	PCB *temp = blocked_on_resource_Q->tail;
	
	blocked_on_resource_Q->tail = temp->previous;
	blocked_on_resource_Q->size--;
	if (blocked_on_resource_Q->size == 0) {
            blocked_on_resource_Q->head = NULL;
	    blocked_on_resource_Q->tail = NULL;
	} else {
	    blocked_on_resource_Q->tail->next = NULL;
	}

	if (temp->previous != NULL) {
		temp->previous = NULL;
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
        printf("Error: Cannot find the PCB to dequeue!");
    }

    return NULL;
}

int time_out_request_enqueue(msg_envelope *msg) {
	if (msg == NULL){
		//Something went wrong
		return FAIL;
	}

	//Check if the timeout list is empty first
	if (sorted_timeout_list->head == NULL) {
	    sorted_timeout_list->head = msg;
	    sorted_timeout_list->tail = msg;
	    sorted_timeout_list->size++;
	    msg->previous=NULL;
	    msg->next=NULL;
	    return SUCCESS;
	}

	msg_envelope *temp_ptr = sorted_timeout_list->tail;

	//Check if this message should be enqueued at the head or tail
	//msg with the smallest n_clock_ticks goes to tail
	if (msg->n_clock_ticks < temp_ptr->n_clock_ticks) {
		sorted_timeout_list->tail = msg;
		msg->previous = temp_ptr;
		temp_ptr->next = msg;
		sorted_timeout_list->size++;
		return SUCCESS;
	} else {
		temp_ptr = sorted_timeout_list->head;
		if (msg->n_clock_ticks > temp_ptr->n_clock_ticks) {
			sorted_timeout_list->head = msg;
			msg->next = temp_ptr;
			temp_ptr->previous = msg;
			sorted_timeout_list->size++;
			return SUCCESS;
		}else {
			temp_ptr = temp_ptr->next;
		}
	}

	//Search for the correct position
	while (msg->n_clock_ticks < temp_ptr->n_clock_ticks){
		temp_ptr = temp_ptr->next;
	}

	msg->previous = temp_ptr->previous->next;
	temp_ptr->previous->next = msg;
	msg->next = temp_ptr;
	temp_ptr->previous = msg;
	sorted_timeout_list->size++;

	return SUCCESS;
}

