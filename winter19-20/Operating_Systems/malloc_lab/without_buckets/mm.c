#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* 
Structure of USED block:
  Cause our maximal heap size is 4GiB = 2^(32)B and we are forced thet the blocks will be alligned to 16B the range of used address decrease 2^(32)/2^(4) = 2^28
  So wee need just 28 bits to represent size of our structure, we will also use 2 bit to control the state of the block - used/free/previous block is free.
  Real size of our block is multiple of 16B, do minimal size have to be 16 B (this is why 4 least significant bits will remain while changing size).
  Header visualisation:
      28 bits for size of our blk       2 bits for state
      <---------------------------><--><-->
                                2 free bits

  Real size of our structure will be multiple of size hold in header and sizeof(word_t).

  Used block visualisation:
      HEADER | PAYLOAD


Structure of free block:
  free block share the same properties as used block, but it uses diffrent the memmory.
  Free block visualisation:
      HEADER | PREV | NEXT | [PAYLOAD]* | FOOTER 

  Free block store extra information about previous and next block coded pointer to previous and next block in the bidirectional list of free blocks
  It also have a footer which is a copy of header. It is used to allow two free blocks to merge.

Heap structure at the beginning

  Allignment      Root of free list with:                 Last (header)
  to 16         Header, prev, next and Footer                 Header
<---------><-----------------------------------------><---------------------->
                            16B                               4B
              Root could be stored in more efficient
              way - prev and next, but it ease the
              work cause we can use functions which we 
              declared before. (cost 8B extra)

Allocating memory:
  Firstly trying to find smallest free blk in which we will fill from the list of free blocks.
  If we do not find such a block we have to expand our heap.
  To do it efficiently we are checking if there is not a free block at the end of the heap so we do not have to 
  allocate so much memory and use it to alloc memory and extend the heap in such a way that we will get the size we wanted.
  If there is not a free block we just increase the heap.
  At the end of the heap we have an 'empty' header (in such a way empty that it does not have a payload with it), which is the beginning
  of the next allocated block. It is used to store information if our previous block is free.

Freeing memory:
  When we free our block firstly we check if we have neighbours blocks which are also free and if it's true
  we try to merge with them, cause a bigger free block gives us much more opportunities.
  When we merge with our neighbours we delete them from the list of free blocks and then we add the result of our merge.
  We are inserint elements to list using LIFO (more effective then FIFO on tests)
*/

/* If you want debugging output, use the following macro.
 * When you hand in, remove the #define DEBUG line. */
//#define DEBUG
#ifdef DEBUG
#define debug(fmt, ...) printf("%s: " fmt "\n", __func__, __VA_ARGS__)
#define msg(...) printf(__VA_ARGS__)
#else
#define debug(fmt, ...)
#define msg(...)
#endif

#define __unused __attribute__((unused))

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* !DRIVER */
#define word 4 //instead using sizeof(word_t)

typedef int32_t word_t; /* Heap is bascially an array of 4-byte words. */

typedef enum {
  FREE = 0,     /* Block is free */
  USED = 1,     /* Block is used */
  PREVFREE = 2, /* Previous block is free (optimized boundary tags) */
  LAST = 3,
} bt_flags;

static word_t *heap_start; /* Address of the first block */
static word_t *heap_end;   /* Address past last byte of last block */
static word_t *last;       /* Points at last block - heap end behind last*/
static word_t *free_list;  /* Pointer to free block root */

/* --=[ boundary tag handling ]=-------------------------------------------- */

static inline word_t bt_size(word_t *bt) {
  return *bt & ~(USED | PREVFREE);
}

static inline int bt_used(word_t *bt) {
  return *bt & USED;
}

static inline int bt_free(word_t *bt) {
  return !(*bt & USED);
}

/* Given boundary tag address calculate it's buddy address. */
static inline word_t *bt_footer(word_t *bt) {
  assert(bt_free(bt));
  return (void *)bt + bt_size(bt) - word;
}

/* Given payload pointer returns an address of boundary tag. */
static inline word_t *bt_fromptr(void *ptr) {
  return (word_t *)ptr - 1;
}

/* Creates boundary tag(s) for given block. */
static inline void bt_make(word_t *bt, size_t size, bt_flags flags) {
  assert((size & (ALIGNMENT - 1)) == 0 || bt == last);
  *bt = size|flags;

  if(bt_free(bt)){                    // for a free block set footer
    word_t *footer = bt_footer(bt);
    *footer = size|flags;
  }
}

/* Previous block free flag handling for optimized boundary tags. */
static inline bt_flags bt_get_prevfree(word_t *bt) {
  return *bt & PREVFREE;
}

static inline void bt_clr_prevfree(word_t *bt) {
  if (bt)
    *bt &= ~PREVFREE;
}

static inline void bt_set_prevfree(word_t *bt) {
  *bt |= PREVFREE;
}

/* Returns address of payload. */
static inline void *bt_payload(word_t *bt) {
  return (word_t *)bt + 1;
}

