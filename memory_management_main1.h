#include <stdio.h>
#include <stdlib.h>
#include "memory_management.h"
#include "list.h"
#include "memory_methods.h"
#include "pthread.h"
#include "semaphore.h"



int number_processes;
int free_frames_number;
int length_VPN_in_bits;
int length_PFN_in_bits;
int length_offset_in_bits;
int tlb_size;
int process_id;
int size_pf;
int size_pt = 300;



     

node_node *free_frame_list_head;
node_queue *tlb_list_head;
node_node *pf_list_head;
node_queue *tlb;
node_node *pt_list_head;



typedef struct rwlock rw_lock;
rw_lock *lck1;
rw_lock *lck2;
rw_lock *lck3;
rw_lock *lck4;
rw_lock *lck5;
rw_lock *lck6;
rw_lock *lck7;
rw_lock *lck8;
rw_lock *lck9;
rw_lock *lck10;
rw_lock *lck11;

sem_t *mutex;



struct rwlock {
	sem_t lock;
	sem_t writelock;
	int readers;
};

void rwlock_init(rw_lock *lck) {
	lck -> readers = 0;
	sem_init(&lck -> lock, number_processes, 1);
	sem_init(&lck -> writelock, number_processes, 1);
}

void rwlock_aquire_readlock(rw_lock *lck) {
	sem_wait(&lck -> lock);
	lck -> readers++;
	if (lck -> readers == 1) {
		sem_wait(&lck -> writelock);
	}
	sem_post(&lck -> lock);
}

void rwlock_release_readlock(rw_lock *lck) {
	sem_wait(&lck -> lock);
	lck -> readers--;
	if (lck -> readers == 0) {
		sem_post(&lck -> writelock);
	}
	sem_post(&lck -> lock);
}

void rwlock_acquire_writelock(rw_lock *lck) {
	sem_wait(&lck -> writelock);
}

void rwlock_release_writelock(rw_lock *lck) {
	sem_post(&lck -> writelock);
}


int memory_init_data(int number_processes1, int free_frames_number1, int length_VPN_in_bits1, int length_PFN_in_bits1, int length_offset_in_bits1, int tlb_size1)                
{
	number_processes = number_processes1;
	free_frames_number = free_frames_number1;
	length_VPN_in_bits = length_VPN_in_bits1;
	length_PFN_in_bits = length_PFN_in_bits1;
	length_offset_in_bits = length_offset_in_bits1;
	tlb_size = tlb_size1;
	size_pf = free_frames_number1;
	

	initialize_free_frame_list(free_frames_number);
	
	lck1 = (rw_lock*)malloc(sizeof(rw_lock));
	lck2 = (rw_lock*)malloc(sizeof(rw_lock));
	lck3 = (rw_lock*)malloc(sizeof(rw_lock));
	lck4 = (rw_lock*)malloc(sizeof(rw_lock));
	lck5 = (rw_lock*)malloc(sizeof(rw_lock));
	lck6 = (rw_lock*)malloc(sizeof(rw_lock));
	lck7 = (rw_lock*)malloc(sizeof(rw_lock));
	lck8 = (rw_lock*)malloc(sizeof(rw_lock));
	lck9 = (rw_lock*)malloc(sizeof(rw_lock));
	lck10 = (rw_lock*)malloc(sizeof(rw_lock));
	lck11= (rw_lock*)malloc(sizeof(rw_lock));

	rwlock_init(lck1); 
	rwlock_init(lck2); 
	rwlock_init(lck3); 
	rwlock_init(lck4); 
	rwlock_init(lck5); 
	rwlock_init(lck6); 
	rwlock_init(lck7); 
	rwlock_init(lck8); 
	rwlock_init(lck9); 
	rwlock_init(lck10); 
	rwlock_init(lck11);
	//mutex = (sem_t*)malloc(sizeof(sem_t));
	
	
	//sem_init(mutex, number_processes, 1);
	

	
	return 0;
}

int get_physical_address(uint64_t virtual_address, int process_id, uint64_t* physical_address, int* tlb_hit)

{
	 
	
	node_node *node = (node_node*)malloc(sizeof(node_node));
	node = process1(virtual_address);
	
	if (node -> i == -1) {
		return -1;
	}
	
	node = process2(node);
	
	if (node -> k == 1) {
		return 1;
	}
	*physical_address = node -> physical_address;

	process3(node);
	
	*tlb_hit = node -> i;
	

	return 0;
}



            




/**
 * initialuze the free frame list with a given size of members
 */
