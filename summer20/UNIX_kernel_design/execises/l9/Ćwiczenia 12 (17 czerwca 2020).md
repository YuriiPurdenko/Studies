# Ćwiczenia 12 (17 czerwca 2020)

## Zadanie 9-1

:::warning
Autor: Jakub Urbańczyk
:::
Wykorzystanie procedur związanych z [pmap(9)](https://netbsd.gw.com/cgi-bin/man-cgi?pmap+9+NetBSD-current):
 * [mmap](https://netbsd.gw.com/cgi-bin/man-cgi?mmap+2+NetBSD-current) - map files or devices into memory
     * pmap_enter - *Create a mapping in physical map pmap for the physical address pa at the virtual address va* - gdy poleci page fault i gdy pager ogarnie strone, to trzeba ją będzie wrzucić do page table - 
     * pmap_zero_page - tutaj podobnie, podczas page fault, pager anonimowej pamięci zapewne będzie chciał wyzerować tę stronę - *Zero the PAGE_SIZE sized region starting at physical address pa.*
 * [munmap](https://netbsd.gw.com/cgi-bin/man-cgi?munmap+2+NetBSD-current) - remove a mapping
     * void pmap_remove - chcemy odpiąć mapowanie, żeby znów odwołania w to miejsce powodowały page faulty *Remove mappings from the virtual address range sva to eva from the specified physical map.* 
 * [mprotect](https://netbsd.gw.com/cgi-bin/man-cgi?mprotect+2+NetBSD-current) - control the protection of pages
     * void pmap_protect - raczej oczywiste - *Set the protection of the mappings in the virtual address range sva to eva in the specified physical map.*
 * [fork](https://netbsd.gw.com/cgi-bin/man-cgi?fork+2+NetBSD-current) - create a new process
     * pmap_create() - tutaj będziemy klonować przestrzeń adresową, więc pewnie chcemy przekopiować page table
     * pmap_copy() - *This function copies the mappings starting at src_addr in src_map for len bytes into dst_map starting at dst_addr.*
> [name=Michał Błaszczyk]
> pmap_page_protect
 * [execv](https://netbsd.gw.com/cgi-bin/man-cgi?execve+2+NetBSD-current) - execute a file
     * pmap_create
     * pmap_destroy
     * pmap_activate
 * pageout daemon
     * pmap_page_protect ??
     * pmap_page_remove
     * pmap_is_referenced
     * pmap_is_modified
> [name=pj] [pdaemon](http://bxr.su/OpenBSD/sys/uvm/uvm_pdaemon.c#492)
 * swapper
     * pmap_page_remove
     * pmap_enter - żeby przywrócić wpisy wyswappowanych stron

**Demon stronicowania** - demon odpowiedzialny za synchronizację brudnych stron wraz z ich obiekatami wspierającymi oraz dba o to, że były odpowiednie stron typu FREE + CACHE oraz INACTIVE.

Każde dodanie wpisu lub usunięcie będzie powodowało konieczność wyczyszczenia części TLB jeśli istnieje taki wpis już z takim samym ASID. Zaś np. taka funkcja jak pmap_activate, czyli przełączenie się na inny page table będzie wymagała zinwalidowanie całej zawartości tablicy stron (gdy nie obsługujemy asid).

Dla każdej ramki `vm_page_t` jest lista złożona z `pv_entry` mówiąca o tym, gdzie jest ta ramka podpięta:
![](https://i.imgur.com/L9KDIVp.png)

Wykorzystujemy to w:
 * [pmap_page_remove()](http://bxr.su/NetBSD/sys/uvm/pmap/pmap.c#741) - Remove this page from all physical maps in which it resides.
* [pmap_page_protect()](http://bxr.su/NetBSD/sys/uvm/pmap/pmap.c#1009) - Lower the permission for all mappings to a given page.
* [pmap_clear_modify()](http://bxr.su/NetBSD/sys/uvm/pmap/pmap.c#1669) - Clear the modify bits on the specified physical page.

## Zadanie 9-2

:::warning
Autor: Mateusz Maciejewski
:::
Bity monitorowania dostępu to metadane na temat tego czy i jakie dostępy miały miejsce dla danej strony. Pozwalają określić jak dana strona została użyta w okresie od ostatniego sprawdzenia (wyczyszczenia). Standardowo są to bity `modified` oraz licznik/flaga `referenced` które kolejno odpowiadają zapisom do strony oraz ogólnym dostępom.

Korzystając z opisanych w treści polecenia wyjątków łatwo jest zaimplementować sprzętowo powyższe metadane. 

**`referenced`**:
Niech w `vm_page` (lub innej strukturze opisującej ramki) znajduje się zmienna (bit) `referenced`. Przy `TLB refill exception` przy okazji wtaczania wpisu do TLB zapal bit `referenced` dla strony (jako że dostęp do niej spowodował chybienie w TLB). Wpis będzie miał zapalony bit `valid`. Następnie za każdym razem, gdy bit `referenced` jest gaszony, zgaś także bit `valid` w TLB. W obsłudze wyjątku `TLB invalid exception` zapalaj bit `referenced` oraz `valid` w TLB. 

**`modified`**:
Analogicznie jak w poprzedniej sytuacji dodajemy bit `modified` w odpowiedniej strukturze. Chcemy także przechowywać bit `dirty` dodatkowo poza TLB. Przy dodawaniu wpisu do TLB ustaw bit `dirty` na 0. W obsłudze wyjątku `TLB modified exception` zapal `modified` oraz `dirty`, zarówno w TLB jak i opisie ramki. Przy gaszeniu `modified` zgaś także `dirty` (w TLB), by kolejna modyfikacja znów wywołała wyjątek.

Powyższy "algorytm" wydaje się być wystarczający by minimalnym nakładem otrzymać emulację bitów `modified` i `referenced` na architekturze MIPS.

## Zadanie 9-3

:::warning
Autor: Krzysztof Boroński
:::

### **TLB** na maszynach **SMP**


Problem na wieloprocesorowych maszynach **SMP** jest taki, że zmiany wszelkie zmiany, kóre musz być
odzwierdciedlane w **TLB** zwykle mogą być sprzętowo odzwierciedlone wyłącznie na **TLB** procesora,
z którego zostały one dokonane. Pozostałe procesory w związku z tym pozostają z błędnymi wpisami w
**TLB**. Trzeba je w związku z tym zsynchronizować z poziomu jądra systemu operacyjnego.

#### Jak to wygląda w *Machu*?

(Prosty algorytm *TLB shootdown*)

Potrzebnych będzie kilka struktur dla każdego z procesorów:

* flaga *active*, która wskazuje, czy dany procesor korzysta obecnie z jakiejś strony pamięci, która
  jest modyfikowalna.
* Kolejka żądań unieważniania translacji z **TLB**.
* Zbiór aktywnych obecnie mapowań pamięci (zwykle są dwa: kernel i wykonywane w danej chwili
  zadanie).

Każde mapowanie ma swojego spinlocka oraz listę procesorów, które go używaja w danej chwili.

W chwili, gdy zostanie wykonana operacja mogąca naruszyć spójność **TLB** między procesorami, jądro
wywołuje alorytm *TLB shootdown*.
1. Procesor, który zainicjalizował zmianę wyłącza wszystkie swoje
   przerwania oraz zeruje flagę *active*.
2. Następnie zakłada on *spin-locka* na odpowiendiego *pmapa*.
3. Potem wysyła do każdego procesora żadanie unieważnienia translacji przy pomocy wcześniej
   wspomnianych kolejek.
4. Po wsadzeniu żądań na kolejki, procesor wywołuje przerwania międzyprocesorowe dla każdego z
   pozostąłych procesorów.
5. Kiedy któryś z tych pozostałych procesorów otrzyma przerwanie, wyłaćza on swoje prerwania,
   odbiera żadanie ze swojej kolejki i czyści swoją flagę *active*.
6. Następnie próbuje założyć *spin-locka* na odpowiednie *pmap*. W tym momenci *pmap jeszcze należy
   do procesora, który zainicjował zmianę, także próba skończy się na czekaniu.
7. Gdy już wszystkie procesory wyzerują swoje flagi *active*, procesor odpowiedzialny z zmianę
   czyści swoje **TLB** i wprowadzą odpowiednią zmianę do *pmap*.
8. Po wprowadzeniu zmiany, procesor zwalnia blokadę, włącza przerwania, ustawia flagę *zctive* i
   wraca do tego, czym zajmował się wcześniej.
9. Pozostałe procesory w tym czasie chwytają blokadę i aktualizują swoje **TLB**, po czym również
   włączają przerwania, ustawiają flagę *active* i wracają do wcześniejszej pracy.

![TLB Shootdown (Mach) algorithm illustration](https://i.imgur.com/YGDYBsh.png)


> [name=pj]
> ![](https://i.imgur.com/qzNhDAR.png)
> ![](https://i.imgur.com/77eLMCm.png)

## Zadanie 9-4

:::warning
Autor: Michał Opanowicz
:::

### Wydruk `x86info -a`:

```
L1 Data TLB: 4KiB pages, 16 entries
L1 Data TLB: 4MiB pages, 4 entries
L2 Data TLB: 4KiB pages, 256 entries
L2 Data TLB: 4MiB pages, 32 entries
```

* Zasięg dla **L1** to $4 \cdot 4MiB + 16 \cdot 4KiB$.
* Zasięg dla **L2** to $32 \cdot 4MiB + 256 \cdot 4KiB$.

### Duże strony

Duże strony zostały wprowadzone ze względu na to, że *zasięg **TLB** nie zwiększał się z ubiegiem
lat tak szybko jak zwiększała się pamięc operacyjna w systemach*. Nieuważne korzystanie z
*Superstron* może jednak przyczynić się do znacznego zużycia pamięci, gdy kolejne strony będą
przydzielane zbyt łapczywie. Dla tego potrzebny jest jakiś *mechanizm*, który pozwoli zażądać nimi
w rozsądny sposób, najlepiej konsolidująć zwyczajne strony do superstron.

Zauważmy że mapowanie pamięci tworzy B-drzewo o 1024 dzieciach w każdym węźle. Stworzenie superstrony odpowiada "ucięciu" drzewa i wstawienie liścia odpowiadającego większemu obszarowi pamięci. Możemy zatem naturalnie promować węzły niebędące liściami do superstron, jeżeli okaże się, że wszystkie dzieci danego wierzchołka są zaalokowanymi stronami. Dzięki temu nie zużyjemy dodatkowej pamięci.

W systemie _**FreeBSD**_ strony anonimowe mogą zostać wypromowane do *superstrony*, jeżeli region pamięci zajmowane przez nie jest odpowiednio duży od razu, lub spodziewamy się, że będzie rosnąć (stos lub sterta).

Strony plików mapowanych w pamięć mogą zostać wypromowane do superstrony, jeżeli pokrywają odpowiedni obszar pamięci - jeżeli po zmapowaniu pliku w pamięć zaalokowane strony pokrywają całą superstronę, obszar ten będzie potencjalną superstroną, w przeciwnym wypadku to nie nastąpi.

> [name=Michał Błaszczyk]
> Rozważanie używania super strony rozpoczyna się rezerwacją super strony. Super story dla 
> procesu są rozważane na poziomie regionów pamięci. Przy pierwszym błędzie strony dla każdego  regionu, system musi ustalić czy dany region jest odpowiedni do używania super stron (np. inne traktowanie dla zmapowanego pliku, stosu, sterty, pamięci jądra). Jeżeli region nie nadaje się na używanie super stron, wówczas pierwszy i wszystkie kolejne błędy strony są uzupełniane stornami bazowymi. Jeżeli region uzyskał rezerwację super story, wówczas dla części regionu zawierającego stronę która spowodowałę błąd, przypisywana jest superstrona. Jednakże jedynie bazowa strona uzupełniająca błąd, jest inicjalizowana oraz umieszczana w tablcy stron. Każda superstrona posiada mapę populacji śledzącą używane strony. W miarę kolejnych błędów stron w obszarze superstrony, kolejne bazowe strony są inicjalizowane oraz dodawane do tablcy stron, jednocześnie aktualizyjąc informację w mapie populacji danej super strony. 
>  ![](https://i.imgur.com/UzeGmM9.jpg)
>  
>  Super strona jest gotowa do promocji gdy każda strona w jej obrębie wykonała błąd strony. Promocja oznacza zastąpienie wpisu katalogu wskazującego na tablicę stron składających się na super stronę przez wpis wskazujący na super stronę.
> ![](https://i.imgur.com/ja1gwqY.jpg)
> 
>   Super strona oznaczana jest jako read-only, chyba że wszystkie tworzące ją strony są zmodyfikowane. Gdy read-only super stona jest modyfikowa, jest ona ponownie dzielona na strony bazowe dzięki czemu modyfikacje mogą być śledzone na poziomie stron bazowych.

## Zadanie 9-5

:::warning
Autor: Paweł Jasiak
:::

![](https://i.imgur.com/3nTOIo2.png)


[alokacja](http://bxr.su/FreeBSD/sys/vm/vm_phys.c#vm_phys_alloc_freelist_pages)

```C=832
/*
 * Allocate a contiguous, power of two-sized set of physical pages from the
 * specified free list.  The free list must be specified using one of the
 * manifest constants VM_FREELIST_*.
 *
 * The free page queues must be locked.
 */
vm_page_t
vm_phys_alloc_freelist_pages(int domain, int freelist, int pool, int order)
{
    struct vm_freelist *alt, *fl;
    vm_page_t m;
    int oind, pind, flind;

    KASSERT(domain >= 0 && domain < vm_ndomains,
        ("vm_phys_alloc_freelist_pages: domain %d is out of range",
        domain));
    KASSERT(freelist < VM_NFREELIST,
        ("vm_phys_alloc_freelist_pages: freelist %d is out of range",
        freelist));
    KASSERT(pool < VM_NFREEPOOL,
        ("vm_phys_alloc_freelist_pages: pool %d is out of range", pool));
    KASSERT(order < VM_NFREEORDER,
        ("vm_phys_alloc_freelist_pages: order %d is out of range", order));

    flind = vm_freelist_to_flind[freelist];
    /* Check if freelist is present */
    if (flind < 0)
        return (NULL);

    vm_domain_free_assert_locked(VM_DOMAIN(domain));
    fl = &vm_phys_free_queues[domain][flind][pool][0];
    for (oind = order; oind < VM_NFREEORDER; oind++) {
        m = TAILQ_FIRST(&fl[oind].pl);
        if (m != NULL) {
            vm_freelist_rem(fl, m, oind);
            /* The order [order, oind) queues are empty. */
            vm_phys_split_pages(m, oind, fl, order, 1);
            return (m);
        }
    }

    /*
     * The given pool was empty.  Find the largest
     * contiguous, power-of-two-sized set of pages in any
     * pool.  Transfer these pages to the given pool, and
     * use them to satisfy the allocation.
     */
    for (oind = VM_NFREEORDER - 1; oind >= order; oind--) {
        for (pind = 0; pind < VM_NFREEPOOL; pind++) {
            alt = &vm_phys_free_queues[domain][flind][pind][0];
            m = TAILQ_FIRST(&alt[oind].pl);
            if (m != NULL) {
                vm_freelist_rem(alt, m, oind);
                vm_phys_set_pool(pool, m, oind);
                /* The order [order, oind) queues are empty. */
                vm_phys_split_pages(m, oind, fl, order, 1);
                return (m);
            }
        }
    }
    return (NULL);
}
```

No to co tutaj się dzieje?
* bierzemy sobie indeks naszej kolejki (możemy ustalić, że on jest stały, bo przyjeliśmy, że to jest wartość \(1\))
```
struct vm_freelist {
    struct pglist pl;
    int lcnt;
};
```
* pglist to po prostu lista `vm_page`
* zakładamy sobie locka
* pobieramy tą listę
* lecimy sobie po tej liście (tam mamy potęgi dwóej pamięci)
* jak coś znajdziemy no to będziemy to brać
* wywalamy gościa z kolejki
* jeśli był zbyt duży to go dzielimy i to czego nie potrzebujemy oddamy do kolejki
* no ale mogło dojść do takiej sytuacji, że w naszej puli nie znaleźliśmy rozwiązania wtedy będziemy przerzucać dane z innej (ale mamy jedną pulę na potrzeby zadania)


[zwalnianie](http://bxr.su/FreeBSD/sys/vm/vm_phys.c#vm_phys_free_pages)

```C=1096
/*
 * Free a contiguous, power of two-sized set of physical pages.
 *
 * The free page queues must be locked.
 */
void
vm_phys_free_pages(vm_page_t m, int order)
{
    struct vm_freelist *fl;
    struct vm_phys_seg *seg;
    vm_paddr_t pa;
    vm_page_t m_buddy;

    KASSERT(m->order == VM_NFREEORDER,
        ("vm_phys_free_pages: page %p has unexpected order %d",
        m, m->order));
    KASSERT(m->pool < VM_NFREEPOOL,
        ("vm_phys_free_pages: page %p has unexpected pool %d",
        m, m->pool));
    KASSERT(order < VM_NFREEORDER,
        ("vm_phys_free_pages: order %d is out of range", order));
    seg = &vm_phys_segs[m->segind];
    vm_domain_free_assert_locked(VM_DOMAIN(seg->domain));
    if (order < VM_NFREEORDER - 1) {
        pa = VM_PAGE_TO_PHYS(m);
        do {
            pa ^= ((vm_paddr_t)1 << (PAGE_SHIFT + order));
            if (pa < seg->start || pa >= seg->end)
                break;
            m_buddy = &seg->first_page[atop(pa - seg->start)];
            if (m_buddy->order != order)
                break;
            fl = (*seg->free_queues)[m_buddy->pool];
            vm_freelist_rem(fl, m_buddy, order);
            if (m_buddy->pool != m->pool)
                vm_phys_set_pool(m->pool, m_buddy, order);
            order++;
            pa &= ~(((vm_paddr_t)1 << (PAGE_SHIFT + order)) - 1);
            m = &seg->first_page[atop(pa - seg->start)];
        } while (order < VM_NFREEORDER - 1);
    }
    fl = (*seg->free_queues)[m->pool];
    vm_freelist_add(fl, m, order, 1);
}
```

* znajdujemy sobie nasz segment
* i będziemy iść od naszej potęgi
* liczymy sobie nasz adres fizyczny
* i adres bliźniaka (patrz rysunek)
* sprawdzamy, czy trafimy w ten sam segment
* potem bierzemy naszego bliźniaka
* usuwamy go z jego listy
* zwiększamy sobie naszą pamięć (robimy merge)
* i wsadzamy go na listę


## Zadanie 9-6

:::warning
Autor: Wiktor Pilarczyk
:::
![](https://i.imgur.com/5eoxw7x.png)
### Argumenty
![](https://i.imgur.com/MeQNpuK.png)
:::spoiler [vmem_xalloc](http://bxr.su/FreeBSD/sys/kern/subr_vmem.c#vmem_xalloc)
```c
int
vmem_xalloc(vmem_t *vm, const vmem_size_t size0, vmem_size_t align,
    const vmem_size_t phase, const vmem_size_t nocross,
    const vmem_addr_t minaddr, const vmem_addr_t maxaddr, int flags,
    vmem_addr_t *addrp)
{
    const vmem_size_t size = vmem_roundup_size(vm, size0);
    struct vmem_freelist *list;
    struct vmem_freelist *first;
    struct vmem_freelist *end;
    bt_t *bt;
    int error;
    int strat;

    flags &= VMEM_FLAGS;
    strat = flags & VMEM_FITMASK;
    MPASS(size0 > 0);
    MPASS(size > 0);
    MPASS((flags & (M_NOWAIT|M_WAITOK)) != (M_NOWAIT|M_WAITOK));
    if ((flags & M_NOWAIT) == 0)
        WITNESS_WARN(WARN_GIANTOK | WARN_SLEEPOK, NULL, "vmem_xalloc");
    MPASS(!VMEM_CROSS_P(phase, phase + size - 1, nocross));

    *addrp = 0;

    end = &vm->vm_freelist[VMEM_MAXORDER];
    /*
     * choose a free block from which we allocate.
     */
    first = bt_freehead_toalloc(vm, size, strat);
    VMEM_LOCK(vm);
    for (;;) {
        /*
         * Scan freelists looking for a tag that satisfies the
         * allocation.  If we're doing BESTFIT we may encounter
         * sizes below the request.  If we're doing FIRSTFIT we
         * inspect only the first element from each list.
         */
        for (list = first; list < end; list++) {
            LIST_FOREACH(bt, list, bt_freelist) {
                if (bt->bt_size >= size) {
                    error = vmem_fit(bt, size, align, phase,
                        nocross, minaddr, maxaddr, addrp);
                    if (error == 0) {
                        vmem_clip(vm, bt, *addrp, size);
                        goto out;
                    }
                }
                /* FIRST skips to the next list. */
                if (strat == M_FIRSTFIT)
                    break;
            }
        }

        /*
         * Retry if the fast algorithm failed.
         */
        if (strat == M_FIRSTFIT) {
            strat = M_BESTFIT;
            first = bt_freehead_toalloc(vm, size, strat);
            continue;
        }

        /*
         * Try a few measures to bring additional resources into the
         * arena.  If all else fails, we will sleep waiting for
         * resources to be freed.
         */
        if (!vmem_try_fetch(vm, size, align, flags)) {
            error = ENOMEM;
            break;
        }
    }
out:
    VMEM_UNLOCK(vm);
    if (error != 0 && (flags & M_NOWAIT) == 0)
        panic("failed to allocate waiting allocation\n");

    return (error);
}
```
:::
* Zaokrąglamy żądany rozmiar.
* Wyciągamy flagi i politykę.
* Zapewniamy, że coś chcemy zaallocowac (size > 0) i nie mamy sprzecznych flag (na raz chcemy i nie chcemy spać)
* Jeśli istniej możliwość czekania na blok stron sprawdzamy czy nie mamy locków, które byśmy przyblokwali.
* Ustawiamy zakres wolnych list.
* Dostajem odpowiedni boundary tag - bt_freehead_toalloc
   * Bierzemy pierwszy bt, w którym może pojawić się block naszego rozmiaru
   * W przypadku FirstFit'a zapewniamy, że każdy blok jest przynajmniej żądanego rozmiaru
* Zakładamy locka i zaczynamy poszukiwania
* Jak znaleźliśmy i wszystko się zgadza to dopasowujemy bt i wychodzimy
* Wpp. 
    * Spróbujmy jeszcze raz, ale tym razem z Bestfitem (we wcześniejszym kubełku mogło coś być, reszta jest pusta więc nas to dużo nie kosztuje)
    * Jeśli znowu się nie uda to próbujemy ściągnąć zasoby i czekamy na nie - vmem_try_fetch
* Zdejmujemy lock'a upewniamy się czy udało i wychodzimy.

:::spoiler [vmem_xfree](http://bxr.su/FreeBSD/sys/kern/subr_vmem.c#vmem_xfree)
```c
void
vmem_xfree(vmem_t *vm, vmem_addr_t addr, vmem_size_t size)
{
    bt_t *bt;
    bt_t *t;

    MPASS(size > 0);

    VMEM_LOCK(vm);
    bt = bt_lookupbusy(vm, addr);
    MPASS(bt != NULL);
    MPASS(bt->bt_start == addr);
    MPASS(bt->bt_size == vmem_roundup_size(vm, size) ||
        bt->bt_size - vmem_roundup_size(vm, size) <= vm->vm_quantum_mask);
    MPASS(bt->bt_type == BT_TYPE_BUSY);
    bt_rembusy(vm, bt);
    bt->bt_type = BT_TYPE_FREE;

    /* coalesce */
    t = TAILQ_NEXT(bt, bt_seglist);
    if (t != NULL && t->bt_type == BT_TYPE_FREE) {
        MPASS(BT_END(bt) < t->bt_start);    /* YYY */
        bt->bt_size += t->bt_size;
        bt_remfree(vm, t);
        bt_remseg(vm, t);
    }
    t = TAILQ_PREV(bt, vmem_seglist, bt_seglist);
    if (t != NULL && t->bt_type == BT_TYPE_FREE) {
        MPASS(BT_END(t) < bt->bt_start);    /* YYY */
        bt->bt_size += t->bt_size;
        bt->bt_start = t->bt_start;
        bt_remfree(vm, t);
        bt_remseg(vm, t);
    }

    if (!vmem_try_release(vm, bt, false)) {
        bt_insfree(vm, bt);
        VMEM_CONDVAR_BROADCAST(vm);
        bt_freetrim(vm, BT_MAXFREE);
    }
}
```
:::
* Zakładamy lock'a
* Szukamy tagu
* Usuwamy go z listy zajętych tagów.
* Następnie sprawdzamy czy poprzednik i następnik nie jest wolny i łączmy tagi.

### Dlaczego vmem?
Zapewnia stały czas wykonania i małą fragmentację, wcześniejsze były liniowe.
