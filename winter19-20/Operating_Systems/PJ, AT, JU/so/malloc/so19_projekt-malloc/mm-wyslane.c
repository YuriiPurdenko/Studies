/* --- Paweł Jasiak 308313 */

/* --- rozwiązanie używa drzewa splay ze strategią best-fit
 * i gorliwym łączeniem bloków 
 * 
 * jednocześnie na końcu pliku zamieściłem rozwiąanie "robocze", które było
 * prezentowane na zajęciach, używa ono strategi bets-fit na pojdeynczej liście
 * wolnych bloków. Co warto zauważyć, otrzymuje ono istotnie lepsze rezultaty
 * ze względu na utylizację pamięci (pomimo tego, że nie zużywa mniej
 * metadanych).
 */

/* --- wynik działania na laptopie studenta 
 * Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz
 * 
 * UWAGA! w dobie CPU zwiększających częstotliwość warto "rozkręcić" procesor
 * przed uruchomieniem testu (w innym wypadku wyniki będą bardzo zaburzone
 * i w mojej ocenie niesprawiedliwe). W tym celu korzystałem z następującego
 * polecenia w terminalu:
 * yes > /dev/null
 * Było ono uruchomione na innych terminalach tak aby rozgrzać CPU (w przypadku
 * laptopa na jednym, ale w przypadku mocniejszej maszyny wyposażonej w kilka
 * fizycznych CPU (2x4) dopiero czterokrotne wywołanie polecenia poskutkowało)

pj: ~/II/so/malloc/so19_projekt-malloc: ./mdriver -l
Using default tracefiles in ./traces/
Measuring performance with a cycle counter.
Processor clock rate ~= 3795.4 MHz

Results for libc malloc:
  valid  util   ops    secs      Kops  trace
 * yes     0%    5694  0.000109  52339 ./traces/amptjp-bal.rep
 * yes     0%    4805  0.000355  13547 ./traces/amptjp.rep
 * yes     0%   12000  0.000426  28198 ./traces/binary-bal.rep
 * yes     0%    8000  0.000285  28103 ./traces/binary.rep
 * yes     0%   24000  0.000283  84825 ./traces/binary2-bal.rep
 * yes     0%   16000  0.000297  53792 ./traces/binary2.rep
 * yes     0%    5848  0.000100  58195 ./traces/cccp-bal.rep
 * yes     0%    5032  0.000330  15226 ./traces/cccp.rep
 * yes     0%   14400  0.000174  82668 ./traces/coalescing-bal.rep
 * yes     0%   14400  0.000197  73062 ./traces/coalescing.rep
 * yes     0%    6648  0.000273  24358 ./traces/cp-decl-bal.rep
 * yes     0%    5683  0.000419  13552 ./traces/cp-decl.rep
 * yes     0%    5380  0.000296  18164 ./traces/expr-bal.rep
 * yes     0%    4537  0.000355  12787 ./traces/expr.rep
 * yes     0%    4800  0.000180  26681 ./traces/random-bal.rep
 * yes     0%    4800  0.000175  27475 ./traces/random.rep
 * yes     0%    4800  0.000179  26777 ./traces/random2-bal.rep
 * yes     0%    4800  0.000191  25195 ./traces/random2.rep
 * yes     0%   14401  0.000342  42125 ./traces/realloc-bal.rep
   yes     0%   14401  0.000351  41021 ./traces/realloc.rep
 * yes     0%   14401  0.000090 159742 ./traces/realloc2-bal.rep
 * yes     0%   14401  0.000098 147586 ./traces/realloc2.rep
 * yes     0%      12  0.000000  53645 ./traces/short1-bal.rep
 * yes     0%      12  0.000000  53708 ./traces/short1.rep
 * yes     0%      12  0.000000  52230 ./traces/short2-bal.rep
 * yes     0%      12  0.000000  53144 ./traces/short2.rep
25 25      0%  194878  0.005154  37807

Results for mm malloc:
  valid  util   ops    secs      Kops  trace
 * yes    90%    5694  0.000199  28680 ./traces/amptjp-bal.rep
 * yes    90%    4805  0.000129  37285 ./traces/amptjp.rep
 * yes    51%   12000  0.000237  50698 ./traces/binary-bal.rep
 * yes    51%    8000  0.000057 139793 ./traces/binary.rep
 * yes    41%   24000  0.000453  53022 ./traces/binary2-bal.rep
 * yes    41%   16000  0.000107 148868 ./traces/binary2.rep
 * yes    91%    5848  0.000198  29580 ./traces/cccp-bal.rep
 * yes    91%    5032  0.000135  37360 ./traces/cccp.rep
 * yes    50%   14400  0.000228  63192 ./traces/coalescing-bal.rep
 * yes    50%   14400  0.000229  62945 ./traces/coalescing.rep
 * yes    90%    6648  0.000227  29345 ./traces/cp-decl-bal.rep
 * yes    90%    5683  0.000157  36170 ./traces/cp-decl.rep
 * yes    90%    5380  0.000174  30911 ./traces/expr-bal.rep
 * yes    90%    4537  0.000124  36735 ./traces/expr.rep
 * yes    82%    4800  0.000400  11994 ./traces/random-bal.rep
 * yes    82%    4800  0.000400  11988 ./traces/random.rep
 * yes    80%    4800  0.000381  12589 ./traces/random2-bal.rep
 * yes    80%    4800  0.000381  12591 ./traces/random2.rep
 * yes    80%   14401  0.000080 180450 ./traces/realloc-bal.rep
   yes    80%   14401  0.000075 192116 ./traces/realloc.rep
 * yes    12%   14401  0.000077 187943 ./traces/realloc2-bal.rep
 * yes    12%   14401  0.000077 187788 ./traces/realloc2.rep
 * yes    66%      12  0.000000  72293 ./traces/short1-bal.rep
 * yes    66%      12  0.000000  71386 ./traces/short1.rep
 * yes    99%      12  0.000000  81621 ./traces/short2-bal.rep
 * yes    99%      12  0.000000  82959 ./traces/short2.rep
25 25     70%  194878  0.004449  43807

Perf index = 25 (util) & 40 (thru) = 65/100

Wersja z listą (PRESENTATION)

Results for mm malloc:
  valid  util   ops    secs      Kops  trace
 * yes    98%    5694  0.000215  26506 ./traces/amptjp-bal.rep
 * yes    98%    4805  0.000183  26289 ./traces/amptjp.rep
 * yes    51%   12000  0.020723    579 ./traces/binary-bal.rep
 * yes    51%    8000  0.021143    378 ./traces/binary.rep
 * yes    41%   24000  0.055932    429 ./traces/binary2-bal.rep
 * yes    41%   16000  0.056153    285 ./traces/binary2.rep
 * yes    99%    5848  0.000224  26069 ./traces/cccp-bal.rep
 * yes    99%    5032  0.000214  23529 ./traces/cccp.rep
 * yes    99%   14400  0.000191  75421 ./traces/coalescing-bal.rep
 * yes    99%   14400  0.000190  75624 ./traces/coalescing.rep
 * yes    99%    6648  0.000226  29366 ./traces/cp-decl-bal.rep
 * yes    99%    5683  0.000213  26684 ./traces/cp-decl.rep
 * yes    99%    5380  0.000171  31504 ./traces/expr-bal.rep
 * yes    99%    4537  0.000154  29464 ./traces/expr.rep
 * yes    95%    4800  0.001883   2549 ./traces/random-bal.rep
 * yes    95%    4800  0.001939   2475 ./traces/random.rep
 * yes    95%    4800  0.001911   2511 ./traces/random2-bal.rep
 * yes    95%    4800  0.001912   2511 ./traces/random2.rep
 * yes    26%   14401  0.035427    406 ./traces/realloc-bal.rep
   yes    26%   14401  0.036501    395 ./traces/realloc.rep
 * yes    30%   14401  0.001823   7901 ./traces/realloc2-bal.rep
 * yes    30%   14401  0.001828   7879 ./traces/realloc2.rep
 * yes    66%      12  0.000000  42405 ./traces/short1-bal.rep
 * yes    66%      12  0.000000  42874 ./traces/short1.rep
 * yes    99%      12  0.000000  48005 ./traces/short2-bal.rep
 * yes    99%      12  0.000000  44916 ./traces/short2.rep
25 25     79%  194878  0.202656    962

Perf index = 45 (util) & 1 (thru) = 46/100

*/

