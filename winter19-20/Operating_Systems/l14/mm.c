/* Wiktor Pilarczyk 308533 */
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
Segregated fit, which use first fit in the buckets.

ALIGNMENT B is used as the size of the smalles block which we can allocate.

Structure of USED block:
  Cause our maximal heap size is 4GiB = 2^(32)B and we are forced that the blocks will be alligned to 16B the range of different used address will be 2^(32)/2^(4) = 2^28
  So wee need just 28 bits to represent size of our structure, we will also use 2 bit to control the state of the block - used/free/previous block is free.
  Real size of our block is multiple of 16B, cause minimal size have to be 16 B (2^4 = 16 ~ this is why 4 least significant bits will remain while changing size).
  Header visualisation:
      28 bits for size of our blk     2 bits for state
      <---------------------------><----><----><-------------------------->
                                2 free bits           rest payload

  Real size of our structure will be multiple of size hold in header and sizeof(word_t).

  Used block visualisation:
      HEADER | PAYLOAD


Structure of free block:
  free block share the same properties as used block, but it uses diffrent the memmory.
  Free block visualisation:
      HEADER | PREV | NEXT | [PAYLOAD]* | FOOTER 
      4B        4B    4B

  Free block store extra information about previous and next block coded pointer to previous and next block in the bidirectional list of free blocks
  It also have a footer which is a copy of header. It is used to allow two free blocks to merge.

Heap structure at the beginning

  Allignment      Root of free lists with:                 Last (header)
  to 16         Header, prev, next and Footer                 Header
<---------><-----------------------------------------><---------------------->
                            16B * 20                            4B
              Root could be stored in more efficient
              way - prev and next or a pointer, but it is easier
              in such a way cause we can use functions which we 
              declared before. (cost 8B extra)

Buckets
  All buckets expect the last hold free block of size in range (2^i, 2^(i-1)-1) * ALIGNMENT B
  The last one holds all free blocks from range (2^i, inf)


Allocating memory:
  Firstly trying to find smallest bucket in which we could store the free blk.
  If we do not find such a block in the bucket we try to find it in the next bucket.
  To do it efficiently we are checking if there is a free block at the end of the heap so we could just exapnd it.
  If not we just increase the heap and create a new free block.
  At the end of the heap we have an 'empty' header (in such a way empty that it does not have a payload with it), which will be the beginning
  of the not yet allocated block. It is used to store information if our previous block is free.

Freeing memory:
  When we free our block firstly we check if we have neighbours blocks which are also free and if it's true
  we try to merge with them, cause a bigger free block gives us much more opportunities.
  When we merge with our neighbours we delete them from the list of free blocks and then we add the result of our merge to a list of free blocks in a corrext bucket.
  We are inserting elements to list using FIFO (more effective then LIFO on tests).
  Our buckets are represent how many minimal blocks could fit in them and they are hold blocks in range from 2^(x) to 2^(x + 1) - 1, */


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

#define word 4            // instead using sizeof(word_t)
#define BUCKETS 19       // number of buckets counting from zero, we have BUCKETS + 1 buckets, you can manipulate the constant and check the performance 
typedef int32_t word_t;   /* Heap is bascially an array of 4-byte words. */

typedef enum {
  FREE = 0,     /* Block is free */
  USED = 1,     /* Block is used */
  PREVFREE = 2, /* Previous block is free (optimized boundary tags) */
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
  //assert(bt_free(bt));
  return (void *)bt + bt_size(bt) - word;
}

/* Given payload pointer returns an address of boundary tag. */
static inline word_t *bt_fromptr(void *ptr) {
  return (word_t *)ptr - 1;
}

