# Ćwiczenia 8 (6 maja 2020)

## Przygoda ze sterownikiem (Linux)

:::success
Autor: Arkadiusz Kozdra
:::

https://hackmd.io/054tJ2eqS72eiBz0dPt6oQ 

## Zadanie 5-8

:::warning
Autor: Samuel Li
:::

## Czym się różni stronnicowalna od zadrutowanj?

Zadrutowana jest przydzielana na początku działaniania systemu, zać pamięć stronnicowalana zawiera strony procesów. 

W pamięci zadrutowanej trzymamy rzeczy z systemu operacyjnego. I nie jest podmienialna tak jak pamięć stronnicowalna. Jest w tym samym miejscu pamięci RAM.
> [name=Samuel Li] Przynajmniej tak to rozumiem

> A nonpageable, or wired, range has physical memory assigned at the time of the call, and this memory is not subject to replacement by the pageout daemon. Wired pages never cause a page fault as that might result in a blocking operation. Wired memory is allocated from either the general allocator malloc(), or the zone allocator described in the last two subsections of this section.
>~ The Design and Implementation of the FreeBSD R Operating System

### Czemu nie można robić dostępów do pamięci stronicowalnej trzymając spin lock?

> Mutexes which do not context switch are MTX_SPIN mutexes.
>~ [mutex(9)]

Muteksy, które nie przełączają kontekstu, są muteksami MTX_SPIN.

Dostęp do pamięci stronnicowalnej grozi błędem strony, na którego naprawienie trzeba będzie czekać.

> Mutexes which do not context switch are MTX_SPIN mutexes.	These should only be used to protect data shared with primary interrupt	code.  This includes interrupt	filters	and low	level scheduling code.	In all architectures both acquiring and releasing of a	uncontested spin mutex is more expensive than the	same operation on a non-spin mutex.  In	order to protect an interrupt service routine from blocking against itself all	interrupts are either blocked or deferred on a processor while holding a spin lock.  It is permissible to hold multiple spin mutexes.
>~ [mutex(9)]

>The pmap_init() function allocates a minimal amount of wired memory to use for kernel page-table pages. The page-table space is expanded dynamically by the pmap_growkernel() routine as it is needed while the kernel is running. Once allocated, it is never freed. The limit on the size of the kernel’s address space is selected at boot time. On 64-bit architectures, the kernel is typically given an address space large enough to directly map all of physical memory. On 32-bit architectures, the kernel is typically given a maximum of 1 Gbyte of address space.
>~ The Design and Implementation of the FreeBSD R Operating System

[mutex(9)]: https://www.freebsd.org/cgi/man.cgi?query=mutex&sektion=9

> Each vmem arena is protected by a single lock as allocations from the vmem arena are infrequent. 
>~ The Design and Implementation of the FreeBSD R Operating System

## Zadanie 6-1

:::warning
Autor: Mateusz Maciejewski
:::

SMP = wieloprocesorowość symetryczna, to znaczy taka architektura komputerowa w której wiele równoważnych procesorów dzieli zasoby (takie jak pamięć operacyjna).

NUMA = non-uniform memory access, architektura komputerowa w której czas dostępu do logicznie spójnej przestrzeni adresowej jest różny dla wielu procesorów. Efektywnie powoduje to "lokalność" pamięci. ccNUMA dodatkowo automatycznie utrzymuje spójność pamięci podręcznych wszystkich procesorów.

Fałszywe współdzielenie występuje, gdy w obrębie odpowiednio małego spójnego obszaru współdzielonej pamięci następują dostępy do niezwiązanych ze sobą danych przez kilka procesorów, tak, że nawzajem wymuszają one synchronizację owego obszaru pomiędzy nimi. Typowym przykładem będą dwie zmienne w obrębie jednej linii cache'u, gdzie zapis do jednej z nich powoduje inwalidację pamięci podręcznej drugiego procesora, który dokonywał odczytu drugiej zmiennej z owej linii. W przypadku, gdy oba procesory dokonują zapisu do różnych zmiennych, nawzajem stają się "właścicielami" tej linii cache ("grają w ping-ponga"), zmuszając drugi do synchronizacji pamięci podręcznej, co powoduje duży narzut czasowy. 

