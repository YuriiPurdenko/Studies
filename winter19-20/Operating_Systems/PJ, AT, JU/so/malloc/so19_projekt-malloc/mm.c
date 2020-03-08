/* --- Paweł Jasiak 308313 */

/* --- rozwiązanie używa drzewa splay ze strategią best-fit
 * i gorliwym łączeniem bloków
 *
 * na metadane zużywamy:
 * 4 bajty nagłówka przechowujące informacje na temat rozmiaru i zajętości bloku
 * oraz bloku poprzedniego
 * 4 bajty wyrównania
 * 16 bajtów na wskaźniki drzewa splay
 * 4 bajty na footer będący kopią nagłówka
 */

/* --- na końcu pliku umieściłem też rozwiązanie ze skompresowanymi wskaźnikami
 * drzewa splay (będące offsetem od mem_heap_lo() zamiast adresem bezwzględnym
 *
 * takie rozwiazanie dostaje 1 pkt więcej za wykorzystanie pamięci, jednak jest
 * istotnie wolniejsze (chociaż wciąż nie gorsze od libc)
 *
 * w takim rozwiązniu mamy 4 bajty na nagłówek, 8 bajtów na węzeł drzewa
 * i 4 bajty na footer
 *
 * jeśli prowadzący ma chęć pobawić się drugim rozwiązaniem wystarczy zamienić
 * define POINTERS na OFFSET (poniżej wstępu)
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


wersja POINTERS

pj: ~/II/so/malloc/so19_projekt-malloc: ./mdriver 
Using default tracefiles in ./traces/
Measuring performance with a cycle counter.
Processor clock rate ~= 3836.1 MHz

Results for mm malloc:
  valid  util   ops    secs      Kops  trace
 * yes    98%    5694  0.000179  31811 ./traces/amptjp-bal.rep
 * yes    98%    4805  0.000118  40651 ./traces/amptjp.rep
 * yes    51%   12000  0.000267  44879 ./traces/binary-bal.rep
 * yes    51%    8000  0.000051 157128 ./traces/binary.rep
 * yes    41%   24000  0.000520  46183 ./traces/binary2-bal.rep
 * yes    41%   16000  0.000090 176839 ./traces/binary2.rep
 * yes    99%    5848  0.000183  32015 ./traces/cccp-bal.rep
 * yes    99%    5032  0.000126  40067 ./traces/cccp.rep
 * yes    99%   14400  0.000142 101311 ./traces/coalescing-bal.rep
 * yes    99%   14400  0.000139 103851 ./traces/coalescing.rep
 * yes    99%    6648  0.000213  31181 ./traces/cp-decl-bal.rep
 * yes    99%    5683  0.000142  39958 ./traces/cp-decl.rep
 * yes    99%    5380  0.000161  33330 ./traces/expr-bal.rep
 * yes    99%    4537  0.000115  39452 ./traces/expr.rep
 * yes    96%    4800  0.000448  10717 ./traces/random-bal.rep
 * yes    96%    4800  0.000462  10393 ./traces/random.rep
 * yes    95%    4800  0.000448  10720 ./traces/random2-bal.rep
 * yes    95%    4800  0.000447  10735 ./traces/random2.rep
 * yes    96%   14401  0.000213  67571 ./traces/realloc-bal.rep
   yes    96%   14401  0.000207  69464 ./traces/realloc.rep
 * yes    83%   14401  0.000122 118208 ./traces/realloc2-bal.rep
 * yes    83%   14401  0.000118 121761 ./traces/realloc2.rep
 * yes    66%      12  0.000000  57686 ./traces/short1-bal.rep
 * yes    66%      12  0.000000  57542 ./traces/short1.rep
 * yes    99%      12  0.000000  67104 ./traces/short2-bal.rep
 * yes    99%      12  0.000000  66715 ./traces/short2.rep
25 25     86%  194878  0.004705  41418

Perf index = 60 (util) & 40 (thru) = 100/100


wersja OFFSET

Results for mm malloc:
  valid  util   ops    secs      Kops  trace
 * yes    99%    5694  0.000187  30395 ./traces/amptjp-bal.rep
 * yes    99%    4805  0.000133  36048 ./traces/amptjp.rep
 * yes    54%   12000  0.000322  37277 ./traces/binary-bal.rep
 * yes    54%    8000  0.000058 138921 ./traces/binary.rep
 * yes    47%   24000  0.000675  35536 ./traces/binary2-bal.rep
 * yes    47%   16000  0.000104 154524 ./traces/binary2.rep
 * yes    99%    5848  0.000223  26281 ./traces/cccp-bal.rep
 * yes    99%    5032  0.000145  34621 ./traces/cccp.rep
 * yes    99%   14400  0.000131 109571 ./traces/coalescing-bal.rep
 * yes    99%   14400  0.000132 108978 ./traces/coalescing.rep
 * yes    99%    6648  0.000249  26717 ./traces/cp-decl-bal.rep
 * yes    99%    5683  0.000162  35080 ./traces/cp-decl.rep
 * yes    99%    5380  0.000192  28094 ./traces/expr-bal.rep
 * yes    99%    4537  0.000129  35142 ./traces/expr.rep
 * yes    95%    4800  0.000603   7964 ./traces/random-bal.rep
 * yes    95%    4800  0.000604   7951 ./traces/random.rep
 * yes    95%    4800  0.000605   7929 ./traces/random2-bal.rep
 * yes    95%    4800  0.000606   7924 ./traces/random2.rep
 * yes    51%   14401  0.000161  89188 ./traces/realloc-bal.rep
   yes    51%   14401  0.000162  88990 ./traces/realloc.rep
 * yes    83%   14401  0.000071 204243 ./traces/realloc2-bal.rep
 * yes    83%   14401  0.000068 211894 ./traces/realloc2.rep
 * yes    66%      12  0.000000  53467 ./traces/short1-bal.rep
 * yes    66%      12  0.000000  53281 ./traces/short1.rep
 * yes    99%      12  0.000000  59194 ./traces/short2-bal.rep
 * yes    99%      12  0.000000  59655 ./traces/short2.rep
25 25     85%  194878  0.005560  35049

Perf index = 60 (util) & 35 (thru) = 95/100
*/