/* --- do wyboru do koloru */
#define SPLAY
//#define PRESENTATION

#ifdef SPLAY

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

/* --- mój assert używany jedynie podczas testów */
#ifdef MASSERT
#define massert(x) assert(x)
#else
#define massert(x)
#endif

typedef int32_t header_t;
typedef header_t footer_t;
typedef union payload payload_t;
typedef struct block block_t;
typedef struct node node_t;

/* --- węzeł w drzewie splay */
struct node {
  node_t *left;
  node_t *right;
};

/* --- unia przechowująca dane bądź wierzchołek drzewa bst */
union payload {
  struct {
    node_t node;
  } splay;
  uint8_t payload[0];
};

/* --- blok zawiera nagłówek i payload (dane bądź wierzchołek drzewa bst) 
 * nagłówek zawiera na ostatnich dwóch bitach metadane czy jest zajęty oraz czy
 * poprzedni blok jest zajęty */
struct block {
  header_t header;
  payload_t payload;
};

/* --- dużo funkcji pomocniczych */
static inline int cmplt(block_t *a, block_t *b); // <
static inline int cmpeq(block_t *a, block_t *b); // ==
static inline int cmpgt(block_t *a, block_t *b); // >
static inline node_t *block_node(block_t *block); // wyciągamy węzeł drzewa BST z bloku
static inline node_t **block_node_left(block_t *block); // wyciągamy lewego syna
static inline node_t **block_node_right(block_t *block); // wyciągamy prawego syna
static inline block_t *node_to_block(node_t *node); // wyciągamy blok z odpowiadającego mu węzła w drzewie
static inline int32_t header_size(header_t *header); // rozmiar bloku
static inline int32_t footer_size(footer_t *footer);
static inline void header_set_size(header_t *header, int32_t size); // ustawienie rozmiaru bloku
static inline int header_is_used(header_t *header); // sprawdzanie zajętości bloku
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
static inline header_t *block_header(block_t *block); // wyciągnięcie nagłówków z bloku
static inline footer_t *block_footer(block_t *block);
static inline void block_set_size(block_t *block, int32_t size);
static inline void block_footer_sync(block_t *block); // synchronizacja headera i footera
static inline void *block_payload(block_t *block);
static inline block_t *block_phys_next(block_t *block); // następny blok w pamięci 
static inline block_t *block_phys_prev(block_t *block); // poprzedni blok w pamięci
static inline int32_t block_size(block_t *block);
static inline int block_is_used(block_t *block);
static inline int block_is_free(block_t *block);
static inline void block_set_used(block_t *block);
static inline void block_set_free(block_t *block);
static inline int block_prev_is_used(block_t *block);
static inline int block_prev_is_free(block_t *block);
static inline void block_prev_set_used(block_t *block);
static inline void block_prev_set_free(block_t *block);
static inline block_t *payload_to_block(void *ptr); // zamiana adresu zwracanego dla użytkownika końcowego na odpowiadający blok

