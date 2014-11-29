#include "my_malloc.h"

/* You *MUST* use this macro when calling my_sbrk to allocate the 
 * appropriate size. Failure to do so may result in an incorrect
 * grading!
 */
#define SBRK_SIZE 2048

/* If you want to use debugging printouts, it is HIGHLY recommended
 * to use this macro or something similar. If you produce output from
 * your code then you will receive a 20 point deduction. You have been
 * warned.
 */
#ifdef DEBUG
#define DEBUG_PRINT(x) printf x
#else
#define DEBUG_PRINT(x)
#endif


/* make sure this always points to the beginning of your current
 * heap space! if it does not, then the grader will not be able
 * to run correctly and you will receive 0 credit. remember that
 * only the _first_ call to my_malloc() returns the beginning of
 * the heap. sequential calls will return a pointer to the newly
 * added space!
 * Technically this should be declared static because we do not
 * want any program outside of this file to be able to access it
 * however, DO NOT CHANGE the way this variable is declared or
 * it will break the autograder.
 */
void* heap;

/* our freelist structure - this is where the current freelist of
 * blocks will be maintained. failure to maintain the list inside
 * of this structure will result in no credit, as the grader will
 * expect it to be maintained here.
 * Technically this should be declared static for the same reasons
 * as above, but DO NOT CHANGE the way this structure is declared
 * or it will break the autograder.
 */
metadata_t* freelist[8];
/**** SIZES FOR THE FREE LIST ****
 * freelist[0] -> 16
 * freelist[1] -> 32
 * freelist[2] -> 64
 * freelist[3] -> 128
 * freelist[4] -> 256
 * freelist[5] -> 512
 * freelist[6] -> 1024
 * freelist[7] -> 2048
 */


void* my_malloc(size_t size)
{
	int i;
	//calculating the required size:
	int required = size + sizeof(metadata_t);

	//checking if required is more than 2048:
	if (required> 2048){
		ERRNO = SINGLE_REQUEST_TOO_LARGE;
		return NULL;
	}

	//initialize heap if it doesn't already exist:
	if (heap == NULL){
		heap = my_sbrk(SBRK_SIZE);
		freelist[7] = (metadata_t *) heap;
		freelist[7]->in_use = 0;
		freelist[7]->size = SBRK_SIZE;
		freelist[7]->next = NULL;
		freelist[7]->prev = NULL;
	}
	//if heap is still null, then something went wrong.
	if (heap == NULL){
		return NULL;
	}


	//calculating the nearest size's (which is greater than or equal to the size required) index.
	int ndx = calcIndex(required);

	//if the index in the freelist is readily available, then return it.
	if (freelist[ndx]!=NULL){
		metadata_t* initial = freelist[ndx];
		metadata_t* nextSpace = initial->next;

		initial->in_use=1;
		initial->next = NULL;
		initial->prev = NULL;

		if(nextSpace ==NULL){
			freelist[ndx] = NULL;
		}else {
			nextSpace->prev=NULL;
			freelist[ndx] = nextSpace;
		}

		ERRNO = NO_ERROR;
		return (initial + 1);
	}
	else{

		for( i=ndx+1; i<8 ; ++i){
			if(freelist[i]!=NULL) break;
		}
		//call sbrk for new heap
		if(i==8){
			metadata_t* heap2 = my_sbrk(SBRK_SIZE);
			if(heap2 == NULL){
				ERRNO = OUT_OF_MEMORY;
				return NULL;
			}
			heap2->in_use = 0;
			heap2->size= SBRK_SIZE;
			heap2->next = NULL;
			heap2->prev = NULL;

			if(freelist[7]!=NULL){
				freelist[7]->next = heap2;
			}
			else
				freelist[7] = heap2;

			i=7;
		}

		splitter(i,ndx);
		metadata_t* initial = freelist[ndx];



		if(freelist[ndx]->next == NULL)
			freelist[ndx] = NULL;

		else {

			freelist[ndx] = freelist[ndx]->next;
			freelist[ndx]->prev = NULL;
		}

		initial->next  =NULL;

		initial->in_use = 1;

		ERRNO = NO_ERROR;

		return (initial+1);
	}
}

void splitter(int ndx , int reqd){
	if(ndx==reqd){
		return;
	}

	metadata_t* block = freelist[ndx];
	block->size = block->size/2;

	metadata_t* block2 = (metadata_t*) ((char*)block + block->size);
	block2->size = block->size;

	if(freelist[ndx]->next == NULL){
		freelist[ndx]=NULL;
	}
	else{
		freelist[ndx]= freelist[ndx]->next;
		freelist[ndx]->prev = NULL;
	}

	block->next  = block2;
	block2->prev = block;
	freelist[ndx-1] = block;

	splitter(ndx-1,reqd);
}



int calcIndex(int required){

	int ndx = 0;
	int size = 16;
	while (size < required) {
		ndx++;
		size = size*2;
	}
	return ndx;
}
void* my_calloc(size_t num, size_t size)
{
	void* ptr = my_malloc(num*size);

	if(ptr == NULL)
		return NULL;

	ERRNO = NO_ERROR;
	for (int j=0 ; j<num*size ; ++j){
		((char*)ptr)[j]=0;
	}
	return ptr;

}

void my_free(void* ptr)
{
	//checks if the pointer being passed in is NULL
	if(ptr==NULL)
		return;


	metadata_t* block = ((metadata_t*)ptr)-1;
	block->in_use =0;

	metadata_t* buddy = findBuddy(block);

	if(buddy == NULL && block->in_use==0){
		ERRNO = DOUBLE_FREE_DETECTED;
		return;
	}

	merge(buddy,block);

	int ndx = calcIndex(block->size);

	if(freelist[ndx]!=NULL){
		metadata_t* start = freelist[ndx];
		start->prev = block;
		block->next = start;
	}

	freelist[ndx] = block;
}

metadata_t* findBuddy(metadata_t* ptr){
	metadata_t* bud = (metadata_t*) (ptr->size ^ ((int)ptr));

	if(bud->size == ptr->size){
		return bud;
	}
	else{
		return NULL;
	}
}

void merge(metadata_t* block1, metadata_t* block2){

	if ( block1==NULL || block1->in_use  )
		return;

	int ndx = calcIndex(block1->size);

	if(block1->next!=NULL){
		if (block1->prev!=NULL){
			block1->next->prev = block1->prev;
			block1->prev->next = block1->next;
		}
		else{
			block1->next->prev = NULL;
		}
	}
	else {
		if (block1->prev!=NULL){

			block1->prev->next=NULL;
		}
		else
			freelist[ndx] = NULL;
	}

	if(block2 > block1)
		block2 = block1;

	block2->size = block2->size *2;
	block1 = findBuddy(block2);

	merge(block1, block2);

}


void* my_memmove(void* dest, const void* src, size_t num_bytes)
{
	if(((char*)src + num_bytes)>=(char*)dest){
		for(int j = (num_bytes -1) ; j >=0 ; --j){
			((char*)dest)[j] = ((char*)src)[j];
		}
	}
	else {
		for (int j=0 ; j<num_bytes ; ++j){
			((char*)dest)[j] = ((char*)src)[j];
		}
	}
	return dest;
}

