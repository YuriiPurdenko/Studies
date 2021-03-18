# Ćwiczenia 2 (18 marca 2020)

## Rozwinięcie skrótów z listy
* **AST** - Asynchronous System Trap
* **PIC** - Programmable interrupt controller (to verify)

## Zadanie 1-3

> Kiedy i w jakim celu jądro woła funkcję userret(9)? Posługując się przeglądarką kodu zreferuj działanie procedur: «mi_userret», «cpu_need_resched», «lwp_userret», «preempt». Przeczytaj również podręczniki cpu_need_resched(9) i preempt(9). Ogranicz się do omawiania ciała wymienionych procedur. Należy skrótowo opisać znaczenie wykorzystywanych podprocedur.

:::warning
Autor: Paweł Jasiak
Redaktor: Mikołaj Kowalik
:::

### man

* [userret(9)](https://netbsd.gw.com/cgi-bin/man-cgi?userret+9)
* [cpu_need_resched(9)](https://netbsd.gw.com/cgi-bin/man-cgi?cpu_need_resched+9+NetBSD-current)
* [preempt(9)](https://netbsd.gw.com/cgi-bin/man-cgi?preempt+9)

### bxr.su

* [`userret`](http://bxr.su/NetBSD/sys/arch/aarch64/aarch64/trap.c#151)
    * funkcja w ogólnym przypadku przygotowywuje do powrotu do userspace
    * Detect a change in the signal disposition of the current
      process and invoke postsig(9) to post the signal to the
      process.
    * Check the `want_resched` flag to see if the scheduler requires
      the current process to be preempted by invoking `preempt`.
    * Update the scheduler state.
    * wołamy [`mi_userret`](http://bxr.su/NetBSD/sys/sys/userret.h#82)
        * `KPREEMPT_DISABLE` -- wyłączamy wywłaszczanie
        * jeśli trzeba ogarnąć schedulera to wywłaszczamy samych siebie
        * i oddajemy sterowanie dalej
        * [`lwp_userret`](http://bxr.su/NetBSD/sys/kern/kern_lwp.c#1587) -- Handle exceptions for `mi_userret`
            * [`lwp`](http://bxr.su/NetBSD/sys/sys/lwp.h#97) -- lightweight process
            * sprawdzamy czy jesteśmy w `userret`
                * dostarczamy sygnały dla procesu
                > [name=Mikołaj Kowalik] czepianie się: jeśli lwp nie ma coredumpa lub exita,
                > [name=Paweł Jasiak] tak, nie wpisuję tutaj wszystkich szczegółów implementacyjnych, to jedynie poglądowe punkty, które wymagają i tak kodu przed oczami
                * sprwdzamy czy proces ma przejść w stan suspend lub czy robimy coredumpa,
                  zrzucamy wrzystkie potrzebne informacje dla takich operacji
                  > [name=Mikołaj Kowalik] a czy tutaj czasem sie tez nie kolejkujemy? - mi_switch? (to już jest głęboko) - nie analizowałem jak się zachowa gdy `mi_switch` jest wołany z `lwp_userret`
                  > [name=Paweł Jasiak] tak, `mi_switch` dokonuje wrzucenia na kolejkę dla schedulera [`sched_enqueue`](http://bxr.su/NetBSD/sys/kern/kern_runq.c#186)
                * jeśli kończymy proces no to po prostu koniec
                * w ostatnim przypadku uaktualniamy na jakim CPU jesteśmy
* [`cpu_need_resched`](http://bxr.su/NetBSD/sys/arch/aarch64/aarch64/cpu_machdep.c#237)
    * The cpu_need_resched() function is the machine-independent interface for
     the scheduler to notify machine-dependent code that a context switch from
     the current LWP, on the cpu ci, is required.
    * If `RESCHED_KPREEMPT` flag is specified in flags and __HAVE_PREEMPTION C
     pre-processor macro is defined in <machine/intr.h>, machine-dependent
     code should make a context switch happen as soon as possible even if the
     cpu is running the kernel code.
    * If `RESCHED_IMMED` flag is specified in flags, machine-dependent code
     should make a context switch happen as soon as possible.  In that case,
     for example, if ci is not the current processor, cpu_need_resched() typi-
     cally issues an inter processor call to the processor to make it notice
     the need of a context switch as soon as possible.
    * Set a per-processor flag which is checked by userret(9) when
               returning to user-mode execution.
    * Post an asynchronous software trap (AST).
    * Send an inter processor interrupt to wake up cpu_idle(9).
    * musimy mieć wyłączone wywłaszczanie
    * jeśli `RESCHED_KPREEMPT` to robimy jeden z punktów wyżej
    * jeśli `RESCHED_REMOTE` no to wysyłamy do wskazanego cpu asynchronicznego software trapa
    * w ostatnim przypadku atomowo ustawiamy bit mówiący, że mamy oczekującego AST na CPU
* [`preempt`](http://bxr.su/NetBSD/sys/kern/kern_synch.c#299)
    * The preempt() function puts the current LWP back on the system run queue
     and performs an involuntary context switch.  The yield() function is
     mostly same as preempt(), except that it performs a voluntary context
     switch.
    * zdejmujemy locki
     > [name=Mikołaj Kowalik] a rozumie ktoś co to są biglocki? - kiedy są używane
     > [name=Mikołaj Kowalik] `biglocki` - pozostałość z czasów gdy jądro było no-preemptive
     > `__predict_false` odpowiednik __builtin_expect(..., 0) w gcc?
    * potem zakładamy locka
     > [name=Mikołaj Kowalik] wrzucam moje komentarze
     > // zakładamy locka na status procesu (ten od schedulera)
	lwp_lock(l);
	// sprawdzamy, że lock na schedulera jest nasz
	// sprawdzamy, że ten lwp jest na procesorze
	// zakładamy locka na schedstate_percpu dla naszego cpu
	spc_lock(l->l_cpu);
    > [name=Jakub Urbańczyk] imo rozpatrywanie tego, w jaki sposób jest zrealizowana synchronizacja kompletnie nie jest istotą tego zadania
    >  [name=Mikołaj Kowalik] ja to wrzuciłem bo dla mnie to było potrzebne żeby zrozumieć co się tu dzieje (co robimy przy wywłaszczaniu) - rezerwujemy statusy schedulera i lwp bo go dalej edytujemy - a myślę że status lwp/schedpercpu jest dosyć kluczowy przy preempt (jego aktualizacja).
    * sprawdzamy czy na 100% są ok locki i czy jesteśmy na CPU
    * kolejny lock
    * potem maigczne sprawdzenie warunków czy mamy mieć boost do priorytetu
    * i mówimy, że czekamy na wywłaszczenie
    * i robimy context switch'a (independent)
    * zakładamy locki które były na początku

## Zadanie 1-4

> Jądro systemu WinNT jest łatwo **przenośne** (ang. _portable_) dzięki implementacji **warstwy abstrakcji sprzętu** (ang. _hardware abstraction layer_). Jakie zadania pełni HAL (§11.3)? Jakie korzyści daje HAL programistom implementującym **sterowniki urządzeń**?
>
> W podręczniku intro(9) opisano podsystemy jądra NetBSD. Wśród wymienionych tam podsystemów zidentyfikuj te, które należą do warstwy abstrakcji sprzętu. Uzasadnij swój wybór przytaczając ustępy danych stron podręcznika. Jakie jest zastosowanie wymienionych podsystemów jądra?

:::warning
Autor: Mateusz Maciejewski
Redaktor: Bartosz Szpila
:::

**Przenośne** = łatwe do przeniesienia na inne platformy, wymagające minimalnej ingerencji w kod.
**Warstwa abstrakcja sprzętu** = zbiór usług, procedur, makr ułatwiających korzystanie ze sprzętu przez programistę, ukrywający nieistotne (dla twórcy sterownika, jądra) różnice.

Funkcje HAL:
* Adresowanie logiczne urządzeń (magistrale PCI, USB) oraz interfejs do rejestrów sprzętowych (porty)
* Nazywanie przerwań oraz przenośne rejestrowanie ISR (procedur obsługi przerwań)
* Zarządzanie poziomami przerwań (priorytety logiczne)
* DMA (adresowanie, ciągłość mapowania)
* Standardowy zegar/timery
* Prymitywy do synchronizacji (np. spinlock)
* Obsługa BIOS (wczytanie konfiguracji sprzętowej)
> * komunikacja między procesorami, memory management

Sterownik urządzenia udostępnia interfejs do danego urządzenia innym częściom systemu operacyjnego oraz programom. Abstrakcja oferowana przez HAL pozwala na łatwe znalezienie oraz obsługę urządzenia, która odporna jest na pozostałą konfigurację sprzętu czy platformę na której uruchomiony jest sterownik. Dzięki temu raz napisany sterownik powinien obsłużyć urządzenie na wielu różnych platformach, bez dodatkowej ingerencji programisty.

Elementy [_intro(9)_](http://mdoc.su/n/9/intro) stanowiące HAL:
> [name=Arek Kozdra] być może warto dodać odnośniki do manualów, jak http://mdoc.su/n/9/intro
> [name=Bartosz Szpila] PROCESS CONTROL nie ma paru machine-dependent fcji?

* MEMORY MANAGEMENT
    * Machine-dependent portion of the virtual memory system. [pmap(9)](http://mdoc.su/n/9/pmap).
    * Virtual memory system external interface [uvm(9)](http://mdoc.su/n/9/uvm)
        * uvm_init() sets up the UVM system at system boot time, after the console has been setup.  It initializes global state, the page, map, kernel virtual memory state, machine-dependent physical map, kernel memory allocator, pager and anonymous memory sub-systems, and then enables paging of kernel objects.
* LOCKING AND INTERRUPT CONTROL [locking(9)](http://mdoc.su/n/9/locking)
    > * (odrzucone) Condition variables [condvar(9)](http://mdoc.su/n/9/condvar)
    > * Kernel lock functions [lock(9)](http://mdoc.su/n/9/lock) ---> spinlocks
    * Machine-independent software interrupt framework [softint(9)](http://mdoc.su/n/9/softint)
    * Functions to modify system interrupt priority level [spl(9)](http://mdoc.su/n/9/spl)
    * Functions to raise the system priority level [splraiseipl(9)](http://mdoc.su/n/9/splraiseipl)
* SYSTEM TIME CONTROL
    * Real-time timer [hardclock(9)](http://mdoc.su/n/9/hardclock)
    * Realtime system clock [microtime(9)](http://mdoc.su/n/9/microtime)
* MACHINE DEPENDENT KERNEL FUNCTIONS
    * Machine-dependent clock setup interface [cpuinitclocks(9)](http://mdoc.su/n/9/cpuinitclocks)
    * Halt or reboot the system [cpu_reboot(9)](http://mdoc.su/n/9/cpu_reboot)
    * Machine-dependent CPU startup [cpu_starup(9)](http://mdoc.su/n/9/cpu_starup)
* DEVICE CONFIGURATION
    * Autoconfiguration frame-work [autoconf(9)](http://mdoc.su/n/9/autoconf)
    * The autoconfiguration framework device definition language. [config(9)](http://mdoc.su/n/9/config)
* MI DEVICE DRIVER API
    * Bus and Machine Independent DMA Mapping Interface [bus_dma(9)](http://mdoc.su/n/9/bus_dma)
    * Bus space manipulation functions [bus_space(9)](http://mdoc.su/n/9/bus_space)
    * Hardware-assisted data mover interface. [dmover(9)](http://mdoc.su/n/9/dmover)
* DEVICE SPECIFIC IMPLEMENTATION
    * Perform PCI bus configuration [pci_configure_bus(9)](http://mdoc.su/n/9/pci_configure_bus)
    * PCI bus interrupt manipulation functions [pci_intr(9)](http://mdoc.su/n/9/pci_intr)
* MISC
    * Power management and inter-driver messaging [pmf(9)](http://mdoc.su/n/9/pmf)

## Zadanie 1-5

> Najważniejszym kryterium w systemach czasu rzeczywistego jest **dotrzymywanie terminów**. Zatem zależy nam na minimalizacji **opóźnienia przetwarzania zdarzeń** (ang. _event latency_). Na podstawie §5.6.1 wymień fazy przetwarzania zdarzenia. Wyjaśnij z czego wynika **opóźnienie przetwarzania przerwania** (ang. _interrupt latency_) i **opóźnienie ekspedycji** (ang. _dispatch latency_).

:::warning
Autor: Michał Błaszczyk
Redaktor: Paweł Jasiak
:::

> [name=Paweł Jasiak] Myślę, że fajnie dodać angielskie nazwy w tym miejscu.

**Dotrzymywanie terminów (meeting deadlines)**
Każde zadanie czasu rzeczywistego posiada **deadline**. Dotrzymanie terminu dla takiego zadania jest równoważnie z obsługą tego zadania przed upłynięciem jego deadline-u.

**Opóźnienie przetwarzania zdarzeń (event latency)** to okres czasu pomiędzy wystąpieniem zdarzenia a odpowiedzią systemu na to zdarzenie.

> [name=Paweł Jasiak]
> ![](https://misc.jasiak.xyz/sju/event-latency.png)

**Opóźnienie przetwarzania przerwania (interrupt latency)** to okres czasu pomiędzy powiadomieniem CPU o wystąpieniu przerwania a rozpoczęciem wykonywania funkcji obsługującej to przerwanie.

> [name=Wiktor Pilarczyk] Dodałbym
> ![](https://i.imgur.com/DEWqPRS.png)

**Opóźnienie ekspedycj (dispatch latency)** to okres czas wymagany przez dyspozytora w celu zatrzymania zadania i rozposzęcia wykonywania kolejnego zadania.

**Fazy przetwarzania zadania (task processing phases)**

![](https://i.imgur.com/BLrKgHy.jpg)

1. **interrupt processing** 
    System operacyjny dokańcza aktualnie wykonywaną instrukcję, ustala typ przerwania, zapisuje kontekst procesora i przechodzi do wykonywania procedury obsługi przerwania.
2. **process made available**
  Procedura obsługi przerwania wybudza wysoko priorytetowy proces mający na celu odpowiedzieć na zdarzenie.
3. **disparch latency**
- **conflicts** 
Faza konfliktów ma dwa komponenty: wywłaszczanie wszelkich procesów działających w jądrze, oraz zwalnianie zasobów przez nisko priorytetowe procesy na rzeczy wysoko priorytetowego procesu.
- **dispatch**
Przekazanie kontorli nad procesorem wysoko priorytetowemu procesowi.
4. **real-time process execution**
Działanie wyszeregowanego procesu, który realizuje odpowiedź na zaistniałe zdarzenie.

> [name=Samuel Li]
> ```mermaid
>gantt
>    title latenacy
>    
>    dateFormat x
>    
>    section Section
>    response interval    : 0, 6 
>    interrupt processing : 0, 2
>    interrupt latency    : 0, 1
>    dispatch latency     : 2, 4
>    process execution    : 4, 6
>```

## Zadanie 1-6

> Opisz zasadę działania algorytmów szeregowania zadań okresowych ze statycznym i dynamicznych przydziałem priorytetów: odpowiednio RMS (ang. Rate Monotonic Scheduling) i EDF (ang. Earliest Deadline First). Podaj kryterium szeregowalności dla powyższych algorytmów.
>
>Dla zadań $T_1, ..., T_3$ kolejno podano (w milisekundach) okres i czas wykonania, tj. odpowiednio ($p_i$, $t_i$):
> * $(100, 20)$, $(150, 50)$, $(250, 100)$
> * $(100, 20)$, $(150, 50)$, $(250, 120)$
>
>Narysuj diagram (patrz obraz 5.22) przydziału czasu procesora dla pierwszych 750ms działania algorytmów.

:::warning
Autor: Wiktor Pilarczyk
Redaktor: Partycja Balik
:::

> [name=Paweł Jasiak] Dodałbym oznaczenia co to $t_i, p_i$ dla przyszłych czytelników.

Oznaczenia:
 - $p_i$ -- okres zadania $i$
 - $t_i$ -- czas potrzebny na wykonanie zadania $i$
 - $n$ -- liczba zadań

Pojęcia:
* RMS (Rate Monotonic Scheduling) -- Priorytety są ustalone na początku. Zadania o większej czestotliwosci (mniejszym okresie) mają większy priorytet. 
  Warunek szeregowalności: $\sum \frac{t_i}{p_i} \le n(2^{\frac{1}{n}} - 1)$.
* EDF (Earliest Deadline First) -- Zadania o najwcześniejszym terminie mają najwyższy priorytet.
  Warunek szeregowalności: $\sum \frac{t_i}{p_i} \le 1$.
* Szeregowalność -- Własność zestawu procesów oraz algorytmu, która mówi, że terminy zostaną dotrzymane.

EDF ma lepszy warunek szeregowalności, ale występuje w nim dużo context switchów, które też zajmują dodatkowo czas. Z tego powodu nie jest to preferowany algorytm.
> [name=Patrycja Balik] Czy szeregowalność raczej nie mówi, czy dla danego algorytmu istnieje taka kolejność?
> [name=Franek Zdobylak] Jak działa to kryterium szeregowalności? W drugim przypadku dla EDF wyszło mi że zachodzi ono.
> [name=Patrycja Balik] Pytanie podczas omówienia: czy warunek szeregowalności to implikacja w jedną stronę, czy równoważność.

> [name=Michał Opanowicz] Myślę że mogę mieć bardziej kolorowy obrazek ![](https://i.imgur.com/EbfshUE.png)
> [name=Patrycja Balik] Wybrałam ten obrazek, jeśli ktoś uważa inny za lepszy, można zamienić.

:::spoiler Alternatywne ilustracje

> [name=Wiktor Pilarczyk]
> 2. Symulacja
>     ![](https://i.imgur.com/WvFBlCx.png)
>     ![](https://i.imgur.com/dmWJaVd.png)
>     Legenda:
>         * P1, P2, P3 nazwy procesów z odpowiednim okresem i czasem wykonywania
>         * Następnie są osie czasu, w których liczba w komórce oznacza czas, w którym się kończy
>         * Nad osią oznaczone jest, który proces zajął okres czasu
>         * Pod spodem jest napisany status czasu potrzebny dla danego procesu np. po 10 ms w komórkach B10 mówi, że proces P1 potrzebuje jeszcze 10 ms
>         * Dla EDF występuje dodatkowy licznik do końca deadlinu

> [name=Patrycja Balik] Ja też mam alternatywne obrazki, moje są zrobione w Inkscapie i są kolorowe.
> Przemianowałam zadania: A - T1, B - T2, C - T3, żeby móc indeksować terminy.
> ![](https://i.imgur.com/36r4POB.png)
> RMS, przykład 1.
> ![](https://i.imgur.com/7bTc1KD.png)
> EDF, przykład 1.
> ![](https://i.imgur.com/K8uGcvj.png)
> EDF, przykład 2.

> [name=Franek Zdobylak] Skoro już się kłócimy kto ma lepsze, to ja mam żmudnie wyrysowane na kartce XD. ![EDF1](https://i.imgur.com/ElE1YD6.jpg)

> [name=Samuel Li] Ja ma tesktowo robione :smile: 
> ```mermaid
>    gantt
>    dateFormat  x
>    title RMS
>
>    section T1 section (0, 100, 200, 300, 400)
>    T1(0)            :done,    0, 20
>    T1(1)            :done,  100, 120
>    T1(2)            :done,  200, 220
>    
>    section T2 section (0, 150, 300, 450)
>    T2(0)            :done,   20, 70
>    T2(1)            :done,  150, 200
>    
>    section T3 section (0, 250)
>    T3(0)            :active, 70, 100
>    T3(0)            :       120, 150
>    T3(0)            :done,  220, 260
> ```

> [name=Zbigniew Drozd i Tomasz Stachowski] https://docs.google.com/spreadsheets/d/1gACyF1yT1k6ubRFNmYuz3xnvK00ocUdJHhYmaaRj5M0/edit?usp=sharing
:::

## Zadanie 1-8

> Peryferia komputera Amiga 500 generują 14 różnych przerwań, które są odwzorowane w 6 przerwań zewnętrznych procesora M68000 o rosnącym priorytecie (Level X Interrupt Autovector, tabela 6-2). Zapoznaj się pobieżnie z dokumentacją opisującą przerwania Interrupts. Nas będzie interesować przerwanie wygaszania pionowego (ang. vertical blank), które zgłaszane jest 50 razy na sekundę.
>
> Zauważ, że istnieją dwa mechanizmy kontroli przerwań zewnętrznych. Pierwszy to poziom priorytetu przerwań (ang. interrupt priority level) (§6.3.2) znajdujący się rejestrze stanu procesora SR. Drugi to rejestry «INTREQ» (ang. interrupt requests) i «INTENA» (ang. interrupt enable) układów specjalizowanych. Pierwszy z rejestrów przechowuje maskę przerwań oczekujących, a drugi maskę przerwań zgłaszanych procesorowi.
>
>Twoim zadaniem jest zreferowanie w jaki sposób w systemie FreeRTOS zachodzi wywłaszczanie zadań. Procedura «SystemClockTickHandler» jest wywoływana w wyniku obsługi przerwania VERTB. Pierwsza instrukcja procedury obsługi przerwania leży pod adresem «AmigaLvl3Handler» (plik intr.S). Należy pokazać jak zostaje osiągnięty kod, który ustawia zmienną «xNeedRescheduleTask». Następnie należy dotrzeć do miejsca, w którym flaga ta jest sprawdzana i wołana jest procedura zmieniająca kontekst «vPortYieldHandler» (plik portasm.S).

:::warning
Autor: Krzysztof Boroński
Redaktor: Franciszek Zdobylak
:::

**priorytety** przerwań -- Przerwanie o wyższym priorytecie jest obsługiwane jako pierwsze (gdy wystąpią dwa je). Przerwanie o wyższym priorytecie może przerwać egzekucję obsługi przerwania o niższym priorytecie.
**VERTB** - (przerwanie wygaszania pionowego)

### Mechanizmy kontroli przerwań

***poziom priorytetu przerwań*** -- (poziom procesora) 7 priorytetów przerwań. Różne urządzenia mogą generować ten sam priorytet. Priorytet o numerze 7 jest specjalny, bo nie można go zamaskować (zawsze będzie obsłużony)
***rejestry układów specjalizowanych*** -- (poziom wyżej, zależne od urządznia na którym działamy) urządzenia peryferyjne mają przypisany numer przerwania który jest zapisywany do rejestru podczas wysyłania przerwania do procesora
* **INTENA** -- Interrupt enable. Przechowuje maskę sygnałów niezablokowanych
* **INTEREQ** -- Interrupt request. Ustawiany gdy chcemy zmusić procesor do obsłużenia przerwania

obsługa przerwań: (`intr.S`) -- wybór odpowiedniego handlera na podstawie rejestru INTEREQ
`vPortYieldHandler`: (`portasm.S`) -- ustawiona też jako handler instrukcji `TRAP 0`

Obsługa `VERTB`:
1. `AmigaLvl3Handler` --> `handle_`
2. `VertBlankChain` --> z tego co rozumiem obsługuje wszystkie handleru przypisane do `VERTB` 
			* do `IntVec` do obsługi `VERTB` jest wpisana funkcja `RunIntChain`
			* w pliku `intsrv.c` funkcja `RunIntChai` nwywołująca po kolei wszystko z `IntChain`
			* w  `main.c` dodajemy do tego łańcucha funkcję `SystemClockHandler`
4. `SystemClockTickHandler()`
5. `xTaskIncrementTask()`
6. wracamy do `handle_`
7. sprawdzamy `xNeedRescheduleTask`
8. wołamy (lub nie) `vPostYieldHandler`

> [name=Krzysztof Boroński] Czy mi się wydaje, czy te wpisy w łańcuchu mają niby jakiś priorytet, ale koniec końców jest on zupełnie ignorowany i przekazywany po prostu do handlera?
> [name=Krzysztof Boroński] Co to jest za dziwna sprawa???
```
/* Reuses following fields of ListItem_t:
 *  - (BaseType_t) pvOwner: data provided for IntSrvFunc_t,
 *  - (TickType_t) xItemValue: priority of interrupt server. */
typedef struct IntServer {
  ListItem_t node;
  IntFunc_t code;
} IntServer_t;
```
> [name=Krzysztof Boroński] Czy to jest jakieś paskudne użycie na nowo nodea listy? To jest taki element, który mnie bardzo skonsternował w kodzie *FreeRTOS*
> [name=Franek Zdobylak] Myślę, że ten obrazek będzie dobrze obrazował to co się dzieje z przerwaniami w Amidze.
![](https://i.imgur.com/0bYCjLJ.png)

## Zadanie 2-1

> Na podstawie strony [Memory Management](https://freertos.org/a00111.html) wyjaśnij ogranicznia poszczególnych manadżerów pamięci sterty systemu **FreeRTOS**. Czemu autorzy dokumentacji odnoszą się z rezerwą do dynamicznego zarządzania pamięci w systemach czasu rzeczywistego? Podaj scenariusze użycia dynamicznego przydziału pamięci uzasadniające wybór dostępny wybór algorytmów zaimplementowanych w plikach `heap_?.c`.

:::warning
Autor: Tomasz Stachowski
Edytor: Zbigniew Drozd
:::

Autorzy preferują statyczne zarządzanie pamięci, ponieważ takie zarządzanie pamięcią działa deterministycznie (dla przykładu często wykonanie się procedur obsługi pamięci zarządzanej dynamicznie wykonuje się w różnych czasach) oraz dodatkowo wiemy, ile będziemy potrzebować pamięci podczas konsolidacji. W przypadku dynamicznego zarządzania pamięcią widzimy, ile dokładnie zużywamy pamięci dopiero podczas działania programu, bo przykładowo nie wiemy, jak będzie wyglądać fragmentacja pamięci.
> [name=Zbigniew Drozd] Prawdopodobnie też plują na tę dynamiczną alokację bo nie powinno się tego robić na małych procesorkach wbudowanych
>> [name=Michał Opanowicz] Ale czemu?
>>> [name=Zbigniew Drozd] Bo systemy wbudowane nie służą do robienia jakichś dziwnych przedstawień tylko do prostych rzeczy. Jak potrzebujesz świrowania to wkładasz SOC z linuxem
* `heap_1` przydziela pamięć i nie możemy nawet jej zwalniać. Przydatne jest to wtedy, kiedy korzystamy z aplikacji, która na początku swojego działania alokuje sobie pamięć i potem już generalnie operuje tylko na niej. Jest deterministyczna, działa w czasie stałym i ma prostą implementację.
* `heap_2` (przestarzały, chcemy używać heap_4) implementuje best fit, ale nie złącza bloków. Przydaje się wtedy kiedy mamy aplikację, która zwalnia i alokuje bloki podobnych rozmiarów. W przeciwnym przypadku mielibyśmy dużą fragmentację.
> [name=Arek Kozdra] kiedy to się psuje, a kiedy warto?
>> [name=Tomasz Stachowski] warto wtedy kiedy program posługuje się tylko blokami z konkretnej puli rozmiarów, a nie alokuje bloków randomowego rozmiaru.
>>> [name=Arek Kozdra] a to oke
* `heap_3` Wrapper na malloc() i free() taki, że powoduje, że jest thread safe. Jest niedeterministyczny, wymaga zaopatrzenia się przez kompilator w bibliotekę z malloc() i free() oraz zwiększa w dużym stopniu rozmiar kodu jądra systemu. Przydaje się przykładowo gdy nasz program ma wiele wątpków.
* `heap_4` uzywa first fit i złącza bloki. Nie jest deterministyczny, ale jest szybki i nie powoduje fragmentacji tak jak to było w przypadku algorytmów bez złączania bloków. Przydaje się, gdy nasz program alokuje bloki zmiennych rozmiarów i je zwalnia wielokrotnie.
* `heap_5` Działa jak heap_4, ale pozwala, żeby sterta była w kilku rozłącznych kawałkach RAMU. Przydaje się, gdy nasz RAM jest fizycznie podzielony na kawałki np. gdy mamy kilka kości RAMu.

## Zadanie 2-2

> Dlaczego należy unikać funkcji rekurencyjnych programując z użyciem systemu **FreeRTOS**? Na podstawie strony [How big should the stack be?](https://freertos.org/FAQMem.html#StackSize) wyjaśnij jak wyznaczyć górne ograniczenie na rozmiar stosu. Czemu należy brać pod uwagę procedury obsługi przerwań? Na podstawie [Stack Usage and Stack Overflow Checking](https://freertos.org/Stacks-and-stack-overflow-checking.html) wyjaśnij strategie wykrywania przepełnienia stosów systemach bez ochrony pamięci.

:::warning
Autor: Zbigniew Drozd
Redaktor: Michał Opanowicz
:::

#### Wyjaśnienie pogrubionych rzeczy
###### * ISR: Procedura (bądź procedury) która wykonywana jest gdy zajdzie jakieś zdarzenie zewnętrzne.
> [name=Jakub Urbańczyk] Może warto rozpisywać nowe skróty?
>> [name=Zbigniew Drozd] Interrupt Service Routine = prodecuda obsługi przerwania
###### * Przepełnienie stosu: gdy wątki są ustawione jeden za drugim w pamięci fizycznej to zagraża nam to że stos jednego zacznie nadpisywać stos jakiegoś innego 

### Dlaczego należy unikać funkcji rekurencyjnych?
Przykładowa ATMEGA8A-AU (procesor z arduino nano używany np. na zajęciach systemy wbudowane) ma 1kB pamięci SRAM. To jest bardzo niewiele. Nawet jeśli pojedyncze wywołanie funkcji zajmuje 8B stosu (czyli bardzo oszczędnie) to będziemy mogli wykonać tę funkcję tylko 256 razy (zakładając że zupełnie nic innego w tej pamięci nie chcemy trzymać, co jest już bardzo dużym uproszczeniem). Więc bardzo szybko możemy nadziać się na problemy z brakiem stosu.

### Jak wyznaczyć górne ograniczenie na rozmiar stosu?
Na rozmiar stosu wpływ będą miały (https://freertos.org/FAQMem.html#StackSize)
* The function call nesting depth
* The number and size of function scope variable declarations
* The number of function parameters
* The processor architecture
* The compiler
* The compiler optimisation level
* The stack requirements of interrupt service routines – which for many RTOS ports is zero as the RTOS will switch to use a dedicated interrupt stack on entry to an interrupt service routine.  

Można zgadywać (bądź szacować rozmiar stosu), ale FreeRTOS daje jakąś metodę na spojrzenie ile stosu faktycznie zużyliśmy, żeby potem go zmniejszyć

### Czemu należy brać pod uwagę procedury obsługi przerwań?
Jeśli przerwania nie są robione na osobnym stosie, to trzeba brać pod uwagę że w dowolnym momencie (czyli potencjalnie gdy stos zapełniony jest "pod kurek") będziemy potrzebowali odłożyć kontekst (I wykonać ISR)

### Strategie wykrywania przepełnienia stosu na sprzęcie bez ochrony pamięci  

* Pierwsza strategia wymaga założenia, że najgłębiej w stosie jesteśmy gdy odkładamy na bok jakieś zadanie https://freertos.org/Stacks-and-stack-overflow-checking.html (It is likely that the stack will reach its greatest (deepest) value after the RTOS kernel has swapped the task out of the Running state because this is when the stack will contain the task context) Możemy wtedy sprawdzić czy stack pointer mieści się w zdefiniowanym wcześniej obszarze.
* Druga strategia zakłada że dół stosu znamy w miarę od początku i nie będzie się on zmieniał podczas działania programu. Więc pierwsze 16B będziemy traktować jako kanarka i od czasu do czasu będziemy sprawdzać czy nikt tego kanarka nie ruszył.
