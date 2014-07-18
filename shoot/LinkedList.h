#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

typedef struct LinkedList {
	struct LinkedList* next;
	char data[1];
} LinkedList;

#endif
