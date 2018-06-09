// codingTask2.cpp : Defines the entry point for the console application.
//irr::
#ifndef _LINK_DELETE_SAME_
#define _LINK_DELETE_SAME_

#include <time.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <stdio.h>
#endif

#include <assert.h>
#include <stdlib.h>


#ifdef DEBUG
#define debug_log printf
#else
#define debug_log
#endif


typedef struct Node{
    struct Node* front;
    struct Node* next;
    int data;
}node_t;

typedef struct{
	float (*check_func)(void*);
	void * data;
	float  score;
	const char* name;
}st_check_case;

extern bool g_watch_mem; 

extern bool DeteleNode(struct Node **pHeader, int Value);

extern void DE(struct Node **pHeadA, struct Node **pHeadB);

extern void dump_list(node_t* list);

extern bool DeteleNode_as(struct Node **pHeader, int Value);
extern void DE_as(struct Node **pHeadA, struct Node **pHeadB);
extern void* ct_malloc(int size);
extern void ct_free(void* p);

extern void free_link(node_t * head);

extern node_t * create_link_by_table(int* table, int num);
extern void marking_test();
#endif
