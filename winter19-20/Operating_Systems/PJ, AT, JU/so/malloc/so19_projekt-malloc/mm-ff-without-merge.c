/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  Blocks are never coalesced or reused.  The size of
 * a block is found at the first aligned word before the block (we need
 * it for realloc).
 *
 * This code is correct and blazingly fast, but very bad usage-wise since
 * it never frees anything.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
//#define DEBUG
#ifdef DEBUG
#define debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

typedef int32_t header_t;
typedef header_t footer_t;

struct block;

typedef union {
  struct {
    struct block *next;
    struct block *prev;
    footer_t footer;
  } free;
  uint8_t payload[0];
} payload_t;

typedef struct block {
  header_t header;
  payload_t payload;
} block_t;

static inline int32_t header_size(header_t *header);
static inline int32_t footer_size(footer_t *footer);
static inline void header_set_size(header_t *header, int32_t size);
static inline int header_is_used(header_t *header);
static inline int header_is_free(header_t *header);
static inline int footer_is_used(footer_t *footer);
static inline int footer_is_free(footer_t *footer);
static inline void header_set_used(header_t *header);
static inline void header_set_free(header_t *header);
static inline int header_prev_is_used(header_t *header);
static inline int footer_prev_is_used(footer_t *footer);
static inline int header_prev_is_free(header_t *header);
static inline int footer_prev_is_free(footer_t *footer);
static inline void header_prev_set_used(header_t *header);
static inline void header_prev_set_free(header_t *header);
static inline header_t *block_header(block_t *block);
static inline footer_t *block_footer(block_t *block);
static inline void block_set_size(block_t *block, int32_t size);
static inline void block_footer_sync(block_t *block);
static inline void *block_payload(block_t *block);
static inline block_t *block_free_next(block_t *block);
static inline block_t *block_free_prev(block_t *block);
static inline block_t *block_phys_next(block_t *block);
static inline block_t *block_phys_prev(block_t *block);
static inline int32_t block_size(block_t *block);
static inline int block_is_used(block_t *block);
static inline int block_is_free(block_t *block);
static inline void block_set_used(block_t *block);
static inline void block_set_free(block_t *block);
static inline int block_prev_is_used(block_t *block);
static inline int block_prev_is_free(block_t *block);
static inline void block_prev_set_used(block_t *block);
static inline void block_prev_set_free(block_t *block);
static inline block_t *payload_to_block(void *ptr);

static inline size_t round_up(size_t size) {
  return (size + ALIGNMENT - 1) & -ALIGNMENT;
}

/*
 * mm_init - Called when a new trace starts.
 */
block_t *mm_list;

int mm_init(void) {
  /* Pad heap start so first payload is at ALIGNMENT. */
  if ((long)mem_sbrk(ALIGNMENT - offsetof(block_t, payload)) < 0)
    return -1;

  mm_list = NULL;

  return 0;
}

/*
 * malloc - Allocate a block by incrementing the brk pointer.
 *      Always allocate a block whose size is a multiple of the alignment.
 */
static block_t *ff(block_t *block, int32_t size) {
  if (NULL == block)
    return NULL;
  assert(block_is_free(block));
  if (block_size(block) >= size)
    return block;
  return ff(block_free_next(block), size);
}

static void mm_list_insert(block_t *block) {
  assert(block_is_free(block));

  if (NULL == mm_list) {
    mm_list = block;
    block->payload.free.prev = NULL;
    block->payload.free.next = NULL;
    return;
  }

  block->payload.free.prev = NULL;
  block->payload.free.next = mm_list;
  mm_list->payload.free.prev = block;
  mm_list = block;
}

static void split(block_t *block, int32_t size) {
  assert(block_is_free(block));
  assert(block_size(block) >= size + sizeof(block_t));

  block_t *next = block->payload.free.next;
  
  int32_t old_size = block_size(block);
  block_set_size(block, size);

  block_t *new_block = block_phys_next(block);
  block_set_size(new_block, old_size - size);
  block_set_free(new_block);
  
  /* 
   * block -> B
   * block -> new block -> B
   */

  new_block->payload.free.prev = block;
  new_block->payload.free.next = next;

  block->payload.free.next = new_block;

  if (next) {
    assert(block_is_free(next));
    next->payload.free.prev = new_block;
  }
}

