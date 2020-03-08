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

typedef struct {
  /* --- structure of header
   * 0      -- 1 allocated block
   *        -- 0 free block
   * 1      -- 1 previous block is allocated
   *        -- 0 previous block is free
   * 
   * 3:2    -- reserved
   * 31:4   -- size (sizes are multpiles of 16)
   */
  int32_t header;
} header_t;

typedef header_t footer_t;

struct block;

typedef struct {
  union {
    struct {
      struct block *prev;
      struct block *next;
    } free;
    /* --- flexible arrays can't be member of unions */
    uint8_t payload[0];
  } data;
} payload_t;

typedef struct block {
  /* ---
   * allocated block  === free block
   *                  ===
   * header           === header
   * payload          === unallocated
   * optional padding === footer
   */
  header_t header;
  payload_t payload;
} block_t;

static inline header_t *get_header(block_t *block) {
  return &block->header;
}

static inline int32_t get_size_header(header_t *h) {
  int32_t size = (h->header / ALIGNMENT) * ALIGNMENT;
  return size;
}

static inline int32_t get_size_footer(footer_t *f) {
  /* --- footer is alias for header */
  return get_size_header(f);
}

static inline int32_t get_size(block_t *block) {
  header_t *h = get_header(block);
  return get_size_header(h);
}

static inline footer_t *get_footer(block_t *block) {
  footer_t *footer =
      (footer_t *)((void *)block + get_size(block) - sizeof(footer_t));
  return footer;
}

static inline void set_allocated(block_t *block) {
  header_t *header = get_header(block);
  /* --- set 0 bit */
  header->header |= 0x1;
  /* --- footer is copy of haeder */
  footer_t *f = get_footer(block);
  *f = *header;
}

static inline void set_free(block_t *block) {
  header_t *header = get_header(block);
  /* --- unset 0 bit */
  header->header &= 0xfffffffe;
  /* --- footer is copy of haeder */
  footer_t *f = get_footer(block);
  *f = *header;
}

static inline void set_prev_allocated(block_t *block) {
  header_t *header = get_header(block);
  /* --- set 1 bit */
  header->header |= 0x2;
  /* --- footer is copy of haeder */
  footer_t *f = get_footer(block);
  *f = *header;
}

static inline void set_prev_free(block_t *block) {
  header_t *header = get_header(block);
  /* -- unset 1 bit */
  header->header &= 0xfffffffd;
  /* --- footer is copy of haeder */
  footer_t *f = get_footer(block);
  *f = *header;
}

static inline void set_size(block_t *block, int32_t size) {
  /* --- ALIGNMENT 16 - last 4 bits should be 0 */
  size = ALIGNMENT * (size / ALIGNMENT);
  assert(0 == (size & 0x0f));
  header_t *h = get_header(block);
  h->header &= 0x0f;
  h->header |= size;
  /* --- footer is copy of haeder */
  footer_t *f = get_footer(block);
  *f = *h;
}

static inline footer_t *get_prev_footer(block_t *block) {
  footer_t *footer = (footer_t *)((void *)block - sizeof(footer_t));
  //assert((void *)footer >= mem_heap_lo());
  return footer;
}

static inline block_t *get_prev_block(block_t *block) {
  footer_t *footer = get_prev_footer(block);
  block_t *ret = (block_t *)((void *)block - get_size_footer(footer)); 
  //assert((void *)ret >= mem_heap_lo());
  return ret;
}

static inline block_t *get_next_block(block_t *block) {
  block_t *ret = (block_t *)((void *)block + get_size(block));
  //assert((void *)ret <= mem_heap_hi());
  return ret;
}

static inline int is_allocated(block_t *block) {
  return (get_header(block)->header & 0x1);
}

static inline int is_free(block_t *block) { return !is_allocated(block); }

static inline int prev_is_allocated(block_t *block) {
  return (get_header(block)->header & 0x2);
}

static inline int prev_is_free(block_t *block) {
  return !prev_is_allocated(block);
}

static inline size_t round_up(size_t size) {
  return (size + ALIGNMENT - 1) & -ALIGNMENT;
}

static inline size_t max(size_t a, size_t b) { return a > b ? a : b; }

/*
 * mm_init - Called when a new trace starts.
 */
block_t *mm_head;

int mm_init(void) {
  /* Pad heap start so first payload is at ALIGNMENT. */
  if ((long)mem_sbrk(ALIGNMENT - offsetof(block_t, payload.data.payload)) < 0)
    return -1;

  mm_head = NULL;

  return 0;
}

/*
 * malloc - Allocate a block by incrementing the brk pointer.
 *      Always allocate a block whose size is a multiple of the alignment.
 */
//static block_t *split(block_t *block, int32_t size) {
//  return NULL;
//}