void initialize_free_frame_list(int i) {

	for (int k = 0; k < i ; k++) {
		node_node *node = (node_node*)malloc(sizeof(node_node));
		free_frame_list_head = add_node(node, free_frame_list_head, free_frames_number, 2);
	}
	
}


/**
 * return the framenumber or miss
 * success if the pagenumber is in the tlb, then the return value is the page frame number
 * miss if the pagenumber is not in the tlb, if the tlb is void, then return -1, else -2
 * for the test success: tlb_hit = 1, else 0
 */
node_node* check_tlb_hit(node_queue *head, node_node *p_node) {
	rwlock_aquire_readlock(lck1);////////////////////////////////////////////////////////acquire read lock
	if (head == NULL) {
		p_node -> i = 0; //value for the test if it is no tlb hit
		p_node -> k = -1;
		rwlock_release_readlock(lck1);///////////////////////////////////////////////////////////////release read lock
		return p_node;
		
	}
	node_queue *node = (node_queue*)malloc(sizeof(node_queue));
	node -> page_number = p_node -> page_number;
	node -> page_frame = p_node -> frame_number;
	node_queue *actual = head;
	while (actual != NULL) {
		if (actual-> page_number == node -> page_number) {
			p_node -> i = 1; //value for the test if it is a tlb hit
			p_node -> k = actual -> page_frame;
			rwlock_release_readlock(lck1);///////////////////////////////////////////////////////////////release read lock
			//return actual -> page_frame;
			return p_node;
			
		}
		actual = actual -> next;
	}
	p_node -> k = -2;
	//*tlb_hit1 = 0; //value for the test if it is no tlb hit
	rwlock_release_readlock(lck1);///////////////////////////////////////////////////////////////release read lock
	return p_node;
	
} 

/**
 * First is checked if the member is yet in the pysical frame, if not it is checked if the frame exists.
 * If not it removes a free frame from the free frame list.
 * Then the (node if necessary) and member are added to the physical frame.
 */
node_node* add_node_and_member_pf(node_node *nodes, node_node *member) {

	rwlock_aquire_readlock(lck2);////////////////////////////////////////////////////////acquire read lock
	if (check_pf(member) != 1) {
		//node_node *nodes = generate_node_pf();
		int i = check_pf_node_exists(nodes);
		if (i == 0 || i == -1) {
			remove_first_node(free_frame_list_head); 
	 		//free_frame_node sollte eigentlich vom pf übernommen werden
			
		}
			
			
		pf_list_head = add_node(nodes, pf_list_head, size_pf, 1);
		pf_list_head = add_member(pf_list_head, nodes, member, 1);	
		rwlock_release_readlock(lck2);///////////////////////////////////////////////////////////////release read lock
		
	}
	return pf_list_head;
}


/**
 * First check if the member is yet in the page table, if not it is checked if the member is in the physical frame
 * if yes the member gets its physical address and the refernce is added to the page table
 */
node_node* add_reference_pt(node_node *node, node_node *member) {
	rwlock_aquire_readlock(lck3);////////////////////////////////////////////////////////acquire read lock
	if (check_ptable(member) != 1) {
		int i = check_pf(member);
		if (i == 1) {
			if (member -> k == 0) {
				member -> k = 2;
			}
			
			pt_list_head = add_node(node, pt_list_head, size_pt, 0);
			pt_list_head = add_member(pt_list_head, node, member, 0);
		}
		
	}
	rwlock_release_readlock(lck3);///////////////////////////////////////////////////////////////release read lock
	return pt_list_head;

}

/**
 * Check if the physical frame number exists
 * if yes the return value is 1, if not 0
 * if the physical frame is void then -1
 */
int check_pf_node_exists(node_node *node){
	rwlock_aquire_readlock(lck4);////////////////////////////////////////////////////////acquire read lock
	node_node *actual = pf_list_head;
	if (actual == NULL) {
		rwlock_release_readlock(lck4);///////////////////////////////////////////////////////////////release read lock
		return -1;
	}
	
	while (actual != NULL) {
		if (actual -> page_number == node -> page_number) {
			rwlock_release_readlock(lck4);///////////////////////////////////////////////////////////////release read lock
			return 1;
		}
		actual = actual -> next;
	}
	rwlock_release_readlock(lck4);///////////////////////////////////////////////////////////////release read lock
	return 0;
}

/**
 * Checks if the physical frame number exists, if yes return the frame number, if not the next free frame number
 * and -1 if the physical frame is void
 */
