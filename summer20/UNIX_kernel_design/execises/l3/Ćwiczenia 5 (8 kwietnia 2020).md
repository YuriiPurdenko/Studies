# Ćwiczenia 5 (8 kwietnia 2020)

## Zadanie 3-7

:::warning
Autor: Arkadiusz Kozdra
:::


[`xSemaphoreCreateBinary`] używa funkcji [`xQueueCreateGeneric`].
[`xSemaphoreCreateMutex`] używa funkcji [`xQueueCreateMutex`], która wykonuje [`xQueueCreateGeneric`] i [`prvInitialiseMutex`] (dodaje możliwość dziedziczenia priorytetów).

```c=471
#if( configUSE_MUTEXES == 1 )

	static void prvInitialiseMutex( Queue_t *pxNewQueue )
	{
		if( pxNewQueue != NULL )
		{
			/* The queue create function will set all the queue structure members
			correctly for a generic queue, but this function is creating a
			mutex.  Overwrite those members that need to be set differently -
			in particular the information required for priority inheritance. */
			pxNewQueue->u.xSemaphore.xMutexHolder = NULL;
			pxNewQueue->uxQueueType = queueQUEUE_IS_MUTEX;

			/* In case this is a recursive mutex. */
			pxNewQueue->u.xSemaphore.uxRecursiveCallCount = 0;

			traceCREATE_MUTEX( pxNewQueue );

			/* Start with the semaphore in the expected state. */
			( void ) xQueueGenericSend( pxNewQueue, NULL, ( TickType_t ) 0U, queueSEND_TO_BACK );
		}
		else
		{
			traceCREATE_MUTEX_FAILED();
		}
	}

#endif /* configUSE_MUTEXES */
```

Branie semafora odbywa się tą samą funkcją, co muteksu (`xSemaphoreTake` -> `xQueueSemaphoreTake`).
Między braniem semafora i muteksu występuje głównie jedna [kluczowa różnica] i [druga kluczowa różnica].

pierwsza (jeśli chcemy wziąć muteks, a ktoś go trzyma):
```c=1548
#if ( configUSE_MUTEXES == 1 )
{
    if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
    {
        taskENTER_CRITICAL();
        {
            xInheritanceOccurred = xTaskPriorityInherit( pxQueue->u.xSemaphore.xMutexHolder );
        }
        taskEXIT_CRITICAL();
    }
```

Dajemy trzymającemu swój priorytet, jeśli jest wyższy, a potem przepinamy go do odpowiedniej listy, jeśli ta zmiana zmienia jego stan (zob. [zadanie 2-4]) z ready w niskim priorytecie na ready w wysokim priorytecie (pozostałe stany są niezależne od priorytetu).

```c=3951
BaseType_t xTaskPriorityInherit( TaskHandle_t const pxMutexHolder )
{
TCB_t * const pxMutexHolderTCB = pxMutexHolder;
BaseType_t xReturn = pdFALSE;

    /* If the mutex was given back by an interrupt while the queue was
    locked then the mutex holder might now be NULL.  _RB_ Is this still
    needed as interrupts can no longer use mutexes? */
    if( pxMutexHolder != NULL )
    {
        /* If the holder of the mutex has a priority below the priority of
        the task attempting to obtain the mutex then it will temporarily
        inherit the priority of the task attempting to obtain the mutex. */
        if( pxMutexHolderTCB->uxPriority < pxCurrentTCB->uxPriority )
        {
            /* Adjust the mutex holder state to account for its new
            priority.  Only reset the event list item value if the value is
            not being used for anything else. */
            if( ( listGET_LIST_ITEM_VALUE( &( pxMutexHolderTCB->xEventListItem ) ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == 0UL )
            {
                listSET_LIST_ITEM_VALUE( &( pxMutexHolderTCB->xEventListItem ), ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) pxCurrentTCB->uxPriority ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
            }
```
```c=3978
            /* If the task being modified is in the ready state it will need
            to be moved into a new list. */
            if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ pxMutexHolderTCB->uxPriority ] ), &( pxMutexHolderTCB->xStateListItem ) ) != pdFALSE )
            {
                if( uxListRemove( &( pxMutexHolderTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
                {
                    taskRESET_READY_PRIORITY( pxMutexHolderTCB->uxPriority );
                }
```
```c=3991
                /* Inherit the priority before being moved into the new list. */
                pxMutexHolderTCB->uxPriority = pxCurrentTCB->uxPriority;
                prvAddTaskToReadyList( pxMutexHolderTCB );
            }
            else
            {
                /* Just inherit the priority. */
                pxMutexHolderTCB->uxPriority = pxCurrentTCB->uxPriority;
            }

            traceTASK_PRIORITY_INHERIT( pxMutexHolderTCB, pxCurrentTCB->uxPriority );

            /* Inheritance occurred. */
            xReturn = pdTRUE;
        }
        else
        {
            if( pxMutexHolderTCB->uxBasePriority < pxCurrentTCB->uxPriority )
            {
                /* The base priority of the mutex holder is lower than the
                priority of the task attempting to take the mutex, but the
                current priority of the mutex holder is not lower than the
                priority of the task attempting to take the mutex.
                Therefore the mutex holder must have already inherited a
                priority, but inheritance would have occurred if that had
                not been the case. */
                xReturn = pdTRUE;
            }
```