/* --- splay ukradziony (a następnie zmodyfikowany) z 
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
node_t *insert(node_t *new, node_t *t) {
  if (NULL == t) {
    new->left = new->right = NULL;
    return new;
  }
  t = splay(node_to_block(new), t);
  if (cmplt(node_to_block(new), node_to_block(t))) {
    new->left = t->left;
    new->right = t;
    t->left = NULL;
    return new;
  } else if (cmpgt(node_to_block(new), node_to_block(t))) {
    new->right = t->right;
    new->left = t;
    t->right = NULL;
    return new;
  } else {
    return t;
  }
}

node_t *delete(block_t *block, node_t *t) {
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
/* --- korzeń drzewa pamięci */
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

/* --- operacja split jedyne co robi to dzieli blok na dwa mniejsze */
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
  size = round_up(sizeof(block_t) + size);
  
  /* --- szukamy odpowiedniego węzła w drzewie pamięci */
  block_t *block = block_find(size, &mm_root);

  /* --- jeśli nie ma żadnego pasującego bloku należy zaalokować nowy */
  if (NULL == block) {
    block = block_alloc(size);
    if (NULL == block) {
      return NULL;
    }

    /* --- sprawdzamy czy blok za nami jest wolny i ustawiamy odpowiedni bit */
    block_t *prev = block_phys_prev(block);
    
    if (NULL != prev) {
      if (block_is_free(prev))
        block_prev_set_free(block);
    }

    block_set_used(block);

    return block_payload(block);
  }

  mm_root = delete(block, mm_root);

  /* --- jeśli podział bloku ma sens to zrób to i dodaj nowy blok do drzewa */
  if (block_size(block) >= size + sizeof(block_t) + ALIGNMENT) {
    block_t *nblock = split(block, size);
    mm_root = insert(block_node(nblock), mm_root);
  }

  block_set_used(block);

  return block_payload(block);
}

