#ifndef MEMORY_METHODS_H
#define MEMORY_METHODS_H

#include "list.h"
#include "memory_management.h"


void calculate_physical_address(uint64_t virtual_address);
void initialize_free_frame_list(int i);
node_node* check_tlb_hit(node_queue *head,  node_node *node);
node_node* add_node_and_member_pf(node_node *node, node_node *member);
node_node* add_reference_pt(node_node *node, node_node *member);
int check_pf(node_node *member);
int check_ptable(node_node *member); 
int check_PageTable_and_add(node_node *member);
node_queue* add_tlb_list(node_queue *member); 
int check_free_frame();
node_node* generate_member();
node_node* generate_node_pf();
node_node* generate_node_pt();
node_queue* generate_node_tlb(); 
int check_pf_number_exists(node_node *node);
int check_pf_node_exists(node_node *node);
node_node* remove_first_node(node_node *head); 
node_node* process1(uint64_t virtual_address);
node_node* process2(node_node *node);
void process3(node_node *node);




#endif