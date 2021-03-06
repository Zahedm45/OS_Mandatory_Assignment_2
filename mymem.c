#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>


/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct MemoryList
{
  // doubly-linked list
  struct MemoryList *previous;
  struct MemoryList *next;

  int size;            // How many bytes in this block?
  char alloc;          // 1 if this block is allocated,
                       // 0 if this block is free.
  void *ptr;           // location of block in memory pool.
};

strategies myStrategy = NotSet;    // Current strategy


size_t mySize;
void *myMemory = NULL;

static struct MemoryList *head = NULL;
static struct MemoryList *next = NULL;

struct MemoryList* worst_fit(size_t requested);
struct MemoryList* first_fit(size_t requested);
struct MemoryList *next_fit(size_t requested);
struct MemoryList* best_fit(size_t requested);


void initmem(strategies strategy, size_t sz) {
    myStrategy = strategy;

    /* all implementations will need an actual block of memory to use */
    mySize = sz;

    if (myMemory != NULL) {                  /* in case this is not the first time initmem2 is called */
        free(myMemory);
    }


    /* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */

    if (head != NULL) {

        // This is what the guideline suggested but it doesn't seem to work

//        for (trav=head; trav.next != NULL; trav=trav->next) {
//            free(trav->previous);
//            free(trav);
//        }

        struct MemoryList *prevNode = head;
        struct MemoryList *crrNode = head->next;

        while (prevNode != NULL && crrNode != NULL) {
            prevNode->next = crrNode->next;
            free(crrNode);
            prevNode = prevNode->next;
            if (prevNode != NULL) {
                crrNode = prevNode->next;
            }
        }
    }


    myMemory = malloc(sz);

    /* TODO: Initialize memory management structure. */

    head = (struct MemoryList*) malloc(sizeof (struct MemoryList));
    head->previous = NULL;
    head->next = NULL;
    head->size = sz;
    head->alloc = '0';
    head->ptr = myMemory;
    next = head;

}


/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested) {

    assert((int)myStrategy > 0);
    // currentNode is the memory block that has to be allocated.
    struct MemoryList *currentNode = NULL;

    switch (myStrategy) {
        case NotSet:
            return NULL;
        case First:
            currentNode = first_fit(requested);
            break;
        case Best:
            currentNode = best_fit(requested);
            break;
        case Worst:
            currentNode = worst_fit(requested);
            break;
        case Next:
            currentNode = next_fit(requested);
    }


    if (currentNode == NULL) {return NULL;}


    if ((currentNode->alloc != '1') && (currentNode->size > requested)) {
        // here restMem is a new block, which represents the rest of the memory after the allocation
        struct MemoryList *restMem =(struct MemoryList*)malloc(sizeof(struct MemoryList));
        restMem->size = currentNode->size - requested;
        restMem->alloc = '0';
        restMem->ptr = currentNode->ptr + requested;


        // currentNode(block to be allocated) is the tail.
        if (currentNode->next == NULL) {
            restMem->next = NULL;                                       //  at the tail

        } else {
            // [restMem]->[crrNode's next]  sets the new block's next to the current block's next
            restMem->next = currentNode->next;
            //  [restMem]<-[crrNode's next]  sets the new block as current block-next's previous.
            currentNode->next->previous = restMem;
        }

        /* So right above we connect the new block's right side to the current block's next block.
         * and if the current block is the tail then the next block will be NULL.
         *
         * The code just right below connects the new block's left side to the current block.
         * The main aim is to connect the new block's right side first and then the left side, so in the end
         * the new block becomes a part of the nodes.
         * */

        // [crrNode]->[restMem]   sets the current block's next to the new block
        currentNode->next = restMem;
        // [crrNode]<-[restMem]     sets the new block's previous to the current block
        restMem->previous = currentNode;

        currentNode->alloc = '1';
        currentNode->size = requested;
        // next is a pointer to the last allocated block's next block, used in next-fit algorithm
        next = currentNode->next;
        return currentNode->ptr;

    } else  {
        currentNode->alloc = '1';
        next = currentNode->next;
        return currentNode->ptr;
    }
}




struct MemoryList* first_fit(size_t requested) {

    if (head != NULL) {
        struct MemoryList *currentNode = head;
        while (currentNode != NULL) {

            if ((currentNode->alloc != '1') && (currentNode->size >= requested)) {
                return currentNode;

            }
            currentNode = currentNode->next;
        }
    }
    return NULL;
}



struct MemoryList* worst_fit(size_t requested) {

    if (head == NULL) {return NULL;}

    int size = 0;
    struct MemoryList *currentNode = head;
    struct MemoryList *largestNode = NULL;


    while (currentNode != NULL) {
        if ((currentNode->alloc != '1') && (currentNode->size >= requested)) {

            if ( (currentNode->size > size) ) {
                largestNode = currentNode;
                size = currentNode->size;
            }
        }
        currentNode = currentNode->next;
    }
    return largestNode;
}


struct MemoryList* best_fit(size_t requested) {

    if (head == NULL) {return NULL;}

    struct MemoryList *currentNode = head;
    int size = mySize;
    struct MemoryList *mostSuitableNode = NULL;

    // While-loop to find the most suitable memory block that is free
    while (currentNode != NULL) {
        if ((currentNode->alloc != '1') && (currentNode->size >= requested)){

            if (currentNode->size == requested) {
                return currentNode;
            }

            if (currentNode->size <= size){
                mostSuitableNode = currentNode;
                size = currentNode->size;
            }
        }
        currentNode = currentNode->next;
    }
    return mostSuitableNode;
}