int check_pf_number_exists(node_node *node) {
	rwlock_aquire_readlock(lck5);////////////////////////////////////////////////////////acquire read lock
	node_node *actual = pf_list_head;
	if (actual == NULL) {
		rwlock_release_readlock(lck5);///////////////////////////////////////////////////////////////release read lock
		return -1; 
	}
	int i = -1; //0
	while (actual != NULL) {
		if (actual -> page_number == node -> page_number) {
			rwlock_release_readlock(lck5);///////////////////////////////////////////////////////////////release read lock
			return actual -> frame_number;
		}
		actual = actual -> next;
		i++;
	}
	rwlock_release_readlock(lck5);///////////////////////////////////////////////////////////////release read lock
	return ++i;
	
}

/**
 * Cheks if the member is yet in the physical frame, if yes the return value is 1, if not 0
 */
int check_pf(node_node *member) {
	rwlock_aquire_readlock(lck6);////////////////////////////////////////////////////////acquire read lock
	node_node *actual = pf_list_head;
	while (actual != NULL) {
		if (actual -> frame_number == member -> frame_number) {
			if (actual -> head != NULL) {
				node_node *memb = actual -> head;
				while (memb != NULL) {
					if (memb -> offset_address == member -> offset_address) {
						rwlock_release_readlock(lck6);///////////////////////////////////////////////////////////////release read lock
						return 1;
					
					} 
					memb = memb -> next;
				}
			}
			
			
		}
		actual = actual -> next;
	}
	rwlock_release_readlock(lck6);///////////////////////////////////////////////////////////////release read lock
	return 0;
}

/**
 * Check if the member is in the page table, if yes, then is checked if the physical address is given 
 * this means the member is in the physical frame, then the frame number is returned, if not in the physical
 * frame, then -4 is returned.
 * if it doesn't exist, then 0 is returned
 */
int check_ptable(node_node *member) {
	rwlock_aquire_readlock(lck7);////////////////////////////////////////////////////////acquire read lock
	node_node *actual = pt_list_head;
	while (actual != NULL) {
		if (actual -> page_number == member -> page_number) {
			if (actual -> head != NULL) {
				node_node *memb = actual -> head;
				while (memb != NULL) {
					if (memb -> offset_address == member -> offset_address) {
						if (memb -> physical_address == 0) {
							//printf("Yet in the page-table, but not in the frame-list -> add to the frame list\n");
							rwlock_release_readlock(lck7);///////////////////////////////////////////////////////////////release read lock
							return -4;
						}
						//printf("Yet in the page-table and in the frame-list\n");
						rwlock_release_readlock(lck7);///////////////////////////////////////////////////////////////release read lock
						return memb -> frame_number;
						
					} 
					memb = memb -> next;
				}
			}
			
		}
		actual = actual -> next;
	}
	//printf("Not in the page table\n");
	rwlock_release_readlock(lck7);///////////////////////////////////////////////////////////////release read lock
	return 0;
}


/**
 * If is not in the physical frame, then is added to it, if is not in the page table, then is added to it.
 * At first it is added to the tlb.
 */
int check_PageTable_and_add(node_node *memb){

	rwlock_aquire_readlock(lck8);////////////////////////////////////////////////////////acquire read lock
	node_queue *node = (node_queue*)malloc(sizeof(node_queue));
	node -> page_frame = memb -> frame_number;
	node -> page_number = memb -> page_number;
	node_node *node_pt = (node_node*)malloc(sizeof(node_node));
	node_pt ->frame_number = memb -> frame_number;
	node_pt -> page_number = memb -> page_number;
	tlb_list_head = add_tlb_list(node);
	
	int k = check_ptable(memb);
	
	if (k == 0 || k == -4 ) {
		pf_list_head = add_node_and_member_pf(node_pt, memb);
		if (check_pf(memb) == 1) {
			memb -> k = 2;
			
			
		}
		if (k == 0) {
			

			pt_list_head = add_reference_pt(node_pt, memb);
		}
		
	}
	
	rwlock_release_readlock(lck8);///////////////////////////////////////////////////////////////release read lock					
	return 1;

} 

/**
 * First checks if yet there, if not is added to tlb
 */
node_queue* add_tlb_list(node_queue *member) {
	rwlock_aquire_readlock(lck9);////////////////////////////////////////////////////acquire read lock
	int i = check_equals(tlb_list_head, member);
	if (i != 1) {
		tlb_list_head = enqueue(member, tlb_list_head, tlb_size);
	}
	rwlock_release_readlock(lck9);/////////////////////////////////////////////////////////////release read lock
	return tlb_list_head;
}