static void mm_list_erase(block_t *block) {
  if (mm_list == block) {
    mm_list = block->payload.free.next;
  }

  /* A <-> block <-> B
   * A -> B
   */
  if (block->payload.free.prev)
    block->payload.free.prev->payload.free.next = block->payload.free.next;
  
  /* A <- block <- B
   * A <- B
   */
  if (block->payload.free.next)
    block->payload.free.next->payload.free.prev = block->payload.free.prev;
}

static block_t *block_alloc(int32_t size) {
  block_t *block = mem_sbrk(size);
  if ((long)block < 0)
    return NULL;

  block_set_size(block, size);
  return block;
}

void *malloc(size_t size) {
  size = round_up(sizeof(block_t) + size);

  debug("malloc(0x%lx)\n", size);

  block_t *block = ff(mm_list, size);

  if (NULL == block) {
    block = block_alloc(size);
    if (NULL == block)
      return NULL;

    block_t *prev = block_phys_prev(block);
    block_prev_set_used(block);
    
    if (NULL != prev) {
      if (block_is_free(prev))
        block_prev_set_free(block);
    }

    block_set_used(block);

    debug("malloced %p\n", block);
    return block_payload(block);
  }


  if (block_size(block) >= size + sizeof(block_t)) {
    split(block, size);
  } 
  
  block_set_used(block);

  mm_list_erase(block);

  debug("malloced %p\n", block);
  return block_payload(block);
}

void free(void *ptr) {
  block_t *block = payload_to_block(ptr);
  debug("freed %p\n", block);
  block_set_free(block);
  mm_list_insert(block);
  return;
}

/*
 * realloc - Change the size of the block by mallocing a new block,
 *      copying its data, and freeing the old block.
 **/
void *realloc(void *old_ptr, size_t size) {
  /* If size == 0 then this is just free, and we return NULL. */
  if (size == 0) {
    free(old_ptr);
    return NULL;
  }

  /* If old_ptr is NULL, then this is just malloc. */
  if (!old_ptr)
    return malloc(size);

  void *new_ptr = malloc(size);

  /* If malloc() fails, the original block is left untouched. */
  if (!new_ptr)
    return NULL;

  /* Copy the old data. */
  block_t *block = old_ptr - offsetof(block_t, payload);
  size_t old_size = block_size(block);
  if (size < old_size)
    old_size = size;
  memcpy(new_ptr, old_ptr, old_size);

  /* Free the old block. */
  free(old_ptr);

  return new_ptr;
}

/*
 * calloc - Allocate the block and set it to zero.
 */
void *calloc(size_t nmemb, size_t size) {
  size_t bytes = nmemb * size;
  void *new_ptr = malloc(bytes);

  /* If malloc() fails, skip zeroing out the memory. */
  if (new_ptr)
    memset(new_ptr, 0, bytes);

  return new_ptr;
}

/*
 * mm_checkheap - So simple, it doesn't need a checker!
 */
void mm_checkheap(int verbose) {
  return;
}

static inline int32_t header_size(header_t *header) {
  return (*header / ALIGNMENT) * ALIGNMENT;
}

static inline int32_t footer_size(footer_t *footer) {
  return (*footer / ALIGNMENT) * ALIGNMENT;
}

static inline void header_set_size(header_t *header, int32_t size) {
  assert(0 == (size & 0x3));
  *header &= 0x3;
  *header |= size;
}

static inline int header_is_used(header_t *header) {
  return *header & 0x1;
}

static inline int header_is_free(header_t *header) {
  return !header_is_used(header);
}

static inline int footer_is_used(footer_t *footer) {
  return *footer & 0x1;
}

static inline int footer_is_free(footer_t *footer) {
  return !footer_is_used(footer);
}

static inline void header_set_used(header_t *header) {
  *header |= 0x1;
}