druga (jeśli upłynie czas, a pewne mniej ważne zadanie nadal trzyma muteks):
```c=1596
#if ( configUSE_MUTEXES == 1 )
{
    /* xInheritanceOccurred could only have be set if
    pxQueue->uxQueueType == queueQUEUE_IS_MUTEX so no need to
    test the mutex type again to check it is actually a mutex. */
    if( xInheritanceOccurred != pdFALSE )
    {
        taskENTER_CRITICAL();
        {
            UBaseType_t uxHighestWaitingPriority;

            /* This task blocking on the mutex caused another
            task to inherit this task's priority.  Now this task
            has timed out the priority should be disinherited
            again, but only as low as the next highest priority
            task that is waiting for the same mutex. */
            uxHighestWaitingPriority = prvGetDisinheritPriorityAfterTimeout( pxQueue );
            vTaskPriorityDisinheritAfterTimeout( pxQueue->u.xSemaphore.xMutexHolder, uxHighestWaitingPriority );
        }
        taskEXIT_CRITICAL();
    }
}
#endif /* configUSE_MUTEXES */
```

Funkcja `prvGetDisinheritPriorityAfterTimeout` wybiera priorytet, który powinno otrzymać zadanie na powrót, jeśli upłynie czas czekania.
Być może nie jest to jego pierwotny priorytet, bo inne zadania mogły mu też podbijać priorytet.
```c=2045
static UBaseType_t prvGetDisinheritPriorityAfterTimeout( const Queue_t * const pxQueue )
{
UBaseType_t uxHighestPriorityOfWaitingTasks;

    /* If a task waiting for a mutex causes the mutex holder to inherit a
    priority, but the waiting task times out, then the holder should
    disinherit the priority - but only down to the highest priority of any
    other tasks that are waiting for the same mutex.  For this purpose,
    return the priority of the highest priority task that is waiting for the
    mutex. */
    if( listCURRENT_LIST_LENGTH( &( pxQueue->xTasksWaitingToReceive ) ) > 0U )
    {
        uxHighestPriorityOfWaitingTasks = ( UBaseType_t ) configMAX_PRIORITIES - ( UBaseType_t ) listGET_ITEM_VALUE_OF_HEAD_ENTRY( &( pxQueue->xTasksWaitingToReceive ) );
    }
    else
    {
        uxHighestPriorityOfWaitingTasks = tskIDLE_PRIORITY;
    }

    return uxHighestPriorityOfWaitingTasks;
}
```

Przywracamy zadaniu jego priorytet lub zadany, ale tylko jeśli trzyma wyłącznie jeden muteks (uproszczenie), i zapamiętujemy poprzedni priorytet.
Jeśli nastąpiło zblokowanie zadania (przez spadek priorytetu), należy to również obsłużyć (przenieść ze stanu ready wyższego priorytetu na ready niższego, podobnie jak wyżej, ale na odwrót).

