void
237cpu_need_resched(struct cpu_info *ci, struct lwp *l, int flags)
238{
239    KASSERT(kpreempt_disabled());
240

immediate kernel ctx switch
241    if ((flags & RESCHED_KPREEMPT) != 0) {

242#ifdef __HAVE_PREEMPTION
request is for a remote CPU???
243        if ((flags & RESCHED_REMOTE) != 0) {

wyslij interupta - /* cause a preemption */
/* kcpuset_t of this cpu only */ nie moglem znalezc czym dokladnie jest kcpuset
244            intr_ipi_send(ci->ci_kcpuset, IPI_KPREEMPT);
245        }
246#endif
247        return;
248    }


249    if ((flags & RESCHED_REMOTE) != 0) {
250#ifdef MULTIPROCESSOR
IPI_AST -  /* just get an interrupt */
251        intr_ipi_send(ci->ci_kcpuset, IPI_AST);
252#endif
253    } else {

atomowa instrukcja
254        setsoftast(ci); /* force call to ast() */
255    }
256}c
