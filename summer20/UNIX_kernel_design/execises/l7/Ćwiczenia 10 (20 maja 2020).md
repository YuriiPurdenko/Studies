# Ćwiczenia 10 (20 maja 2020)

## Zadanie 7-1

:::warning
Autor: Bartosz Szpila
:::
**średnie obciążenie** - w systemach unixowych - średnia liczba procesów w stanie *runnable*, oczekujących na czas procesora przez dany okres czasu

**wykładnicze uśrednianie** - technika przewidywania przyszłej wartości na podstawie ciągu poprzednich wartości. Polega na uśrednianiu wyniku z wagą w następujący sposób
$$S_{n+1} = \alpha*T_n + (1-\alpha)S_n$$
gdzie $0>\alpha>1$, $T_i$ - i-ta faktyczna wartość, $S_i$ - szacowana i-ta wartość.   

W ten sposób nowsze wartości (lepiej odzwierciedlające aktualne warunki w systemie) mają większą wagę w obliczaniu kolejnego czasu działania.
![](https://i.imgur.com/5jguqPi.png)

Wzór:
``` c=605
/*
 * Compute a tenex style load average of a quantity on
 * 1, 5 and 15 minute intervals.
 */
static void
loadav(void *arg)
{
    int i, nrun;
    struct loadavg *avg;

    nrun = sched_load();
    avg = &averunnable;

    for (i = 0; i < 3; i++)
        avg->ldavg[i] = (cexp[i] * avg->ldavg[i] +
            nrun * FSCALE * (FSCALE - cexp[i])) >> FSHIFT;

    /*
     * Schedule the next update to occur after 5 seconds, but add a
     * random variation to avoid synchronisation with processes that
     * run at regular intervals.
     */
    callout_reset_sbt(&loadav_callout,
        SBT_1US * (4000000 + (int)(random() % 2000001)), SBT_1US,
        loadav, NULL, C_DIRECT_EXEC | C_PREL(32));
}
```
Skala:
```c
/*
 * Scale factor for scaled integers used to count %cpu time and load avgs.
 *
 * The number of CPU `tick's that map to a unique `%age' can be expressed
 * by the formula (1 / (2 ^ (FSHIFT - 11))).  The maximum load average that
 * can be calculated (assuming 32 bits) can be closely approximated using
 * the formula (2 ^ (2 * (16 - FSHIFT))) for (FSHIFT < 15).
 *
 * For the scheduler to maintain a 1:1 mapping of CPU `tick' to `%age',
 * FSHIFT must be at least 11; this gives us a maximum load avg of ~1024.
 */
#define FSHIFT  11      /* bits to right of fixed binary point */
#define FSCALE  (1<<FSHIFT)
```
Stałe:
```c=86
/*
 * Constants for averages over 1, 5, and 15 minutes
 * when sampling at 5 second intervals.
 */
static fixpt_t cexp[3] = {
    0.9200444146293232 * FSCALE,    /* exp(-1/12) */
    0.9834714538216174 * FSCALE,    /* exp(-1/60) */
    0.9944598480048967 * FSCALE,    /* exp(-1/180) */
};
```
*nrun* ($n$) - liczba gotowych wątków w systemie
*averunnable* ($S$) - średnie natężenie
*cexp* ($\alpha$) - stałe 
*FSCALE* ($\beta$) - scale factor = $2^{11}$

$S_{i_{n+1}} = (\alpha_i *S_{i_n} + n * \beta * (\beta-\alpha_i))/\beta = \frac{\alpha_i}{\beta}S_{i_n} + n * \beta * (1 - \frac{\alpha_i}{\beta})$

> [name=Samuel Li] Stała to `cexp`$=e^{-\frac{5s}{Δt}} \cdot 2^{11}$

## Zadanie 7-2

:::warning
Autor: Krzysztof Boroński
:::

We wczesnych systemach unixowych, scheduler w danym kroku liczy dla danego procesu jego priorytet następującym wzorem:

$P_j(i) = B_j + \frac{CPU_j(i - 1)}{2} + nice_j$

gdzie

* $B_j$ - podstawowy priorytet zadania
* $nice$ - wartość dostosowywana przez użytkownika
* $CPU_j(i-1)$ - "zużycie procesora" w ostatnim okresie szeregowania.

Obecne "zużycie procesora" jest dzielone na dwa w ramach filtracji przy szeregowaniu zadań. Jest tak dla tego, że spodziewamy się, że poźniejsze przybliżenie tej wartości będzie lepiej odzwierciedlać czasochlonność procesu. 
z każdym wywołaniem `hardclock` będzie ono inkrementowane.

Priorytety dzielą się na "spektra", odpowiadające rodzajom zadań w systemie operacyjnym.

1. Swapper
2. Block I/O
3. Pliki
4. Character I/O
5. Procesy użytkownika

$CPU_j(i-1)$ oraz $nice$ są odpowiednio przycinane, tak aby priorytet nie wychodził poza swoje spektrum.

Z tego co rozumiem, każde spektrum ma własną kolejkę wątków.

### `l_estcpu`
```
 * Here we describe how l_estcpu is decreased.
 *
 * Constants for digital decay (filter):
 *     90% of l_estcpu usage in (5 * loadavg) seconds
 *
 * We wish to decay away 90% of l_estcpu in (5 * loadavg) seconds. That is, we
 * want to compute a value of decay such that the following loop:
 *     for (i = 0; i < (5 * loadavg); i++)
 *         l_estcpu *= decay;
 * will result in
 *     l_estcpu *= 0.1;
 * for all values of loadavg.
 *
 * Mathematically this loop can be expressed by saying:
 *     decay ** (5 * loadavg) ~= .1
 *
 * And finally, the corresponding value of decay we're using is:
 *     decay = (2 * loadavg) / (2 * loadavg + 1)
```

Algorytm dla wylicza taki damping dla `l_estcpu`, aby pozbyć się 90% wartości zebranych w poprzednich pięciu cyklach. Jest to analogiczne do
wczceśniej wspomnianego dzielenia przez 2.

> [name=Paweł] ![](https://i.imgur.com/EXBti5X.png)

> [name= Wiktor] O tym Paweł mówi:
 $CPU_{j}(i) = \frac{CPU_{j}(i-1)}{2}$
## Zadanie 7-3

:::warning
Autor: Patrycja Balik
:::

> Przeczytaj [2,§10.3] lub [1,§10.3.4] o algorytmie Completely Fair Scheduler stosowanym w bieżących wersjach jądra Linux. Opowiedz o niedostatkach planisty O(1) i implementacji idei **sprawiedliwego szeregowania** (ang. *fair-share scheduling*). W jaki sposób algorytm CFS wybiera kolejne zadanie do uruchomienia? W jaki sposób nalicza wirtualny czas wykonania «`vruntime`», aby wymusić sprawiedliwy przydział czasu procesora?
>
> **Wskazówka:** Szczegóły można znaleźć w artykule [„Inside the Linux 2.6 Completely Fair Scheduler”](https://developer.ibm.com/tutorials/l-completely-fair-scheduler/) oraz [4, strona 46].

### Planista O(1)

Planista O(1) był fajny, bo jak nazwa wskazuje, operacje odbywały się w O(1), niezależnie od liczby aktywnych wątków.

Implementacja wyglądała tak, że były dwie tablice kolejek: *active* i *expired*. Wątki były wrzucane na procesor od tych z najwyższym priorytetem z tablicy *active*. Jeżeli wątek zużył cały kwant czasu, trafiał na kolejkę w *expired*. W przeciwnym wypadku zostawał na swoim miejscu w kolejce w tablicy *active*. Gdy kolejki *active* były puste, wskaźniki były zamieniane i *expired* stawało się nowym *active*. Redukowało to problem głodzenia wątków o niskich priorytetach.

:::spoiler Ilustracja struktury danych dla planisty O(1).
![](https://i.imgur.com/iDEWWW3.png)
:::

Ale jak pisze Tanenbaum, heurystyki do mierzenia interaktywności zadania były skomplikowane i niedoskonałe. To odbijało się na wydajności zadań o dużej interaktywności. Dodatkowo trzeba było mieć dużo kolejek.

### Planista CFS

Planista CFS umieszcza zadania w drzewie czerwono-czarnym, uporządkowanym ze względu `vruntime`. Wartość `vruntime` jest związana z czasem spędzonym przez zadanie na CPU. Ale co z priorytetami? To, jak zmienia się `vruntime` jest przeskalowane ze względu na priorytet, i dla zadań o niskim priorytecie zwiększa się szybciej. Skoro mamy zachowany porządek BST, to zadanie o najniższym `vruntime`, czyli to, które wybierzemy do uruchomienia, będzie najbardziej oddalonym na lewo wierzchołkiem. Dzięki skalowaniu zmieniania `vruntime`, wystarcza jedno drzewo/CPU. Za to tracimy złożoność O(1) na rzecz $logn$ przy niektórych operacjach. CFS też dba, aby procesy np. po powrocie ze spania na IO dostawały sensowny `vruntime` (jak?).

:::spoiler Drzewo biało-szare
![](https://i.imgur.com/hYdAAID.png)
:::

> [name=Krystian Bacławski] Więcej na temat `vruntime` w 2.6.2 książki o Linuksie.

## Zadanie 7-4

:::warning
Autor: Wiktor Pilarczyk
:::

Stopień interaktywności pomaga nam określić czy wątek jest interaktywny, a co za tym idzie taki wątek jest traktowany priorytetowo - dostaje większy priorytet niż zwykłe procesy. Widać to w [sched_priority](http://bxr.su/FreeBSD/sys/kern/sched_ule.c#sched_priority).

#### Wzór na stopień interaktywności

:::spoiler [sched_interact_score](http://bxr.su/FreeBSD/sys/kern/sched_ule.c#sched_interact_score)
```C
static int
sched_interact_score(struct thread *td)
{
    struct td_sched *ts;
    int div;

    ts = td_get_sched(td);
    
    if (sched_interact <= SCHED_INTERACT_HALF &&
        ts->ts_runtime >= ts->ts_slptime)
            return (SCHED_INTERACT_HALF);

    if (ts->ts_runtime > ts->ts_slptime) {
        div = max(1, ts->ts_runtime / SCHED_INTERACT_HALF);
        return (SCHED_INTERACT_HALF +
            (SCHED_INTERACT_HALF - (ts->ts_slptime / div)));
    }
    if (ts->ts_slptime > ts->ts_runtime) {
        div = max(1, ts->ts_slptime / SCHED_INTERACT_HALF);
        return (ts->ts_runtime / div);
    }
    /* runtime == slptime */
    if (ts->ts_runtime)
        return (SCHED_INTERACT_HALF);

    return (0);
}
```
:::

Jeśli czas snu jest większy niż czas działania
$$Scale  (T_{r} / T_{s})$$
Jeśli czas działania jest większy niż czas snu
$$Scale (2 - T_{s}/T_{r}) $$

#### Aktualizacja trzymania informacji o czasie przebiegu procesu

::: spoiler [sched_interact_update](http://bxr.su/FreeBSD/sys/kern/sched_ule.c#sched_interact_update)
```C
static void
sched_interact_update(struct thread *td)
{
    struct td_sched *ts;
    u_int sum;

    ts = td_get_sched(td);
    sum = ts->ts_runtime + ts->ts_slptime;
    if (sum < SCHED_SLP_RUN_MAX)
        return;
    /*
     * This only happens from two places:
     * 1) We have added an unusual amount of run time from fork_exit.
     * 2) We have added an unusual amount of sleep time from sched_sleep().
     */
    if (sum > SCHED_SLP_RUN_MAX * 2) {
        if (ts->ts_runtime > ts->ts_slptime) {
            ts->ts_runtime = SCHED_SLP_RUN_MAX;
            ts->ts_slptime = 1;
        } else {
            ts->ts_slptime = SCHED_SLP_RUN_MAX;
            ts->ts_runtime = 1;
        }
        return;
    }
    if (sum > (SCHED_SLP_RUN_MAX / 5) * 6) {
        ts->ts_runtime /= 2;
        ts->ts_slptime /= 2;
        return;
    }
    ts->ts_runtime = (ts->ts_runtime / 5) * 4;
    ts->ts_slptime = (ts->ts_slptime / 5) * 4;
}
```
:::

Głównym zadaniem update jest umożliwienie liczenie priorytetu za pomocą tego jak się program zachowywał w przeszłości, a dokładnie na aktualizowaniu czasu ile zadanie spało/pracowało w przeczłości. Funkcja stara się, aby suma czasów była od 0 do SCHED_SLP_RUN_MAX.
Mamy 3 warianty:
1. Suma jest bardzo duża - dwa razy większa od SCHED_SLP_RUN_MAX
    * Większy czas ustawiamy na SCHED_SLP_RUN_MAX, a mniejszy na 1
2. Suma jest większa niż $\frac{6}{5}$ * SCHED_SLP_RUN_MA wtedy dzielimy czasy na dwa (umożliwi to nam powrót do porządanego przedziału)
3. Wpp mnożymy czasy razy $\frac{4}{5}$


## Zadanie 7-5

:::warning
Autor: Michał Opanowicz
:::

::: spoiler [sched_priority](http://bxr.su/FreeBSD/sys/kern/sched_ule.c#sched_priority)
```c=
static void
sched_priority(struct thread *td)
{
	int score;
	int pri;

	if (PRI_BASE(td->td_pri_class) != PRI_TIMESHARE)
		return;
	/*
	 * If the score is interactive we place the thread in the realtime
	 * queue with a priority that is less than kernel and interrupt
	 * priorities.  These threads are not subject to nice restrictions.
	 *
	 * Scores greater than this are placed on the normal timeshare queue
	 * where the priority is partially decided by the most recent cpu
	 * utilization and the rest is decided by nice value.
	 *
	 * The nice value of the process has a linear effect on the calculated
	 * score.  Negative nice values make it easier for a thread to be
	 * considered interactive.
	 */
	score = imax(0, sched_interact_score(td) + td->td_proc->p_nice);
	if (score < sched_interact) {
		pri = PRI_MIN_INTERACT;
		pri += ((PRI_MAX_INTERACT - PRI_MIN_INTERACT + 1) /
		    sched_interact) * score;
		KASSERT(pri >= PRI_MIN_INTERACT && pri <= PRI_MAX_INTERACT,
		    ("sched_priority: invalid interactive priority %d score %d",
		    pri, score));
	} else {
		pri = SCHED_PRI_MIN;
		if (td_get_sched(td)->ts_ticks)
			pri += min(SCHED_PRI_TICKS(td_get_sched(td)),
			    SCHED_PRI_RANGE - 1);
		pri += SCHED_PRI_NICE(td->td_proc->p_nice);
		KASSERT(pri >= PRI_MIN_BATCH && pri <= PRI_MAX_BATCH,
		    ("sched_priority: invalid priority %d: nice %d, " 
		    "ticks %d ftick %d ltick %d tick pri %d",
		    pri, td->td_proc->p_nice, td_get_sched(td)->ts_ticks,
		    td_get_sched(td)->ts_ftick, td_get_sched(td)->ts_ltick,
		    SCHED_PRI_TICKS(td_get_sched(td))));
	}
	sched_user_prio(td, pri);

	return;
}
```

:::

* sprawdzamy, czy wątek jest typu z podziałem czasu
* wyliczamy 'score' interaktywności  (liczymy minimum z zera i score'a interaktywności + `nice`)
* jeśli wątek jest interaktywny (na podstawie 'score'), to
  * ustawiamy jego priorytet na uśredniony priorytet przeznaczony na procesy  interaktywne, przeskalowany liniowo
* w przeciwnym razie, czyli gdy wątek nie jest interaktywny, to
  * ustawiamy jego priorytet na minimum + wartość zależna od tego ile czekał + `nice`


::: spoiler [tdq_runq_add](http://bxr.su/FreeBSD/sys/kern/sched_ule.c#tdq_runq_add)
``` c=
/*
 * Add a thread to the actual run-queue.  Keeps transferable counts up to
 * date with what is actually on the run-queue.  Selects the correct
 * queue position for timeshare threads.
 */
static __inline void
tdq_runq_add(struct tdq *tdq, struct thread *td, int flags)
{
	struct td_sched *ts;
	u_char pri;

	TDQ_LOCK_ASSERT(tdq, MA_OWNED);
	THREAD_LOCK_BLOCKED_ASSERT(td, MA_OWNED);

	pri = td->td_priority;
	ts = td_get_sched(td);
	TD_SET_RUNQ(td);
	if (THREAD_CAN_MIGRATE(td)) {
		tdq->tdq_transferable++;
		ts->ts_flags |= TSF_XFERABLE;
	}
	if (pri < PRI_MIN_BATCH) {
		ts->ts_runq = &tdq->tdq_realtime;
	} else if (pri <= PRI_MAX_BATCH) {
		ts->ts_runq = &tdq->tdq_timeshare;
		KASSERT(pri <= PRI_MAX_BATCH && pri >= PRI_MIN_BATCH,
			("Invalid priority %d on timeshare runq", pri));
		/*
		 * This queue contains only priorities between MIN and MAX
		 * realtime.  Use the whole queue to represent these values.
		 */
		if ((flags & (SRQ_BORROWING|SRQ_PREEMPTED)) == 0) {
			pri = RQ_NQS * (pri - PRI_MIN_BATCH) / PRI_BATCH_RANGE;
			pri = (pri + tdq->tdq_idx) % RQ_NQS;
			/*
			 * This effectively shortens the queue by one so we
			 * can have a one slot difference between idx and
			 * ridx while we wait for threads to drain.
			 */
			if (tdq->tdq_ridx != tdq->tdq_idx &&
			    pri == tdq->tdq_ridx)
				pri = (unsigned char)(pri - 1) % RQ_NQS;
		} else
			pri = tdq->tdq_ridx;
		runq_add_pri(ts->ts_runq, td, pri, flags);
		return;
	} else
		ts->ts_runq = &tdq->tdq_idle;
	runq_add(ts->ts_runq, td, flags);
}


```
:::
Wątki dodawane są na kolejkę zadań 
* real_time (i już też interaktywne), gdy priorytet jest mniejszy niż `PRI_MIN_BATCH`
* z podziałem czasu, gdy priorytet jest w przedziale od `PRI_MIN_BATCH` do `PRI_MAX_BATCH`
* jałowych, w przeciwnym przypadku (priorytet jest dużą liczbą)

Zadania z podziałem czasu dodajemy do kolejki przy pomocy `runq_add_pri` i usuwamy za pomocą `run_choose_from`, bo używamy run_q jako kolejki kalendarzowej, natomiast zadania real_time po prostu dodajemy do run_q na miejsce wskazane przez priorytet.

## Zadanie 7-6

:::warning
Autor: Jakub Urbańczyk
:::

[sched_pickcpu()](http://bxr.su/FreeBSD/sys/kern/sched_ule.c#sched_pickcpu)
Kolejne kryteria według którego wybieramy CPU na którym chcemy uruchomić wątek:
1. Wątki mocno związane z konkretnym CPU nie chcemy przerzucać na inne cpu:
```c=1
    if ((flags & SRQ_OURSELF) || !THREAD_CAN_MIGRATE(td))
        return (ts->ts_cpu);
```
2. Wątki pochodzące od przerwań lepiej jest schedulować na procesorze do którego przyszło to przerwane jeśli piorytet pozwala na natychmiastowe wykonanie. Może być to umotywowane tym, że chcemy szybko obsłużyć procedurę obsługi przerwania i raczej ona długo nie zajmie.
```c=1
	if (td->td_priority <= PRI_MAX_ITHD && THREAD_CAN_SCHED(td, self) &&
	    curthread->td_intr_nesting_level) {
		tdq = TDQ_SELF();
		if (tdq->tdq_lowpri >= PRI_MIN_IDLE) {
			SCHED_STAT_INC(pickcpu_idle_affinity);
			return (self);
		}
		ts->ts_cpu = self;
		intr = 1;
		cg = tdq->tdq_cg;
		goto llc;
	} else {
		intr = 0;
		tdq = TDQ_CPU(ts->ts_cpu);
		cg = tdq->tdq_cg;
	}
```
3. Jeśli cpu na którym ostatnio działał wątek jest idle to go wykorzystujemy. Opłaca się to z tego powodu, że pamięc cache może być wciąż aktualna
```C
#define SCHED_AFFINITY(ts, t)   ((ts)->ts_rltick > ticks - ((t) * affinity))
```
```c=1
    /*
     * If the thread can run on the last cpu and the affinity has not
     * expired and it is idle, run it there.
     */
	if (THREAD_CAN_SCHED(td, ts->ts_cpu) &&
	    tdq->tdq_lowpri >= PRI_MIN_IDLE &&
	    SCHED_AFFINITY(ts, CG_SHARE_L2)) {
		if (cg->cg_flags & CG_FLAG_THREAD) {
			/* Check all SMT threads for being idle. */
			for (cpu = CPU_FFS(&cg->cg_mask) - 1; ; cpu++) {
				if (CPU_ISSET(cpu, &cg->cg_mask) &&
				    TDQ_CPU(cpu)->tdq_lowpri < PRI_MIN_IDLE)
					break;
				if (cpu >= mp_maxid) {
					SCHED_STAT_INC(pickcpu_idle_affinity);
					return (ts->ts_cpu);
				}
			}
		} else {
			SCHED_STAT_INC(pickcpu_idle_affinity);
			return (ts->ts_cpu);
		}
	}
```
4. Patrzymy na inne procesory w kolejności wyznaczonej spokrewnieniem z procesorem, na którym ostatnio wątek działał i jeśli jakiś jest idle to go bierzemy:
```c=1
for (ccg = NULL; cg != NULL; cg = cg->cg_parent) {
		if (cg->cg_flags & CG_FLAG_THREAD)
			continue;
		if (cg->cg_children == 1 || cg->cg_count == 1)
			continue;
		if (cg->cg_level == CG_SHARE_NONE ||
		    (!intr && !SCHED_AFFINITY(ts, cg->cg_level)))
			continue;
		ccg = cg;
	}
	/* Found LLC shared by all CPUs, so do a global search. */
	if (ccg == cpu_top)
		ccg = NULL;
	cpu = -1;
	mask = td->td_cpuset->cs_mask;
	pri = td->td_priority;
	/*
	 * Try hard to keep interrupts within found LLC.  Search the LLC for
	 * the least loaded CPU we can run now.  For NUMA systems it should
	 * be within target domain, and it also reduces scheduling overhead.
	 */
	if (ccg != NULL && intr) {
		cpu = sched_lowest(ccg, mask, pri, INT_MAX, ts->ts_cpu);
		if (cpu >= 0)
			SCHED_STAT_INC(pickcpu_intrbind);
	} else
	/* Search the LLC for the least loaded idle CPU we can run now. */
	if (ccg != NULL) {
		cpu = sched_lowest(ccg, mask, max(pri, PRI_MAX_TIMESHARE),
		    INT_MAX, ts->ts_cpu);
		if (cpu >= 0)
			SCHED_STAT_INC(pickcpu_affinity);
	}
```
5. Patrzymy na wszystkie CPU i bierzemy taki, który jest IDLE i ma najmniejsze obciążenie:
```c=1
	/* Search globally for the least loaded CPU we can run now. */
	if (cpu < 0) {
		cpu = sched_lowest(cpu_top, mask, pri, INT_MAX, ts->ts_cpu);
		if (cpu >= 0)
			SCHED_STAT_INC(pickcpu_lowest);
	}
```
6. Patrzymy na wszystkie CPU i bierzemy taki, który ma namniejsze obciążenie:
```c=1
	if (cpu < 0) {
		cpu = sched_lowest(cpu_top, mask, -1, INT_MAX, ts->ts_cpu);
		if (cpu >= 0)
			SCHED_STAT_INC(pickcpu_lowest);
	}
```
7. Jeśli znalezione cpu nie jest obecnym procesorem to porównujemy go z nami (tzn. z procesorem, który wykonuje ten kod):
```c=1
tdq = TDQ_CPU(cpu);
	if (THREAD_CAN_SCHED(td, self) && TDQ_SELF()->tdq_lowpri > pri &&
	    tdq->tdq_lowpri < PRI_MIN_IDLE &&
	    TDQ_SELF()->tdq_load <= tdq->tdq_load + 1) {
		SCHED_STAT_INC(pickcpu_local);
		cpu = self;
	}
```

Koszt migracji uwzględnia topologię architektury tj. procesory, rdzenie, wątki, pamięć podręczna, pamięć operacyjna.

## Zadanie 7-7

:::warning
Autor: Arkadiusz Kozdra
:::

### Co ile czasu uruchamiana jest procedura [`sched_balance`]?

>The long-term load balancer balances the worst path pair in the hierarchy to avoid socket-, cache-, and chip-level imbalances. It runs from an interrupt handler in a randomized interval of roughly 1 second.
> \~ The Design and Implementation of the FreeBSD ® Operating System

Odstęp nie może być równy, bo mogą się wykonywać jakieś okresowe zadania, które będą zawsze przyłapane w tym samym stanie, zamiast różnie.

### Procedura [`sched_balance_pair`]

```c=918
/*
 * Transfer load between two imbalanced thread queues.
 */
static int
sched_balance_pair(struct tdq *high, struct tdq *low)
{
    struct thread *td;
    int cpu;

    tdq_lock_pair(high, low);
    td = NULL;
    /*
     * Transfer a thread from high to low.
     */
    if (high->tdq_transferable != 0 && high->tdq_load > low->tdq_load &&
        (td = tdq_move(high, low)) != NULL) {
        /*
         * In case the target isn't the current cpu notify it of the
         * new load, possibly sending an IPI to force it to reschedule.
         */
        cpu = TDQ_ID(low);
        if (cpu != PCPU_GET(cpuid))
            tdq_notify(low, td);
    }
    tdq_unlock_pair(high, low);
    return (td != NULL);
}
```

### Ile zadań na raz przenosi procedura [`tdq_move`]?

> Move a thread from one thread queue to another.

Jedno.

### Kiedy [`tdq_notify`] musi wysłać przerwanie międzyprocesorowe do procesora biorcy?

Kiedy `tdq->tdq_owepreempt` jest zerowe (nie wysyłamy dzięki temu dwa razy)
i [`sched_shouldpreempt`] zwróci prawdę (czyli faktycznie należy przerwać zadanie na tamtym procesorze, bo ma niższy priorytet).

[`sched_shouldpreempt`]: http://bxr.su/FreeBSD/sys/kern/sched_ule.c#sched_shouldpreempt
[`sched_balance`]: http://bxr.su/FreeBSD/sys/kern/sched_ule.c#sched_balance
[`sched_balance_pair`]: http://bxr.su/FreeBSD/sys/kern/sched_ule.c#sched_balance_pair
[`tdq_move`]: http://bxr.su/FreeBSD/sys/kern/sched_ule.c#tdq_move
[`tdq_notify`]: http://bxr.su/FreeBSD/sys/kern/sched_ule.c#tdq_notify


### Dyskusja

> [name=Samuel Li] Notatki: https://hackmd.io/@lasamlai/H1HC-N1oI