/* Returns address of next block or NULL. */
static inline word_t *bt_next(word_t *bt) {
  word_t size = bt_size(bt);
  return (void *) bt + size;
}

/*Set the code of previous pointer and next pointer to element from free blk list*/
static inline void bt_set_ptr(word_t *bt, word_t *prev, word_t *next){
  *(bt + 1) = (word_t) ((void *) prev - (void *) free_list);
  *(bt + 2) = (word_t) ((void *) next - (void *) free_list);
}

/*Get previous pointer from free list which is calculated using reference to free_list pointer*/ 
static inline word_t *bt_get_prevptr(word_t *bt){
  assert(bt_free(bt));
  return (word_t*) ((void*) free_list + *((word_t *)bt + 1));
}

/*Get next pointer from free list which is calculated using reference to free_list pointer*/ 
static inline word_t *bt_get_nextptr(word_t *bt){
  assert(bt_free(bt));
  return (word_t*) ((void*) free_list + *((word_t *)bt + 2));
}

/*Function update the free list erasing given element*/
static inline void del_free_list(word_t *bt){
  word_t *next = bt_get_nextptr(bt);
  word_t *prev = bt_get_prevptr(bt);

  msg("prev: %p\nnext: %p\n",prev,next);

  bt_set_ptr(prev, bt_get_prevptr(prev), next);
  bt_set_ptr(next, prev, bt_get_nextptr(next));
  
  bt_clr_prevfree(bt_next(bt));
}

/*Function update the free list adding given element*/
static inline void insert_free_list(word_t *bt){
  //LIFO
  word_t *prev = bt_get_prevptr(free_list);
  
  bt_set_ptr(prev, bt_get_prevptr(prev), bt);
  bt_set_ptr(free_list, bt, bt_get_nextptr(free_list));
  bt_set_ptr(bt, prev, free_list);
  /*FIFO
  word_t *next = bt_get_nextptr(free_list);
  
  bt_set_ptr(next, bt, bt_get_nextptr(next));
  bt_set_ptr(free_list, bt_get_prevptr(free_list), bt);
  bt_set_ptr(bt, free_list, next);
  */
  msg("last: %p\n", last);
  bt_set_prevfree(bt_next(bt));
}

/* Returns address of previous block or NULL. */
static inline word_t *bt_prev(word_t *bt) {
  if(bt_get_prevfree(bt)){  //We can not refer to previous used block cause they do not have footers and we do not need it
    word_t prevsize = bt_size((void *)bt - word);
    return (void *)bt - prevsize;
  }
  return NULL;
}


/* --=[ miscellanous procedures ]=------------------------------------------ */

/* Calculates block size incl. header, footer & payload,
 * and aligns it to block boundary (ALIGNMENT). */
static inline size_t blksz(size_t size) {
  if((size + word) & (ALIGNMENT - 1))  //checking if size + sizeof header are alignet if no align them
    return (size + word + ALIGNMENT) & (~(ALIGNMENT-1));

  return size + word; 
}

static void *morecore(size_t size) {
  void *ptr = mem_sbrk(size);
  if (ptr == (void *)-1)
    return NULL;
  return ptr;
}

/*Function used to split the rest of the free block which was not used to mallock and add it to freelist*/
static inline void split(word_t *bt, size_t size){
  if(bt_size(bt) > size){
    size_t newsize = bt_size(bt) - size;
    word_t *free = (void*) bt + size;

    bt_make(free, newsize, FREE);
    insert_free_list(free); 
  }
}

/* --=[ mm_init ]=---------------------------------------------------------- */

int mm_init(void) {
  void *ptr = morecore(ALIGNMENT - word);
  if (!ptr)
    return -1;

  free_list = morecore(ALIGNMENT);  //init root of free blk
  if (!free_list)
    return -1;

  bt_make(free_list, ALIGNMENT, FREE);
  bt_set_ptr(free_list, free_list, free_list);

  last = morecore(word);  //pointer to header without payload
  if (!last)
    return -1;
  
  bt_make(last, 0, USED); //the only blk with size 0, cause last uses only 4B we will not break the convention that blk are alligned to 16
  heap_start = ptr;
  heap_end = last + word;
  return 0;
}

/* --=[ malloc ]=----------------------------------------------------------- */

#if 0
/* First fit startegy. */
static word_t *find_fit(size_t reqsz) {
  word_t *search = bt_get_nextptr(free_list); //searching for first element which will fit
  
  while(search != free_list){
    msg("Searching free %p\n",search);
    assert(bt_free(search));

    if(bt_size(search) >= reqsz){
      del_free_list(search);
      split(search, reqsz);
      return search;
    }
    search = bt_get_nextptr(search);
  }

  search = bt_prev(last); //if there is a free block at the end of heap enlarge it
  
  if(search == NULL)      //if not creat a new one
    search = last;
  else
    del_free_list(search);
    
  last = morecore(reqsz - bt_size(search));
  last = (void *) search + reqsz;
  bt_make(last, 0, USED);
  return search;
}

