# Ćwiczenia 6 (22 kwietnia 2020)

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

* w pętli while sprawdzamy, czy teraźniejszość przekroczyła czas hardclocka, jeśli tak, to poprawiamy czas następnego hardlocka i zwiększamy licznik, ile hardlocków się wykonało
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

## Zadanie 5-1

:::warning
Autor: Mateusz Maciejewski
:::
![](https://i.imgur.com/C327pHK.png)


Zagubiona pobudka następuje w momencie, kiedy pomiędzy nieudanym założeniem blokady a ustąpieniem wątku w oczekiwaniu na jej zwolnienie blokada została odblokowana przez inny wątek. W tym momencie wątek nie został jeszcze wpisany na listę oczekujących, co skutkuje tym, że przegapi właśnie wygenerowaną pobudkę.

```c
#define mtx_sleep(chan, mtx, pri, wmesg, timo)              \
       _sleep((chan), &(mtx)->lock_object, (pri), (wmesg),     \
           tick_sbt * (timo), 0, C_HARDCLOCK)
```

Przy oczekiwaniu na wydarzenie `chan` najpierw musimy zdobyć chroniący go mutex. Dopiero wtedy sprawdzamy, czy musimy oczekiwać. Jeśli tak, to wywołanie `mtx_sleep` wpisze nas na kolejkę oczekujących a następnie zwolni blokadę i zaśnie. Przy założeniu że `wakeup` może się wykonać tylko gdy wątek go generujący posiada mutex, zabezpieczyliśmy się przed zgubieniem pobudki.

[Przykład w sterowniku SPI](http://bxr.su/FreeBSD/sys/dev/intel/spi.c#407)
`sc` stanowi muteks chroniący kanał `dev`. Chcemy zablokować `sc_flags` (linia 361).
Strona wybudzająca:
```c 
358    INTELSPI_LOCK(sc);
...
405    /* Release the controller and wakeup the next thread waiting for it. */
406    sc->sc_flags = 0;
407    wakeup_one(dev);
408    INTELSPI_UNLOCK(sc);
```
Strona oczekująca:
```c 
358    INTELSPI_LOCK(sc);
359
360    /* If the controller is in use wait until it is available. */
361    while (sc->sc_flags & INTELSPI_BUSY) {
362        err = mtx_sleep(dev, &sc->sc_mtx, 0, "intelspi", 0);
363        if (err == EINTR) {
364            INTELSPI_UNLOCK(sc);
365            return (err);
366        }
367    }
```

(W kontekście wcześniejszego przykładu)
Wątek T2 wykonuje następującą sekwencję instrukcji:
```C
mtx_lock(resource.flag_lock);
if (resource.is_locked) {
  mtx_unlock(resource.flag_lock);
  sleep(chanel);
}
...
```

Problem stanowi wyścig pomiędzy zwolnieniem mutexa a supaniem wątku. Procedura mtx_sleep rozwiązuje ten problem, łącząc te dwa kroki w jeden niepodzielny (atomiczny) krok:
```C
mtx_lock(resource.flag_lock);
if (resource.is_locked) {
  mtx_sleep(chanel, resource.flag_lock, ...);
}
...
```

## Zadanie 5-2

:::warning
Autor: Michał Opanowicz
:::

Rozważmy następujący ciąg wydarzeń: mamy wątki A, B, C i semafor S, oraz procesory P1, P2. Początkowo A działa na P1, C działa na P2.

```c=
B woła P(S), B zasypia
A woła V(S), S zostaje przyznany B, który zostaje zaschedulowany 
C woła P(S), blokuje się, zwalnia P2
B rozpoczyna wykonywanie się na P2
```
Wówczas potrzebne są dwa context switche na procesorze P2, żeby kontynuować wykonywanie programu, ponieważ semafory zachowują kolejność procesów oczekujących.

Oto alternatywny przykład do tego zadania, z trochę innymi nazwami:
![Figure 7-7]

Podział sekcji krytycznej na dwie mniejsze chronione osobnymi semaforami nie niweluje problemu, bo jest to po prostu podzielenie sytuacji na dwie takie same, w których mamy identyczny problem.
Użycie mutexu rozwiązuje problem, ponieważ oddanie mutexu nie oznacza natychmiastowego wzięcia go przez inny wątek (mutex jest niczyj, dopóki inny wątek nie zrobi na nim lock) - oznacza to jednak, że tracimy gwarancję kolejności.

## Notatki do zadania P1

## Notatki do zadania P2

## Dyskusja
> [name=Samuel Li]
> https://hackmd.io/@3yEk1oluR_GXLRGqWMgB8Q/r1cGGzQd8

[Figure 7-7]: https://cdn.discordapp.com/attachments/689468490490708023/699649604605444176/unknown.png
