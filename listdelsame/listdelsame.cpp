// codingTask2.cpp : Defines the entry point for the console application.
//
//#ifdef WIN32
#include "stdafx.h"
//#endif
#include <assert.h>
#include <stdlib.h>
#include "listdelsame.h"

#pragma comment(lib, "../x64/Debug/marking.lib")
void examination(void);

bool DeteleNode(struct Node **pHeader, int Value){
    //TODO: please add code here,
    
    
    return false;
}


// ATTENTIONS: 
//     This test want you to implement an API "DE()" : delete node 
//       in two input lists if the data in the list's node is the equal.   

// 1. Delete node if the node's data value is equal in pListA and pListB.
// 2. The lists are double link list.
// 3. Delete both list's node that match condition. (data value equal).
// 4. Please cosider all of the scenarios you can think.
// 5. The marking_test() will check and print the score of your answer.
// 6. The full mark is 100, you are expected to achieve 70.
// 7. Any memory leak will mark down 40
// 8. To malloc and free memory, please use ct_malloc() and ct_free(), using free()
//    and malloc() is not allowed
// 9. please finished this test in one hour, if overtime, will 10 mark down per ten minutes.
void DE(struct Node **pHeadA, struct Node **pHeadB)
{
    //TODO: please add code here, recomemd to call DeteleNode()
    
}


// test code start, please don't modify below
int main()
{
    examination();
    marking_test();

	system("pause");
	return 0;
}

void dump_list(node_t* list)
{

    node_t* tmp = list;
    if (tmp) {
        do {
            debug_log("l1: 0x%x = %d \n",tmp, tmp->data);
            tmp=tmp->next;
        }while (tmp != list);
    } else {
        debug_log("list is empty.\n" );
    }
}

void examination(void)
{ 
    int table1[] = {7,8,3,9};
    node_t* list1 = create_link_by_table(table1, sizeof(table1)/sizeof(int));
    
    int table2[] = {4,3,2,1};
    node_t* list2 = create_link_by_table(table2, sizeof(table2)/sizeof(int));
    
    dump_list(list1);
    dump_list(list2);
    
    DE(&list1,&list2);

    dump_list(list1);
    dump_list(list2);
    
    free_link(list2);
    free_link(list1);
     
}

void free_link(node_t * head)
{
    if (!head) 
        return;
    
    node_t * node = head;
    void * p; 
    do {
        p = node;
        node = node->next;
        ct_free(p);
    
    }while(node != head);
}

node_t * create_link_by_table(int* table, int num)
{
    int i = 0;
    node_t * head = NULL;
    node_t * node;
    
    while (i < num) {
        node = (node_t*)ct_malloc(sizeof(node_t));
        if (!node) {
            free_link(head);
            return NULL;
        }
        
        node->data = table[i];
        
        if (!head) {
            head = node;
            head->front = node;
            head->next = node;
        } else {
            node->next = head;
            node->front = head->front;
            head->front->next = node;
            head->front = node;
        }
        i++;
    }
	return head;
}