struct MemoryList *next_fit(size_t requested) {

    if ((next == NULL) && (head == NULL)) return NULL;

    struct MemoryList *crrNode = next;
    while (crrNode != NULL) {
        if ((crrNode->alloc != '1') && (crrNode->size >= requested) ) {
            return crrNode;
        }
        crrNode = crrNode->next;
    }

    struct MemoryList *crrNode2 = head;
    while (crrNode2 != next) {
        if ((crrNode2->alloc != '1') && (crrNode2->size >= requested) ) {
            return crrNode2;
        }
        crrNode2 = crrNode2->next;
    }
    return NULL;
}



/* Frees a block of memory previously allocated by mymalloc. */

void myfree(void * block) {

    struct MemoryList *trav = head;
    while (trav != NULL) {
        if (trav->ptr == block ) {
            trav->alloc = '0';


            /* First of all, trav is like the current block/node of the while-loop. So, inside the if statement we check if
             * the current block's previous block is not the head, and it is also not allocated(free block). If these two statements
             * are true then we remove the current/trav block and if previous block or next block is free then we reduce to
             * one large block by extending the size of the free block.
             * */
            if ((trav->previous != NULL) && (trav->previous->alloc == '0')) {
                trav->previous->size += trav->size;                                 // two empty memory blocks become one block

                struct MemoryList *temp = trav;
                if (temp->next != NULL) {                                       // not the tail
                    temp->previous->next = temp->next;
                    temp->next->previous = temp->previous;
                    trav =trav->previous;                                       // goes back to the previous iteration
                    if (temp == next) next = trav;
                    free(temp);
                }
            }

            // if next block is also free, becomes one block
            if ((trav->next != NULL) && (trav->next->alloc == '0') ) {
                trav->next->size += trav->size;

                struct MemoryList *temp = trav;
                if (temp->previous != NULL) {
                    temp->previous->next = temp->next;
                    temp->next->previous = temp->previous;
                    if (temp == next) next = temp->next;
                    free(temp);
                    break;
                }

                if (temp->previous == NULL){
                    head = temp->next;
                    next = head;
                    head->previous = NULL;
                    free(temp);
                    break;
                }

                break;
            }
        }
        trav = trav->next;
    }

}





/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes() {
    if (head == NULL) {
        printf("Memory in not initialized");
        return 0;
    }
    struct MemoryList *curr = head;
    int counter = 0;
    while (curr != NULL) {
        if (curr->alloc == '0') {
            counter++;
        }
        curr = curr->next;
    }

    return counter;
}

/* Get the number of bytes allocated */
int mem_allocated() {

    if (head == NULL) {
        printf("Memory in not initialized");
        return 0;
    }
    struct MemoryList *curr = head;
    int counter = 0;
    while (curr != NULL) {
        if (curr->alloc == '1') {
            counter++;
        }

        curr = curr->next;
    }

    return counter;
}

/* Number of non-allocated bytes */
int mem_free() {

    if (head == NULL) {
        printf("Memory in not initialized");
        return 0;
    }
    struct MemoryList *curr = head;
    int size = 0;
    while (curr != NULL) {
        if (curr->alloc == '0') {
            size += curr->size;
        }
        curr = curr->next;
    }
    return size;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free() {
    if (head == NULL) {
        printf("Memory in not initialized");
        return 0;
    }

    struct MemoryList *current = head;
    int size = 0;

    while (current != NULL) {
        if (current->alloc == '0') {
            if (current->size > size){
                size = current->size;
            }
        }
        current = current->next;
    }

    return size;
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size) {
    if (head == NULL) {
        printf("Memory is not initialized");
        return 0;
    }

    struct MemoryList *current = head;
    int counter = 0;
    while (current != NULL) {
        if (current->alloc == '0') {
            if (current->size <= size){
                counter++;
            }
        }
        current = current->next;
    }

    return counter;
}

char mem_is_alloc(void *ptr) {
    struct MemoryList *currenNode = head;
    while (currenNode != NULL) {
        if (currenNode->ptr == ptr) {
            return currenNode->alloc;
        }
        currenNode = currenNode->next;
    }
    return currenNode->alloc;
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool()
{
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total()
{
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy)
{
	switch (strategy)
	{
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy)
{
	if (!strcmp(strategy,"best"))
	{
		return Best;
	}
	else if (!strcmp(strategy,"worst"))
	{
		return Worst;
	}
	else if (!strcmp(strategy,"first"))
	{
		return First;
	}
	else if (!strcmp(strategy,"next"))
	{
		return Next;
	}
	else
	{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory() {
    struct MemoryList *temp = head;

    if (head == NULL){
        printf("Node is empty\n");
        return;
    }

    printf("Hello, World\n");
    while (temp != NULL) {
        printf("%d ", temp->size);
        temp = temp->next;
    }

	return;
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status()
{
	printf("%d out of %d bytes allocated.\n",mem_allocated(),mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/mem_holes());
}


/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	


	a = mymalloc(100);
	b = mymalloc(100);
	c = mymalloc(100);
	myfree(b);
	d = mymalloc(50);
	myfree(a);
	e = mymalloc(25);

	print_memory_status();
}
