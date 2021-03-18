# Ćwiczenia 4 (1 kwietnia 2020)

## Zadanie 3-1

:::success
Autor: Mateusz Maciejewski
:::

[`SerialInit`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/drivers/serial.c?r=211d93d8#31)
* Ustawia baud rate (częstotliwość nadawania)
* Tworzy dwie kolejki, wyjściową i wejściową (`SendQ`,`RecvQ`)
* Rejestruje obsługę przerwań TBE (_transmit buffer empty_) i RBF (_receiver buffer full_)

Kolejka - struktura danych która pozwala w bezpieczny sposób umieszczać i pobierać dane z bufora FIFO. Stanowi standardowy sposób komunikacji pomiędzy zadaniem a ISR.

Sprzętowe bufory są rozmiaru ~10bit na wejściu (`SERDATR[9:0]`) i ~16bit na wyjściu (`SERDAT[15:0]`).
Bufory programowe są wielkości `QUEUELEN = 64` bajtów.

Komunikacja pomiędzy połówkami polega na tym, że górna połowa odczytuje lub zapisuje dane do kolejek, natomiast dolna, wyzwolona odpowiednim przerwaniem, przekazuje dane pomiędzy owymi kolejkami a buforami sprzętowymi.

[`TriggerSend`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/drivers/serial.c?r=211d93d8#55)
Służy do natychmiastowego wysłania pojedynczego bajtu, bądź w przypadku trwającej transmisji dodania go na koniec kolejki wyjściowej. ~~To znaczy takie jest zamierzenie, ponieważ przez buga przypadek alternatywny nigdy się nie wydarzy.~~

(Już wprowadzony patch):
```c
if (uxQueueMessagesWaiting(SendQ) == 0 && (custom.serdatr & SERDATF_TBE)) 
```

Priorytety RBF i TBE są różne, ponieważ wydarzenie RBF jest istotnie ważniejsze od TBE -- sprzęt wymaga od nas przyjęcia odbioru danych poprzez wyczyszczenie bitu przerwania, daje nam na to czas trwania 8-10 symboli. W przeciwnym razie może zostać ustawiony bit `OVERRUN` (`SERDATR[15]`), sygnalizujący potencjalną utratę danych, które zostały nadpisane przez kolejne przychodzące bajty.


## Zadanie 3-2

:::success
Autor: Zbigniew Drozd
:::

### Wprowadzenie - Interrupt Server
`IntServer` to struktura zawierająca ListItem nazwany node oraz wskaźnik na kod.  
`INTSERVER_DEFINE(LineCounter, 0, (ISR_t)LineCounterHandler, &WaitingTasks);`  
rozwinięty do   
`#define INTSERVER(PRI, CODE, DATA)                                            
{.node = {.pvOwner = DATA, .xItemValue = (127 - (PRI))}, .code = CODE} `
Tworzy strukturę która zawiera pola
*   `pvOwner` = DATA (w tym wypadku wskaźnik na waiting tasks)
*   `xItemValue` = 127 - (PRI) priorytet
*   `code` = CODE (w tym przypadku funkcja LineCounterHandler)  
*    nazwa intservera, w tym wypadku LineCounter

(https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/include/interrupt.h?r=3c6ca3a9#109)

No i dobra, taki node Interrupt Servera sobie wejdzie na chain (po wywołaniu funkcji `AddIntServer`) (gdzie zostanie posortowany po priorytecie) albo zostanie odpięty od przerwania po wywołaniu `RemIntServer`.  
Teraz, jak odpalimy sobie `RunIntChain` (tak naprawdę to nie my to robimy, tylko przerwanie jakoś robi call do tej fkcji) to on sobie przejdzie po liście tych serwerów i w jakiś sposób go będzie callował (prawdopodobnie po prostu wywoływał, bo wykonuje is->code).

 (https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/include/interrupt.h?r=3c6ca3a9#INTSERVER)
### Procedury pomocnicze.
#### Procedura GetCounter(void)
Procedura bierze i zwraca liczbę 32b, która składa się z wartości rejestrów `ciatod[hi/mid/low]` w następujący sposób `|00000000|ciatodhi|ciatodmid|ciatodlow`. ciatod jest skrótem od `cia` (nazwa timera) + `tod` (50/60 Hz event counter bits) (http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node012E.html)
#### Procedura SetCounter(uint32_t line)
Analogiczna do `GetCounter`. Bierze 32b, ignoruje 8 z nich, a pozostałe 24 wciska w rejestry event countera.
#### Procedura SetAlarm(uint32_t line)
```c
static void SetAlarm(uint32_t line) {
  volatile uint8_t *ciacrb = &ciab.ciacrb;
  BSET(*ciacrb, CIACRAB_TODIN);
  SetCounter(line);
  BCLR(*ciacrb, CIACRAB_TODIN);
}
```
Dobra, zobaczmy co tutaj się dzieje. Wyciągamy wskaźnik na `ciab.ciacrb`. `ciab` to timer B, `crb` to `CIAB Control register B`. Makra `BCLR` i `BSET` odpowiednio wyłączają i włączają bit CIACRAB_TODIN (najstarszy) w tym rejestrze. 
Ten bit odpowiada za alarm. gdy ALARM jest równy:  
1 - wpisane to rejestru TOD (procedura `SetCounter`) ustawi Alarm    
0 - wpisanie do rejestru TOD ustawia licznik TOD     
(http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node0148.html)

Więc ta procedura mówi zegarowi
*   To co teraz będzie zapisane, to będzie nowy Alarm
*   Faktycznie zapisuje alarm
*   Wszystko co od teraz będzie zapisane, traktuj jako nadpisanie rejestrów TOD

### Procedura włączenia LineCountera
```c
void LineCounterInit(void) {
   SetCounter(0);
   vListInitialise(&WaitingTasks);
   AddIntServer(ExterChain, LineCounter);
}
```
Mówimy że znajdujemy się na zerowej linii, inicjalizujemy listę pustych tasków (tutaj dość nieciekawie, wrzucamy jakiś element z maksymalną wagą tak żeby zawsze był na końcu, oraz robimy jakiś setup żeby wiedzieć czy lista jest pusta. Wykonujemy jakieś checki, ale jak wcześniej, nieciekawe), a następnie wołamy `AddIntServer`.   
To co `AddIntServer` robi, to bierze jakiś chain przerwań oraz _prawdopodobnie_ funkcję, w groku nie było powiedziane czym jest LineCounter, może zaraz się to okaże. Następnie patrzymy na chain przerwań, (jeśli jest on pusty to jeszcze włączamy przerwanie) a następnie wykonujemy procedury `vListInitializeItem` oraz `vListInsert`. Pierwsza z nich robi jakieś integrity checki, druga z nich wrzuca nasz `IntServer` na listę `InterruptChain`. Wszystko dzieje się w sekcji krytycznej.

### Procedura wyłączenia LineCountera
```c
void LineCounterKill(void) {
  WriteICR(CIAB, CIAICRF_ALRM);
  RemIntServer(LineCounter);
  configASSERT(WaitingTasks.uxNumberOfItems == 0);
}
```
Najpierw wołamy WriteICR(CIAB, CIAICRF_ALRM).  TODO

Potem robimy RemIntSerwer(LineCounter). Odpinamy licznik linii od przerwania generowanego przez układ rysujący grafikę. A następnie robimy assert który sprawdza czy wszystko oczekujące na wybudzenie przez nas już na to nie oczekuje.

### Mięso, czyli LineCounterHandler
Jest to funkcja która wykonuje się co przerwanie które dostajemy od timera. (Konkretnie jest to przerwanie Alarm).
```c
static void LineCounterHandler(List_t *tasks) {
  if (listLIST_IS_EMPTY(tasks))
    return;

  /* Remove all items with counter value not greater that current one.
   * Wake up corresponding tasks. */
  uint32_t curr = GetCounter();
  while (listGET_ITEM_VALUE_OF_HEAD_ENTRY(tasks) <= curr) {
    xTaskHandle task = listGET_OWNER_OF_HEAD_ENTRY(tasks);
    uxListRemove(listGET_HEAD_ENTRY(tasks));
    vTaskNotifyGiveFromISR(task, &xNeedRescheduleTask);
  }

  /* Reprogram TOD alarm if there's an item on list,
   * otherwise disable alarm interrupt. */
  if (listLIST_IS_EMPTY(tasks))
    WriteICR(CIAB, CIAICRF_ALRM);
  else
    SetAlarm(listGET_ITEM_VALUE_OF_HEAD_ENTRY(tasks));
}
```
Ok, to najpierw patrzymy czy jakiś task jest na liście. Jeśli lista jest pusta to umieramy na zawsze (bo niko więcej nie trzeba będzie budzić)  
Jeśli ktoś jest na liście, to iterujemy się po tej liście, aż znajdziemy gościa który jeszcze chce spać (wtedy przestajemy) i każdego odwiedzonego gościa szturchamy patykiem (vTaskNotifyGiveFromISR) i wywalamy z listy (skoro go zbudziliśmy to nie musimy tego zrobić ponownie).

Jak skończymy budzić wszystkich którzy chcieli wstać, to patrzymy czy jeszcze ktoś nam nie został. Jeśli nie, to po prostu mówimy zegarowi żeby dał nam spokój, i więcej nam nie przeszkadzał. Jeśli ktoś jest na liście, to pytamy się go kiedy chce wstać, i to mówimy zegarowi. Zostaniemy (`LineCountHandler`) zbudzeni wtedy gdy ten task chciał (i dzięki temu będziemy mogli go zbudzić)

### Mięso pt. 2 czyli LineCounterWait
```c
void LineCounterWait(uint32_t lines) {
  taskENTER_CRITICAL();
  {
    /* If the list is empty before insertion then enable the interrupt. */
    if (WaitingTasks.uxNumberOfItems == 0) WriteICR(CIAB, CIAICRF_SETCLR | CIAICRF_ALRM);

    uint32_t alarm = GetCounter() + lines; /* Calculate wakeup time. */

    /* Insert currently running task onto waiting tasks list. */
    xTaskHandle owner = xTaskGetCurrentTaskHandle();
    ListItem_t item = { .xItemValue = alarm, .pvOwner = owner };
    vListInitialiseItem(&item);
    vListInsert(&WaitingTasks, &item);
   
    /* Reprogram TOD alarm if inserted task should be woken up as first. */
    if (listGET_HEAD_ENTRY(&WaitingTasks) == &item)  SetAlarm(alarm);

    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
  }
  taskEXIT_CRITICAL();
}
```

Ta procedura wołana jest przez procedurę która chce zostać zbudzona.  
Najpierw patrzymy czy ktoś czeka na obudzenie. Jeśli nie, to znaczy że przerwania od układu graficznego są wyłączone i należy je włączyć.  
Potem patrzymy sobie jaki jest bezwzględny czas w którym chcemy się zbudzić (argumentem jest zmiana czasu, więc absolutny to aktualny + zmiana).   
Następnie tworzymy list entry dla tego zadania i wrzucamy je na liste tasków które obsłuży interrupt server. Może się okazać że nowe zadanie chce się zbudzić najwcześniej. Więc na sam koniec patrzymy czy nasze zadanie spadło na początek listy sortowanej po czasie zbudzenia. Jeśli tak, to wołamy `SetAlarm` z nowym czasem pobudki, a następnie zatrzymuje proces i czeka na powiadomienie

> [name=Patrycja Balik] Btw, wydaje się, że w `SetAlarm` ta stała powinna być `CIACRBB_ALARM`, bo `CIACRAB_*` są dla rejestru A. Obie akurat wynoszą 7, więc tak jak pisałeś, jest mimo wszystko zgodnie z dokumentacją.
>[name=Krzysztof Boroński] Testowałem sobie ten `LineCounterWait` i niestety przerwanie nigdy nie zostaje wysłane, co zdaje się sugerować, jakoby licznk nie był uruchomiony. Ustawienie bitu `CIACRAB_START` na stan wysoki uruchamia licznik, ale po pierwszym interrupcie, wysyłane są bez przerwy kolejne przerwania **EXTER** efektywnie zawieszając system. Jak dotąd nie doszukałem się przyczyny takiego zachowania, ale zdaje mi się ono niezgodnym z zamierzeniem tejże funkcji.

## Zadanie 3-3

:::warning
Autor: Paweł Jasiak
:::

Była potrzebna taka mała zmiana (już jest w repo)
:::spoiler

```diff
diff --git a/include/rtc.h b/include/rtc.h
index a98ad5f..9bfb294 100644
--- a/include/rtc.h
+++ b/include/rtc.h
@@ -3,6 +3,7 @@
 
 /* Complete register description can be found in m6242b_oki_datasheet.pdf */
 
+#if 0
 struct msm6242b {
   uint32_t : 28, second2 : 4;
   uint32_t : 28, second1 : 4;
@@ -21,6 +22,26 @@ struct msm6242b {
   uint32_t : 28, control2 : 4;
   uint32_t : 28, control3 : 4;
 };
+#endif
+
+struct msm6242b {
+  uint32_t : 28, second2 : 4;
+  uint32_t : 28, second1 : 4;
+  uint32_t : 28, minute2 : 4;
+  uint32_t : 28, minute1 : 4;
+  uint32_t : 28, hour2 : 4;
+  uint32_t : 28, hour1 : 4;
+  uint32_t : 28, day2 : 4;
+  uint32_t : 28, day1 : 4;
+  uint32_t : 28, month2 : 4;
+  uint32_t : 28, month1 : 4;
+  uint32_t : 28, year2 : 4;
+  uint32_t : 28, year1 : 4;
+  uint32_t : 28, weekday : 4;
+  uint32_t : 28, control1 : 4;
+  uint32_t : 28, control2 : 4;
+  uint32_t : 28, control3 : 4;
+}; 
 
 extern struct msm6242b volatile msm6242b;
```

:::

Dodajemy nasz _katalog_ z kodem -- nazwałem go u siebie _rtc_

```diff
diff --git a/examples/Makefile b/examples/Makefile
index 009be7a..83582c9 100644
--- a/examples/Makefile
+++ b/examples/Makefile
@@ -1,7 +1,7 @@
 TOPDIR = $(realpath ..)
 
 SOURCES = startup.c trap.c fault.c
-SUBDIR = console floppy graphics preemption
+SUBDIR = console floppy graphics preemption rtc
 
 include $(TOPDIR)/build/build.lib.mk
 
diff --git a/include/rtc.h b/include/rtc.h
index a98ad5f..9bfb294 100644
```

Następnie mamy taki kodzik (_main.c_)

```c=1
#include <FreeRTOS/FreeRTOS.h>
#include <FreeRTOS/task.h>

#include <include/rtc.h>

#include <interrupt.h>
#include <stdio.h>

#define mainRTCTIMER_TASK_PRIORITY 3

/* --- mięsko */
static void vRtcTimerTask(__unused void *data) {
  for (;;) {
    /* --- okropny printf */
    printf("%d%d-%d%d-%d%d %d%d:%d%d:%d%d\n", msm6242b.year1 % 10,
           msm6242b.year2, msm6242b.month1, msm6242b.month2, msm6242b.day1,
           msm6242b.day2, msm6242b.hour1, msm6242b.hour2, msm6242b.minute1,
           msm6242b.minute2, msm6242b.second1, msm6242b.second2);
    /* --- czekamy sekundę */
    vTaskDelay(50);
  }
}

/* --- cała reszta jest ukradziona z przykładu z wywłaszczaniem */

static void SystemClockTickHandler(__unused void *data) {
  /* Increment the system timer value and possibly preempt. */
  uint32_t ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();
  xNeedRescheduleTask = xTaskIncrementTick();
  portCLEAR_INTERRUPT_MASK_FROM_ISR(ulSavedInterruptMask);
}

INTSERVER_DEFINE(SystemClockTick, 10, SystemClockTickHandler, NULL);

static xTaskHandle rtcTimerHandler;

int main(void) {
  portNOP(); /* Breakpoint for simulator. */

  AddIntServer(VertBlankChain, SystemClockTick);

  xTaskCreate(vRtcTimerTask, "rtc", configMINIMAL_STACK_SIZE, NULL,
              mainRTCTIMER_TASK_PRIORITY, &rtcTimerHandler);

  vTaskStartScheduler();

  return 0;
}

void vApplicationIdleHook(void) { custom.color[0] = 0x00f; }

```

> [name=Krystian Bacławski] To trzeba byłoby przepisać używając `vTaskDelayUntil`, albo powiadomień. Z tym rozwiązaniem jest problem z 50 + $\epsilon$.

## Zadanie 3-4

:::warning
Autor: Jakub Urbańczyk
:::

Schemat oddawania kontroli:
1. Wykonujemy instrukcję `trap #0`. [Link](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/portmacro.h?r=3484a8f2#97):
```c
#define vPortYield() { asm volatile("\ttrap\t#0\n"); }
```
2. Generowane jest przerwanie przez CPU
3. Wskakujemy do procedury obsługi przerwania `vPortYieldHandler`. [Link](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/portasm.S?r=3c6ca3a9#28):
```c
ENTRY(vPortYieldHandler)
	portSAVE_CONTEXT()
	jsr vTaskSwitchContext
	portRESTORE_CONTEXT()
END(vPortYieldHandler)
```
Które jest ustawiane w `xPortStartScheduler`. [Link](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/port.c?r=3484a8f2#85):
```c
ExcVec[EXC_TRAP(0)] = vPortYieldHandler;
```
5. Odkładamy obecny kontekst na stos, a wskaźnik na stos zapisujemy w obecnym TCB. [Link](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/portasm.S?r=3c6ca3a9#3):
```c
#define portSAVE_CONTEXT()                                              \
	or.w    #0x0700,sr;             /* wyłączamy interrupty */       \
	movem.l d0-a6,-(sp);            /* zapisujemy rejestry na stos */      \
	move.l  pxCurrentTCB,a0;        /* zapisujemy wskaźnik na obecne TCB do rejestru */ \
	move.l  sp,(a0);                /* zapisujemy stack pointer pod adres wskazywany przez pxCurrentTCB */
```
6. Wykonujemy MI kod zmiany kontekstu [Link](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#2947) W nim:
	6.1. Zapisujemy ewentualne statystki o obecnie działającym programie
	6.2. Sprawdzamy czy niezmienniki stosu zostały popsute (stack overflow)
	6.3. Spośród wszystkich zadań w stanie Ready wybieramy ten o najwyższym piorytecie. Jeśli jest takich zadań wybieramy kolejne z listy- round robin, żeby wprowadzić sprawiedliwość.
7. Przywracamy kontekst nowo wybranego zadania. [Link](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/portasm.S?r=3c6ca3a9#9):
```c
#define portRESTORE_CONTEXT()                      
	move.l  pxCurrentTCB,a0;        /* wczytujemy adres obecnego TCB */ \
	move.l  (a0),sp;                /* skoro pierwszym polem jest wskaźnik na stos
					  to wystarczy odczytać adres a0 */     \
	movem.l (sp)+,d0-a6;            /* wczytujemy rejestry */   \
	rte;                            /* wczytujemy SR i PC */
```

Zauważmy, że gdy odkładamy wskaźnik na stos to wpisujemy go pod adres wskazywany przez `pxCurrentTCB`, czyli tak właściwie do pierwszego pola tejże struktury. Nie widzę powodu, dlaczego nie mogłoby to być np. drugie pole. Wtedy po prostu można było by wpisywać ten wskaźnik na stos do `pxCurrentTCB` z pewnym offsetem.
## Zadanie 3-5

:::warning
Autor: Michał Błaszczyk
:::

**Rozruch planisty**

[vTaskStartScheduler()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1968):
1. Tworzymy wątek **Idle**.
2. (Potencjalnie) tworzymy wątek czasomierza.
3. Inicjalizujemy **czas najbliższej pobudki** (xNextTaskUnblockTime), zmienną **stanu planisty** (xSchedulerRunning), oraz **zegar systemowy** (xTickCount).
4. Wywołujemy funkcję [xPortStartScheduler()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/port.c?r=3484a8f2#85).

[xTaskCreate()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#731):
1. Alokujemy stos dla nowego zadania.
2. Alokujemy strukturę **TCB** dla nowego zadania.
3. Inicjalizujemy nowe zadanie [prvInitialiseNewTask()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#822).
4. Dodajemy stworzone zadanie do listy zadań w stanie **Ready** ([prvAddNewTaskToReadyList()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1073)).

[prvInitialiseNewTask()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#822):
1. (Potencjalnie) ustawiamy **tryb uprzywilejowania (privileged mode)**.
2. (Potencjalnie) wypełniamy stos ustalonym bajtem (na potrzeby wykrywania **przepełnienia stosu (stack overflow)**).
3. Inicjalizujemy wskaźnik stosu uwzględniając odpowiednie wyrównanie (u nas do 4 bajtów).
4. Zapisujemy nazwę wątku do jego TCB.
5. Ustawiamy priorytet zadania (na podstawie argumentu mod configMAX_PRIORITIES).
6. Ustawiamy **bazowy priorytet** zadania i zerujemy liczbę posiadanych mutexów.
7. Inicjalizujemy **węzeł stanu (xStateListItem)** oraz **węzeł zdarzenia (xEventListItem)**.
8. Ustawiamy pole pvOwner węzła stanu na tworzony wątek.
9. Ustawiamy wartości kluczy dla powyższych węzłów.
10. Inicjalizujemy resztę zmiennych ze struktury **TCB**.
11. Inicjalizujemy stos wątku ([pxPortInitialiseStack()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/port.c?r=3484a8f2#62)).

[prvAddNewTaskToReadyList()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#1073):
1. Wchodzimy do sekcji krytycznej (chronimy listy zadań).
2. Inkrementujemy aktualną liczbę zadań (uxCurrentNumberOfTasks).
3. Jeśli nie ma innych zadań lub wszystkie inne zadania są w stanie **Suspended**, wówczas ustawiamy pxCurrentTCB na zadanie dodawane do stanu **Ready**. Jeśli jest to faktycznie pierwsze zadanie, wówczas inicjalizujemy listy zadań ([prvInitialiseTaskLists()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#3546)).
4. W przeciwnym przypadku, jeśli scheduler jest wyłączony (a tak jest w przypadku rozważanego wywołania tej funkcji w trakcie rozruchu planisty), to jeśli nowe zadanie na liście ready ma wyższy priorytet od zadania wskazywanego przez pxCurrentTCB to ustawiamy ten wskaźnik na to zadanie.
5. Inkrementujemy liczbę zadań (na użytek kodu śledzącego (traceing)).
6. Dodajemy zadanie do listy ready ([prvAddTaskToReadyList()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#218)).
7. Jeżeli scheduler działa i jest potrzeba, przełączamy kontekst.

[pxPortInitialiseStack()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/port.c?r=3484a8f2#62):
1. Umieszczamy na stosie argumenty dla procedury wątku.
2. Ustawiamy adres powrotu z procedury wątku na 0.
3. (Potecnjalnie) umieszczamy na stosie początkowy wektor formatu.
4. Umieszczamy na stosie adres procedury wątku.
5. Umieszczamy na stosie początkowy **rejestr stanu** (bit S = 1, reszta = 0 (w tym **IPL** = 0)) (jest to niezbędne jako że w procedurze [vPortStartFirstTask()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/portasm.S?r=3c6ca3a9#34) wątek wykona instrukcję **rte (Return from Exception)**).
6. Umieszczamy na stosie rejestry danych d0 - d7 oraz rejestry adresowe a0 - a6 (nie inicjalizujemy ich wartości).

[xPortStartScheduler()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/port.c?r=3484a8f2#85):
1. Ustawiamy **wektor wyjątku (exception vector)** dla wyjątku `trap #0`. Procedura obsługująca to [vPortYieldHandler()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/port.c?r=3484a8f2#85).
2. Włączamy wszystkie przerwania (rejestr INTENA).
3. Rozpoczynamy wykonywanie pierwszego wątku ([vPortStartFirstTask()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/portasm.S?r=3c6ca3a9#34)).

[vPortStartFirstTask()](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/portable/m68k-amiga/portasm.S?r=3c6ca3a9#34):
1. Zapisujemy adres struktury TCB zadania wskazywanego przez pxCurrentTCB do rejestru a0.
2. Odtwarzamy wskaźnik stosu (rejestr SR).
3. Przywracamy wartości rejestrów danych d0 - d7 oraz rejestrów adresowych a0 - a6.
4. Wykonujemy instrukcję rte (tj. ściągamy ze stosu wartość rejestrów SR i PC).

## Zadanie 3-6

:::warning
Autor: Franciszek Zdobylak
:::
### `xTaskNotify`
Wywołujemy ogólniejszą procedurę [`xTaskGenericNotify`](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#4714), w której:
* zachowujemy poprzednią wartość notyfikacji (może być potrzebna zadaniu które wysyła powiadomienie)
* Ustawiamy zgodnie z `eAction` nową wartość notyfikacji
* Jeśli zadanie spało oczekując na powiadomienie to je budzimy
* Jeśli ma wyższy priorytet to od razu oddajemy sterowanie (nie wychodząc z sekcji krytycznej)
```c=4714
BaseType_t xTaskGenericNotify( TaskHandle_t xTaskToNotify, 
    uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue ) {
TCB_t * pxTCB;
BaseType_t xReturn = pdPASS;
uint8_t ucOriginalNotifyState;

  pxTCB = xTaskToNotify;
  
  taskENTER_CRITICAL();
  {
    if( pulPreviousNotificationValue != NULL ) {
    	*pulPreviousNotificationValue = pxTCB->ulNotifiedValue;
    }
    
    ucOriginalNotifyState = pxTCB->ucNotifyState;
    
    pxTCB->ucNotifyState = taskNOTIFICATION_RECEIVED;
    
    switch( eAction ) {
      case eSetBits	:
        pxTCB->ulNotifiedValue |= ulValue;
        break;
    
      case eIncrement	:
        ( pxTCB->ulNotifiedValue )++;
        break;
    
      case eSetValueWithOverwrite	:
        pxTCB->ulNotifiedValue = ulValue;
        break;
    
      case eSetValueWithoutOverwrite :
        if( ucOriginalNotifyState != taskNOTIFICATION_RECEIVED ) {
           pxTCB->ulNotifiedValue = ulValue;
         } else {
           xReturn = pdFAIL;
         }
         break;

       case eNoAction:
         break;
    
    	default:
        /* tu nie powinniśmy trafić */
        break;
    }
    
    if( ucOriginalNotifyState == taskWAITING_NOTIFICATION ) {
    	( void ) uxListRemove( &( pxTCB->xStateListItem ) );
    	prvAddTaskToReadyList( pxTCB );
    
    
    	if( pxTCB->uxPriority > pxCurrentTCB->uxPriority ) {
    		taskYIELD_IF_USING_PREEMPTION();
    	}
    }
  }
  taskEXIT_CRITICAL();
  
  return xReturn;
}
```
### `xTaskNotifyWait` [(kod)](https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/tasks.c?r=a5ea5b69#4634)
Czekanie na powiadomienie:
* Czyścimy bity w zmiennej `ulNotifiedValue` wg podanego parametru
* ustawiamy stan (stan mówiący o powiadomieniach) na `taskWAITING_NOTIFICATION`
* Jeśli zadanie chce czekać na powiadomienie, to dodajemy do listy uśpionych i oddajemy sterowanie
Odbieranie powiadomienia:
* pobieramy wartość powiadomienia z TCB
* Jeśli zadanie nie odebrało powiadomienia (mogło się tak zdarzyć, bo na przykład nie chciało czekać), to zwrócimy fałsz
* Jeśli odebrało, czyścimy bity wg podanego parametru
```c=4634
BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, 
uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait ) {
  BaseType_t xReturn;

  taskENTER_CRITICAL();
  {
    if( pxCurrentTCB->ucNotifyState != taskNOTIFICATION_RECEIVED )
    {
      pxCurrentTCB->ulNotifiedValue &= ~ulBitsToClearOnEntry;
      pxCurrentTCB->ucNotifyState = taskWAITING_NOTIFICATION;

      if( xTicksToWait > ( TickType_t ) 0 ) {
        prvAddCurrentTaskToDelayedList( xTicksToWait, pdTRUE );

        portYIELD_WITHIN_API();
      }
    }
  }
  taskEXIT_CRITICAL();

  taskENTER_CRITICAL();
  {
    if( pulNotificationValue != NULL ) {
      *pulNotificationValue = pxCurrentTCB->ulNotifiedValue;
    }

    if( pxCurrentTCB->ucNotifyState != taskNOTIFICATION_RECEIVED ) {
      xReturn = pdFALSE;
    } else {
      pxCurrentTCB->ulNotifiedValue &= ~ulBitsToClearOnExit;
      xReturn = pdTRUE;
    }

    pxCurrentTCB->ucNotifyState = taskNOT_WAITING_NOTIFICATION;
  }
  taskEXIT_CRITICAL();

  return xReturn;
}
```


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

Dajemy trzymającemu swój priorytet, jeśli jest wyższy, a potem przepinamy go do odpowiedniej listy, jeśli ta zmiana zmienia jego stan (zob. zadanie 2-4) z ready w niskim priorytecie na ready w wysokim priorytecie (pozostałe stany są niezależne od priorytetu).

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
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }

            /* If the task being modified is in the ready state it will need
            to be moved into a new list. */
            if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ pxMutexHolderTCB->uxPriority ] ), &( pxMutexHolderTCB->xStateListItem ) ) != pdFALSE )
            {
                if( uxListRemove( &( pxMutexHolderTCB->xStateListItem ) ) == ( UBaseType_t ) 0 )
                {
                    taskRESET_READY_PRIORITY( pxMutexHolderTCB->uxPriority );
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }

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
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }

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
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    prvAddTaskToReadyList( pxTCB );
                }
```

[`xSemaphoreCreateMutex`]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/include/semphr.h?r=a5ea5b69#xSemaphoreCreateMutex
[`xSemaphoreCreateBinary`]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/include/semphr.h?r=a5ea5b69#xSemaphoreCreateBinary
[kluczowa różnica]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/queue.c?r=a5ea5b69#1550
[druga kluczowa różnica]: https://mimiker.ii.uni.wroc.pl/source/xref/FreeRTOS-Amiga/FreeRTOS/queue.c?r=a5ea5b69#1596

# Dyskusje

>[name=Samuel Li] dorzucam własne notatki:
>https://hackmd.io/61Q3NLjIRWC93KyTwq8csg?both