/* Creates boundary tag(s) for given block. */
static inline void bt_make(word_t *bt, size_t size, bt_flags flags) {
  //assert((size & (ALIGNMENT - 1)) == 0 || bt == last);
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

/* Set the code of previous pointer and next pointer to block from free block list */
static inline void bt_set_ptr(word_t *bt, word_t *prev, word_t *next){
  //assert((void *) prev >= (void *) free_list);
  //assert((void *) next >= (void *) free_list);
  *(bt + 1) = (word_t) ((void *) prev - (void *) free_list);
  *(bt + 2) = (word_t) ((void *) next - (void *) free_list);
}

/* Get previous pointer from free list which is calculated using reference to free_list pointer */ 
static inline word_t *bt_get_prevptr(word_t *bt){
  //assert(bt_free(bt));
  return (word_t*) ((void*) free_list + *((word_t *)bt + 1));
}

/* Get next pointer from free list which is calculated using reference to free_list pointer */ 
static inline word_t *bt_get_nextptr(word_t *bt){
  //assert(bt_free(bt));
  return (word_t*) ((void*) free_list + *((word_t *)bt + 2));
}

/* Returns address of previous block or NULL. */
static inline word_t *bt_prev(word_t *bt) {
  if(bt_get_prevfree(bt)){  // We can not refer to previous used block cause they do not have footers and we do not need it
    word_t prevsize = bt_size((void *)bt - word);
    return (void *)bt - prevsize;
  }
  return NULL;
}


/* --=[ miscellanous procedures ]=------------------------------------------ */

/* Function which check if our bucket consist just of the root */
static inline int bucket_empty(word_t *bt){
  return bt_get_prevptr(bt) == bt;
}

static inline word_t *next_bucket(word_t *bucket){
  return bucket + 4;
}

static inline int last_bucket(word_t *bucket){
  return bucket == (free_list + 4 * BUCKETS);
}

/* Function calculate bucket in which we could hold a free block of size size */
static inline word_t *which_bucket(size_t size){ 
  word_t *res = free_list;  // First bucket which consist of block size ALIGNMENT B
  size = size >> 4;         // To simplify our comparisions
  int var = 2;              // Not 1, because the while is running while our bucket is to small to hold block of size 
  while(size >= var && var <= (1 << BUCKETS)){
    res = next_bucket(res);
    var = var << 1;
  }
  return res;
}


/* Function return a block with minimum size of size from correct bucket */
static inline word_t *find_free_blk(word_t size){
  word_t *res = which_bucket(size);

  msg("In Find free blk minimal bucket: %p\n", res);
  
  word_t *search = bt_get_nextptr(res);

  while(search != res){ //Check if there is a block of the expected size
    if(bt_size(search) >= size)
      return search;
    search = bt_get_nextptr(search);
  }
  
  if(last_bucket(res))     //If res is not pointing to the last bucket we try to find it in a next one, 
    return NULL;            //in which we will be sure that all blocks are greater/equal then our requested 
                            //size if the bucket is not empty
  res = next_bucket(res);
  while(bucket_empty(res) && !last_bucket(res))
    res = next_bucket(res);

  if(bucket_empty(res)) //If the bucket is empty it means we got to the last bucket and it do not have any free blocks
    return NULL;

  msg("Found bucket: %p\n", res);
  return bt_get_nextptr(res);
}

/*Function update the free list erasing given element*/
static inline void del_free_list(word_t *bt){
  word_t *next = bt_get_nextptr(bt);
  word_t *prev = bt_get_prevptr(bt);

  msg("DEL frm free list\nprev: %p\nnext: %p\n",prev,next);

  bt_set_ptr(prev, bt_get_prevptr(prev), next);
  bt_set_ptr(next, prev, bt_get_nextptr(next));
  
  bt_clr_prevfree(bt_next(bt));
}

/*Function update the free list adding given element*/
static inline void insert_free_list(word_t *bt){
  //FIFO
  word_t *free_blk = which_bucket(bt_size(bt));
  word_t *prev = bt_get_prevptr(free_blk);
  
  msg("Blk size %d addr %p  insert to bukcet %p\n", bt_size(bt), bt, free_blk);
  
  bt_set_ptr(prev, bt_get_prevptr(prev), bt);
  bt_set_ptr(free_blk, bt, bt_get_nextptr(free_blk));
  bt_set_ptr(bt, prev, free_blk);
  
  msg("Check %p\n", bt_get_nextptr(free_blk));
  /*LIFO
  word_t *next = bt_get_nextptr(free_list);
  
  bt_set_ptr(next, bt, bt_get_nextptr(next));
  bt_set_ptr(free_list, bt_get_prevptr(free_list), bt);
  bt_set_ptr(bt, free_list, next);
  */
  msg("last: %p\n", last);
  bt_set_prevfree(bt_next(bt));
}

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

  msg("SPLIT\n");
  if(bt_size(bt) > size){
    size_t newsize = bt_size(bt) - size;
    word_t *free = (void*) bt + size;

    bt_make(free, newsize, FREE);
    insert_free_list(free); 
  }
}