> [name=Mikołaj Kowalik]
> Fajny przykład z wikipedii - dla ping ponga i false sharingu jednocześnie.
> :::spoiler
> ```c=1
> struct foo {
>    int x;
>    int y; 
>};
>
>static struct foo f;
>
>/* The two following functions are running concurrently: */
>
>int sum_a(void)
>{
>    int s = 0;
>    for (int i = 0; i < 1000000; ++i)
>        s += f.x;
>    return s;
>}
>
>void inc_b(void)
>{
>    for (int i = 0; i < 1000000; ++i)
>        ++f.y;
>}
>```
> :::
Z powodu fałszywego współdzielenia w pojedynczej linii pamięci podręcznej może znajdować się tylko jedna blokada wirująca. W przeciwnym razie zwolnienie jakiejkolwiek blokady spowoduje unieważnienie linii pamięci, a tym samym wymusi dostęp do pamięci współdzielonej z osobna przez każdy wirujący procesor, obciążając szynę pamięci.

Kilka definicji:
[`SPINLOCK_BACKOFF`](http://bxr.su/NetBSD/sys/sys/lock.h#94)
```c
94  #define SPINLOCK_BACKOFF(count)                 \
95  do {                                \
96      int __i;                        \
97      for (__i = (count); __i != 0; __i--) {          \
98          SPINLOCK_BACKOFF_HOOK;              \
99      }                           \
100     if ((count) < SPINLOCK_BACKOFF_MAX)         \
101         (count) += (count);             \
102 } while (/* CONSTCOND */ 0);
```

[`SPINLOCK_BACKOFF_HOOK`](http://bxr.su/NetBSD/sys/sys/lock.h#94)
```c
84  #ifndef SPINLOCK_BACKOFF_HOOK
85  #define SPINLOCK_BACKOFF_HOOK       nullop(NULL)
86  #endif
```

Implementacja "exponential backoff" na przykładzie [`mutex_vector_enter`](http://bxr.su/NetBSD/sys/kern/kern_mutex.c#mutex_vector_enter):

```c
442  void
443  mutex_vector_enter(kmutex_t *mtx)
444  {
445      uintptr_t owner, curthread;

448      u_int count;
455  
456      /*
457       * Handle spin mutexes.
458       */
459      owner = mtx->mtx_owner;
460      if (MUTEX_SPIN_P(owner)) {

464          MUTEX_SPIN_SPLRAISE(mtx);
465          MUTEX_WANTLOCK(mtx);

467          if (MUTEX_SPINBIT_LOCK_TRY(mtx)) {
468              MUTEX_LOCKED(mtx);
469              return;
470          }
474  
477          count = SPINLOCK_BACKOFF_MIN;
479          /*
480           * Spin testing the lock word and do exponential backoff
481           * to reduce cache line ping-ponging between CPUs.
482           */
483          do {
484              while (MUTEX_SPINBIT_LOCKED_P(mtx)) {
485                  SPINLOCK_BACKOFF(count);
490              }
491          } while (!MUTEX_SPINBIT_LOCK_TRY(mtx));
492  

501          MUTEX_LOCKED(mtx);
502          return;
503      }
    .........

```

## Zadanie 6-2

:::warning
Autor: Krzysztof Boroński
:::

Problem wygłodniałej  hordy występuje w momencie, gdy wątek odblokowuje zasób, na który czeka wiele innych wątków.
Wszystkie czekające wątki próbują zdobyć dostęp do zasobu, ale tylko jeden z nich go zablokuje, przez co reszta
zostanie niepotrzebnie wybudzona i doprowadzi to do zbędnych context switchów.

Problem ten dotyczy przeważnie maszyn wieloprocesorowych, ponieważ w przypadku maszyny jednoprocesorowej, pierwszy wybudzony
wątek uzyska dostęp do zasobu i z dużym prawdopodobieństwem go zwolni zanim, nastąpi context switch. Jednak na maszynach
wieloprocesorowych, wątki z pozostałych procesorów które nie przejęły zasobu będą przeszeregowane.

Ponadto w przypadku maszyn wieloprocesorowych może wystąpić sytuacja, w której pewien wątek czekający na zasób będzie
notorycznie "wyprzedzany" przez inny, niezależny wątek, który będzie ten zasób próbował chwycić,
po tym jak ten zostanie zwolniony. Może to doprowadzić do zagłodzenia wątku.

### `turnstile_broadcast`

Procedura `turnstile_broadcast` przenosi wszystkie wątki czekające na zasób na listę pending.
Wątki są wybudzane w kolejności od najwyższego priorytetu do najniższego, co zmniejsza problem, choć nie eliminuje go zupełnie.

Przydatna może być również procedura `turnstile_signal`, która budzi tylko wątek o najwyższym priorytecie jednak ona może doprowadzić do zagłodzenia wątków o niższych priorytetach.

## Zadanie 6-3

:::success
Autor: Mikołaj Kowalik
:::

1. Wątki, które czekają na jakiś zasób w kolejce są ustawione od najwyższego priorytetu do najniższego.
2. Jeśli wątek odblokowywuje zasób (na który ktoś czeka) - zachodzi potrzeba wyznacznia nowego wątku, który go zablokuje - to nie jest najtańsze.
3. Jeśli nikt nie czeka na wątek to zwolnienie zasobu jest tanie.
4. Jeśli raz odblokujemy wszystkie oczekujące wątki (z zachowaniem priorytetów) - jest duża szansa, że wątki jeden za drugim będą brały zasób wykonując tylko tanią operację zwalniania zasobu (zamiast tego z drugiego punktu) - zachodzi spora szansa, że nie będziemy musieli pożyczać priorytetu.
5. Jeśli odblokujemy jeden wątek - a wiele innych czeka - będziemy za każdym razem wykonywać droższą operację zwalniania zasobu - przez to bardziej opłaca się po prostu przepiąć uporządkowaną kolejkę wątków oczekujących do kolejki gotowych wątków.


Wniosek:
Jeśli zrobimy `mutex_unlock` i będziemy za każdym razem rozgłaszać, że zasób zwolniony (poprzez `turnstile_broadcast`) uzyskamy efekty opisane powyżej

>Z [mimikera](https://github.com/cahirwpz/mimiker/pull/418/files/7ec7d9df8c4c03aa58a66b0f31e4e01f4dbdf0f5).
>[name=Wiktor Pilarczyk]
:::spoiler
![](https://i.imgur.com/whEs2I3.png)
:::
>[name=Mikołaj Kowalik]
>dzięki, Wiktor!
## Zadanie 6-4

:::warning
Autor: Michał Opanowicz
:::

#### kmutex

```c
struct kmutex {
    volatile uintptr_t  mtx_owner;
    ipl_cookie_t        mtx_ipl = IPL_NONE //oznacza, że mutex jest adaptacyjny
    __cpu_simple_lock_t mtx_lock;
};
```

Bit `WAITERS` pola `mtx_owner` oznacza, że na zwolnienie mutexu czekają jakieś wątki. W następnym zadaniu jego użycie powinno być dokładniej opisane.

#### O barierach:
Bariery jedynie synchronizują wydarzenia **w tym wątku** i dotyczą **jedynie** out of order execution. Bariery są konieczne aby zagwarantować, że nie zaczniemy w wyniku out of order execution wykonywać operacji na chronionej sekcji, zanim weźmiemy mutexa - przykład:

Program A:
```
mutex_acquire(mtx)
print(val)
mutex_release(mtx)
```

Program B:
```
mutex_acquire(mtx)
val = 2137
val = 1
mutex_release(mtx)
```

W takiej sytuacji program A może sprawdzić wartość val zanim otrzyma mtx, następnie B zwolni mutex, A otrzyma mutex i zadowolony wyprintuje 2137.


#### MUTEX_ACQUIRE
```c
static inline int MUTEX_ACQUIRE(kmutex_t *mtx, uintptr_t curthread) {
    int rv;
    uintptr_t oldown = 0;        /* stary owner = 0 (brak ownera) */
    uintptr_t newown = curthread /* nowy owner = my */;

    /* compare and swap patrzące czy możemy wziąć mutex */
    rv = MUTEX_CAS(&mtx->mtx_owner, oldown, newown);
    
    MUTEX_MEMBAR_ENTER();
    /* gwarantujemy że na pewno spróbowaliśmy wziąć mutex przed 
     * wyjściem z funkcji */
    
    return rv;
}
```
#### MUTEX_RELEASE
```c
static inline void MUTEX_RELEASE(kmutex_t *mtx) {
    uintptr_t newown;
    
    MUTEX_MEMBAR_EXIT();
    /* gwarantujemy że wszystkie operacje w sekcji krytycznej
     * odbyły się zanim wyszliśmy z funkcji */ 
    newown = 0;
    mtx->mtx_owner = newown;
}
```

## Zadanie 6-5

:::warning
Autor: Paweł Jasiak
:::


[`mutex_vector_enter`](http://bxr.su/NetBSD/sys/kern/kern_mutex.c#mutex_vector_enter)

> Support routine for mutex_enter() that must handle all cases.  In
> the LOCKDEBUG case, mutex_enter() is always aliased here, even if
> fast-path stubs are available.  If a mutex_spin_enter() stub is
> not available, then it is also aliased directly here.
   
Definicja `kmutex_t` z arm:
```C=49
struct kmutex {
    union {
        /* Adaptive mutex */
        volatile uintptr_t  mtxa_owner; /* 0-3 */

#ifdef _KERNEL
        /* Spin mutex */
        struct {
            /*
             * Since the low bit of mtxa_owner is used to flag this
             * mutex as a spin mutex, we can't use the first byte
             * or the last byte to store the ipl or lock values.
             */
            volatile uint8_t    mtxs_dummy;
            ipl_cookie_t        mtxs_ipl;
            __cpu_simple_lock_t mtxs_lock;
            volatile uint8_t    mtxs_unused;
        } s;
#endif
    } u;
};
```


Książka:
```
mutex_vector_enter()
    if (lock is a spin lock)
        lock_set_spl() /* enter spin-lock specific code path */
     increment cpu_sysinfo.ademters.
spin_loop:
    if (lock is not owned)
        mutex_trylock() /* try to acquire the lock */
        if (lock acquired)
            goto bottom
        else
            continue /* lock being held */
    if (lock owner is running on a processor)
        goto spin_loop
    else
        lookup turnstile for the lock
        set waiters bit
        if (lock owner is running on a processor)
            drop turnstile
            goto spin_loop
        else
            block /* the sleep queue associated with the turnstile */
bottom:
    update lockstat statistics
```
   
```C=442
void
mutex_vector_enter(kmutex_t *mtx)
{
    uintptr_t owner, curthread;
    turnstile_t *ts;
    u_int count;

    owner = mtx->mtx_owner;
    if (MUTEX_SPIN_P(owner)) {
        MUTEX_SPIN_SPLRAISE(mtx);
        MUTEX_WANTLOCK(mtx);
#ifdef FULL
        if (MUTEX_SPINBIT_LOCK_TRY(mtx)) {
            MUTEX_LOCKED(mtx);
            return;
        }
        MUTEX_ABORT(mtx, "locking against myself");

        count = SPINLOCK_BACKOFF_MIN;

        do {
            while (MUTEX_SPINBIT_LOCKED_P(mtx)) {
                SPINLOCK_BACKOFF(count);
            }
        } while (!MUTEX_SPINBIT_LOCK_TRY(mtx));

        if (count != SPINLOCK_BACKOFF_MIN) {
        }
#endif  /* FULL */
        MUTEX_LOCKED(mtx);
        return;
    }

    curthread = (uintptr_t)curlwp;

    MUTEX_DASSERT(mtx, MUTEX_ADAPTIVE_P(owner));
    MUTEX_ASSERT(mtx, curthread != 0);
    MUTEX_ASSERT(mtx, !cpu_intr_p());
    MUTEX_WANTLOCK(mtx);

    if (panicstr == NULL) {
        KDASSERT(pserialize_not_in_read_section());
    }

    KPREEMPT_DISABLE(curlwp);
    for (;;) {
        if (!MUTEX_OWNED(owner)) {
            if (MUTEX_ACQUIRE(mtx, curthread))
                break;
            owner = mtx->mtx_owner;
            continue;
        }
        if (__predict_false(MUTEX_OWNER(owner) == curthread)) {
            MUTEX_ABORT(mtx, "locking against myself");
        }
        
        if (mutex_oncpu(owner)) {
            count = SPINLOCK_BACKOFF_MIN;
            do {
                KPREEMPT_ENABLE(curlwp);
                SPINLOCK_BACKOFF(count);
                KPREEMPT_DISABLE(curlwp);
                owner = mtx->mtx_owner;
            } while (mutex_oncpu(owner));
            if (!MUTEX_OWNED(owner))
                continue;
        }

        ts = turnstile_lookup(mtx);

        if (!MUTEX_SET_WAITERS(mtx, owner)) {
            turnstile_exit(mtx);
            owner = mtx->mtx_owner;
            continue;
        }

        membar_consumer();
        if (mutex_oncpu(owner)) {
            turnstile_exit(mtx);
            owner = mtx->mtx_owner;
            continue;
        }
        membar_consumer();
        if (!MUTEX_HAS_WAITERS(mtx)) {
            turnstile_exit(mtx);
            owner = mtx->mtx_owner;
            continue;
        }

        turnstile_block(ts, TS_WRITER_Q, mtx, &mutex_syncobj);

        owner = mtx->mtx_owner;
    }
    KPREEMPT_ENABLE(curlwp);

    MUTEX_DASSERT(mtx, MUTEX_OWNER(mtx->mtx_owner) == curthread);
    MUTEX_LOCKED(mtx);
}
```

* najpierw sprawdzamy czy mamy ustawiony bit na `MUTEX_SPIN`
    * potem mamy pętlę w której kręcimy się i sprawdzamy, czy można założyć mutex
    * i kończymy jak się skończy
* w innym wypadku jesteśmy adaptacyjnym mutexem
    * wyłączamy wywłaszczanko
    * i się kręcimy
        * jeśli mutex nie ma ownera
            * próbujemy go zablokować -- mógł zostać własnie zwolniony albo dopiero jest zkaładany
        * jeśli jesteśmy właścicielem to jesteśmy głupi -- po co blokować samych siebie?
        * jeżeli właściciel mutexa robi coś na jakimś procesorze (wykonuje się) to będziemy się kręcić (spinlock) -- zauważmy, że jak nie złapiemy mutexa w tym momencie, to poleci `continue` i pętla wróci do początku więc najwyżej znów tutaj wpadniemy
        * szukamy sobie chaina z turnstile
        * jeśli nie uda nam się powiedzieć, że czekamy na mutexa, to kręcimy się znów `continue`
        * robimy bariery pamięciowe
        * jeśli właściciel się kręci na CPU lub nie ma żadnych czekających na mutexie to abortujemy operację na turnstile i kręcimy się dalej
        * jeśli nic się nie udało to robimy turnstile_block
    * włączamy wywłaszczanko


[`mutex_vector_exit`](http://bxr.su/NetBSD/sys/kern/kern_mutex.c#mutex_vector_exit)

```C=713
void
mutex_vector_exit(kmutex_t *mtx)
{
    turnstile_t *ts;
    uintptr_t curthread;

    if (MUTEX_SPIN_P(mtx->mtx_owner)) {
#ifdef FULL
        if (__predict_false(!MUTEX_SPINBIT_LOCKED_P(mtx))) {
            MUTEX_ABORT(mtx, "exiting unheld spin mutex");
        }
        MUTEX_UNLOCKED(mtx);
        MUTEX_SPINBIT_LOCK_UNLOCK(mtx);
#endif
        MUTEX_SPIN_SPLRESTORE(mtx);
        return;
    }

#ifndef __HAVE_MUTEX_STUBS
    if (__predict_false(cold)) {
        MUTEX_UNLOCKED(mtx);
        MUTEX_RELEASE(mtx);
        return;
    }
#endif

    curthread = (uintptr_t)curlwp;
    MUTEX_DASSERT(mtx, curthread != 0);
    MUTEX_ASSERT(mtx, MUTEX_OWNER(mtx->mtx_owner) == curthread);
    MUTEX_UNLOCKED(mtx);
    __USE(curthread);

    ts = turnstile_lookup(mtx);

    if (ts == NULL) {
        MUTEX_RELEASE(mtx);
        turnstile_exit(mtx);
    } else {
        MUTEX_RELEASE(mtx);
        turnstile_wakeup(ts, TS_WRITER_Q,
            TS_WAITERS(ts, TS_WRITER_Q), NULL);
    }
}
```

* jeśli byliśmy spinlockiem to po prostu czyścimy z mutexa informacje o tym, że był zajęty + bariery pamięciowe i koniec
* potem mamy jakiegoś ifa na `MUTEX_STUBS` no i tam po prostu zwalniamy mutex i koniec
* szukamy łańcucha turnstile
* jak nic nie znaleźlismy to zwalniamy mutex i idziemy
* jak coś znaleźliśmy to zwalniamy mutex i robimy wakeupa z łańcucha

# Dyskusja

> [name=Samuel Li] Moje notatki:
> https://hackmd.io/@lasamlai/rJbqtVJqU


> [name=Paweł Jasiak]
> ciekawy komentarz: ![](https://i.imgur.com/whEs2I3.png)


> [name=Paweł Jasiak]
> obrazek do zadania 1. ![](https://i.imgur.com/jzFlKsp.png)