```c=4118
void vTaskPriorityDisinheritAfterTimeout( TaskHandle_t const pxMutexHolder, UBaseType_t uxHighestPriorityWaitingTask )
{
TCB_t * const pxTCB = pxMutexHolder;
UBaseType_t uxPriorityUsedOnEntry, uxPriorityToUse;
const UBaseType_t uxOnlyOneMutexHeld = ( UBaseType_t ) 1;

    if( pxMutexHolder != NULL )
    {
        /* If pxMutexHolder is not NULL then the holder must hold at least
        one mutex. */
        configASSERT( pxTCB->uxMutexesHeld );

        /* Determine the priority to which the priority of the task that
        holds the mutex should be set.  This will be the greater of the
        holding task's base priority and the priority of the highest
        priority task that is waiting to obtain the mutex. */
        if( pxTCB->uxBasePriority < uxHighestPriorityWaitingTask )
        {
            uxPriorityToUse = uxHighestPriorityWaitingTask;
        }
        else
        {
            uxPriorityToUse = pxTCB->uxBasePriority;
        }

        /* Does the priority need to change? */
        if( pxTCB->uxPriority != uxPriorityToUse )
        {
            /* Only disinherit if no other mutexes are held.  This is a
            simplification in the priority inheritance implementation.  If
            the task that holds the mutex is also holding other mutexes then
            the other mutexes may have caused the priority inheritance. */
            if( pxTCB->uxMutexesHeld == uxOnlyOneMutexHeld )
            {
                /* If a task has timed out because it already holds the
                mutex it was trying to obtain then it cannot of inherited
                its own priority. */
                configASSERT( pxTCB != pxCurrentTCB );

                /* Disinherit the priority, remembering the previous
                priority to facilitate determining the subject task's
                state. */
                traceTASK_PRIORITY_DISINHERIT( pxTCB, pxTCB->uxBasePriority );
                uxPriorityUsedOnEntry = pxTCB->uxPriority;
                pxTCB->uxPriority = uxPriorityToUse;

                /* Only reset the event list item value if the value is not
                being used for anything else. */
                if( ( listGET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ) ) & taskEVENT_LIST_ITEM_VALUE_IN_USE ) == 0UL )
                {
                    listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( TickType_t ) configMAX_PRIORITIES - ( TickType_t ) uxPriorityToUse ); /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
                }
```
```c=4175
                /* If the running task is not the task that holds the mutex
                then the task that holds the mutex could be in either the
                Ready, Blocked or Suspended states.  Only remove the task
                from its current state list if it is in the Ready state as
                the task's priority is going to change and there is one
                Ready list per priority. */
                if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ uxPriorityUsedOnEntry ] ), &( pxTCB->xStateListItem ) ) != pdFALSE )
                {
                    if( uxListRemove( &( pxTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
                    {
                        taskRESET_READY_PRIORITY( pxTCB->uxPriority );
                    }
```
```c=4192
                    prvAddTaskToReadyList( pxTCB );
                }
```

[`xSemaphoreCreateMutex`]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/include/semphr.h?r=a5ea5b69#xSemaphoreCreateMutex
[`xSemaphoreCreateBinary`]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/include/semphr.h?r=a5ea5b69#xSemaphoreCreateBinary
[`xQueueCreateGeneric`]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/include/semphr.h?r=a5ea5b69#xQueueCreateGeneric
[`xQueueCreateMutex`]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/include/semphr.h?r=a5ea5b69#xQueueCreateMutex
[`prvInitialiseMutex`]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/include/semphr.h?r=a5ea5b69#prvInitialiseMutex
[kluczowa różnica]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/queue.c?r=a5ea5b69#1550
[druga kluczowa różnica]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/queue.c?r=a5ea5b69#1596
[zadanie 2-4]: https://hackmd.io/@iiuwr-sju-20/BJmR2cD8L#Zadanie-2-4

