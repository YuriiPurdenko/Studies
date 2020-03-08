#include "csapp.h"
#include "bitstring.h"

typedef struct {
  int data[8];          /* contents does not matter, sizeof(...) = 32 */
} object_t;

#define ARENA_EXTRA                                                            \
  struct {                                                                     \
    size_t nitems;      /* number of items */                                  \
    size_t nfree;       /* number of free items */                             \
    void *items;        /* pointer to first item */                            \
    bitstr_t bitmap[0]; /* bitmap of free items */                             \
  }

#include "arena.h"


/* 
  let's now use 2-level bitmap. The most optimal layout is to put 2038 objects.
   
  structure of single arena:

  START OF THE ARENA
  +------------+ 
  |arena header| - 32 bytes
  +------------+
  |L2 bitmap   | - 8 bytes (but actually we use 4)
  +------------+
  |L1 bitmap   | - 256 bytes (but accualy we use 255)
  +------------+
  |unused      | - 16 bytes
  +------------+
  |objects     | - 2038 * 32 bytes
  |            |
  ..............
  |            |
  +------------+
  END OF THE ARENA

 */

// We could find formulas for calculating those sizes, but i don't see point doing this
const size_t L2_size = 8;
const size_t L1_size = 256;


static arenalist_t arenas = STAILQ_HEAD_INITIALIZER(arenas);

static arena_t *init_arena(arena_t *ar) {
  /* TODO: Calculate nitems given ARENA_SIZE, size of arena_t and object_t. */
  size_t freemem = ARENA_SIZE - sizeof(arena_t); // subtract memory used for a header
  
  size_t nitems = 64 * 8 * freemem / (64 * 8 * sizeof(object_t) + 1 + 64);
  ar->nitems = nitems;
  ar->nfree = nitems;

  memset(ar->bitmap, 255, (L1_size + L2_size) * 8);

  /* Determine items address that is aligned properly. */
  ar->items = arena_end(ar) - nitems * sizeof(object_t);
  return ar;
}

static void *alloc_block(arena_t *ar) {
  assert(ar->nfree > 0);

  int index;
  /* TODO: Calculate index of free block and mark it used, update nfree. */

  unsigned long* l2_word = (unsigned long*)ar->bitmap;
  int index_l2 = ffsl(*l2_word) - 1; // it tells in which word of L1 should we search next
  unsigned long* l1_word = (unsigned long*)(ar->bitmap + L2_size + 8 * index_l2);
  int index_l1 = ffsl(*l1_word) - 1;
  index = index_l2 * 64 + index_l1;

  assert(index_l2 != -1);
  assert(index_l1 != -1);

  *l1_word &= ~(1L << index_l1);
  if(*l1_word == 0L)
    *l2_word &= ~(1L << index_l2);

  ar->nfree--;

  return ar->items + sizeof(object_t) * index;
}

static void free_block(arena_t *ar, void *ptr) {
  int index = (ptr - ar->items) / sizeof(object_t);
  /* TODO: Determine if ptr is correct and mark it free, update nfree. */
  assert(0 <= index && index < ar->nitems); // range
  assert(((uintptr_t) ptr & (sizeof(object_t) - 1)) == 0); // alignment

  int bit_index = index % 64;
  int l1_word_index = index / 64; // index of word in L1 bitmap
  int l2_word_index = l1_word_index / 64;
  unsigned long* l1_word = (unsigned long*)(ar->bitmap + L2_size + 8 * l1_word_index);
  unsigned long* l2_word = (unsigned long*)(ar->bitmap + 8 * l2_word_index);

  *l1_word |= (1L << bit_index);
  *l2_word |= (1L << (l1_word_index % 64));

  ar->nfree++;
}

static void *objalloc(void) {
  /* Find arena with at least one free item. */
  arena_t *ar = NULL;
  STAILQ_FOREACH(ar, &arenas, arenalink) {
    if (ar->nfree > 0)
      return alloc_block(ar);
  }
  /* If none found then allocate an item from new arena. */
  return alloc_block(init_arena(alloc_after_arena(&arenas, NULL)));
}

static void objfree(void *ptr) {
  if (ptr == NULL)
    return;
  arena_t *ar = find_ptr_arena(&arenas, ptr);
  assert(ar != NULL);
  free_block(ar, ptr);
}

static void objmemcheck(void) {
  arena_t *ar;
  STAILQ_FOREACH(ar, &arenas, arenalink) {
    /* Check if nfree matches number of cleared bits in bitmap. */
    size_t nused = 0;
    for (int i = 0; i < ar->nitems; i++)
      nused += bit_test(ar->bitmap + L2_size, i) ? 0 : 1;
    assert(nused == ar->nitems - ar->nfree);
  }
}

/* The test */

#define MAX_PTRS 10000
#define CYCLES 100

static void *alloc_fn(int *lenp) {
  *lenp = sizeof(object_t);
  return objalloc();
}

#define free_fn objfree
#define memchk_fn objmemcheck

#include "test.h"