static inline void header_set_free(header_t *header) {
  *header &= 0xfffffffe;
}

static inline int header_prev_is_used(header_t *header) {
  return *header & 0x2;
}

static inline int footer_prev_is_used(footer_t *footer) {
  return *footer & 0x2;
}

static inline int header_prev_is_free(header_t *header) {
  return !header_prev_is_used(header);
}

static inline int footer_prev_is_free(footer_t *footer) {
  return !footer_prev_is_used(footer);
}

static inline void header_prev_set_used(header_t *header) {
  *header |= 0x2;  
}

static inline void header_prev_set_free(header_t *header) {
  *header &= 0xfffffffd;
}

static inline header_t *block_header(block_t *block) {
  return &block->header;
}

static inline footer_t *block_footer(block_t *block) {
  int32_t size = header_size(block_header(block));
  void *ptr = block;
  ptr += size;
  ptr -= sizeof(footer_t);
  return (footer_t *)ptr;
}

static inline void block_set_size(block_t *block, int32_t size) {
  header_set_size(block_header(block), size);
  block_footer_sync(block);
}

static inline void block_footer_sync(block_t *block) {
  *block_footer(block) = *block_header(block);
}

static inline void *block_payload(block_t *block) {
  assert((int64_t)block->payload.payload % ALIGNMENT == 0);
  return block->payload.payload;
}

static inline block_t *block_free_next(block_t *block) {
  assert(block_is_free(block));
  if (NULL == block->payload.free.next)
    return NULL;
  assert(block_is_free(block->payload.free.next));
  return block->payload.free.next;
}

static inline block_t *block_free_prev(block_t *block) {
  assert(block_is_free(block));
  if (NULL == block->payload.free.prev)
    return NULL;
  assert(block_is_free(block->payload.free.prev));
  return block->payload.free.prev;
}

static inline block_t *block_phys_next(block_t *block) {
  int32_t size = block_size(block);
  void *ptr = block;
  ptr += size;
  if (ptr > mem_heap_hi())
    return NULL;
  return (block_t *)ptr;
}

static inline block_t *block_phys_prev(block_t *block) {
  void *ptr = block;
  ptr -= sizeof(footer_t);
  if (ptr < mem_heap_lo())
    return NULL;
  
  footer_t *footer = (footer_t *)ptr;
  ptr = block;
  ptr -= footer_size(footer);
  
  if (ptr < mem_heap_lo() || ptr == block)
    return NULL;

  return (block_t *)ptr;
}

static inline int32_t block_size(block_t *block) {
  assert(header_size(block_header(block)) == footer_size(block_footer(block)));
  return header_size(block_header(block));
}

static inline int block_is_used(block_t *block) {
  assert(header_is_used(block_header(block)) ==
         footer_is_used(block_footer(block)));
  return header_is_used(block_header(block));
}

static inline int block_is_free(block_t *block) {
  return !block_is_used(block);
}

static inline void block_set_used(block_t *block) {
  header_set_used(block_header(block));
  block_footer_sync(block);

  block_t *next = block_phys_next(block);
  if (NULL != next) {
    block_prev_set_used(next);
  }
}

static inline void block_set_free(block_t *block) {
  header_set_free(block_header(block));
  block_footer_sync(block);

  block_t *next = block_phys_next(block);
  if (NULL != next) {
    block_prev_set_free(next);
  }
}

static inline int block_prev_is_used(block_t *block) {
  assert(header_prev_is_used(block_header(block)) ==
         footer_prev_is_used(block_footer(block)));
  return header_prev_is_used(block_header(block));
}

static inline int block_prev_is_free(block_t *block) {
  return !block_prev_is_used(block);
}

static inline void block_prev_set_used(block_t *block) {
  header_prev_set_used(block_header(block));
  block_footer_sync(block);
}

static inline void block_prev_set_free(block_t *block) {
  header_prev_set_free(block_header(block));
  block_footer_sync(block);
}

static inline block_t *payload_to_block(void *ptr) {
  ptr -= offsetof(block_t, payload);
  return (block_t *)ptr;
}