#define POINTERS
//#define OFFSET

#ifdef POINTERS

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

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

/* --- mój assert używany jedynie podczas testów 
 * warto zaznaczyć, że pełni on efektywnie rolę mm_checkheap podczas działania
 * programu -- sprawdza on dokładnie wszystkie warunki w funkcjach pomocniczych
 * */
#ifdef MASSERT
#define massert(x) assert(x)
#else
#define massert(x)
#endif

typedef uint32_t header_t;
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
static inline block_t *node_to_block(node_t *node); // wyciągamy blok z odpowiadającego mu węzła w drzewie
static inline uint32_t header_size(header_t *header); // rozmiar bloku
static inline uint32_t footer_size(footer_t *footer);
static inline void header_set_size(header_t *header, uint32_t size); // ustawienie rozmiaru bloku
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
static inline void block_set_size(block_t *block, uint32_t size);
static inline void block_footer_sync(block_t *block); // synchronizacja headera i footera
static inline void *block_payload(block_t *block);
static inline block_t *block_phys_next(block_t *block); // następny blok w pamięci
static inline block_t *block_phys_prev(block_t *block); // poprzedni blok w pamięci
static inline uint32_t block_size(block_t *block);
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

node_t *delete (block_t *block, node_t *t) {
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

node_t *find_block(uint32_t size, node_t *t) {
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

node_t *find(uint32_t size, node_t **t) {
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

  uint32_t old_size = block_size(block);
  block_set_size(block, size);

  block_t *new_block = block_phys_next(block);
  block_set_size(new_block, old_size - size);
  block_set_free(new_block);

  node_t *node = block_node(new_block);
  node->left = NULL;
  node->right = NULL;

  return new_block;
}

static inline block_t *block_alloc(uint32_t size) {
  block_t *block = mem_sbrk(size);
  if ((long)block < 0)
    return NULL;

  block_set_size(block, size);
  return block;
}

static inline block_t *block_find(uint32_t size, node_t **root) {
  node_t *ret = find(size, root);
  if (NULL == ret)
    return NULL;
  return node_to_block(ret);
}

void *malloc(size_t size) {
  debug("malloc>> %lu\n", size);
  size = round_up(sizeof(block_t) + size);
  debug("malloc>> new size %lu\n", size);

  /* --- szukamy odpowiedniego węzła w drzewie pamięci */
  block_t *block = block_find(size, &mm_root);

  /* --- jeśli nie ma żadnego pasującego bloku należy zaalokować nowy */
  if (NULL == block) {
    debug("malloc>> alloc new block\n");
    block = block_alloc(size);
    if (NULL == block) {
      return NULL;
    }

    /* --- sprawdzamy czy blok za nami jest wolny i ustawiamy odpowiedni bit */
    block_t *prev = block_phys_prev(block);
    debug("malloc>> prev block 0x%p\n", prev);

    if (NULL != prev) {
      if (block_is_free(prev))
        block_prev_set_free(block);
    }

    block_set_used(block);

    debug("malloc>> final block 0x%p [%d]\n", block, block_size(block));

    return block_payload(block);
  }

  debug("malloc>> found block 0x%p [%d]\n", block, block_size(block));

  mm_root = delete (block, mm_root);

  /* --- jeśli podział bloku ma sens to zrób to i dodaj nowy blok do drzewa */
  if (block_size(block) >= size + sizeof(block_t) + ALIGNMENT) {
    block_t *nblock = split(block, size);
    debug("malloc>> splited block 0x%p [%d]\n", nblock, block_size(nblock));
    mm_root = insert(block_node(nblock), mm_root);
  }

  block_set_used(block);
  debug("malloc>> final block 0x%p [%d]\n", block, block_size(block));

  return block_payload(block);
}

/* --- złączenie dwóch wolnych bloków */
void merge(block_t *block, block_t *next_block, node_t **root) {
  massert(block_is_free(block));
  massert(block_is_free(next_block));
  debug("merge>> 0x%p with 0x%p\n", block, next_block);

  uint32_t b_size = block_size(block);
  uint32_t next_b_size = block_size(next_block);
  uint32_t size = b_size + next_b_size;

  *root = delete (block, *root);
  *root = delete (next_block, *root);

  block_set_size(block, size);
  *root = insert(block_node(block), *root);
}

void free(void *ptr) {
  debug("free>> 0x%p\n", ptr);
  block_t *block = payload_to_block(ptr);
  debug("free>> block at 0x%p\n", block);
  block_set_free(block);

  mm_root = insert(block_node(block), mm_root);

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
  debug("realloc>> 0x%p %lu\n", old_ptr, size);
  /* If size == 0 then this is just free, and we return NULL. */
  if (size == 0) {
    free(old_ptr);
    return NULL;
  }

  /* If old_ptr is NULL, then this is just malloc. */
  if (!old_ptr) {
    debug("realloc>> just malloc\n");
    return malloc(size);
  }

  size_t tmp_size = size;

  size = round_up(sizeof(block_t) + size);
  debug("realloc>> real size %lu\n", size);

  /* --- chcemy uniknąć kosztownego memcpy */
  block_t *block = payload_to_block(old_ptr);
  size_t old_size = block_size(block);

  /* --- jeśli nasz blok jest większy niż potrzebujemy zwalniamy wolną pamięć */
  if (size <= block_size(block)) {
    debug("realloc>> current block 0x%p [%d] is good enough\n", block,
          block_size(block));

    if (block_size(block) >= size + sizeof(block_t) + ALIGNMENT) {
      block_t *nblock = split(block, size);
      debug("realloc>> splited block 0x%p [%d]\n", nblock, block_size(nblock));
      mm_root = insert(block_node(nblock), mm_root);
    }

    return old_ptr;
  }

  /* --- jeśli możemy przedłużamy blok na następny wolny */
  debug("realloc>> current block 0x%p [%d]\n", block, block_size(block));
  for (;;) {
    block_t *next = block_phys_next(block);
    if (NULL != next && block_is_free(next)) {
      uint32_t osize = block_size(block);
      uint32_t nsize = block_size(next);
      mm_root = delete (next, mm_root);
      block_set_size(block, osize + nsize);
    } else {
      break;
    }
    debug("realloc>> block resized 0x%p [%d]\n", block, block_size(block));
    if (block_size(block) >= size) {
      debug("realloc>> resize with success\n");
      return old_ptr;
    }
  }

  /* --- i na poprzedni wolny */
  for (;;) {
    block_t *prev = block_phys_prev(block);
    if (NULL != prev && block_is_free(prev)) {
      uint32_t osize = block_size(block);
      uint32_t psize = block_size(prev);
      if (osize + psize >= size) {
        /* --- połączmy się z poprzednim blokiem */
        mm_root = delete(prev, mm_root);
        block_set_size(prev, osize + psize);

        /* --- teraz trzeba ostrożnie przenieść pamięć 
         * jednocześnie już wiemy, że nie zawiedliśmy
         * więc warunek aby oryginalny blok nie został naruszony 
         * w przypadku błędu będzie spełniony
         * */

        /* --- memmove ponieważ obszary mogą się nakładać */
        memmove(block_payload(prev), old_ptr, old_size);
        block_set_used(prev);

        if (block_size(prev) >= size + sizeof(block_t) + ALIGNMENT) {
          block_t *nblock = split(prev, size);
          debug("realloc>> splited block 0x%p [%d]\n", nblock, block_size(nblock));
          mm_root = insert(block_node(nblock), mm_root);
        }

        debug("realloc>> resize with success\n");
        return block_payload(prev);
      } else {
        break;
      }
    } else {
      break;
    }
  }

  void *new_ptr = malloc(tmp_size);

  /* If malloc() fails, the original block is left untouched. */
  if (!new_ptr)
    return NULL;

  /* Copy the old data. */
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

static inline uint32_t header_size(header_t *header) {
  return (*header >> 4) << 4;
}

static inline uint32_t footer_size(footer_t *footer) {
  return (*footer >> 4) << 4;
}

static inline void header_set_size(header_t *header, uint32_t size) {
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

static inline void block_set_size(block_t *block, uint32_t size) {
  header_set_size(block_header(block), size);
  block_footer_sync(block);
}

static inline void block_footer_sync(block_t *block) {
  *block_footer(block) = *block_header(block);
}

static inline void *block_payload(block_t *block) {
  massert((uint64_t)block->payload.payload % ALIGNMENT == 0);
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

static inline uint32_t block_size(block_t *block) {
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

#endif /* POINTERS */

#ifdef OFFSET

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

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

typedef uint32_t header_t;
typedef header_t footer_t;
typedef union payload payload_t;
typedef struct block block_t;
typedef struct node node_t;
typedef uint32_t pointer_t;

/* --- węzeł w drzewie splay */
struct node {
  pointer_t left;
  pointer_t right;
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
static inline void *ptr_to_addr(pointer_t ptr);
static inline pointer_t addr_to_ptr(void *addr);
static inline int cmplt(block_t *a, block_t *b); // <
static inline int cmpeq(block_t *a, block_t *b); // ==
static inline int cmpgt(block_t *a, block_t *b); // >
static inline node_t *block_node(block_t *block); // wyciągamy węzeł drzewa BST z bloku
static inline block_t *node_to_block(node_t *node); // wyciągamy blok z odpowiadającego mu węzła w drzewie
static inline uint32_t header_size(header_t *header); // rozmiar bloku
static inline uint32_t footer_size(footer_t *footer);
static inline void header_set_size(header_t *header, uint32_t size); // ustawienie rozmiaru bloku
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
static inline void block_set_size(block_t *block, uint32_t size);
static inline void block_footer_sync(block_t *block); // synchronizacja headera i footera
static inline void *block_payload(block_t *block);
static inline block_t *block_phys_next(block_t *block); // następny blok w pamięci
static inline block_t *block_phys_prev(block_t *block); // poprzedni blok w pamięci
static inline uint32_t block_size(block_t *block);
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
  N.left = N.right = -1;
  l = r = &N;

  for (;;) {
    if (cmplt(block, node_to_block(t))) {
      if (-1 == t->left)
        break;
      if (cmplt(block, node_to_block(ptr_to_addr(t->left)))) {
        y = ptr_to_addr(t->left);
        t->left = y->right;
        y->right = addr_to_ptr(t);
        t = y;
        if (-1 == t->left)
          break;
      }
      r->left = addr_to_ptr(t);
      r = t;
      t = ptr_to_addr(t->left);
    } else if (cmpgt(block, node_to_block(t))) {
      if (-1 == t->right)
        break;
      if (cmpgt(block, node_to_block(ptr_to_addr(t->right)))) {
        y = ptr_to_addr(t->right);
        t->right = y->left;
        y->left = addr_to_ptr(t);
        t = y;
        if (-1 == t->right)
          break;
      }
      l->right = addr_to_ptr(t);
      l = t;
      t = ptr_to_addr(t->right);
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
    new->left = new->right = -1;
    return new;
  }
  t = splay(node_to_block(new), t);
  if (cmplt(node_to_block(new), node_to_block(t))) {
    new->left = t->left;
    new->right = addr_to_ptr(t);
    t->left = -1;
    return new;
  } else if (cmpgt(node_to_block(new), node_to_block(t))) {
    new->right = t->right;
    new->left = addr_to_ptr(t);
    t->right = -1;
    return new;
  } else {
    return t;
  }
}

node_t *delete (block_t *block, node_t *t) {
  node_t *x;
  if (NULL == t)
    return NULL;
  t = splay(block, t);
  if (cmpeq(block, node_to_block(t))) {
    if (-1 == t->left) {
      x = ptr_to_addr(t->right);
    } else {
      x = splay(block, ptr_to_addr(t->left));
      x->right = t->right;
    }
    return x;
  }
  return t;
}

node_t *find_block(uint32_t size, node_t *t) {
  if (NULL == t)
    return NULL;
  if (size == block_size(node_to_block(t)))
    return t;
  if (size < block_size(node_to_block(t))) {
    node_t *ret = find_block(size, ptr_to_addr(t->left));
    return ret ? ret : t;
  }
  return find_block(size, ptr_to_addr(t->right));
}

node_t *find(uint32_t size, node_t **t) {
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

  uint32_t old_size = block_size(block);
  block_set_size(block, size);

  block_t *new_block = block_phys_next(block);
  block_set_size(new_block, old_size - size);
  block_set_free(new_block);

  node_t *node = block_node(new_block);
  node->left = -1;
  node->right = -1;

  return new_block;
}

static inline block_t *block_alloc(uint32_t size) {
  block_t *block = mem_sbrk(size);
  if ((long)block < 0)
    return NULL;

  block_set_size(block, size);
  return block;
}

static inline block_t *block_find(uint32_t size, node_t **root) {
  node_t *ret = find(size, root);
  if (NULL == ret)
    return NULL;
  return node_to_block(ret);
}

void *malloc(size_t size) {
  debug("malloc>> %lu\n", size);
  size = round_up(sizeof(block_t) + size);
  debug("malloc>> new size %lu\n", size);

  /* --- szukamy odpowiedniego węzła w drzewie pamięci */
  block_t *block = block_find(size, &mm_root);

  /* --- jeśli nie ma żadnego pasującego bloku należy zaalokować nowy */
  if (NULL == block) {
    debug("malloc>> alloc new block\n");
    block = block_alloc(size);
    if (NULL == block) {
      return NULL;
    }

    /* --- sprawdzamy czy blok za nami jest wolny i ustawiamy odpowiedni bit */
    block_t *prev = block_phys_prev(block);
    debug("malloc>> prev block 0x%p\n", prev);

    if (NULL != prev) {
      if (block_is_free(prev))
        block_prev_set_free(block);
    }

    block_set_used(block);

    debug("malloc>> final block 0x%p [%d]\n", block, block_size(block));

    return block_payload(block);
  }

  debug("malloc>> found block 0x%p [%d]\n", block, block_size(block));

  mm_root = delete (block, mm_root);

  /* --- jeśli podział bloku ma sens to zrób to i dodaj nowy blok do drzewa */
  if (block_size(block) >= size + sizeof(block_t) + ALIGNMENT) {
    block_t *nblock = split(block, size);
    debug("malloc>> splited block 0x%p [%d]\n", nblock, block_size(nblock));
    mm_root = insert(block_node(nblock), mm_root);
  }

  block_set_used(block);
  debug("malloc>> final block 0x%p [%d]\n", block, block_size(block));

  return block_payload(block);
}

/* --- złączenie dwóch wolnych bloków */
void merge(block_t *block, block_t *next_block, node_t **root) {
  massert(block_is_free(block));
  massert(block_is_free(next_block));
  debug("merge>> 0x%p with 0x%p\n", block, next_block);

  uint32_t b_size = block_size(block);
  uint32_t next_b_size = block_size(next_block);
  uint32_t size = b_size + next_b_size;

  *root = delete (block, *root);
  *root = delete (next_block, *root);

  block_set_size(block, size);
  *root = insert(block_node(block), *root);
}

void free(void *ptr) {
  debug("free>> 0x%p\n", ptr);
  block_t *block = payload_to_block(ptr);
  debug("free>> block at 0x%p\n", block);
  block_set_free(block);

  mm_root = insert(block_node(block), mm_root);

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
  debug("realloc>> 0x%p %lu\n", old_ptr, size);
  /* If size == 0 then this is just free, and we return NULL. */
  if (size == 0) {
    free(old_ptr);
    return NULL;
  }

  /* If old_ptr is NULL, then this is just malloc. */
  if (!old_ptr) {
    debug("realloc>> just malloc\n");
    return malloc(size);
  }

  size_t tmp_size = size;

  size = round_up(sizeof(block_t) + size);
  debug("realloc>> real size %lu\n", size);

  /* --- chcemy uniknąć kosztownego memcpy
   * w tym celu staramy się powiększyć nasz zajęty blok */
  block_t *block = payload_to_block(old_ptr);
  size_t old_size = block_size(block);
  if (size <= block_size(block)) {
    debug("realloc>> current block 0x%p [%d] is good enough\n", block,
          block_size(block));
    return old_ptr;
  }

  debug("realloc>> current block 0x%p [%d]\n", block, block_size(block));
  for (;;) {
    block_t *next = block_phys_next(block);
    if (NULL != next && block_is_free(next)) {
      uint32_t osize = block_size(block);
      uint32_t nsize = block_size(next);
      mm_root = delete (next, mm_root);
      block_set_size(block, osize + nsize);
    } else {
      break;
    }
    debug("realloc>> block resized 0x%p [%d]\n", block, block_size(block));
    if (block_size(block) >= size) {
      debug("realloc>> resize with success\n");
      return old_ptr;
    }
  }
  
  for (;;) {
    block_t *prev = block_phys_prev(block);
    if (NULL != prev && block_is_free(prev)) {
      uint32_t osize = block_size(block);
      uint32_t psize = block_size(prev);
      if (osize + psize >= size) {
        /* --- połączmy się z poprzednim blokiem */
        mm_root = delete(prev, mm_root);
        block_set_size(prev, osize + psize);

        /* --- teraz trzeba ostrożnie przenieść pamięć */
        memmove(block_payload(prev), old_ptr, old_size);
        block_set_used(prev);

        if (block_size(prev) >= size + sizeof(block_t) + ALIGNMENT) {
          block_t *nblock = split(prev, size);
          debug("realloc>> splited block 0x%p [%d]\n", nblock, block_size(nblock));
          mm_root = insert(block_node(nblock), mm_root);
        }

        debug("realloc>> resize with success\n");
        return block_payload(prev);
      } else {
        break;
      }
    } else {
      break;
    }
  }

  void *new_ptr = malloc(tmp_size);

  /* If malloc() fails, the original block is left untouched. */
  if (!new_ptr)
    return NULL;

  /* Copy the old data. */
  // block_t *block = old_ptr - offsetof(block_t, payload);
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
  splay_check(ptr_to_addr(node->left));
  splay_check(ptr_to_addr(node->right));
}

void mm_checkheap(int verbose) {
  /* --- rekurencyjnie sprawdźmy drzewo */
  splay_check(mm_root);
  return;
}

static inline uint32_t header_size(header_t *header) {
  return (*header >> 4) << 4;
}

static inline uint32_t footer_size(footer_t *footer) {
  return (*footer >> 4) << 4;
}

static inline void header_set_size(header_t *header, uint32_t size) {
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

static inline void block_set_size(block_t *block, uint32_t size) {
  header_set_size(block_header(block), size);
  block_footer_sync(block);
}

static inline void block_footer_sync(block_t *block) {
  *block_footer(block) = *block_header(block);
}

static inline void *block_payload(block_t *block) {
  massert((uint64_t)block->payload.payload % ALIGNMENT == 0);
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

static inline uint32_t block_size(block_t *block) {
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

static inline void *ptr_to_addr(pointer_t ptr) {
  if (-1 == ptr) return NULL;
  return (void *)(mem_heap_lo() + ptr);
}

static inline pointer_t addr_to_ptr(void *addr) {
  if (NULL == addr) return -1;
  return addr - mem_heap_lo();
}

#endif /* OFFSET */
