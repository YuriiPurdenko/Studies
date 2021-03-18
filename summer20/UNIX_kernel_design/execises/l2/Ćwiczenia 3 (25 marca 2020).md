# Ćwiczenia 3 (25 marca 2020)

## Rozwiązania (15 minut)

### Zadanie 2-3

:::warning
Autor: Michał Opanowicz
:::
Struktura przechowująca informacje o wątkach na danym procesorze
http://bxr.su/FreeBSD/sys/sys/pcpu.h#177
Wątek z FreeBSD:
http://bxr.su/FreeBSD/sys/sys/proc.h#228

Zmienne globalne z zadania w FreeRTOS:
https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#334

* `pxCurrentTCB`- current task pointer; odpowiednik to pc_curthread 
* `pxReadyTasksLists` - tablica list taskow w stanie ready lub running, i-ta lista to lista procesow o priorytecie i-tym, uzywa jej scheduler; odpowiednik to `runq` (run queue) w pliku http://bxr.su/FreeBSD/sys/sys/runq.h
Scheduler wybiera z run queue wątek, który powinien zostać umieszczony na procesorze. 
Trzy typy kolejek runq (realtime, timeshare, idle) są zgrupowane w strukturze `tdq`
http://bxr.su/FreeBSD/sys/kern/sched_ule.c#251, przechowującej ponadto różne statystyki o wątkach w tych kolejkach.
(uwaga: to chyba zależy od wybranej strategii schedulera i niekoniecznie jest na temat)

* `xTickCount` - zmienna trzymajaca licznik przerwań zegarowych, może się przekrecać; odpowiednik to któreś z pól w `td_sched`, prawdopodobnie `ts_ticks`
http://bxr.su/FreeBSD/sys/kern/sched_ule.c#92
(uwaga: to znowu wydaje się zależeć od strategii schedulera i niekoniecznie odpowiadać na pytanie, być może lepszą odpowiedzią byłyby pola `pc_switchtime` i `pc_switchticks` w `pcpu`)

Taski kończące się z pomocą `vTaskDelete` nie mogą się same usunąć ponieważ potrzebują stosu, aby wywołać `prvDeleteTask`,który nie może usunąć stosu "spod siebie". Są usuwane z kolejki przez `prvCheckTasksWaitingTermination`, który zdejmuje je z kolejki i woła `prvDeleteTask`.

### Zadanie 2-4

:::warning
Autor: Bartosz Szpila
:::

