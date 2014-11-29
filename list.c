#include <stdlib.h>
#include "./list.h"

/**
 * pushFront
 * Takes in ptr to a list and data to add
 * Should add new node containing the data to the head of the list, and increment size
 */
void pushFront(LIST *list, int data)
{
	NODE *new = malloc(sizeof(NODE));
	new->data = data;
	new->prev = NULL;
	new->next = NULL;

	if (list->size!=0){
		new->next = list->head;
		list->head->prev = new;
	}
	else {
		list->tail = new;
	}

	list->head = new;
	list->size = list->size +1;


}

/**
 * pushFront
 * Takes in ptr to a list and data to add
 * Should add new node containing the data to the tail of the list, and increment size
 */
void pushBack(LIST *list, int data)
{
	NODE *new = malloc(sizeof(NODE));
	new->data = data;
	new->prev = NULL;
	new->next = NULL;

	//If size isn't zero, set new node'e prev to tail and the list's tail to new node.
	if(list->size!=0){
		new->prev = list->tail;
		list->tail->next = new;
	}
	//if size is zero, set the list's head to new node as well.
	else {
		list->head = new;
	}
	//this happens regardless of size: setting list's tail to new node and incrementing size
	list->tail = new;
	list->size = list->size +1;

}

/**
 * popFront
 * Takes in ptr to a list
 * Remove and free node at the front of the list, and decrement size
 * Return the value of that node
 * Return 0 if the size of the list is 0
 */
int popFront(LIST *list)
{	// if the size is 0, just return 0
	if (list->size ==0){
		return 0;
	}
	//the value of the node to be removed
	int data = list->head->data;
	//free the memory pointed to by the head
	free(list->head);
	//if the size is one, then the head and tail should be set to null
	
	 if (list->size==1){
		list->head = NULL;
		list->tail = NULL;
	}
	 //if the size is greater than one, the tail should now point to it's prev and the new tail's next should be NULL
	 else if (list->size > 1) {
		 list->head = list->head->next;
		 list->head->prev = NULL;
	 }

	 //decrementing size and return the node's data.
	 list->size = list->size -1;

	 return data;


}

/**
 * popBack
 * Takes in ptr to a list
 * Remove and free node at the back of the list, and decrement size
 * Return the value of that node
 * Return 0 if the size of the list is 0
 */
int popBack(LIST *list)
{
	// if the size is 0, just return 0
	if (list->size ==0){
		return 0;
	}
	//the value of the node to be removed
	int data = list->tail->data;
	//free the memory pointed to by the tail
	free(list->tail);
	//if the size is one, then the head and tail should be set to null
	 if (list->size==1){
		list->head = NULL ;
		list->tail = NULL;
	}
	 //if the size is greater than one, the tail should now point to it's prev and the new tail's next should be NULL
	 if (list->size > 1) {
		 list->tail = list->tail->prev;
		 list->tail->next = NULL;
	 }
	 //decrementing size
	 list->size = list->size -1;

	 return data;

}


