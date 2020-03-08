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

/* If you want debugging output, use the following macro.
 * When you hand in, remove the #define DEBUG line. */
// #define DEBUG
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

/*
Maksymalny rozmiar sterty wynosi 4GiB = 2^(32)B
ponieważ bloki muszą być wyrównane do 16 bajtów więc liczba bloków, które rzeczywiście będziemy adresować wynosi - 2^32/2^4 = 2^28 (2^4 = 16)
Bloki będą zawierały payload o nieparzystej liczbie  8 Bajtowych slow

Więc header dla bloków zajętych będzie mial postać
  28 bitów na rozmiar bloku   2- nic     2 - bity czy poprzedni blok jest wolny i czy my jestesmy zajeci
<---------------------------><--------><--------->

Struktura bloku zajetego to head | payload

struktura bloku wolnego to header | previous | next | wolne | footer
footer i header sa identyczne, a previous i next sa wskaznikami (lub zakodowanymi wskaznikami), gdzie znajduja sie poprzednie i nastepne wolne bloki na liscie



	
*/
typedef int32_t word_t; /* Heap is bascially an array of 4-byte words. */

typedef enum {
  FREE = 0,     /* Block is free */
  USED = 1,     /* Block is used */
  PREVFREE = 2, /* Previous block is free (optimized boundary tags) */
} bt_flags;


static word_t *free_list; // Adres listy wolnych blokow

#define WSIZE 4
#define MAX(x, y) ((x) > (y)? (x) : (y))
#define PACK(size, alloc)  ((size) | (alloc))
#define GET(p)(*(word_t *)(p))
#define PUT(p, val)  (*(word_t *)(p) = (val))


static word_t *heap_start; /* Address of the first block */
static word_t *heap_end;   /* Address past last byte of last block */
static word_t *last;       /* Points at last block */

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
  return (void *)bt + bt_size(bt) - sizeof(word_t);
}

/* Given payload pointer returns an address of boundary tag. */
static inline word_t *bt_fromptr(void *ptr) {
  return (word_t *)ptr - 1;
}

/* Creates boundary tag(s) for given block. */
static inline void bt_make(word_t *bt, size_t size, bt_flags flags) {
/*TODO*/
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
  return bt + 1;
}

/* Returns address of next block or NULL. */
static inline word_t *bt_next(word_t *bt) {
/*TODO*/
}

/* Returns address of previous block or NULL. */
static inline word_t *bt_prev(word_t *bt) {
/*TODO*/
}

/* --=[ miscellanous procedures ]=------------------------------------------ */

/* Calculates block size incl. header, footer & payload,
 * and aligns it to block boundary (ALIGNMENT). */
static inline size_t blksz(size_t size) {
/*TODO*/
}

static void *morecore(size_t size) {
  void *ptr = mem_sbrk(size);
  if (ptr == (void *)-1)
    return NULL;
  return ptr;
}

/* --=[ mm_init ]=---------------------------------------------------------- */

int mm_init(void) {
  void *ptr = morecore(ALIGNMENT - sizeof(word_t));	//wyrównuje aby payload zaczynal sie pod adresem podzielnym przez 16
  if (!ptr)
    return -1;

  free_list = morecore(4 * WSIZE);
  if(!free_list)
    return -1;
  
  PUT()
  heap_start = NULL;
  heap_end = NULL;
  last = NULL;
  return 0;
}

/* --=[ malloc ]=----------------------------------------------------------- */

#if 0
/* First fit startegy. */
static word_t *find_fit(size_t reqsz) {
}
#else
/* Best fit startegy. */
static word_t *find_fit(size_t reqsz) {
}
#endif

void *malloc(size_t size) {
}

/* --=[ free ]=------------------------------------------------------------- */

void free(void *ptr) {
}

/* --=[ realloc ]=---------------------------------------------------------- */

void *realloc(void *old_ptr, size_t size) {
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
}
