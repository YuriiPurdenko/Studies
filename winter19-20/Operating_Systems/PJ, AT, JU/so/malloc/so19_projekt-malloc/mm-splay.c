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
#define MASSERT
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

#ifdef MASSERT
#define massert(x) assert(x)
#else
#define massert(x)
#endif

// #define TREE_SIZE

typedef int32_t header_t;
typedef header_t footer_t;
typedef union payload payload_t;
typedef struct block block_t;
typedef struct node node_t;

struct node {
  node_t *left;
  node_t *right;
};

union payload {
  struct {
    node_t node;
  } splay;
  uint8_t payload[0];
};

//__attribute__((packed))
struct block {
  header_t header;
  payload_t payload;
};

static inline int cmplt(block_t *a, block_t *b);
static inline int cmpeq(block_t *a, block_t *b);
static inline int cmpgt(block_t *a, block_t *b);
static inline node_t *block_node(block_t *block);
static inline node_t **block_node_left(block_t *block);
static inline node_t **block_node_right(block_t *block);
static inline block_t *node_to_block(node_t *node);
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

/* --- splay implementation
 * http://www.link.cs.cmu.edu/link/ftp-site/splaying/top-down-splay.c */

node_t *splay(block_t *block, node_t *t) {
  node_t N, *l, *r, *y;
  if (NULL == t)
    return t;
  N.left = N.right = NULL;
  l = r = &N;
  
  for (;;) {
    if (cmplt(block, node_to_block(t))) {
      if (NULL == t->left)
        break;
      if (cmplt(block, node_to_block(t->left))) {
        y = t->left;
        t->left = y->right;
        y->right = t;
        t = y;
        if (NULL == t->left)
          break;
      }
      r->left = t;
      r = t;
      t = t->left;
    } else if (cmpgt(block, node_to_block(t))) {
      if (NULL == t->right)
        break;
      if (cmpgt(block, node_to_block(t->right))) {
        y = t->right;
        t->right = y->left;
        y->left = t;
        t = y;
        if (NULL == t->right)
          break;
      }
      l->right = t;
      l = t;
      t = t->right;
    } else {
      break;
    }
  }
  l->right = t->left;
  r->left = t->right;
  t->left = N.right;
  t->right = N.left;
  return t;
}