#else
/* Best fit startegy. */
static word_t *find_fit(size_t reqsz) {
  word_t *search = bt_get_nextptr(free_list);
  word_t *optimal = NULL;
  
  while(search != free_list){
    msg("Searching free %p\n",search);  //Try to find optimal free block
    assert(bt_free(search));
    
    if(bt_size(search) >= reqsz){
      if(optimal == NULL)
        optimal = search;
      if(bt_size(search) < bt_size(optimal))  //If found better: update
        optimal = search;
    }
    search = bt_get_nextptr(search);
  }

  if(optimal != NULL){          //If found a block in free list use it
      del_free_list(optimal);
      split(optimal, reqsz);
      return optimal;
  }
                              //Else: if there is a free block at the end of heap enlarge him
  search = bt_prev(last);     //if not creat a new one

  if(search == NULL)
    search = last;
  else
    del_free_list(search);
    
  last = morecore(reqsz - bt_size(search));
  last = (void *) search + reqsz;
  bt_make(last, 0, USED);
  return search;

}
#endif

void *malloc(size_t size) {
  msg("\n\nalloc %ld\n", size);
  if(size == 0)
    return NULL;
  
  size_t realsize = blksz(size);
  word_t *blk = find_fit(realsize);
  
  msg("size: %ld   pointer: %p\n",realsize, bt_payload(blk));
  
  bt_make(blk, realsize, USED);
  bt_clr_prevfree(bt_next(blk));
  
  return bt_payload(blk);
}

/* --=[ free ]=------------------------------------------------------------- */

void free(void *ptr) {
  msg("\n\nfree %p\n", ptr);
  word_t *blk = bt_fromptr(ptr);
  assert(!bt_free(blk));

  if(bt_get_prevfree(blk)){     //check if a blk before is free if yes connect
    word_t *prev = bt_prev(blk);
    del_free_list(prev);
    size_t sumsize = (size_t) bt_size(blk) + bt_size(prev); 
    bt_make(prev, sumsize, FREE);
    blk = prev;
  }
  
  word_t *next = bt_next(blk);  //the same but with next blk
  if(next != last){
    if(bt_free(next)){
      del_free_list(next);
      size_t sumsize = (size_t) bt_size(blk) + bt_size(next);
      bt_make(blk, sumsize, FREE);
    }
  }
  
  bt_make(blk, bt_size(blk), FREE);
  insert_free_list(blk);
}

/* --=[ realloc ]=---------------------------------------------------------- */

void *realloc(void *old_ptr, size_t size) {
  msg("\n\nrealloc %ld  %p\n", size, old_ptr);
  if(old_ptr == NULL)
    return malloc(size);
  
  if(size == 0){
    free(old_ptr);
    return NULL;
  }

  word_t *blk = bt_fromptr(old_ptr);
  size_t realsize = blksz(size);
  
  if(realsize <= bt_size(blk)){ //case when realloc want to shrink the blk
    split(blk, realsize);
    bt_make(blk, realsize, USED);
    return old_ptr;
  }
  
  word_t *next = bt_next(blk);
  
  if(next != last){
    if(bt_free(next) && (bt_size(next) + bt_size(blk) >= realsize)){ //case when we are able to enlarge using next blk, which is free
      
      del_free_list(next);
      split(next, realsize - bt_size(blk));
      bt_make(blk, realsize, USED);
      
      memset((void *)old_ptr + size, 0, realsize - size - word);
      return old_ptr;    
    }
  }

  void *ptr = malloc(size); //case when we have to alloc a new blk and free old one
  
  memset((void *)ptr + size, 0, realsize - size - word);
  memcpy(ptr, old_ptr, size);
  free(old_ptr);
  
  return ptr;
}

/* --=[ calloc ]=----------------------------------------------------------- */

void *calloc(size_t nmemb, size_t size) {
  size_t bytes = nmemb * size;
  void *new_ptr = malloc(bytes);
  if (new_ptr)
    memset(new_ptr, 0, bytes);
  return new_ptr;
}

/* --=[ mm_checkheap ]=----------------------------------------------------- */

void mm_checkheap(int verbose) {
  heap_end = (void *) last + word;

  assert(bt_used(last));  //last should be always used and with size 0
  assert((bt_size(last) == 0));
  
  word_t *search = bt_get_nextptr(free_list); //check the free list, if each element is free, size and addres are correct
  word_t *previous = free_list;
  while(search != free_list){
    word_t *next = bt_next(search);
    assert(bt_used(next));  //check if neighbours which are used

    assert(search < heap_end);  //check if the pointers are in range of heap
    assert(search >= heap_start);
    
    assert((bt_size(search) & (ALIGNMENT-1)) == 0); //check if size is correct - dividable by 16
    assert(bt_free(search)); //check if blks in free list are free
    assert(*search == *bt_footer(search)); //check if footer is the same as header

    assert(previous == bt_get_prevptr(search)); //check if the list is for sure bidirectional

    previous = search;
    search = bt_get_nextptr(search);
  }
}