/* --=[ mm_init ]=---------------------------------------------------------- */
/*Function initialize buckets and last header*/
int mm_init(void) {
  msg("\n\n\nINIT\n");
  void *ptr = morecore(ALIGNMENT - word);
  if (!ptr)
    return -1;

  free_list = morecore(ALIGNMENT);  //init first root of free blk list
  if (!free_list)
    return -1;
  msg("bracket %d: %p\n", 0, free_list);
  bt_make(free_list, ALIGNMENT, FREE);
  bt_set_ptr(free_list, free_list, free_list);
  
  int x = BUCKETS;  //init rest of free block lists
  while(x--){
    void *var = morecore(ALIGNMENT);
    if(!var)
      return -1;
    msg("bracket %d: %p\n", BUCKETS - x, var);
    bt_make(var, ALIGNMENT, FREE);
    bt_set_ptr(var, var, var);
  }

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
/* First fit startegy. NOT SUPPORTED FOR BUCKETS*/
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
/* Segregated free list*/
static word_t *find_fit(size_t reqsz) {
  word_t *search = find_free_blk(reqsz);
  msg("SEARCH: %p\n",search);
  if(search != NULL){          //If found a block in free list use it
      del_free_list(search);
      split(search, reqsz);
      bt_make(search, reqsz, USED);
      return search;
  }
                              //Else: if there is a free block at the end of heap enlarge him
  search = bt_prev(last);     //if not creat a new one
  if(search == NULL)
    search = last;
  else
    del_free_list(search);
  if(reqsz <= bt_size(search)){   //we always take a bucket in which we are sure that elements are bigger
    del_free_list(search);        //there is a possibility that there will be an element with accurate size
    split(search, reqsz);         //but in the bucket before
    bt_make(search, reqsz, USED);
    return search;
  }

  msg("SEARCH: %p size search: %d  reqs %ld\n",search, bt_size(search), reqsz);
  if(reqsz <= 256 && reqsz >= 64){	//Part of policy increase internal fragmentation to decrease external fragmentation in some situatuions
    last = morecore(reqsz - bt_size(search) + ALIGNMENT);
    
    if(!last)
      return NULL;

    last = (void *) search + reqsz + ALIGNMENT;  
    bt_make(search, reqsz + ALIGNMENT, USED);
  }
  else{
    last = morecore(reqsz - bt_size(search));
    
    if(!last)
      return NULL;
    
    last = (void *) search + reqsz;  
    bt_make(search, reqsz, USED);
  }
  bt_make(last, 0, USED);
  
  return search;

}
#endif

void *malloc(size_t size) {
  msg("\n\nmalloc %ld\n", size);
  if(size == 0)
    return NULL;
  
  size_t realsize = blksz(size);
  word_t *blk = find_fit(realsize);
  
  if(!blk)
      return NULL;

  msg("size: %d   pointer: %p\n", bt_size(blk), bt_payload(blk));
  
  bt_clr_prevfree(bt_next(blk));
  
  return bt_payload(blk);
}

/* --=[ free ]=------------------------------------------------------------- */

void free(void *ptr) {
  msg("\n\nfree %p\n", ptr);
  word_t *blk = bt_fromptr(ptr);
  //assert(!bt_free(blk));

  if(bt_get_prevfree(blk)){     //check if a blk before is free if yes connect
    msg("prevfree\n");

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

  /* Without works faster on tests
    WITH: 
        25 25     85%  194878  0.009637  20221
        Perf index = 60 (util) & 20 (thru) = 80/100

    WITHOUT:
        25 25     87%  194878  0.001917 101655
        Perf index = 60 (util) & 40 (thru) = 100/100

  if(bt_get_prevfree(blk)){     //check if a blk before is free if yes connect
    word_t *prev = bt_prev(blk);
    del_free_list(prev);

    size_t block_size = bt_size(blk);
    size_t sumsize = (size_t) block_size + bt_size(prev); 
    msg("block: %ld, preav: %d\n", block_size, bt_size(prev));
    memmove(bt_payload(prev), old_ptr , block_size - word);
    old_ptr = bt_payload(prev);
    
    blk = prev;
    bt_make(blk, sumsize, USED);
    if(sumsize >= realsize){
      msg("REALISZE %ld sumsize: %ld\n", realsize, sumsize);
      split(blk, realsize);
      bt_make(blk, realsize, USED);
      msg("%p", prev);
      return old_ptr;
    }
  }
  */

  word_t *next = bt_next(blk);
  
  if(next != last){
    if(bt_free(next) && (bt_size(next) + bt_size(blk) >= realsize)){ //case when we are able to enlarge using next blk, which is free
      
      del_free_list(next);
      split(next, realsize - bt_size(blk));
      
      bt_make(blk, realsize, USED);
      
      return old_ptr;    
    }
  }

  void *ptr = malloc(size); //case when we have to alloc a new blk and free old one
  
  if(!ptr)
      return NULL;
  
  size_t block_size = bt_size(blk);

  memcpy(ptr, old_ptr , block_size - word);

  free(old_ptr);  //I can not free it before malloc cause i could rewrite the information, not only with pointers of free lists but also when i split block in mallock with the new free block boundary tag
  
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

  word_t *list = free_list;
  int x = BUCKETS + 1;
  while(x--){
    word_t *search = bt_get_nextptr(list); //check the free list[bucket], if each element is free, size and addres are correct
    word_t *previous = list;
    while(search != list){
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
    list += 4;
  }
}