/* --- złączenie dwóch wolnych bloków */
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

  /* --- teraz złączamy gorliwie wolne bloki */
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


  /* If old_ptr is NULL, then this is just malloc. */
  if (!old_ptr)
    return malloc(size);
  
  size_t tmp_size = size;
  
  size = round_up(sizeof(block_t) + size);

  /* --- chcemy uniknąć kosztownego memcpy
   * w tym celu staramy się powiększyć nasz zajęty blok */
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
  /* --- wolne bloki powinny być wolne */
  assert(block_is_free(block));
  /* --- i nie powinny mieć wolnych bloków obok siebie */
  if (block_phys_prev(block))
    assert(block_is_free(block_phys_prev(block)));
  splay_check(node->left);
  splay_check(node->right);
}


void mm_checkheap(int verbose) {
  /* --- rekurencyjnie sprawdźmy drzewo */
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

/* --- jeśli rozmiary są takie same to porównujemy po adresie */
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

#endif // SPLAY

#ifdef PRESENTATION

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

/* --- first fit */
/*
static block_t *ff(block_t *head, int32_t size) {
  for (block_t *block = head; NULL != block; block = block_free_next(block)) {
    assert(block_is_free(block));
    if (block_size(block) >= size)
      return block;
  }
  return NULL;
}
*/

/* --- best fit */
static block_t *bf(block_t *head, int32_t size) {
  block_t *ret = NULL;

  for (block_t *block = head; NULL != block; block = block_free_next(block)) {
    assert(block_is_free(block));
    int32_t s = block_size(block);
    if (s >= size && (NULL == ret || s < block_size(ret)))
      ret = block;
  }

  return ret;
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

  block_t *block = bf(mm_list, size);

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

    return block_payload(block);
  }


  if (block_size(block) >= size + sizeof(block_t)) {
    split(block, size);
  } 
  
  block_set_used(block);

  mm_list_erase(block);

  return block_payload(block);
}


void merge(block_t *block, block_t *next_block) {
  assert(block_is_free(block));
  assert(block_is_free(next_block));
  
  int32_t b_size = block_size(block);
  int32_t next_b_size = block_size(next_block);
  int32_t size = b_size + next_b_size;

  mm_list_erase(block);
  mm_list_erase(next_block);

  block_set_size(block, size);
  mm_list_insert(block);
}

void free(void *ptr) {
  block_t *block = payload_to_block(ptr);
  block_set_free(block);
  mm_list_insert(block);

  block_t *p;
  while ((p = block_phys_prev(block)) != NULL) {
    if (block_is_free(p) && p != block) {
      merge(p, block);
      block = p;
    } else {
      break;
    }
  }

  while ((p = block_phys_next(block)) != NULL) {
    if (block_is_free(p) && p != block) {
      merge(block, p);
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
  for (block_t *block = mm_list; NULL != block; block = block->payload.free.next) {
    assert(block_is_free(block));
    assert(block_prev_is_used(block));
    assert((int64_t)block_payload(block) % ALIGNMENT == 0);
  }
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

#endif