/* --- size is number of nodes in tree */
#ifdef TREE_SIZE
node_t *insert(node_t *new, node_t *t, int *size) {
#else
node_t *insert(node_t *new, node_t *t) {
#endif
  if (NULL == t) {
    new->left = new->right = NULL;
#ifdef TREE_SIZE
    *size = 1;
#endif
    return new;
  }
  t = splay(node_to_block(new), t);
  if (cmplt(node_to_block(new), node_to_block(t))) {
    new->left = t->left;
    new->right = t;
    t->left = NULL;
#ifdef TREE_SIZE
    (*size)++;
#endif
    return new;
  } else if (cmpgt(node_to_block(new), node_to_block(t))) {
    new->right = t->right;
    new->left = t;
    t->right = NULL;
#ifdef TREE_SIZE
    (*size)++;
#endif
    return new;
  } else {
    return t;
  }
}

#ifdef TREE_SIZE
node_t *delete(block_t *block, node_t *t, int *size) {
#else
node_t *delete(block_t *block, node_t *t) {
#endif
  node_t *x;
  if (NULL == t)
    return NULL;
  t = splay(block, t);
  if (cmpeq(block, node_to_block(t))) {
    if (NULL == t->left) {
      x = t->right;
    } else {
      x = splay(block, t->left);
      x->right = t->right;
    }
#ifdef TREE_SIZE
    (*size)--;
#endif
    return x;
  }
  return t;
}

node_t *find_block(int32_t size, node_t *t) {
  if (NULL == t)
    return NULL;
  if (size == block_size(node_to_block(t)))
    return t;
  if (size < block_size(node_to_block(t))) {
    node_t *ret = find_block(size, t->left);
    return ret ? ret : t;
  }
  return find_block(size, t->right);
}

node_t *find(int32_t size, node_t **t) {
  node_t *ret = find_block(size, *t);
  if (ret)
    *t = splay(node_to_block(ret), *t);
  return ret;
}

static inline size_t round_up(size_t size) {
  return (size + ALIGNMENT - 1) & -ALIGNMENT;
}

/*
 * mm_init - Called when a new trace starts.
 */
node_t *mm_root;

int mm_init(void) {
  /* Pad heap start so first payload is at ALIGNMENT. */
  if ((long)mem_sbrk(ALIGNMENT - offsetof(block_t, payload)) < 0)
    return -1;

  mm_root = NULL;
  return 0;
}

/*
 * malloc - Allocate a block by incrementing the brk pointer.
 *      Always allocate a block whose size is a multiple of the alignment.
 */

static block_t *split(block_t *block, int32_t size) {
  massert(block_is_free(block));

  int32_t old_size = block_size(block);
  block_set_size(block, size);

  block_t *new_block = block_phys_next(block);
  block_set_size(new_block, old_size - size);
  block_set_free(new_block);

  *block_node_left(new_block) = NULL;
  *block_node_right(new_block) = NULL;

  return new_block;
}

static inline block_t *block_alloc(int32_t size) {
  block_t *block = mem_sbrk(size);
  if ((long)block < 0)
    return NULL;

  block_set_size(block, size);
  return block;
}

static inline block_t *block_find(int32_t size, node_t **root) {
  node_t *ret = find(size, root);
  if (NULL == ret)
    return NULL;
  return node_to_block(ret);
}


void *malloc(size_t size) {
  //size = round_up(sizeof(block_t) + size + sizeof(footer_t));
  size = round_up(sizeof(block_t) + size);
  
  block_t *block = block_find(size, &mm_root);

  if (NULL == block) {
    block = block_alloc(size);
    if (NULL == block) {
      return NULL;
    }

    block_t *prev = block_phys_prev(block);
    block_prev_set_used(block);
    
    if (NULL != prev) {
      if (block_is_free(prev))
        block_prev_set_free(block);
    }

    block_set_used(block);

    return block_payload(block);
  }

  mm_root = delete(block, mm_root);

  if (block_size(block) >=
      size + sizeof(block_t) + sizeof(footer_t) + ALIGNMENT) {
    block_t *nblock = split(block, size);
    mm_root = insert(block_node(nblock), mm_root);
  }

  block_set_used(block);

  return block_payload(block);
}

void merge(block_t *block, block_t *next_block, node_t **root) {
  massert(block_is_free(block));
  massert(block_is_free(next_block));
  
  int32_t b_size = block_size(block);
  int32_t next_b_size = block_size(next_block);
  int32_t size = b_size + next_b_size;

  *root = delete(block, *root);
  *root = delete(next_block, *root);

  block_set_size(block, size);
  *root = insert(block_node(block), *root);
}

void free(void *ptr) {
  block_t *block = payload_to_block(ptr);
  block_set_free(block);

  block_t *p;
  while ((p = block_phys_prev(block)) != NULL) {
    if (block_is_free(p) && p != block) {
      merge(p, block, &mm_root);
      block = p;
    } else {
      break;
    }
  }

  while ((p = block_phys_next(block)) != NULL) {
    if (block_is_free(p) && p != block) {
      merge(block, p, &mm_root);
    } else {
      break;
    }
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

  size_t tmp_size = size;

  /* If old_ptr is NULL, then this is just malloc. */
  if (!old_ptr)
    return malloc(size);
  
  size = round_up(sizeof(block_t) + size);

  block_t *block = payload_to_block(old_ptr);
  for (;;) {
    block_t *next = block_phys_next(block);
    if (NULL != next && block_is_free(next)) {
      int32_t osize = block_size(block);
      int32_t nsize = block_size(next);
      mm_root = delete(next, mm_root);
      block_set_size(block, osize + nsize);
    } else {
      break;
    }
    if (block_size(block) >= size)
      return old_ptr;
  }

  void *new_ptr = malloc(tmp_size);

  /* If malloc() fails, the original block is left untouched. */
  if (!new_ptr)
    return NULL;

  /* Copy the old data. */
  //block_t *block = old_ptr - offsetof(block_t, payload);
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
void splay_check(node_t *node) {
  if (NULL == node)
    return;
  block_t *block = node_to_block(node);
  assert(block_is_free(block));
  splay_check(node->left);
  splay_check(node->right);
}


void mm_checkheap(int verbose) {
  splay_check(mm_root);
  return;
}

static inline int32_t header_size(header_t *header) {
  return (*header / ALIGNMENT) * ALIGNMENT;
}

static inline int32_t footer_size(footer_t *footer) {
  return (*footer / ALIGNMENT) * ALIGNMENT;
}

static inline void header_set_size(header_t *header, int32_t size) {
  massert(0 == (size & 0x3));
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
  massert((int64_t)block->payload.payload % ALIGNMENT == 0);
  return block->payload.payload;
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
  massert(header_size(block_header(block)) == footer_size(block_footer(block)));
  return header_size(block_header(block));
}

static inline int block_is_used(block_t *block) {
  massert(header_is_used(block_header(block)) ==
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
  massert(header_prev_is_used(block_header(block)) ==
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

static inline node_t *block_node(block_t *block) {
  massert(block_is_free(block));
  return &block->payload.splay.node;
}

static inline node_t **block_node_left(block_t *block) {
  massert(block_is_free(block));
  return &block->payload.splay.node.left;
}

static inline node_t **block_node_right(block_t *block) {
  massert(block_is_free(block));
  return &block->payload.splay.node.right;
}

static inline int cmplt(block_t *a, block_t *b) {
  int32_t size_a = block_size(a);
  int32_t size_b = block_size(b);
  if (size_a == size_b)
    return a < b;
  return size_a < size_b;
}

static inline int cmpeq(block_t *a, block_t *b) {
  return a == b;
}

static inline int cmpgt(block_t *a, block_t *b) {
  int32_t size_a = block_size(a);
  int32_t size_b = block_size(b);
  if (size_a == size_b)
    return a > b;
  return size_a > size_b;
}

static inline block_t *node_to_block(node_t *node) {
  void *ptr = node;
  ptr -= offsetof(block_t, payload);
  return (block_t *)ptr;
}
