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

static inline void set_allocated(block_t *block) {
  header_t *header = get_header(block);
  header->header |= 1;
}

static inline void set_free(block_t *block) {
  header_t *header = get_header(block);
  header->header &= 0xfffffffe;
}

static inline void set_prev_allocated(block_t *block) {
  header_t *header = get_header(block);
  header->header |= 2;
}

static inline void set_prev_free(block_t *block) {
  header_t *header = get_header(block);
  header->header &= 0xfffffffd;
}

// it's not REAL SIZE!!!
static inline int32_t get_size(block_t *block) {
  header_t *h = get_header(block);
  int32_t size = (h->header / ALIGNMENT);
  return size;
}

static inline int32_t get_size_h(header_t *h) {
  int32_t size = (h->header / ALIGNMENT);
  return size;
}

static inline int32_t get_real_size(block_t *block) {
  return get_size(block) * ALIGNMENT;
}

static inline int32_t get_real_size_h(header_t *h) {
  return get_size_h(h) * ALIGNMENT;
}

static inline footer_t *get_footer(block_t *block) {
  footer_t *footer =
      (footer_t *)((void *)block + get_real_size(block) - sizeof(footer_t));
  return footer;
}

static inline void set_size(block_t *block, int32_t size) {
  assert(0 == (size & 0x0f));
  header_t *h = get_header(block);
  h->header &= 0x0f;
  h->header |= size;
  footer_t *f = get_footer(block);
  *f = *h;
}

static inline void set_real_size(block_t *block, int32_t size) {
  set_size(block, ALIGNMENT * (size / ALIGNMENT));
}

static inline block_t *get_prev_block(block_t *block) {
  footer_t *footer = (footer_t *)((void *)block - sizeof(footer_t));
  block_t *ret = (block_t *)((void *)block - get_real_size_h(footer)); 
  return ret;
}

static inline block_t *get_next_block(block_t *block) {
  block_t *ret = (block_t *)((void *)block + get_real_size(block));
  return ret;
}

static inline int is_allocated(block_t *block) {
  return (get_header(block)->header & 1);
}

static inline int is_free(block_t *block) { return !is_allocated(block); }

static inline int prev_is_allocated(block_t *block) {
  return (get_header(block)->header & 2);
}

static inline int prev_is_free(block_t *block) {
  return !prev_is_allocated(block);
}

static inline footer_t *get_prev_footer(block_t *block) {
  assert(prev_is_free(block));
  footer_t *footer = (footer_t *)((void *)block - sizeof(footer_t));
  return footer;
}

static inline void set_prev_block(block_t *block, block_t *ptr) {
  if (NULL == block)
    return;
  assert(is_free(block));
  if (NULL != ptr)
    assert(is_free(ptr));
  block->payload.data.free.prev = ptr;
}

static inline void set_next_block(block_t *block, block_t *ptr) {
  if (NULL == block)
    return;
  assert(is_free(block));
  if (NULL != ptr)
    assert(is_free(ptr));
  block->payload.data.free.next = ptr;
}

static inline int32_t min(int32_t a, int32_t b) { return a < b ? a : b; }

static size_t round_up(size_t size) {
  return (size + ALIGNMENT - 1) & -ALIGNMENT;
}

/*
 * mm_init - Called when a new trace starts.
 */
static block_t *mm_free_list;

int mm_init(void) {
  /* Pad heap start so first payload is at ALIGNMENT. */
  if ((long)mem_sbrk(ALIGNMENT - offsetof(block_t, payload.data.payload)) < 0)
    return -1;

  mm_free_list = NULL;
  return 0;
}

/*
 * malloc - Allocate a block by incrementing the brk pointer.
 *      Always allocate a block whose size is a multiple of the alignment.
 */
void *malloc(size_t size) {
  size = round_up(sizeof(block_t) + size + sizeof(footer_t));

  /* --- no free blocks */
  if (NULL == mm_free_list) {
    block_t *block = mem_sbrk(size);
    if ((int64_t)block < 0)
      return NULL;
    
    set_allocated(block);
    set_prev_allocated(block);
    set_real_size(block, size);
    debug("return %p\n", block->payload.data.payload);
    return block->payload.data.payload;
  }

  block_t *block = mm_free_list;

  for (; NULL != block; block = block->payload.data.free.next) {
    debug("try %d : %ld\n", get_real_size(block), size);
    if (get_real_size(block) >= size) {
      set_allocated(block);
      set_next_block(block->payload.data.free.prev,
                     block->payload.data.free.next);
      set_prev_block(block->payload.data.free.next,
                     block->payload.data.free.prev);
      block_t *next_block = get_next_block(block);

      if ((intptr_t)next_block <= (intptr_t)mem_heap_hi) {
        set_prev_allocated(next_block);
      }

      if (mm_free_list == block)
        mm_free_list = block->payload.data.free.next;

      debug("return %p\n", block->payload.data.payload);
      return block->payload.data.payload;
    }
  }

  block = (block_t *)mem_sbrk(size);
  set_allocated(block);
  if (is_allocated(get_prev_block(block)))
    set_prev_allocated(block);
  set_real_size(block, size);
  debug("return %p\n", block->payload.data.payload);
  return block->payload.data.payload;

  debug("return (nil)\n");
  return NULL;
}

/* --- block must be before next_block */
void merge(block_t *block, block_t *next_block) {
  debug("merge %p %p\n", block, next_block);
  assert(get_next_block(block) == next_block);

  set_next_block(next_block->payload.data.free.prev,
                 next_block->payload.data.free.next);
  set_prev_block(next_block->payload.data.free.next,
                 next_block->payload.data.free.prev);
  set_real_size(block, get_real_size(block) + get_real_size(next_block));
}

void free(void *ptr) {
  debug("free %p\n", ptr);
  block_t *block = (block_t *)(ptr - offsetof(block_t, payload));
  set_free(block);
  footer_t *footer = get_footer(block);
  header_t *header = get_header(block);
  
  if (NULL == mm_free_list) {
    mm_free_list = block;
    block->payload.data.free.prev = NULL;
    block->payload.data.free.next = NULL;
    *footer = *header;
    return;
  }

  block->payload.data.free.prev = NULL;
  block->payload.data.free.next = mm_free_list;
  set_prev_block(mm_free_list, block);
  mm_free_list = block;

  /*
  block_t *n = get_next_block(block);
  if (is_free(n)) {
    merge(block, n);
    return;
  }
  */

  /*
  block_t *p = get_prev_block(block);
  if (is_free(p)) {
    merge(p, block);
    mm_free_list = p;
    return;
  }
  */
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
}
