/*
74 * Define the MI code needed before returning to user mode, for trap and
75 * syscall.
76 *
77 * We handle "exceptional" events: pending signals, stop/exit actions, etc.
78 * Note that the event must be flagged BEFORE any AST is posted as we are
79 * reading unlocked.
80 */
81static __inline void

82mi_userret(struct lwp *l)
lwp - struktura przechowujaca wiekszosc informacji na temat watku jadra (light weight procces)

83{
84    struct cpu_info *ci;
85

zwieksza counter licznika (nie wywlaszczaj mnie)
86    KPREEMPT_DISABLE(l);

jak sama nazwa wskazuje
87    ci = l->l_cpu;

diagnostyczny assert  cpu_biglock_count;  /* # recursive holds */
88    KASSERTMSG(ci->ci_biglock_count == 0, "kernel_lock leaked");

u_short     l_blcnt;    /* !: count of kernel_lock held */
89    KASSERT(l->l_blcnt == 0);

makro zwiekszajace czytelnosc kodu?
90    if (__predict_false(ci->ci_want_resched)) {
91        preempt();

aktualizacja informacji o procesorze
92        ci = l->l_cpu;
93    }


/*
* Mask indicating that there is "exceptional" work to be done on return to
* user.
*/ LW_USERRET
94#ifdef __HAVE_FAST_SOFTINTS
95    if (__predict_false(l->l_flag & LW_USERRET)) {
96#else
97    if (((l->l_flag & LW_USERRET) | ci->ci_data.cpu_softints) != 0) {
98#endif
99        KPREEMPT_ENABLE(l);
100        lwp_userret(l);
101        KPREEMPT_DISABLE(l);
102        ci = l->l_cpu;
103    }
104    /*
105     * lwp_eprio() is too involved to use here unlocked.  At this point
106     * it only matters for PTHREAD_PRIO_PROTECT; setting a too low value
107     * is OK because the scheduler will find out the true value if we
108     * end up in mi_switch().
109     *
110     * This is being called on every syscall and trap, and remote CPUs
111     * regularly look at ci_schedstate.  Keep the cache line in the
112     * SHARED state by only updating spc_curpriority if it has changed.
113     */
has kernel priority boost
114    l->l_kpriority = false;
115    if (ci->ci_schedstate.spc_curpriority != l->l_priority) {
116        ci->ci_schedstate.spc_curpriority = l->l_priority;
117    }
118    KPREEMPT_ENABLE(l);
119
120    LOCKDEBUG_BARRIER(NULL, 0);
121    KASSERT(l->l_nopreempt == 0);
122    PSREF_DEBUG_BARRIER();
123    KASSERT(l->l_psrefs == 0);
124}