[`lwp`](http://bxr.su/NetBSD/sys/sys/lwp.h#84) - struktura jądra NetBSD przechowująca informacje potrzebne do zarządzania wątkiem (*lwp* - lightweight process)

[`tskTCB`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#252) - struktura jądra FreeRTOS zdefiniowana w **task.c**, również przechowująca informacje potrzebne do zarządzania wątkiem

###### Pola struktury `tskTCB`:
*note: we FreeRTOS, wątek ---> zadanie*
* `pxTopOfStack` - adres ostatniej rzeczy odłożonej na stack wątku opisywanego przez strukturę. Używany do sprawdzenia, czy wystąpił stack overflow oraz przy przywracaniu kontekstu.
* `uxPriority` -  priorytet wątku
* `uxBasePriority` - priorytet wątku używany przez mechanizm dziedziczenia priorytetów (efektywny priorytet)
* `ulRunTimeCounter` - czas, który wątek spędził w stanie *Running*


###### Listy wątków:
* `xStateListItem` - węzeł zawsze znajduje się na jednej z poniższych list, zależnie od stanu wątku
	* `pxReadyTasksLists[priority]`
	* `xDeleyedTaskList1/2`
	* `xPendingReadyList`
	* `xSuspendedTaskList`
* `xEventListItem` - węzeł używany do referencji wątku w *event list* - używane są np. przez procedury ISR podczas obsługi przerwań

API do zarządzania listami eventów:
* [`xTaskRemoveFromEventList( const List_t * const pxEventList )`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#3094)
* [`vTaskRemoveFromUnorderedEventList`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#3162)
* [`uxTaskResetEventItemValue`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#4533)
* [`vTaskPlaceOnEventList`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#3020)
* [`vTaskPlaceOnUnorderedEventList`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#3037)


>[name=Mateusz Maciejewski] (start)

#### `xStateListItem`:
* [`pxReadyTasksLists[priority]`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#221)
* [`xSuspendedTaskList`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1730)
* [`pxDelayedTaskList`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#5137)
* [`pxOverflowDelayedTaskList`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#5131)
* [`xTasksWaitingTermination`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1201)

#### `xEventListItem`:
* [`xPendingReadyList`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1952)
* [`pxEventList`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#3031)

>[name=Mateusz Maciejewski] (stop)


### Zadanie 2-5

:::warning
Autor: Paweł Jasiak
:::

Wyłączanie wywyłaszczenia może nam służyć jako środek synchronizacji (TASK -- TASK), np. przy wołaniu [malloca](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/heap.c#43). Przerwania blokujemy, gdy manipulujemy coś zadaniami (TASK -- ISR), np. [przy usuwaniu zadania](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1158). Jeśli ktoś zawoła [vTaskSuspend](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1697) będąc w sekcji krytycznej to zostanie zmieniony kontekst.

Możemy robić context switch będąc w sekcji krytycznej -- `vTaskSuspend`, jak wrócimy do taska, to musimy pamiętać, że on był w krytycznej sekcji.

Implementacja we FreeRTOS
```C=
#define portENABLE_INTERRUPTS() { asm volatile("\tand.w\t#0xf8ff,%sr\n"); }
#define portDISABLE_INTERRUPTS() { asm volatile("\tor.w\t#0x0700,%sr\n"); }

void vTaskEnterCritical( void )
{
	portDISABLE_INTERRUPTS();

	if( xSchedulerRunning != pdFALSE )
	{
		( pxCurrentTCB->uxCriticalNesting )++;
		if( pxCurrentTCB->uxCriticalNesting == 1 )
		{
			portASSERT_IF_IN_ISR();
		}
	}
}

void vTaskExitCritical( void )
{
	if( xSchedulerRunning != pdFALSE )
	{
		if( pxCurrentTCB->uxCriticalNesting > 0U )
		{
			( pxCurrentTCB->uxCriticalNesting )--;

			if( pxCurrentTCB->uxCriticalNesting == 0U )
			{
				portENABLE_INTERRUPTS();
			}
		}
	}
}
```

W mimikerze jest to zdefiniowane w [interrupt.c].

```C=18
void intr_disable(void) {
  cpu_intr_disable();
  thread_self()->td_idnest++;
}

void intr_enable(void) {
  assert(intr_disabled());
  thread_t *td = thread_self();
  td->td_idnest--;
  if (td->td_idnest == 0)
    cpu_intr_enable();
}
```
[interrupt.c]: https://mimiker.ii.uni.wroc.pl/source/xref/mimiker/sys/kern/interrupt.c?r=1bdd3de0#18


- blokowanie wywłaszczeń - zabraniamy wywłaszczania w określonym czasie - gdy są warunki, że powinniśmy się wstrzymać z wywłaszczeniem piszemy do zmiennej `xYieldPending = pdTRUE;`

- ```c=2101
   void vTaskSuspendAll( void )
    {
    	/* A critical section is not required as the variable is of type
    	BaseType_t.  Please read Richard Barry's reply in the following link to a
    	post in the FreeRTOS support forum before reporting this as a bug! -
    	http://goo.gl/wu4acr */
    	++uxSchedulerSuspended;
    }
  ```

  ```c=2951
    		/* The scheduler is currently suspended - do not allow a context
    		switch. */
    		xYieldPending = pdTRUE;
  ```

- blokowania wywłaszczeń używamy gdy:

  - np. `vTaskDelayUntil`

    Ogólnie w wielu miejscach gdy robimy coś z TCB (wystarczy sprawdzać po vTaskSuspendAll)

- Czemu  liczba  wywołań  «taskENTER_CRITICAL»  jest  utrzymywana  dla  każdego  wątku  osobno  (w  polu«uxCriticalNesting» bloku kontrolnym wątku) zamiast globalnie?

  [Całkiem dobre wytłumaczenie](https://freertos.org/FreeRTOS_Support_Forum_Archive/November_2005/freertos_uxCriticalNesting_1386799.html). 

  - A task can, if it wishes (and care is taken) yield while it is within a  critical section. Therefore each task needs to maintain its own  critical nesting state. If a task yields from within a critical section interrupts must again be disabled when the task starts to run again
### Zadanie 2-6

:::warning
Autor: Krzysztof Boroński
:::

`ulPortSetIPL` ustawia maskę przerwań w **SR** (*Status Register*) na odpowiednią wartość, blokując tymczasowo wszelkie przerwania nawet te o wyższym priorytecie. Zwraca poprzedni stan rejestru **SR**.

`portSET_INTERRUPT_MASK_FROM_ISR` wywołuje `ulPortSetIPL` z argumentem **0x700** (same jedynki)

`portCLEAR_INTERRUPT_MASK_FROM_ISR` wywołuje `ulPortSetIPL` z podanym argumentem (zapisany **SR** - zwrócony przez `portSET_INTERRUPT_MASK_FROM_ISR`)

Konieczne jest to gdy chcemy zmienić jakiś stan współdzielony z **ISR** o wyższym priorytecie.

Przykład: zmienna `xNeedRescheduleTask`, globalne listy i kolejki, TCB.

Być może nie ma sensu zabezpieczać tak każdy typ na wszystkich architekturach, ponieważ zmienną `xNeedRescheduleTask` możnabyłoby ustalać pojedynczymi instrukcjami. Niezbędnym będzie jednak zabezpieczanie bardziej złożonych struktur.

Działa podobnie do `taskENTER_CRITICAL`, ale pozwala przywrócić poprzednią maskę przerwań.

We **_FreeRTOS_** przerwania do priorytetu wskazanego przez `configMAX_SYSCALL_INTERRUPT_PRIORITY` mogą wywoływać funkcje API, w związku z tym właśnie tam będzie wykorzystywane takie maskowanie przerwań.

![](https://i.imgur.com/aiN42ob.png)


We **_NetBSD_** podobną rolę pełni `spl(9)`. cytując manual:
```
These functions raise and lower the interrupt priority level. They are
used by kernel code to block interrupts in critical sections, in order to
protect data structures
```

>[name=Patrycja Balik] Czy nie jest tak, że `ulPortSetIPL` ustawia te bity na to, co było w argumencie, a nie koniecznie same jedynki?
>>[name=Krzysztof Boroński] Faktycznie, jest skrót myślowy, jaki poczyniłem ze względu na to, że `portSET_INTERRUPT_MASK_FROM_ISR` wywołuje `ulPortSetIPL` z argumentem **0x700**.
>>>[name=Patrycja Balik] Ok, tylko zaznaczam, bo `portCLEAR_INTERRUPT_MASK_FROM_ISR` też korzysta z `ulPortSetIPL`.
>>>>[name=Krzysztof Boroński] Fakt, powiniem zmienić nieco opis.
>[name=Krzysztof Boroński] Poprawione. 

### Zadanie 2-7

:::warning
Autor: Michał Błaszczyk
:::

**Dlaczego usypiać zadania?**

Aplikacje w systemach czasu rzeczywistego są najczęściej **sterowane zdarzeniami (event driven)**. Wątek odpowiadający obsłudze danego zadarzenia, czaka na odpowiednie okoliczność (wybudzenie przez **ISR**), aby wykonać właściwe sobie przetwarzanie i znowu rozpocząć oczekiwanie. Jeśli nie chcemy takiego wątku usypiać, to alternatywnym rozwiązaniem jest **aktywne oczekiwanie (busy waiting)** w **pustej pętli (null loop)**. Jest to rozwiązanie wyjątkowo niewydajne i potencjalnie niebezpieczne, jeśli bowiem jeden wątek ma priorytet wyższy od pozostalych, to żaden z tych innych wątków nigdy nie otrzyma procesora (bo każdy wątek - w szczególności ten o najwyższym priorytecie - ma zawsze jakieś przetwarzanie (processing) do wykonania). Usypianie wątków gdy nie mają nic do roboty, rozwiązuje wspomiane problemy.

**Zegar systemowy**

W systemie FreeRTOS rolę zegara systemowego pełni zmienna **xTickCount**. Wartość zegara określa ile **przerwań zegarowych (tick interrupt)** upłyneło od wystartowania scheduler-a (zakładając że nie wystąpiło **przepełnienie (overflow)**).

**Usypianie**

Usypianie zadania to umieszczenie go w stanie **Blocked** na zadaną liczbę **taktów zegara**.

**[vTaskDelay()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1334)**

1. Procedura sprawdza czy wywołano ją z argumentem zerowym. Takie wywołanie jest równoważne oddaniu procesora (bez umieszczania zadania w stanie **Blocked**).
2. Wyłączamy scheduler-a (czyli blokujemy wywłaszczanie) w celu realizacji **sekcji krytycznej** chroniącej dostęp do listy zadań w stanie **Blocked**.
3. Zmieniamy stan zadania na **Blocked**. Jest to implementowane poprzez wpisanie zadania na **listę zadań opóźnionych (pxDelayedTaskList** lub **pxOverflowDelayedTaskList)**.
4. Funkcja [xTaskResumeAll()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#2174) zwraca prawdę, jeśli przed powrotem nastąpiło wywłaszczenie, w przeciwnym przypadku zwraca fałsz. Jeśli do wywłaszczenia przed powrotem nie doszło, to teraz wykonujemy wywłaszczenie (oddajemy procesor, przecież nie jesteśmy już w stanie **Ready**).

**Wybudzanie**

Wybudzanie zadanie polega na potencjalnym usunięciu go z **listy zdarzenia (event list)** oraz zmianie jego stanu na **Ready**.

**[xTaskIncrementTick()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#2665)**

Procedura xTaskIncrementTick() wywoływana jest zawsze (przez warstwę portu, to ona implementuje ISR dla **przerwania zegarowego (tick interrupt)**) po wystąpieniu przerwania zegarowego.

1. Jeżeli scheduler jest wyłączony, to odraczamy to wywołanie do momentu wznowienia działania scheduler-a (tj. do momentu wywołania xTaskResumeAll() zerującego wartość zmiennej **uxSchedulerSuspended**) inkrementując zmienną **uxPendedTicks**.
2. Inkrementujemy zegar systemowy (zmienna **xTickCount**) i w przypadku **przepełnienia (overflow)** przełączamy listę zadań zblokowanych.
3. Jeżeli należy wybudzić jakieś zadania to tak robimy (robimy to w pętli, tak długo jak lista zadań w stanie blocked jest nie pusta lub czas wybudzenia kolejnego zadania jest nie większy od nowej wartości zegara systemowego). Jeśli wybudzimy zadanie o prorytecie nie mniejszym od priorytetu zadania bieżącego, to ustawiamy wartość zwracaną funkcji na true.
4. W celu zapewnienia sprawiedliwości scheduler-a w kontekście zadań o równym priorytecie, jeśli istnieje zadanie o priorytecie równym priorytetowi bieżącego zadania, to zwrócimy informacje o potrzebie przełączenia kontekstu.
5. Jeżeli oczekuje jakieś wywłaszczenie to również zwrócimy wartość true. Dlaczego? Może się zdarzyć, że podczas zawieszenia scheduler-a przyjdzie przerwanie, które wybudzi zadanie ([xTaskRemoveFromEventList()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#3094)), wówczas ISR może ustawić flagę **xYieldPending** na true. Gdy teraz przyjdzie przerwania zegarowe (które zostanie obsłużone po obsłużeniu wcześniejszego przerwania), wywołana zostanie procedura xTaskIncrementTick(). Chcemy by niezależnie od tego czy scheduler już działa czy jeszcze nie, procedura ta zwróciła true (bo przecież wymagane jest przełączenie kontekstu).

### Zadanie 2-8

:::warning
Autor: Mateusz Maciejewski
:::

[`xTaskResumeAll`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#2174) nie może być tak trywialna, ponieważ podczas pobytu w sekcji krytycznej mógł upłynąć termin wybudzenia innych wątków. Jest to tym ważniejsze przy wyłączonym wywłaszczaniu. Prócz włączenia planisty funkcja ta musi więc:
> [name= Wiktor Pilarczyk] Co się może stać z zadaniami, które oczekiwały na pewne zdarzenie pomiędzy vTaskSuspsendAll, a xTaskResumeAll?
> [name= Michał Błaszczyk] Wyłączenie scheduler-a nie wyłącza przerwań. Jeśli sekcja krytyczna zrealizowana poprzez blokowanie wywłaszczania jest długa, to prawdopodobnie, w międzyczasie zaszło jakieś przerwanie. Zachodzą teraz dwa przypadki: Jeśli było to przerwanie zegarowe, wówczas zwiększana jest stała uxPendedTicks. Jest to mechanizm odraczania obsługi tego przerwania co musi się kiedyś odbyć (trzeba uaktualnić zmienną xTickCount i ewentualnie wybudzić zblokowane zadania, czyli wywołać xTaskIncrementTick()). Jeśli było to inne przerwanie to prawdopodobnie wybudziło jakiś wątek (xTaskRemoveFromEventList()). Taki wątek jest dodawany do listy xPendingReadyList, z której musi zostać przeniesiony do stanu Ready (i usunięty z listy zdarzenia (event list)) po wznowieniu działania scheduler-a.

* Przerzucić wszystko z kolejki Pending do kolejki Ready
* Zresetować aktualny termin odblokowania wątku (`prvResetNextTaskUnblockTime()`)
* Nadrobić zatrzymane takty zegara (`xTaskIncrementTick` odpowiednią liczbę razy, tzn `uxPendedTicks`), potencjalnie odblokowując zadania
* Oddać procesor innemu wątkowi, jeżeli to konieczne

## Dyskusja (75 minut)

> [name=Samuel Li] Wrzucam swoje notatki: https://hackmd.io/fWX6h5uKQkGuAsXytl9UwQ?view