/**
 * checks if there is a free frame if yes the return value is 1, if not 0
 */
int check_free_frame(node_node *head) {
	rwlock_aquire_readlock(lck10);////////////////////////////////////////////////////////acquire read lock
	int i = isempty_List(head);
	if (i == 0) {
		rwlock_release_readlock(lck10);/////////////////////////////////////////////////////////////release read lock
		return 1;
	}
	rwlock_release_readlock(lck10);/////////////////////////////////////////////////////////////release read lock
	return 0;
}

/**
 * If there is no free frame then the return value is NULL, else the node is returned
 */
node_node* remove_first_node(node_node *head) {
	rwlock_aquire_readlock(lck11);////////////////////////////////////////////////////////acquire read lock
    if (head == NULL) {
		rwlock_release_readlock(lck11);/////////////////////////////////////////////////////////////release read lock
        return NULL;
    }
    node_node *actual = head;
    free_frame_list_head = head -> next;
	
	
	rwlock_release_readlock(lck11);/////////////////////////////////////////////////////////////release read lock
    return actual;
}



node_node* process1(uint64_t virtual_address){
	//rwlock_aquire_readlock(lck3);////////////////////////////////////////read_aquire
	//sem_wait(mutex);//////////////////mutex for only compute
	uint64_t OFFSET_MASK = ((1 << length_offset_in_bits)-1);
	
	uint64_t offset2 = virtual_address & OFFSET_MASK;
	int page_num2 = (virtual_address >> length_offset_in_bits);

	node_node *node = (node_node*)malloc(sizeof(node_node));
	node -> offset_address = offset2;
	node -> page_number = page_num2;
	

	//printf("physical = %#lx\n", physical_address);
	if ((virtual_address >> (length_VPN_in_bits + length_offset_in_bits )) > 0) {
		node -> i = -1;
		//sem_post(mutex);////////////////////////////////////////////end of mutex
		//rwlock_release_readlock(lck3);//////////////////////////////////////////////////release_read
		//printf("The virual address is too long\n!");
		 //return value for the test, if the virtual address is too long
	}
	
	
	//sem_post(mutex);//////////////////////////////////////////////end of mutex
	//rwlock_release_readlock(lck3);//////////////////////////////////////////////////release_read
	return node;
}
node_node* process2(node_node *node) {
	//rwlock_aquire_readlock(lck1);////////////////////////////////////////read_aquire
	
	int g = check_pf_node_exists(node);
	int h = check_pf_number_exists(node);
	if (g == -1) {
		node -> frame_number = 0; //1
		
	}
	if (g == 0) {
		if (size_actual_list(pf_list_head, free_frames_number) == - 1) {
			node -> k = 1;
			//rwlock_release_readlock(lck1);//////////////////////////////////////////////////release_read
			//printf("No free physical frame!");
			
		} else {
			
			node -> frame_number = h;
			
		}

	}
	if (h >= 0) {
		node -> frame_number = h;
		
	}
	node -> physical_address = ((node -> frame_number) << length_offset_in_bits) + (node -> offset_address);
	//rwlock_release_readlock(lck1);//////////////////////////////////////////////////release_read
	//printf("physical = %#lx\n", *physical_address1);
	return node;
}

void process3(node_node *node) {
	
	//rwlock_aquire_readlock(lck2);////////////////////////////////////////read_aquire
	node = check_tlb_hit(tlb_list_head, node);
	if (node -> k == -1) {
		//printf("First entry in tlb \n");
		
		node -> i = 0;
		node -> k = 0;
		check_PageTable_and_add(node);
		node -> i = 0;
		//rwlock_release_readlock(lck2);//////////////////////////////////////////////////release_read
		//printf("Return value of check_PageTable_and_add = %d\n", i);
		
	}
	if (node -> k == -2) {
		//printf("No tlb hit \n");
		
		node -> i = 0;
		node -> k = 0;
		check_PageTable_and_add(node);
		node -> i = 0;
		//rwlock_release_readlock(lck2);//////////////////////////////////////////////////release_read
		////printf("Return value of check_PageTable_and_add = %d\n", i);
		
	}
	//if (node -> k >= 0){
		//printf("tlb-hit, the value of the page_frame is = %d\n", i);
		//return node -> i;
	//}

	
	//free(mutex);
	//free(node);
	
}