> [name=Patrycja Balik] Brakuje mi tu `xSemaphoreGive` ([semphr.h:447](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/include/semphr.h?r=a5ea5b69#447)), w którym w pewnym momencie jest ciekawa procedura `xTaskPriorityDisinherit`.
> Ciąg ciekawych pod względem mechanizmu dziedziczenia priorytetów wywołań: `xSemaphoreGive` -> `xQueueGenericSend` -> `prvCopyDataToQueue` -> `xTaskPriorityDisinherit`

## Zadanie 4-1

:::warning
Autor: Michał Błaszczyk
:::

**Abstrakcja**

Każdemu numerowi przerwania odpowiada jeden **[łańcuch przerwania (interrupt chain / event (FreeBSD))](http://bxr.su/FreeBSD/sys/sys/interrupt.h#109)**. Każdy łańcuch przerwania posiada listę **[serwerów przerwań (interrupt server / handler (FreeBSD))](http://bxr.su/FreeBSD/sys/sys/interrupt.h#48)**. Każdy serwer przerwania posiada pola **ih_filter**, oraz **ih_handler**, będące procedurami wywoływanymi odpowiednio w **kontekście przerwania sprzętowego (hard interrupt context)** oraz w **kontekście wątku przerwania (interrupt thread context)**.

**ih_filter** to funkcja filtrująca. Jeśli jest dostarczona (tj, != NULL), wówczas **jądro** wywoła ją w **dolnej połówce (bottom half)** po wystąpieniu odpowiedniego przerwania.

Zadanie procedury ih_filter polega na odpytaniu konkretnego urządzenia w celu ustalenia potrzeby obsługi.
 - Jeśli taka potrzeba istnieje, wówczas wykonane może być dodatkowe przetwarzanie mające na celu np. zapisanie danych z urządzenia do bufora w pamięci. Jeśli procedura ih_filter całkowicie obsłuży przerwanie, wówczas zerowana jest flaga **ih_need**, oraz zwracana jest wartość **FILTER_HANDLED**. W przeciwnym przypadku (przerwanie wymaga dodatkowego przetwarzania), ustawiana jest flaga ih_need oraz zwracana jest wartość **FILTER_SCHEDULE_THREAD**.
- Jeżeli dane urządzenie nie wymaga obsługi, wówczas zerowana jest flaga **ih_need** oraz zwracana jest wartość **FILTER_STRAY**.

**Implementacja**

Każdemu numerowi przerwania odpowiada jeden łańcuch przerwania. W momencie wystąpienia przerwania o numerze **irq** znajdowana jest struktura intr_event (utrzymywana w tablicy **intr_events**) o polu **ie_irq** równym wartości irq . Następnie wywoływana jest funkcja **[intr_event_handle()](http://bxr.su/FreeBSD/sys/kern/kern_intr.c#1304)**. Dla każdego serwera przerwania uruchamia ona jego funkcję filtrującą. Jeżeli istnieje serwer który nie posiada funkcji filtrującej lub taki, którego funkcja filtrująca zwróciła wartość FILTER_SCHEDULE_THREAD, wówczas wyszeregowany zostanie wątek przerwania **ie_thread**. Wątek ten pośrednio wywołuje funkcję **[intr_event_execute_handlers()](http://bxr.su/FreeBSD/sys/kern/kern_intr.c#1090)**, która dla każdego serwera przerwania wymagającago obsługi (ih_need) wywołuje jego funkcję ih_handle.

**Kontekst przerwania sprzętowego (hard interrupt context)**

Przerwania sprzętowe są **asynchroniczne**. W momencie obsługi przerwania procesor przechodzi w **tryb jądra (kernel mode)**, zapisuje kontekst na stosie jądra bieżącego wątku i przechodzi do **procedury obsługującej (ISR)**. Procedura ISR wykonuje się w kontekście wątku będącego wątkiem bieżącym w momencie rozpoczęcia przez procesor procedury obsługi przerwania (exception processing).

Przedstawia to nastepujące implikacje to dla procedury ih_filter: 
- Konieczność oszczędności stosu.
- Brak możliwości **przełączenia kontekstu (constext switch)**.
- Jedyne dostępne środki synchronizacji to **blokady wirujące (spin lock)**.
- Konieczność minimalicazji czasu wykonywania.

Czemu tylko blokady wirujące?

Jeśli wątek byłby w stanie wejść w posiadanie blokady blokującej, mógłby tym samym wejść w stan **snu ograniczonego**, w efekcie zostałby przeniesiony do stanu **Blocked** i nastąpiłoby przełączenie konteksu. Zatem wspomiany wątek musiałby czekać, aż wszystkie **wątki jądra** o wyższym priorytecie (czyli niższym priorytecie numerycznym we FreeBSD) wyszły by ze stanu **Runnable**, jednakże przed rozpoczęciem wykonywania procedury obsługującej wyłączane są wszystkie (we FreeBSD, na innych systemach nie koniecznie) przerwania (sprzętowo nie w rejestrze stanu (choć tam oczywiście też, w kontekście wątku)), zatem tak długo jak wątek czeka na procesor, przerwania pozostają zablokowane.

## Zadanie 4-2

:::warning
Autor: Wiktor Pilarczyk
:::

1. Pojęcia:
    * wątki przerwań - [ithread(9)](http://mdoc.su/f/9/ithread) - Wątki przerwań są wątkami kernela, która uruchamiają posortowaną listę handlerów (od handlera z najwyższym priorytetm). Wykonują się w dolnej połówce.
    * sen nieograniczony -  zadanie oczekuje na zdarzenie zewnętrzne, które wydarzy się w niedalekiej przyszłości (intencja programisty jądra)

### Wady uruchamiania procedury obsługi przerwania za pomocą wątków przerwań.

* Context switch - the interrupt threads currently in FreeBSD are referred to as heavyweight interrupt threads. They are called this because switching to an interrupt thread involves a full context switch. 
[link](https://www.freebsd.org/doc/en_US.ISO8859-1/books/arch-handbook/smp-design.html)
* Brak snu nieprzerywalnego - chcielibyśmy zapewnić, że coś się wydarzy

Pewna optymalizacja do 2. to  lightweight context switches (nie trzeba przełączać VM space'a), ale w skrócie może być z tym wiele problemów.

>[name=Tomasz Stachowski] Wydaje się że priorytety handlerów mogą nie być uszanowane: weżmy ithread A z handlerami o priorytetach 1 i 3, i ithread B z handlerem o priorytecie 2 - priorytet ithreada to priorytet najwyższego handlera w nim, zatem handlerowi o priorytecie 1 z A będzie mieć pierwszeństwo względem B.
>[name=Wiktor Pilarczyk] Priorytet ithread'a jest ustawiany statycznie, priorytety handlerów służą jedynie do szeregowania handlerów.

### Dlaczego wątki przerwań nie mogą przejść w sen nieograniczony?

Ponieważ możemy czekać na zdarzenie zewnętrzne, które może się nigdy nie wydarzyć i np. czekając na to zdarzenie możemy blokować inne ithready przez co nie obsłużymy przyszłych interuptów. 
W śnie ograniczonym, jeśli żaden wątek nie śpi snem nieograniczonym to mamy pewność, że zawsze istnieje wątek, który robi progres (korzysta z czasu procesora).


## Zadanie 4-3

:::warning
Autor: Mateusz Maciejewski
:::

```c
int intr_event_create(
1	struct intr_event **event, //out
2	void *source,
3	int flags,
4	int irq,
5	void (*pre_ithread)(void *),
6	void (*post_ithread)(void *),
7	void (*post_filter)(void *),
8	int (*assign_cpu)(void *,int),
9	const char *fmt,
+	...)
```

[`intr_event_create`](http://bxr.su/FreeBSD/sys/kern/kern_intr.c#intr_event_create) tworzy strukturę `intr_event` na podstawie konfiguracji podanej przez sterownik, standardowo w funkcji `*_setup_intr(...)`. Struktura ta zawiera metadane niezbędne do obsługi zdarzenia (numery przerwań, lista handlerów, adresy hooków (`post/pre`)).


Przykładowe użycia `intr_event_create` w sterownikach:
* http://bxr.su/FreeBSD/sys/mips/atheros/apb.c#301
* http://bxr.su/FreeBSD/sys/mips/atheros/ar71xx_pci.c#572
* http://bxr.su/FreeBSD/sys/mips/atheros/ar724x_pci.c#545
* http://bxr.su/FreeBSD/sys/mips/atheros/qca955x_pci.c#471

```c 
// intr_event_handle(...)
// ...
1390    if (thread) { // jeżeli planujemy uruchomić ithread
1391        if (ie->ie_pre_ithread != NULL)
1392            ie->ie_pre_ithread(ie->ie_source);
1393    } else {
1394        if (ie->ie_post_filter != NULL)
1395            ie->ie_post_filter(ie->ie_source);
1396    }
```


Zastosowanie poszczególnych parametrów:
* [`post_filter`](http://bxr.su/FreeBSD/sys/kern/kern_intr.c#1394) - uruchamiane po działaniu wszystkich filtrów, jeżeli nie został zaplanowany ithread, w powyższych przykładach często `null` (bo stosują ithready).
* [`pre_ithread`](http://bxr.su/FreeBSD/sys/kern/kern_intr.c#1392) - uruchamiane przed działaniem wątku przerwania, standardowo maskowanie przerwania.
* [`post_ithread`](http://bxr.su/FreeBSD/sys/kern/kern_intr.c#1197) - uruchamiane po wszystkich handlerach, standardowo włączenie przerwania.

* [mv_gpio_setup_intrhandler()](http://bxr.su/FreeBSD/sys/arm/mv/gpio.c#416)

## Zadanie 4-4

:::warning
Autor: Michał Opanowicz
:::

Sygnatura tsleep:
```c
int tsleep(void *chan,	int priority, const char *wmesg, int timo);
```

Z manuala `tsleep`:
*If	priority includes the PCATCH flag, pending signals are allowed to interrupt the sleep,	otherwise pending signals are ignored during the sleep.  If	PCATCH is set and a signal becomes pending, ERESTART is	returned if the current system call should be restarted if possible, and EINTR is returned if the system call should be interrupted	by the signal (return EINTR).*

Problemy (bez PCATCH):
* Taki proces nigdy się nie zakończy. Zajmuje niepotrzebnie miejsce w pamięci fizycznej oraz w strukturach jądra np. PID.
* Jeżeli proces trzymał jakieś locki, na których zablokowały się snem nieprzerywalnym inne procesy, to efektywnie te inne procesy znajdują się w tej samej sytuacji, co proces, który jest uśpiony w readzie na zawsze. 
* W szczególności ta sytuacja mogła by się zdarzyć dla locków trzymanych przez jądro, potencjalnie blokując wiele wątków jądra.

## Zadanie 4-5

:::warning
Autor: Paweł Jasiak
:::

* _tick_ -- 1000 razy na sekundę (generuje przerwanie zegarowe)
* ale mamy oszustwo i jądro zazwyczaj sobie robi to "much less frequently" (bo uznaje, że nie warto to obsługiwać częściej)
* dlaczego oszukujemy? bo jak komputer idzie sobie spać i nie musi obsługiwać przerwanka to zjada mniej prądu (fajne w laptopie)
* `hardclock` to była procedurka, która sobie to obsługiwała
    * z oczywistych powodów musi on działać bardzo szybko (szybciej niż jeden tick) bo zaczniemy gubić czas i będzie bardzo nieprzyjemnie
    * jako, że ma priorytet niczym Królowa Angielska to wszystko inne jest zablokowane na czas jego wykonania -- a co się dzieje jak blokujemy obsługę karty sieciowej? no pakiety nie dochodzą.... a co się dzieje jak nie dochodzą nam pakiety UDP jak oglądamy Barwy Szczęścia? no jesteśmy nieszczęśliwi
* `softclock` no to jak wiemy, że `hardclock` jest jak Królowa Angielska to potrzebujemy czegoś, do czego deleguje się mniej ważne rzeczy, no i tym zajmuje się softclock
* `statclock` 127 razy na sekundę, dla jakiś statystyk systemowych
* `profclock` 8128 razy na sekudnę do profilowania

No i co robi `hardclock`?
* jeśli mamy ustawiony timer wirtualny lub profilowania to dekrementujemy go i jeśli upłynął czas to dostarczamy sygnał (trochę nie po polsku)
* poprawiamy czas dnia o odpowiednią liczbę (względem poprzedniego wywołania timera)
* jeśli nie ma ustawionego `profclock` to wykonujemy jego robotę
* jak wyżej dla `statclock`
* jeśli `softclock` powinien zostać wykonany, to robimy proces z nim `runnable`
* wymusza context switcha (historycznie tak robił)


`statclock`
* po 4 tickach dla procesu robimy ponowne przeliczenie priorytetu
* dalsza statystyka (co robiliśmy w momencie ticka)


`softclock`
* przeliczanie priorytetów (ale scheduler mamy już raz na sekundę)
* timeouty -- informowanie o nich procesów
* retransmisja dropniętych pakietów
* watchdog

mamy coś takiego jak `callout queue` w którą wrzuamy co się ma stać jak przyjdzie timeout (obrazek był na wykładzie) -- mamy 200 takich kolejek (po jednej na tick, umiemy więc reprezentować czas od teraz do teraz + 199)

kiedy robimy`hardclock` to `teraz++` (odnosząc się do kolejek), jeśli kolejka z `teraz` nie jest pusta, to będziemy zapuszczać `softclock` który zajmuje się interpretacją zawartości kolejki

oczywiście w naszym wpisie w kolejce pamiętamy kiedy przyjdzie `teraz`, więc jak chcemy wrzucić się do czasu `teraz + n` to liczymy `teraz + n mod 200` (jeśli umiem matematykę, ale wiadomo o co chodzi) i wrzucamy też do kolejki informację o tym jaki tick nas interesuje (jak przyjdzie zły tick to po porstu będziemy nadal czekać w tej kolejce, a jak przyjdzie interesujący nas tick to wywalimy się z kolejki -- teraz zastanawiam się czy nazwanie tego kolejką jest sensowne?)


```C=136
int
hardclockintr(void)
{
    sbintime_t now;
    struct pcpu_state *state;
    int done;

    if (doconfigtimer() || busy)
        return (FILTER_HANDLED);
    state = DPCPU_PTR(timerstate);
    now = state->now;
    done = handleevents(now, 0);
    return (done ? FILTER_HANDLED : FILTER_STRAY);
}
```

`doconfigtimer` -- Reconfigure specified per-CPU timer on other CPU. Called from IPI handler. (na podstawie action z pcpu_state)

```C=113
struct pcpu_state {
    struct mtx  et_hw_mtx;  /* Per-CPU timer mutex. */
    u_int       action;     /* Reconfiguration requests. */
    u_int       handle;     /* Immediate handle resuests. */
    sbintime_t  now;        /* Last tick time. */
    sbintime_t  nextevent;  /* Next scheduled event on this CPU. */
    sbintime_t  nexttick;   /* Next timer tick time. */
    sbintime_t  nexthard;   /* Next hardclock() event. */
    sbintime_t  nextstat;   /* Next statclock() event. */
    sbintime_t  nextprof;   /* Next profclock() event. */
    sbintime_t  nextcall;   /* Next callout event. */
    sbintime_t  nextcallopt;    /* Next optional callout event. */
    int     ipi;        /* This CPU needs IPI. */
    int     idle;       /* This CPU is in idle mode. */
};
```

Pytanie, dlaczego `typedef __int64_t   sbintime_t;`? `__int64_t` to w tym przypadku typedef na long long, dlaczego nie używamy po prostu nazewnictwa `int64_t`? 


[`handleevents`](http://bxr.su/FreeBSD/sys/kern/kern_clocksource.c#handleevents)
```C=155
static int
handleevents(sbintime_t now, int fake)
{
    sbintime_t t, *hct;
    struct trapframe *frame;
    struct pcpu_state *state;
    int usermode;
    int done, runs;

    done = 0;
    frame = curthread->td_intr_frame;
    usermode = TRAPF_USERMODE(frame);

    state = DPCPU_PTR(timerstate);

    runs = 0;
    while (now >= state->nexthard) {
        state->nexthard += tick_sbt;
        runs++;
    }
    if (runs) {
        hct = DPCPU_PTR(hardclocktime);
        *hct = state->nexthard - tick_sbt;
        if (fake < 2) {
            hardclock(runs, usermode);
            done = 1;
        }
    }
    runs = 0;
    while (now >= state->nextstat) {
        state->nextstat += statperiod;
        runs++;
    }
    if (runs && fake < 2) {
        statclock(runs, usermode);
        done = 1;
    }
    if (profiling) {
        runs = 0;
        while (now >= state->nextprof) {
            state->nextprof += profperiod;
            runs++;
        }
        if (runs) {
            profclock(runs, usermode, TRAPF_PC(frame));
            done = 1;
        }
    } else
        state->nextprof = state->nextstat;
    if (now >= state->nextcallopt || now >= state->nextcall) {
        state->nextcall = state->nextcallopt = SBT_MAX;
        callout_process(now);
    }

    t = getnextcpuevent(0);
    ET_HW_LOCK(state);
    if (!busy) {
        state->idle = 0;
        state->nextevent = t;
        loadtimer(now, (fake == 2) &&
            (timer->et_flags & ET_FLAGS_PERCPU));
    }
    ET_HW_UNLOCK(state);
    return (done);
}
```

```C=457
void
hardclock(int cnt, int usermode)
{
    struct pstats *pstats;
    struct thread *td = curthread;
    struct proc *p = td->td_proc;
    int *t = DPCPU_PTR(pcputicks);
    int global, i, newticks;

    /*
     * Update per-CPU and possibly global ticks values.
     */
    *t += cnt;
    global = ticks;
    do {
        newticks = *t - global;
        if (newticks <= 0) {
            if (newticks < -1)
                *t = global - 1;
            newticks = 0;
            break;
        }
    } while (!atomic_fcmpset_int(&ticks, &global, *t));

    /*
     * Run current process's virtual and profile time, as needed.
     */
    pstats = p->p_stats;
    if (__predict_false(
        timevalisset(&pstats->p_timer[ITIMER_VIRTUAL].it_value) ||
        timevalisset(&pstats->p_timer[ITIMER_PROF].it_value)))
        hardclock_itimer(td, pstats, cnt, usermode);

    /* We are in charge to handle this tick duty. */
    if (newticks > 0) {
        tc_ticktock(newticks);
        if (watchdog_enabled > 0) {
            i = atomic_fetchadd_int(&watchdog_ticks, -newticks);
            if (i > 0 && i <= newticks)
                watchdog_fire();
        }
    }
    if (curcpu == CPU_FIRST())
        cpu_tick_calibration();
    if (__predict_false(DPCPU_GET(epoch_cb_count)))
        GROUPTASK_ENQUEUE(DPCPU_PTR(epoch_cb_task));
}
```

```C=231
static sbintime_t
getnextcpuevent(int idle)
{
    sbintime_t event;
    struct pcpu_state *state;
    u_int hardfreq;

    state = DPCPU_PTR(timerstate);
    /* Handle hardclock() events, skipping some if CPU is idle. */
    event = state->nexthard;
    if (idle) {
        hardfreq = (u_int)hz / 2;
        if (tc_min_ticktock_freq > 2
#ifdef SMP
            && curcpu == CPU_FIRST()
#endif
            )
            hardfreq = hz / tc_min_ticktock_freq;
        if (hardfreq > 1)
            event += tick_sbt * (hardfreq - 1);
    }
    /* Handle callout events. */
    if (event > state->nextcall)
        event = state->nextcall;
    if (!idle) { /* If CPU is active - handle other types of events. */
        if (event > state->nextstat)
            event = state->nextstat;
        if (profiling && event > state->nextprof)
            event = state->nextprof;
    }
    return (event);
}
```

* w pętli while w 178 (w oryginalnym kodzie, sorki, ale po przeróbkach na szybko zmieniła się numeracja) sprawdzamy, czy teraźniejszość przekroczyła czas hardclocka, jeśli tak, to poprawiamy czas następnego hardlocka i zwiększamy licznik, ile hardlocków się wykonało
* jeśli wykonaliśmy tick, to robimy `hardclocka`
    * omówiliśmy już wyżej co robi zgodnie z książką
    * robimy update liczby ticków
    * jeśli mamy ustawiony timer virtual lub profile to liczymy jakieś tam statystyki
    * potem robimy watchdoga jeśli trzeba
    * i kalibrujemy czas
* to samo powtarzamy dla statclocka
* i to samo dla profclocka
* `getnextcpuevent`
    * Schedule binuptime of the next event on current CPU.
    * generalnie liczymy według jakiś ustalonych reguł następny event
* `ET_HW_LOCK` pod spodem to spinlock
* jeśli nie jesteśmy busy to będziemy chcieli być idle
