# 2020/03/25 SJU -- notatki

## Zadanie 3
**(TCB) Task Control Block**- to odpowiednik *PCB* zawierający ważne i mniej istotne informacje o zadaniu. Każde istniejące zadanie ma swój *TCB*
* *pxCurrentTCB* -- wskazuje na obecnie wykonywane zadanie lub, jeśli scheduler jeszcze nie jest włączony, na zadanie, które jako pierwsze będzie uruchomione.
* *pxReadyTasksLists* -- dla każdego piorytetu trzymamy listę zadań, które są gotowe do działania
* *xTickCount* -- odpowiada za liczenie czasu. Jest inkrementowany co pewne przerwanie systemowe. Służy do odmierzania czasu działania dla zadań.

Odpowiedniki w 
* *pxCurrentTCB* -> [struct pcb  *pc_curpcb;](http://bxr.su/FreeBSD/sys/sys/pcpu.h#182)
* *pxReadyTasksLists* -> nie znalazłem odpowiednika w tej strukturze
* *xTickCount* -> [uint64_t    pc_switchtime;](http://bxr.su/FreeBSD/sys/sys/pcpu.h#184)

Dlaczego niektóre wątki są odkładane na listę? Z komentarza w kodzie:
> A task is deleting itself.  This cannot complete within the task itself, as a context switch to another task is required. Place the task in the termination list. The idle task will check the termination list and free up any memory allocated by the scheduler for the TCB and stack of the deleted task. 

Nie widzę od razu dlaczego nie można zrobić context switcha. Domyślam się, że nie ma jakiegoś poważnego problemu, ale po prostu tak jest wygodniej.

Usuwane jest przez wątek _idle_.

## Zadanie 4
* _pxTopOfStack_ -- ostatnio używany adres na stosie danego wątku
* _uxPriority_ -- piorytet zadania na podstawie którego jest robiony scheduling. Może być zmieniony w trakcie runtime poprzez dziedziczenie priorytetu
* _uxBasePriority_ -- piorytet, który został przez nas nadany
* _ulRunTimeCounter_ -- mówi ile czasu pewne zadania chodziło. Może być liczone w tickach pewnego zegara.

Dla każdego ze stanu w jakim może być wątek jest pewna lista na której może się znajdować. Skoro może być w jednym stanie maksymalnie to jest tylko jeden wskaźnik na listę i jest to właśnie _xStateListItem_. [Tutaj](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#340) są te listy. _xEventListItem_ służy do synchronizacji.

## Zadanie 5
Wyłączanie wywyłaszczenia może nam służyć jako środek synchronizacji, np. przy wołaniu [malloca](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/heap.c#43). Przerwania blokujemy, gdy manipulujemy coś zadaniami, np. [przy usuwaniu zadania](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1158).
Jeśli ktoś zawoła [vTaskSuspend](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1697) będąc w sekcji krytycznej to zostanie zmieniony kontekst.

Możemy robić context switch będąc w sekcji krytycznej -- `vTaskSuspend`, jak wrócimy do taska, to musimy pamiętać, że on był w krytycznej sekcji.

[post na forum](https://freertos.org/FreeRTOS_Support_Forum_Archive/November_2005/freertos_uxCriticalNesting_1386799.html)

## Zadanie 6
Motywacja: czasami chcielibyśmy mieć sekcje krytyczne w ISR, bo one też mogą być przerywane przez przerwania mające wyższy piorytet. Przykładem może być odwołanie do współdzielonej pamięci. Nie ma tego w treści zadania, ale było w artykule, więc też o tym chcę wspomnieć. Są dwa różne poziomy przerwań we FreeRTOS. Te niższe mogą wołać specjalne funkcje API, ale za to będą czasami maskowane, więc czas za nim zostaną obsłużone nie jest stały, lecz jest ograniczony. Te wyższe nie mogą wołać żadnych funkcji, ale za to od razu dostaną kontrolę. Wszystkie mogą wywłaszczać przerwania o niższym piorytecie.

Funkcje odpowiednio blokują wszystkie przerwania (ale zapisuje stary stan) a druga odzyskuje stan.

```asm
15  ENTRY(ulPortSetIPL)
16          move.l  4(sp),d0                zapisujemy maskę, którą dostaliśmy jako argument
17          move.w  sr,-(sp)                robimy backup status registera
18          move.w  sr,d1                   kopiujemy obecną maskę do d1
19          and.w   #0xf8ff,d1              /* z obecnego SR kasujemy bity maski interruptow */
20          and.w   #0x0700,d0              /* z argumentu zostawiamy tylko bity maski */
21          or.w    d0,d1                   laczymy maske SR z bitami maski
22          move.w  d1,sr                   /* wpisujemy nowe SR */
23          move.w  (sp)+,d0                sciagamy stary SR
24          and.w   #0x0700,d0              /* zostawiamy ze starego SR tylko bity maski */
25          rts                             i je zwracamy
26  END(ulPortSetIPL)
```

## Zadanie 7

:::info
Przeprowadź uczestników zajęć przez proces usypiania zadania na pewną liczbę taktów zegara systemowego. W trakcie prezentacji skup się na omówieniu usypiania zadania procedurą «vTaskDelay» i wybudzania po upłynięciu terminu w procedurze «xTaskIncrementTick».
:::

* **takty zegara systemowego** (system ticks) -- Jednostka czasu używana do schedulowania zadań.
* **usypianie** -- usunięcie zadania z kolejki zadań gotowych do wykonywania na pewien czas
* **wybudzanie** -- ustawienia zadania jako ready (wrzucenie go spowrotem na kolejkę)

```C
void vTaskDelay( const TickType_t xTicksToDelay )
{
BaseType_t xAlreadyYielded = pdFALSE;

	/* A delay time of zero just forces a reschedule. */
	if( xTicksToDelay > ( TickType_t ) 0U )
	{
		configASSERT( uxSchedulerSuspended == 0 );
		vTaskSuspendAll();
		{
			traceTASK_DELAY();

			/* A task that is removed from the event list while the
			scheduler is suspended will not get placed in the ready
			list or removed from the blocked list until the scheduler
			is resumed.

			This task cannot be in an event list as it is the currently
			executing task. */
			prvAddCurrentTaskToDelayedList( xTicksToDelay, pdFALSE );
		}
		xAlreadyYielded = xTaskResumeAll();
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}

	/* Force a reschedule if xTaskResumeAll has not already done so, we may
	have put ourselves to sleep. */
	if( xAlreadyYielded == pdFALSE )
	{
		portYIELD_WITHIN_API();
	}
	else
	{
		mtCOVERAGE_TEST_MARKER();
	}
}
```

* interesujący jest jedynie ciało _if_-a
* wymagamy, aby Scheduler był uśpiony
* następnie usypiamy wszystkie taski
* `traceTASK_DELAY` -- niestety w kodzie występuje jedynie _define_ który wygląda dość bez sensu
* [dodajemy](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#prvAddCurrentTaskToDelayedList) aktualnego taska do listy _delayed_ (z wartością o ile tików mamy zostać opóźnieni)
* no i na koniec aktualizujemy wartość `xAlreadyYielded` o wynik wywołania [`xTaskResumeAll`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#2174)
* jeśli nie zrobiliśmy _yield_ to robimy go na siłę (trap... trap... trap... :smile:)


[`xTaskIncrementTick`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#2665) -- niestety to arcydzieło jest zbyt długie aby wkleić do notatki.

```C
/* Called by the portable layer each time a tick interrupt occurs.
Increments the tick then checks to see if the new tick value will cause any
tasks to be unblocked. */
traceTASK_INCREMENT_TICK( xTickCount );
```

* robimy tick `traceTASK_INCREMENT_TICK( xTickCount );` i sprawdzamy, czy spowodowało to jakąkolwiek zmianę
* większamy ticka o 1 `xTickCount = xConstTickCount;` (ktoś wie dlaczego to wsadzenie _const_ cokolwiek zrobi?)
* jeśli dokonał się overflow to robimy swapa `pxDelayedTaskList` z `pxOverflowDelayedTaskList`, zwiększamy liczbę przekręceń o 1 i resetujemy czas odblokowania następnego taska [`prvResetNextTaskUnblockTime`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#prvResetNextTaskUnblockTime)
* teraz patrzymy czy mamy timeout
> Tasks are stored in the queue in the order of theri wake time
* jeśli tak
    * jeśli żaden task nie czeka w kolejce to ustawiamy `xNextTaskUnblockTime = portMAX_DELAY;`
    * jeśli jakiś task czekał
        * wyciągamy pierwszy element (czyli z najwcześniejszym timeoutem)
        * jeśli nasz tick jest wciąż mniejszy niż czas w którym mamy timeout dla procesu to ustawiamy nowy czas następnego odblokowania `xNextTaskUnblockTime` i przerywamy pętlę
        * nadszedł czas usunięcia taska z kolejki (robimy to)
        * wrzucamy tego taska na kolejkę _ready_
        * jeśli mamy wywłaszczenia to jest to dobry moment aby powiedzieć, że jeśli mamy wyższy priorytet, to oczekiwalibyśmy takowego
    * jeśli mamy wywłaszczanie i _TIME_SLICING_ to jeśli istnieje jakiś proces o takim samym priorytecie jak my, to chcemy zapewne oddać mu czas, więc prosimy o wywłaszczenie
* inkrementujemy `uxPendedTicks`
* jeśli mamy wywłaszczanie i _yield_ nie jest pending, to mówimy, że będziemy chcieli zrobić _switch_

## Zadanie 8

:::info
Kod procedury «vTaskSuspendAll» wyłączającej wywłaszczanie jest trywialny. Wydawałoby się, że jedyne co powinna zrobić procedura «xTaskResumeAll» to zmniejszyć wartość zmiennej «uxSchedulerSuspended» o 1, ale tak nie jest. Wyjaśnij zatem co robi ta procedura i dlaczego?
:::

`vTaskSuspendAll` -- trywialny:

```C
void vTaskSuspendAll( void )
{
	/* A critical section is not required as the variable is of type
	BaseType_t.  Please read Richard Barry's reply in the following link to a
	post in the FreeRTOS support forum before reporting this as a bug! -
	http://goo.gl/wu4acr */
	++uxSchedulerSuspended;
}
```

`uxSchedulerSuspended` jest po prostu atomową zmienną typu `UBaseType_t` czyli `unsigned long`, zmieniamy jej wartość w `vTaskSuspendAll` oraz w [`xTaskResumeAll`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#2174).

### Co się dzieje w tej magicznej procedurze?

`TCB_t` -- [`tskTaskControlBlock`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#252)

> Task control block.  A task control block (TCB) is allocated for each task, and stores task state information, including a pointer to the task's context (the task's run time environment, including register values)
  
* sprawdzamy, czy robimy resume w sytuacji, gdy wcześniej był wywołany suspend (assert)
* [`taskENTER_CRITICAL();`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#4220) -- tutaj po prostu wyłączamy przerwania + dodatkowe sprawdzenia
* dotykamy naszej atomowej zmiennej `uxSchedulerSuspended`, jeśli po zmianie jej wartości przeszliśmy do stanu `false` to wykonujemy następujące akcje
* liczba tasków musi być większa od 0 (sprawdzenie)
* na liście _pending_ tasków może się coś znajdować zgodnie z:
> It is possible that an ISR caused a task to be removed from an event list while the scheduler was suspended.  If this was the case then the removed task will have been added to the xPendingReadyList.  Once the scheduler has been resumed it is safe to move all the pending ready tasks from this list into their appropriate ready list.
* dopóki na liście _pending_ tasków znajduje się cokolwiek
    * sciągamy taska i wrzucamy na listę tasków _ready_
    * jeśli task ma większy priorytet niż my to będziemy chcieli oddać mu sterowanie `xYieldPending = pdTRUE`
* jeśli ściągneliśmy żadnego zadania, to wykonujemy procedurę [`prvResetNextTaskUnblockTime`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#prvResetNextTaskUnblockTime) która ustawia `xNextTaskUnblockTime` na podstawie pierwszego elementu w `pxDelayedTaskList`
* sprawdzamy ile mamy `uxPendedTicks` i tyle razy wykonujemy _tick_ mówiący czy należy dokonać _switch_, jeśli tak to ustawiamy `xYieldPending`. 
* jeśli mamy obsługę wywłaszczania to go dokonujemy (o ile oczekujemy na _yield_)