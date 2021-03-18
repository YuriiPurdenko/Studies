# Ćwiczenia 9 (13 maja 2020)

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

>[name=Wiktor Pilarczyk]
>Z [mimikera](https://github.com/cahirwpz/mimiker/pull/418/files/7ec7d9df8c4c03aa58a66b0f31e4e01f4dbdf0f5).
>:::spoiler
>![](https://i.imgur.com/whEs2I3.png)
>:::
>>[name=Mikołaj Kowalik]
>>dzięki, Wiktor!
## Zadanie 6-4

:::warning
Autor: Michał Opanowicz
:::

#### kmutex

```c
struct kmutex {
    volatile uintptr_t  mtx_owner;
    ipl_cookie_t        mtx_ipl = IPL_NONE;
    __cpu_simple_lock_t mtx_lock;
};
```

>[name=Samuel Li] A nie na odwrót?

Bit `WAITERS` pola `mtx_owner` oznacza, że na zwolnienie mutexu czekają jakieś wątki. W następnym zadaniu jego użycie powinno być dokładniej opisane.

#### O barierach:
Bariery jedynie synchronizują wydarzenia **w tym wątku** i dotyczą **jedynie** out of order execution. Bariery są konieczne aby zagwarantować, że nie zaczniemy w wyniku out of order execution wykonywać operacji na chronionej sekcji, zanim weźmiemy mutexa - przykład (zakładający naiwną implementację niestosującą barier):

Program A:
```
mutex_acquire(mtx)
temp = val
mutex_release(mtx)
print(temp)
```

Program B:
```
mutex_acquire(mtx)
val = 2137
val = 1
mutex_release(mtx)
```

Rozważmy następujący ciąg zdarzeń:
* B bierze mutex i wpisuje do val 2137
* A wczytuje wartość val do temp, zanim otrzyma mtx (bo procesor nie jest świadomy, że val może się zmienić w międzyczasie)
* B wpisuje 1 do val i zwalnia mutex
* A otrzymuje mutex, zwalnia mutex nie robiąc nic - bo wczytał wartość do temp wcześniej
* A printuje 2137.


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

mutex_vector_exit(kmutex_t *mtx)
{
    turnstile_t *ts;
    uintptr_t curthread;

    if (MUTEX_SPIN_P(mtx->mtx_owner)) {
#ifdef FULL
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

## Zadanie 6-6

:::warning
Autor: Franciszek Zdobylak
:::
#### W jakim celu rogatka śledzi właściciela `ts_owner` blokady?  
Znając właściciela blokady, gdy na turnstile'u zacznie czekać wątek o wyższym priorytecie, to będziemy wiedzieć komu należy pożyczyć priorytet.

#### Czemu istnieją dwie listy wątków zablokowanych `ts_blocked` na danej rogatce?  
Dwie listy są używane przez implementację niektórych środków synchroniezcji (co zobaczymy w implementacji `rwlock`)

#### Względem jakiego parametru posortowane są listy `ts_blocked` i dlaczego?  
Listy są posortowane względem priorytetu, aby ułatwić wyszukanie priorytetu, który chcemy pożyczyć wątkowi, który trzyma blokadę.  
Aby ustalić priorytet, który należy wziąc wystarczy spojrzeć na pierwszy element listy i nie trzeba po niej całej przechodzić.

#### W jakim celu wątek przechowuje listę rogatek `td_contested` wszystkich posiadanych blokad, o które istnieje współzawodnictwo?  
Aby móc wyznaczyć priorytet wątku po zwolnieniu jednej z trzymanych blokad. Wystarczy wtedy przejść po liście `td_contested` i wziąć z niej najwyższy priorytet.

#### Czy wybudzanie wątków byłyby prostsze, gdyby rogatki były przypisane na stałe do wątków?  
Problem powstaje, gdy chcemy wybudzić wszystkich czekających na tej rogatce, ale w między czasie przyjdzie ktoś nowy, kto się zablokuje. 

1. Przenosimy wszystkie czekające wątki na kolejkę pending.
2. Przychodzi nowy wątek który już nie zostanie przeniesiony.
3. Trzeba przepiąć jakoś turnstile, bo wątek który jest właścicielem aktualnego ucieka sobie. (ten punkt może być bardzo pokręcony)

![](https://i.imgur.com/qZG7JFE.jpg)

> [name=Arek] 
> :::spoiler
> Jeśli wątek, który użyczył swojej rogatki blokadzie, zostanie wybudzony,
> nie zawsze może zabrać swoją rogatkę (zwłaszcza jeśli coś na niej jest, tzn ponad dwa wątki czekają na blokadę).
> Wtedy musi efektywnie zamienić się rogatką z kimś, kto na niej jest,
> przepisując wszystkie pola zamiast zamienić wskaźnik.
> Inaczej jeśli ponownie będzie chciał użyczyć rogatki, nie będzie mógł.
> Nie będzie też mógł zakończyć działania (właśc. zwolnić pamięci struktur), dopóki nie zwolni się jego rogatka.
> :::
> [name=Bartosz] W ostatnim pytaniu - czy może przyjść nowy wątek podczas turnstaile_broadcast, jeśli mamy blokadę na chain'a?
## Zadanie 6-7

:::warning
Autor: Arek Kozdra
:::

:::warning
## Solaris
![](https://i.imgur.com/0oZC3ka.png)
> There are two states for the reader writer lock, depending on whether the lock is held by a writer, as indicated by bit wrlock. 2, Bit 2, wrlock, is the actual write lock, and it determines the meaning of the high-order bits. If the write lock is held (bit 2 set), then the upper bits contain a pointer to the kernel thread holding the write lock. If bit 2 is clear, then the upper bits contain a count of the number of threads holding the lock as a read lock.
> ~ Solaris Internals: Solaris 10 and OpenSolaris Kernel Architecture
:::

### semantyka słowa rw_owner
Według [komentarza](http://bxr.su/NetBSD/sys/sys/rwlock.h#61):
Niskie pola:
* RW_HAS_WAITERS — czy jest ktoś chętny zablokowany
* RW_WRITE_WANTED — czy wśród chętnych jest ≥ 1 pisarz (służy do głodzenia czytelników)
* RW_WRITE_LOCKED — czy blokada jest wzięta przez pisarza (opcja druga: wszyscy pisarze czekają, aż czytelnicy zwolnią blokadę)
* cośtam debug, wyłącza logi odpluskwiania per-blokada, nieistotne
* RW_THREAD — maska bitowa, pod którą znajduje się:
  a) numer wątku trzymającego pisarza, jeśli RW_WRITE_LOCKED jest ustawione,
  b) liczba trzymających czytelników, w p.p.
* RW_READ_INCR — najmłodszy bit maski, o tyle zmienia się słowo, gdy pojawia się/znika trzymający czytelnik.

### użyczanie priorytetu
Z oczywistych przyczyn użyczać priorytet da się tylko pisarzowi, który trzyma blokadę (jest to jedyna sytuacja, w której wiemy, komu priorytet użyczyć).

### głodzenie
[komentarz](http://bxr.su/NetBSD/sys/kern/kern_rwlock.c#498) opisuje zasady budzenia i głodzenia.
Jeśli pisarz oddaje blokadę, to budzi czytelników, a w drugiej kolejności pisarzy.
Jeśli jest czekający pisarz, to nie dopuszcza nowych czytelników.
Dzięki temu każdy ma zagwarantowany postęp w drodze do dostępu i głodzenie jest minimalne.

### dodatkowe operacje
Według tego [komentarza](http://bxr.su/NetBSD/sys/kern/kern_rwlock.c#412) po otrzymaniu sterowania mamy pewność, że blokada jest nasza.

> [name=Franek] Kto jest właścicielem turnstile w przypadku gdy locka trzymają czytelnicy?
> > [name=Samuel Li] Nikt. Jest nią grupa czytelników.

> [name=Krzysztof] Co to znaczy, że głodzenie jest "minimalne"? Istnieje scenariusz głodzenia writerów.
> >[name=Franek] Chyba chodzi o to, że normalnie Ci czytelnicy też byliby zagłodzeni, z powodu niskiego priorytetu.
>> [name=Arek] to raczej nie głodzi writerów
>>> [name=Krzysztof] Możesz cały czas budzić tylko jednego writera, głodząc tych o niższym priorytecie.
>>>> [name=Arek] ten scenariusz głodzi też na zwykłych semaforach
>>>>> [name=Krzysztof] No tak, ale istnieję środki synchronizacji, w których ten scenariusz nie występuje.
>>>
>>>> [name=Patrycja] ![](https://i.imgur.com/oEeTQ1p.png) A to nie pomaga? Czy nie rozumiem scenariusza.
>>>>> [name=Krzysztof (Guest Joseph)] Hmm, wygląda jakby rozwiązywało ten problem, tylko zastanawia mnie w takim razie, jak wygląda tutaj kwestia priorytetów. Priorytety w sleep queue zostają po prostu podbite, gdy dodany jest wątek o wyższym priorytecie, niż te w kolejce?
>>>>> [name=Bartosz] Ale to jest z Solarisu

> [name=Bartosz] Pożyczamy priorytet tylko pisarzom, ponieważ rw_owner(wchan_t obj) zwraca wskaźnik tylko w przypadku gdy aktualnie w sekcji krytycznej jest pisarz. Fcja rw_owner jest wskazywana przez syncobj.\*(\*socj_owner)(wchan_t)
## Zadanie 6-8

:::warning
Autor: Wiktor Pilarczyk
:::
Przy próbach zdobywania blokad ustawiamy odpowiednie bity.

[rw_vector_enter](http://bxr.su/NetBSD/sys/kern/kern_rwlock.c#rw_vector_enter)
1. Ustawiwamy odpowiednie flagi w zmiennych w zależności kim jesteśmy
2. Wchodzimy do pętli
    1. Jeśli blokada nie ma właściciela lub nie musimy czekać próbujemy ją zdobyć, jeśli się udało to powiadamiamy o tym innych i wychodzimy z pętli, wpp. kręcimy się dalej
    2. Dopóki blokada działa na innym procesorze to kręcimy się z backoffem mając nadzieję, że uda nam się ją przechwycić
    3. Powyższe rzeczy zawiodły chyba czas iść spać - szukamy naszego turnstile'a
    4. W sumie przed pójściem spać upewnijmy się czy jednak nie możemy zabrać blokady, jeśli istnieje szansa wróć do początku pętli, wpp. idź spać
::: spoiler Kod
```=C
/*
 * Bits in the owner field of the lock that indicate lock state.  If the
 * WRITE_LOCKED bit is clear, then the owner field is actually a count of
 * the number of readers.  The rw_owner field is laid out like so:
 *
 *  N                     5        4        3        2        1        0
 *  +------------------------------------------------------------------+
 *  | owner or read count | nodbug | <free> | wrlock | wrwant |  wait  |
 *  +------------------------------------------------------------------+
 */
 
#define RW_HAS_WAITERS      0x01UL  /* lock has waiters */
#define RW_WRITE_WANTED     0x02UL  /* >= 1 waiter is a writer */
#define RW_WRITE_LOCKED     0x04UL  /* lock is currently write locked */

#define RW_READ_COUNT_SHIFT 5
 define RW_READ_INCR        (1UL << RW_READ_COUNT_SHIFT)
#define RW_THREAD       ((uintptr_t)-RW_READ_INCR)
#define RW_OWNER(rw)        ((rw)->rw_owner & RW_THREAD)
#define RW_COUNT(rw)        ((rw)->rw_owner & RW_THREAD)
#define RW_FLAGS(rw)        ((rw)->rw_owner & ~RW_THREAD)
```
```C
typedef enum krw_t {
    RW_READER = 0,
    RW_WRITER = 1
} krw_t;
```

```C=287
void
rw_vector_enter(krwlock_t *rw, const krw_t op)
{
    uintptr_t owner, incr, need_wait, set_wait, curthread, next;
    turnstile_t *ts;
    int queue;
    lwp_t *l;

    l = curlwp;
    curthread = (uintptr_t)l;

    if (__predict_true(op == RW_READER)) {
        incr = RW_READ_INCR;
        set_wait = RW_HAS_WAITERS;
        need_wait = RW_WRITE_LOCKED | RW_WRITE_WANTED;
        queue = TS_READER_Q;
    } else {
        RW_ASSERT(rw, op == RW_WRITER);
        incr = curthread | RW_WRITE_LOCKED;
        set_wait = RW_HAS_WAITERS | RW_WRITE_WANTED;
        need_wait = RW_WRITE_LOCKED | RW_THREAD;
        queue = TS_WRITER_Q;
    }

    KPREEMPT_DISABLE(curlwp);
    for (owner = rw->rw_owner;;) {
        if ((owner & need_wait) == 0) {
            next = rw_cas(rw, owner, (owner + incr) &
                ~RW_WRITE_WANTED);
            if (__predict_true(next == owner)) {
                /* Got it! */
                RW_MEMBAR_ENTER();
                break;
            }

            owner = next;
            continue;
        }
        if (__predict_false(RW_OWNER(rw) == curthread)) {
            rw_abort(__func__, __LINE__, rw,
                "locking against myself");
        }
        if (rw_oncpu(owner)) {
            u_int count = SPINLOCK_BACKOFF_MIN;
            do {
                KPREEMPT_ENABLE(curlwp);
                SPINLOCK_BACKOFF(count);
                KPREEMPT_DISABLE(curlwp);
                owner = rw->rw_owner;
            } while (rw_oncpu(owner));
            if ((owner & need_wait) == 0)
                continue;
        }

        ts = turnstile_lookup(rw);

        owner = rw->rw_owner;
        if ((owner & need_wait) == 0 || rw_oncpu(owner)) {
            turnstile_exit(rw);
            continue;
        }
        next = rw_cas(rw, owner, owner | set_wait);
        if (__predict_false(next != owner)) {
            turnstile_exit(rw);
            owner = next;
            continue;
        }

        turnstile_block(ts, queue, rw, &rw_syncobj);

        if (op == RW_READER || (rw->rw_owner & RW_THREAD) == curthread)
            break;

        owner = rw->rw_owner;
    }
    KPREEMPT_ENABLE(curlwp);
}
```
:::
[rw_vector_exit](http://bxr.su/NetBSD/sys/kern/kern_rwlock.c#rw_vector_exit)

Działanie rw_vector_exit:
1. Podobnie jak wcześniej sprawdzamy czy jesteśmy pisarzem czy czytelnikiem i ustawiamy odpowiednie rzeczy
2. Pętla
    1. Jeśli jesteśmy pisarzem to czyścimy wskaźnik na nas, jeśli jesteśmy czytelnikiem to zmniejszamy liczbe czytelników
    2. Jeśli początkowe x bitów się wyczyściło i ktoś czeka to nikt nie ma blokady więc wychodzimy z pętli 
    3. Aktualizujemy początkowe x bitów jeśli się nie udało to powtarzamy pętle - ktoś inny zaktualizował
3. Blokada jest pusta i ktoś na nią czeka
4. Jeśli nie ma czytelników czekających lub jesteśmy czytelnikiem to jeśli jest jeden pisarz to jemu przekaż locka wpp niech sie bija o blokade
5. Jeśli są czytelnicy i jesteśmy pisarzem oddaj locka wszystkim czytelnikom
:::spoiler Kod
```C=440
void
rw_vector_exit(krwlock_t *rw)
{
    uintptr_t curthread, owner, decr, newown, next;
    turnstile_t *ts;
    int rcnt, wcnt;
    lwp_t *l;

    l = curlwp;
    curthread = (uintptr_t)l;
    RW_ASSERT(rw, curthread != 0);

    owner = rw->rw_owner;
    if (__predict_false((owner & RW_WRITE_LOCKED) != 0)) {
        RW_ASSERT(rw, RW_OWNER(rw) == curthread);
        decr = curthread | RW_WRITE_LOCKED;
    } else {
        RW_ASSERT(rw, RW_COUNT(rw) != 0);
        decr = RW_READ_INCR;
    }

    RW_MEMBAR_EXIT();
    for (;;) {
        newown = (owner - decr);
        if ((newown & (RW_THREAD | RW_HAS_WAITERS)) == RW_HAS_WAITERS)
            break;
        next = rw_cas(rw, owner, newown);
        if (__predict_true(next == owner))
            return;
        owner = next;
    }

    ts = turnstile_lookup(rw);
    owner = rw->rw_owner;
    RW_ASSERT(rw, ts != NULL);
    RW_ASSERT(rw, (owner & RW_HAS_WAITERS) != 0);

    wcnt = TS_WAITERS(ts, TS_WRITER_Q);
    rcnt = TS_WAITERS(ts, TS_READER_Q);

    if (rcnt == 0 || decr == RW_READ_INCR) {
        RW_ASSERT(rw, wcnt != 0);
        RW_ASSERT(rw, (owner & RW_WRITE_WANTED) != 0);

        if (rcnt != 0) {
            l = TS_FIRST(ts, TS_WRITER_Q);
            newown = (uintptr_t)l | (owner & RW_NODEBUG);
            newown |= RW_WRITE_LOCKED | RW_HAS_WAITERS;
            if (wcnt > 1)
                newown |= RW_WRITE_WANTED;
            rw_swap(rw, owner, newown);
            turnstile_wakeup(ts, TS_WRITER_Q, 1, l);
        } else {
            newown = owner & RW_NODEBUG;
            newown |= RW_WRITE_WANTED;
            rw_swap(rw, owner, newown);
            turnstile_wakeup(ts, TS_WRITER_Q, wcnt, NULL);
        }
    } else {
        RW_ASSERT(rw, rcnt != 0);

        newown = owner & RW_NODEBUG;
        newown += rcnt << RW_READ_COUNT_SHIFT;
        if (wcnt != 0)
            newown |= RW_HAS_WAITERS | RW_WRITE_WANTED;

        rw_swap(rw, owner, newown);
        turnstile_wakeup(ts, TS_READER_Q, rcnt, NULL);
    }
}
```
:::
### Jakie różnice zauważasz w stosunku do algorytmu opisanego w [3, §17.6.1]?
1. Pisarz podczas zwalniania blokady nie oddaje swojego priorytetu czytelnikom
2. Nie ma kolejki fifo na pisarzach
> [name=Michał Błaszczyk]
> Ale kod mówi: /* Give the lock to the longest waiting writer. */
> [fragment kodu](http://bxr.su/NetBSD/sys/kern/kern_rwlock.c#514)
> Pisarze są wybudzani względem malejącego czasu oczekiwania. Więc jest FIFO.
4. Jeśli jesteśmy pisarzem to zawsze staramy się wybudzić czytelników

> [name=Michał Błaszczyk] 
> - **Głodzenie**
    - SOLARIS:
      1. Wątki czytelników mogą być głodzone (w przypadku napływu pisarzy o wyższym priorytecie).
      2. Wątki pisarzy nie są głodzone (ponieważ pisarze wybudzani są w kolejności malejących czasów oczekiwania).
    - NetBSD:
      1. Wątki czytelników nie są głodzone (ponieważ pisarz zwalniający blokadę wybudza **wszystkich** oczekujących czytelników).
      2. Jak w SOLARIS (pod warunkiem że przytoczony wyżej fragment kodu mówi prawdę (wtedy jest FIFO)).
> - **Własność blokady po wybudzeniu wątku**
    - SOLARIS:
        Po wybudzeniu, zarówno wątek pisarza jak i czytelnika, ma gwarancję posiadania blokady.
    - NetBSD:
      1. Po wybudzeniu, wątk czytelnika ma gwarancję posiadania blokady (zwalniający inkrementuje odpowiednio licznik czytających).
      2. Jeżeli przy wybudzaniu pisarza istnieją oczekujący czytelnicy, wówczas pisarz ten po wybudzeniu ma gwarancję posiadania blokady. Jeżeli przy wybudzaniu pisarza liczba oczekujących czytelników wynosi 0, wówczas budzeni są **wszyscy** pisarze i potencjalnie występuje rywalizacja o własość blokady.

## Dyskusja
> [name=Samuel Li] Notatki https://hackmd.io/@2xEkfCY7RzWt-1lQh8Ifhw/H1yXpWd58
