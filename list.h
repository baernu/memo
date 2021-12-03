#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "queue.h"




typedef struct Node node_node;


int free_free_list(node_node *head);
int free_node_and_member_list(node_node *head);
int isfull_List(node_node *head, int size);
int isempty_List(node_node *head);

int remove_member(node_node *node, node_node *member);
node_node* add_member(node_node *head, node_node *node, node_node *member,int p_or_f);
node_node* add_node(node_node *node, node_node *head, int size, int p_or_f); 
int size_actual_list(node_node *head, int size);

/* Node of the List*/
struct Node {

    node_node *next;
    node_node *previous;
    int page_number;
    int frame_number;
    uint64_t physical_address;
    uint64_t offset_address;
    node_node *head;
    int i;
    int k;
    
}; 

/**
 * Adds a node. Checks if the list is full.For Page Table and Physical Frame List it is first checked, if yet exist. 
 * The last parameter is to difference: for 0 it is a page table,
 * for 1 it is a physical frame list, 
 * for other value the node is added without check.
 */
node_node* add_node(node_node *node, node_node *head, int size, int p_or_f) {
    
    if (head  == NULL) {
        //tlb_list_head = node;
        head = (node_node*)malloc(sizeof(node_node));

        head = node;

        return head;
        
    } 
    if (isfull_List(head, size) == 1) {
        //printf("The List is full, can't add an element!\n");
        return NULL;
    }
    node_node *actual = head;
    
    while (actual != NULL) {
        if (p_or_f == 0){
            if (actual -> page_number == node -> page_number) {
                return head;
            }
        }
        if (p_or_f == 1){
            if (actual -> frame_number == node -> frame_number) {
                return head;
            }
        }
        
        actual = actual -> next;
    }
    actual = head;
    head  = node;
    head -> next = actual;
    actual -> previous = node;
     
    return head;
    
}

/**
 * Adds a member, if no node then is created one. 
 * Checks fist if the member exists.
 * The last parameter is to difference: for 0 it is a page table,
 * for 1 it is a physical frame list, 
 * for other value the head is returned without manipulation
 */
node_node* add_member(node_node *head, node_node *node, node_node *member, int p_or_f) {

    node_node *nod = head;
    while (nod != NULL) {
        if (p_or_f == 0) {
            if (nod -> page_number == node -> page_number) {
                if (nod -> head == NULL) {
                    nod -> head  = (node_node*)malloc(sizeof(node_node));
                    nod -> head = member;
                    return head;
                }
                
                node_node *actual = nod -> head;
                nod -> head = member;
                member -> next = actual;
                actual -> previous = member;
   
                return head;
            }
        }
        if (p_or_f == 1) {
            if (nod -> frame_number == node -> frame_number) {
                if (nod -> head == NULL) {
                    nod -> head  = (node_node*)malloc(sizeof(node_node));
                    nod -> head = member;
                    return head;
                }
                node_node *actual = nod -> head;

                nod -> head = member;
                member -> next = actual;
                actual -> previous = member;
                
   
                return head;
            }
        }
        nod = nod -> next;
    }
    return head;
    
    
}

int remove_member(node_node *node, node_node *member) {

    node_node *actual = node -> head;

    while (actual != NULL) {
        if (actual == member) {
            node_node *temp = actual;
            temp = temp -> next;
            temp -> previous = actual -> next;
            free(member);
        }  
        actual = actual -> next;
        
    } 
    
    return 1;
}


/**
 * Returns 1 if the list is empty,
 * 0 if not
 */
int isempty_List(node_node *head) {
    if (head == NULL) {
        return 1;
    } 
    return 0;
}

/**
 * Returns 0 if the list is not full,
 * 1 if the list is full
 */
int isfull_List(node_node *head, int size) {
    int i = 0;
    node_node *node = head;
    while (node != NULL) {
        node = node -> next;
        i++;
    }
    if (i < size) {
        return 0;
    } 
    return 1;
}

/**
 * Returns the size of a list or -1 if the size is greater than the size of the list
 */
int size_actual_list(node_node *head, int size) {
    int i = 0;
    node_node *node = head;
    while (node != NULL) {
        node = node -> next;
        i++;
    } 
    if (i <= size) {
        return i;
    } else {
        return -1;
    }
    
}

/**
 * Frees all the members and nodes of a list from type node_node
 */
int free_node_and_member_list(node_node *head) {
    
    while (head != NULL) {
        node_node *actual = head;
        
        while (head -> head != NULL) {
            node_node *member = head -> head;
            head -> head = (head -> head) -> next;
            free(member);
        }
        
        head = head -> next;
        free (actual);
        
    }
    


    return 1;
}

/**
 * Return 1, if the head of the list is NULL and the list void
 */
int free_free_list(node_node *head) {
    
    while (head != NULL) {
        node_node *actual = head;
        head = head -> next;
        free(actual);

    }
    return 1;
}


#endif