void *malloc(size_t size) {
  debug("===== MALLOC =====\n");
  /* --- we need extra footer at the end */
  debug("malloc %ld -> ", size);
  size = max(size, 2 * sizeof(struct block*));
  debug("%ld -> ", size);
  size = round_up(sizeof(block_t) + size + sizeof(footer_t));
  debug("%ld\n", size);

  if (NULL == mm_head) {
    debug("we don't have free memory\n");
    block_t *block = mem_sbrk(size);
    if ((long)block < 0)
      return NULL;

    set_size(block, size);
    set_allocated(block);
    set_prev_allocated(block);
    
    debug("return %p -> %p\n", block, block->payload.data.payload);
    return block->payload.data.payload;
  }

  for (block_t *block = mm_head; NULL != block;
       block = block->payload.data.free.next) {
    assert(is_free(block));
    debug("try %p which size is %d\n", block, get_size(block));
    if (get_size(block) >= size) {
      debug("hit\n");
      
      set_allocated(block);
      
      /* --- fix pointers */
      if (NULL != block->payload.data.free.next) {
        block->payload.data.free.next->payload.data.free.prev =
            block->payload.data.free.prev;
      }
      if (NULL != block->payload.data.free.prev) {
        block->payload.data.free.prev->payload.data.free.next =
            block->payload.data.free.next;
      }
      if (mm_head == block) {
        mm_head = block->payload.data.free.next;
      }

      /* --- fix headers */
      block_t *n = get_next_block(block);
      if ((void *)n <= mem_heap_hi()) {
        set_prev_allocated(n);
      }

      debug("return %p -> %p\n", block, block->payload.data.payload);
      return block->payload.data.payload;
    }
  }

  debug("we don't have enough memory\n");
  block_t *block = mem_sbrk(size);
  if ((long)block < 0)
    return NULL;

  set_size(block, size);
  set_allocated(block);
  if (is_allocated(get_prev_block(block))) 
    set_prev_allocated(block);
  
  debug("return %p -> %p\n", block, block->payload.data.payload);
  return block->payload.data.payload;
}

/* --- block must be before next_block */
static void merge(block_t *block, block_t *next_block) {
  debug("===== MERGE =====\n");
  debug("merge blocks %p %p\n", block, next_block);
  /* --- blocks must be free */
  assert(is_free(block));
  assert(is_free(next_block));

  /* --- block must be before next_block */
  assert(block < next_block);

  /* --- block must be previous physical block for next block */
  assert(get_prev_block(next_block) == block);
  /* --- next_block must be next physical block for block */
  assert(get_next_block(block) == next_block);

  int32_t size = get_size(block) + get_size(next_block);
  debug("size = %d = %d + %d\n", size, get_size(block), get_size(next_block));
  
  /* --- fix pointers 
   * we have some sad caeses
   * 1) block is previous block for next_block
   * 2) block is next block for next_block
   * 3) they are not directly connected on list
   */
  
  /* --- if next_block is first on free list we need to update head */
  if (mm_head == next_block)
    mm_head = block;

  if (block->payload.data.free.next == next_block) {
    assert(next_block->payload.data.free.prev == block);
    /* --- it's okay prev and next are before footer */
    set_size(block, size); 

    /* --- set next free block for block to next free block of next_block */
    block->payload.data.free.next = next_block->payload.data.free.next;
    /* --- check if it's legal operation */
    if (NULL != block->payload.data.free.next) {
      assert(is_free(block->payload.data.free.next));
      /* --- set prevous free block for next free block to block */
      block->payload.data.free.next->payload.data.free.prev = block;
    }
    assert(get_size(block) == size);
    return;
  }

  if (block->payload.data.free.prev == next_block) {
    assert(next_block->payload.data.free.next == block);
    /* --- it's okay prev and next are before footer */
    set_size(block, size); 

    block->payload.data.free.prev = next_block->payload.data.free.prev;
    if (NULL != block->payload.data.free.prev) {
      assert(is_free(block->payload.data.free.prev));
      block->payload.data.free.prev->payload.data.free.next = block;
    }
    assert(get_size(block) == size);
    return;
  }

  /* --- the most funny case
   *
   * now we have
   *
   * a -> block -> b 
   * c -> next_block -> d
   *
   * and we want
   *
   * a -> block -> b
   * c -> d
   */
  
  set_size(block, size); 

  block_t *a = block->payload.data.free.prev;
  block_t *b = block->payload.data.free.next;

  if (NULL != a)
    a->payload.data.free.next = b;
  if (NULL != b)
    b->payload.data.free.prev = a;

  block_t *c = next_block->payload.data.free.prev;
  block_t *d = next_block->payload.data.free.next;

  if (NULL != c)
    c->payload.data.free.next = block;
  if (NULL != d)
    d->payload.data.free.prev = block;

  block->payload.data.free.prev = c;
  block->payload.data.free.next = d;
  assert(get_size(block) == size);
}

void free(void *ptr) {
  debug("===== FREE =====\n");
  debug("free %p\n", ptr);
  block_t *block = (block_t *)(ptr - offsetof(block_t, payload));
  debug("block at %p\n", block);

  set_free(block);

  if (NULL == mm_head) {
    if ((void *)get_next_block(block) <= mem_heap_hi())
      set_prev_free(get_next_block(block));
    block->payload.data.free.prev = NULL;
    block->payload.data.free.next = NULL;
    mm_head = block;
    return;
  }

  block->payload.data.free.next = mm_head;
  mm_head = block;
  block->payload.data.free.next->payload.data.free.prev = block;
  block->payload.data.free.prev = NULL;

  if ((void *)get_next_block(block) <= mem_heap_hi())
    set_prev_free(get_next_block(block));

  while ((void *)get_next_block(block) <= mem_heap_hi() &&
         is_free(get_next_block(block))) {
    merge(block, get_next_block(block));
  }
  while ((void *)get_prev_block(block) >= mem_heap_lo() &&
         is_free(get_prev_block(block))) {
    block_t *p = get_prev_block(block);
    /* --- padding */
    if (p == block)
      break;
    merge(p, block);
    debug("%p = %p\n", block, p);
    block = p;
  }
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
  size_t old_size = get_size(block);
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
  debug("===== CHECKHEAP =====\n");
  for (block_t *block = mm_head; NULL != block;
       block = block->payload.data.free.next) {
    debug("check %p\n", block);
    assert(is_free(block));
    if (block->payload.data.free.prev)
      assert(block->payload.data.free.prev->payload.data.free.next == block);
    if (block->payload.data.free.next)
      assert(block->payload.data.free.next->payload.data.free.prev == block);
    block_t *n = get_next_block(block);
    if ((void *)n <= mem_heap_hi()) {
      assert(is_allocated(n));
    }
  }
}
