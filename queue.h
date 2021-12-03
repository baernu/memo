#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Nodequeue node_queue;
int isfull(node_queue *head, int size);
int isempty(node_queue *head);
node_queue* enqueue(node_queue *node, node_queue *head, int size);
node_queue* dequeue(node_queue *head);
int check_equals(node_queue *head, node_queue *node);

/* Node of the Queue */
struct Nodequeue {

    node_queue *next;
    node_queue *previous;
    int page_number;
    int page_frame;
}; 


/**
 * Check first if equals are in the queue, if not, check if is full, then dequeue the queue
 * , if is empty then initialize the head of the queue with the given node, if not in the queue, then 
 * add to the queue
 */
node_queue* enqueue(node_queue *node, node_queue *head, int size) {

    int i = check_equals(head, node);
    if (i != 1) {
        if (isfull(head, size) == 1) {
            dequeue(head);
        } 
        if (isempty(head) == 1) {
            head = (node_queue*)malloc(sizeof(node_queue));
            head = node;
            return head;
        }
        node_queue *actual = head;
        head  = node;
        head -> next = actual;
        actual -> previous = head;
     
        return head;
    }
    return head;

    
    
}
/**
 * First check if is empty, then return null. If not empty, then remove the first the tail 
 * element in the queue, this means the first which was entered.
 */
node_queue* dequeue(node_queue *head) {

     if (isempty(head) == 1) {
        printf ("The  Queue is empty!");
        return NULL;
    } else {
        node_queue *node = head;
        node_queue *temp = node;
       
        
        while(node != NULL) {
            if (node -> next == NULL) {
                temp = node;
            }
            node = node -> next;
        }
        if (temp == head) {
            temp = NULL;
        } else {
            (temp -> previous) -> next = NULL;
            free (temp);
        }
        
    
        return head;
    }

}

/**
 * If is empty return 1, else 0
 */
int isempty(node_queue *head) {
    if (head  == NULL) {
        return 1;
    } 
    return 0;
}

/**
 * If is full, then return 1, else 0
 */
int isfull(node_queue *head, int size) {
    int i = 0;
    node_queue *node = head;
    while (node != NULL) {
        node = node -> next;
        i++;
    }
    if (i < size ) {
        return 0;
    } 
    return 1;

}

int free_queue(node_queue *head) {
    
    while (head != NULL) {
        node_queue *actual = head;
        head = head -> next;
        free (actual);
    }
    return 1;
}

/**
 * If equals then return 1, else 0
 */
int check_equals(node_queue *head, node_queue *node) {
    node_queue *actual = head;
    while (actual != NULL) {
        if (actual -> page_number == node -> page_number) {
            return 1;
        }
        actual = actual -> next;
    }
    return 0;
}


#endif