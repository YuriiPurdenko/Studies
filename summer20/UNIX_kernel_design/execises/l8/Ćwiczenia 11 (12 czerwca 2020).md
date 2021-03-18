# Ćwiczenia 11 (12 czerwca 2020)

## Zadanie 8-1

:::warning
Autor: Bartosz Szpila
:::

::: spoiler Polecenie
Rozważmy wielowątkowy proces, w którym jeden z wątków wyołał [_exit(2)](https://netbsd.gw.com/cgi-bin/man-cgi?_exit+2+NetBSD-current). Na podstawie implementacji procedury [exit_lwps](http://bxr.su/NetBSD/sys/kern/kern_exit.c#exit_lwps) wyjaśnij uczestniokm zajęć w jaki sposób jądro kończy wykonanie pozostałych wątków w tym procesie. Czy można wyznaczyć górne ograniczenie na czas wykonania wywołania \<\<exit\>\>? Gdzie jeszcze w jądrze wymagane jest użycie procedury \<\<exit_lwps\>\>? 
:::
<br>

::: spoiler Procedura [exit_lwps](http://bxr.su/NetBSD/sys/kern/kern_exit.c#exit_lwps)
``` c=604
void
exit_lwps(struct lwp *l)
{
    proc_t *p = l->l_proc;
    lwp_t *l2;

retry:
    /*
     * Interrupt LWPs in interruptable sleep, unsuspend suspended
     * LWPs and then wait for everyone else to finish.
     */
    LIST_FOREACH(l2, &p->p_lwps, l_sibling) {
        if (l2 == l)
            continue;
        lwp_lock(l2);
        l2->l_flag |= LW_WEXIT;
        if ((l2->l_stat == LSSLEEP && (l2->l_flag & LW_SINTR)) ||
            l2->l_stat == LSSUSPENDED || l2->l_stat == LSSTOP) {
            l2->l_flag &= ~LW_DBGSUSPEND;
                /* setrunnable() will release the lock. */
            setrunnable(l2);
            continue;
        }
        lwp_need_userret(l2);
        lwp_unlock(l2);
    }

    /*
     * Wait for every LWP to exit.  Note: LWPs can get suspended/slept
     * behind us or there may even be new LWPs created.  Therefore, a
     * full retry is required on error.
     */
    while (p->p_nlwps > 1) {
        if (lwp_wait(l, 0, NULL, true)) {
            goto retry;
        }
    }

    KASSERT(p->p_nlwps == 1);
}
```
:::
<br>

Sposób zakończenia pozostałych wątków w procesie polega:
1. zaznaczeniu we flagach statusu wątków flagi LW_EXIT
2. w zależności czy wątek aktualnie jest uruchomiony na innym rdzeniu:
    - jeślie nie (wątek zatrzymany lub uśpiony) - wznowienie wątku, przy powrocie do przestrzeniu użykownika, sprawdzi flagi statusu i znajdzie flagę LW_EXIT
    - jeśli tak - procedurą [lwp_need_userret](http://bxr.su/NetBSD/sys/kern/kern_lwp.c#1688) wysyłamy do wątku sygnał, po jego obsłudze wracając do przestrzeni użytkownika sprawdzi flagi statusu i znajdzie flagę LW_EXIT
3. Czekanie na wyjście procesów, w przypadku błędu zacznij od pkt 1. - szczegóły poniżej (kod)

::: spoiler Obcięta procedura [lwp_wait](http://bxr.su/NetBSD/sys/kern/kern_lwp.c#575)
``` c=568
/*
 * Wait for an LWP within the current process to exit.
 * lid is 0, so we are waiting for all lwps to exit
 * exiting is true
 *
 * Must be called with p->p_lock held.
 */
int
lwp_wait(struct lwp *l, lwpid_t lid, lwpid_t *departed, bool exiting)
{
    const lwpid_t curlid = l->l_lid;
    proc_t *p = l->l_proc;
    lwp_t *l2, *next;
    int error;

    p->p_nlwpwait++;
    l->l_waitingfor = lid;

    for (;;) {
        int nfound;

        /*
         * First off, drain any detached LWP that is waiting to be
         * reaped.
         */
        while ((l2 = p->p_zomblwp) != NULL) {
            p->p_zomblwp = NULL;
            lwp_free(l2, false, false);/* releases proc mutex */
            mutex_enter(p->p_lock);
        }

        /*
         * Now look for an LWP to collect.  If the whole process is
         * exiting, count detached LWPs as eligible to be collected,
         * but don't drain them here.
         */
        nfound = 0;
        error = 0;

        /*
         * W naszym przypadku lid = 0, więc przechodzimy przez wszystkie wątki
         */
        l2 = LIST_FIRST(&p->p_lwps);
        for (; l2 != NULL; l2 = next) {
            next = LIST_NEXT(l2, l_sibling);

            if (l2 == l)
                continue;
            if ((l2->l_prflag & LPR_DETACHED) != 0) {
                nfound += exiting;
                continue;
            }
            
            if (l2->l_waiter != 0) {
                /*
                 * It already has a waiter - so don't
                 * collect it.  If the waiter doesn't
                 * grab it we'll get another chance
                 * later.
                 */
                nfound++;
                continue;
            }
            nfound++;

            /* No need to lock the LWP in order to see LSZOMB. */
            if (l2->l_stat != LSZOMB)
                continue;

            /*
             * We're no longer waiting.  Reset the "first waiter"
             * pointer on the target, in case it was us.
             */
            l->l_waitingfor = 0;
            l2->l_waiter = 0;
            p->p_nlwpwait--;
            sched_lwp_collect(l2);

            /* lwp_free() releases the proc lock. */
            lwp_free(l2, false, false);
            mutex_enter(p->p_lock);
            return 0;
        } /* for(;l2!=NULL;l2=next) */

        if (error != 0)
            break;
        if (nfound == 0) {
            error = ESRCH;
            break;
        }

        /*
         * Note: since the lock will be dropped, need to restart on
         * wakeup to run all LWPs again, e.g. there may be new LWPs.
         */
        if (exiting) {
            KASSERT(p->p_nlwps > 1);
            error = cv_timedwait(&p->p_lwpcv, p->p_lock, 1);
            break;
        }
    }/*for(;;)

    /*
     * We didn't find any LWPs to collect, we may have received a
     * signal, or some other condition has caused us to bail out.
     *
     * If waiting on a specific LWP, clear the waiters marker: some
     * other LWP may want it.  Then, kick all the remaining waiters
     * so that they can re-check for zombies and for deadlock.
     */
    p->p_nlwpwait--;
    l->l_waitingfor = 0;
    cv_broadcast(&p->p_lwpcv);

    return error;
}
```
:::
<br>

Powody dlaczego nie można wyznaczyć górnego ograniczenia na czas wywołania \<\<exit\>\>:
1. W czasie ustawiania flag statusu na LW_EXIT, wątek działający na innym procesorze utworzył nowy wątek - procedura *lwp_wait* zwróci błąd, operacja wyjścia zacznie się od nowa (goto retry)
2. Musimy czekać aż wątki uśpione snem nieprzerywalnym same się obudzą
3. Wątek pomiędzy obudzeniem się a sprawdzeniem statusu flag mógł odebrać sygnał zatrzymujący go
4. Nie możemy ocenić jak szybko wątki będą odbierały sygnał/budziły się i przechodziły do stanu zombie

## Zadanie 8-2

:::warning
Autor: Mikołaj Kowalik
:::
:::spoiler waitpid
The *wait()* function suspends execution of its calling process until
     status information is available for a terminated child process, or a sig-
     nal is received.  On return from a successful wait() call, the status
     area contains termination information about the process that exited as
     defined below.
:::

:::spoiler WXXX
```
/*
78 * POSIX option bits for the "options" argument of wait{3,4,6} wait{,p}id:
79 *
80 * WNOHANG
81 *  Causes the wait to not hang if there are no stopped or terminated
82 *  processes, rather returning an error indication in this case (pid==0).
83 *
84 * WSTOPPED/WUNTRACED
85 *  Indicates that the caller should receive status about untraced
86 *  children which stop due to signals. If children are stopped
87 *  and a wait without this option is done, it is as though they
88 *  were still running, nothing about them is returned.
89 *
90 * WCONTINUED
91 *  Returns information for children that were continued from job
92 *  control.
93 *
94 * WEXITED
95 *  Is the default for wait/wait3/wait4/waitpid (to report children
96 *  that have exited), but needs to be explicitly specified for
97 *  waitid/wait6.
98 *
99 * WNOWAIT
100 *  Returns information about the children without reaping them
101 *  (changing their status to have been already waited for).
102 */
```
:::

```
  struct proc *child, *dead;
   int error;

   KASSERT(mutex_owned(&proc_lock));

   if (options & ~WALLOPTS) {
       *child_p = NULL;
       return EINVAL;
    }

    if ((options & WSELECTOPTS) == 0) {
        /*
         * We will be unable to find any matching processes,
         * because there are no known events to look for.
         * Prefer to return error instead of blocking
         * indefinitely.
         */
        *child_p = NULL;
        return EINVAL;
    }

```
1. Na początek sprawdzamy czy wybraliśmy jakąś opcję, potem robimy jeszcze jeden jakiś sanity-check.
2. sprawdzamy czy chodzi nam o grupę procesów:
```
   if ((pid_t)id == WAIT_MYPGRP && (idtype == P_PID || idtype == P_PGID)) {
1014        mutex_enter(parent->p_lock);
1015        id = (id_t)parent->p_pgid;
1016        mutex_exit(parent->p_lock);
1017        idtype = P_PGID;
1018    }
1019
```
i ustawiamy tutaj poprawne id.

4. FOREACH dziecko:
   sprawdzamy czy dziecko matchuje się do podanych opcji - jeśli nie to nie obchodzi nas -> continue.
```
      LIST_FOREACH(child, &parent->p_children, p_sibling) {
1025            int rv = match_process(parent, &child, idtype, id,
1026                options, wru, si);
1027            if (rv == -1)
1028                break;
1029            if (rv == 0)
1030                continue;
```

5. Czekamy na kończące dzieci - jeśli zmatchowalimiśmy jakieś dziecko (rv=2)
```
1032            /*
1033             * Wait for processes with p_exitsig != SIGCHLD
1034             * processes only if WALTSIG is set; wait for
1035             * processes with p_exitsig == SIGCHLD only
1036             * if WALTSIG is clear.
1037             */
1038            if (((options & WALLSIG) == 0) &&
1039                (options & WALTSIG ? child->p_exitsig == SIGCHLD
1040                        : P_EXITSIG(child) != SIGCHLD)){
1041                if (rv == 2) {
1042                    child = NULL;
1043                    break;
1044                }
1045                continue;
1046            }

```

6. sprawdzamy czy może być zombiak i ewentualnie jeśli nie jest zombie to go zapisujemy na dead:
```
1048            error = 0;
1049            if ((options & WNOZOMBIE) == 0) {
1050                if (child->p_stat == SZOMB)
1051                    break;
1052                if (child->p_stat == SDEAD) {
1053                    /*
1054                     * We may occasionally arrive here
1055                     * after receiving a signal, but
1056                     * immediately before the child
1057                     * process is zombified.  The wait
1058                     * will be short, so avoid returning
1059                     * to userspace.
1060                     */
1061                    dead = child;
1062                }
1063            }
```

7. Czekamy na dzieci, które mają kontynuwać się właśnie kontynuować (jeśli nas obchodzą)
```
1065            if ((options & WCONTINUED) != 0 &&
1066                child->p_xsig == SIGCONT &&
1067                (child->p_sflag & PS_CONTINUED)) {
1068                if ((options & WNOWAIT) == 0) {
1069                    child->p_sflag &= ~PS_CONTINUED;
1070                    child->p_waited = 1;
1071                    parent->p_nstopchild--;
1072                }
1073                if (si) {
1074                    si->si_status = child->p_xsig;
1075                    si->si_code = CLD_CONTINUED;
1076                }
1077                break;
1078            }
```
8. Jeśli obchodzą nas zastopowane dzieci to na nie poczekamy:
```
1080            if ((options & (WTRAPPED|WSTOPPED)) != 0 &&
1081                child->p_stat == SSTOP &&
1082                child->p_waited == 0 &&
1083                ((child->p_slflag & PSL_TRACED) ||
1084                options & (WUNTRACED|WSTOPPED))) {
1085                if ((options & WNOWAIT) == 0) {
1086                    child->p_waited = 1;
1087                    parent->p_nstopchild--;
1088                }
1089                if (si) {
1090                    si->si_status = child->p_xsig;
1091                    si->si_code =
1092                        (child->p_slflag & PSL_TRACED) ?
1093                        CLD_TRAPPED : CLD_STOPPED;
1094                }
1095                break;
1096            }
1097            if (parent->p_nstopchild == 0 || rv == 2) {
1098                child = NULL;
1099                break;
1100            }
```

Poza pętlą czekamy albo zwracamy proces zmatchowany (w zależności od WNOHANG):
```
1103        /*
1104         * If we found nothing, but we are the bereaved parent
1105         * of a stolen child, look and see if that child (or
1106         * one of them) meets our search criteria.   If so, then
1107         * we cannot succeed, but we can hang (wait...),
1108         * or if WNOHANG, return 0 instead of ECHILD
1109         */
1110        if (child == NULL && error == ECHILD &&
1111            (parent->p_slflag & PSL_CHTRACED) &&
1112            debugged_child_exists(idtype, id, options, si, parent))
1113            error = 0;
1114
1115        if (child != NULL || error != 0 ||
1116            ((options & WNOHANG) != 0 && dead == NULL)) {
1117            *child_p = child;
1118            return error;
1119        }
1120
1121        /*
1122         * Wait for another child process to stop.
1123         */
1124        error = cv_wait_sig(&parent->p_waitcv, &proc_lock);
1125
1126        if (error != 0) {
1127            *child_p = NULL;
1128            return error;
1129        }
```

## Zadanie 8-3

:::warning
Autor: Arek Kozdra
:::


### Jaką rolę pełnią w systemach BSD aktywatory obrazów procesów (ang. image activators)?
System rozpoznaje różne typy plików wykonywalnych (ELF, shebang `#!`, a.out, java, ...).
Każdy format i podformat jest opisany przez odpowiednią strukturę.
Dodatkowe formaty można ładować np. w FreeBSD z [binmiscctl(8)].

[binmiscctl(8)]: http://mdoc.su/f/8/binmiscctl

### Jakie jest zadanie procedur należących do [`exec_elf64_execsw`]?
Rozpoznanie typu pliku, przygotowanie listy poleceń pamięci do załadowania, zarządzanie pamięcią specyficzne dla formatu wykonywalnego.

### Przeprawa przez [`exec_elf_makecmds`].

Śliczny obrazek: 

[![ELF101](https://raw.githubusercontent.com/corkami/pics/master/binary/ELF101.png)](https://raw.githubusercontent.com/corkami/pics/master/binary/ELF101.png)


:::spoiler exec_elf_makecmds
```c
701    error = exec_read(l, epp->ep_vp, eh->e_phoff, ph, phsize,
702        IO_NODELOCKED);

711    for (i = 0; i < eh->e_phnum; i++) {
712        pp = &ph[i];
713        if (pp->p_type == PT_INTERP) {
714            if (pp->p_filesz < 2 || pp->p_filesz > MAXPATHLEN) {
715                DPRINTF("bad interpreter namelen %#jx",
716                    (uintmax_t)pp->p_filesz);
717                error = ENOEXEC;
718                VOP_UNLOCK(epp->ep_vp);
719                goto bad;
720            }
721            interp = PNBUF_GET();
722            error = exec_read(l, epp->ep_vp, pp->p_offset, interp,
723                pp->p_filesz, IO_NODELOCKED);
724            if (error != 0) {
725                VOP_UNLOCK(epp->ep_vp);
726                goto bad;
727            }
728            /* Ensure interp is NUL-terminated and of the expected length */
729            if (strnlen(interp, pp->p_filesz) != pp->p_filesz - 1) {
730                DPRINTF("bad interpreter name");
731                error = ENOEXEC;
732                VOP_UNLOCK(epp->ep_vp);
733                goto bad;
734            }
735            break;
736        }
737    }

765    /*
766     * Load all the necessary sections
767     */
768    for (i = 0; i < eh->e_phnum; i++) {
769        Elf_Addr addr = ELFDEFNNAME(NO_ADDR);
770        u_long size = 0;
771
772        switch (ph[i].p_type) {
773        case PT_LOAD:
774            if ((error = elf_load_psection(&epp->ep_vmcmds,
775                epp->ep_vp, &ph[i], &addr, &size, VMCMD_FIXED))
776                != 0) {
777                VOP_UNLOCK(epp->ep_vp);
778                goto bad;
779            }

797            if (ph[i].p_offset == 0) {
798                computed_phdr = ph[i].p_vaddr + eh->e_phoff;
799            }
800            break;
801

809        case PT_PHDR:
810            /* Note address of program headers (in text segment) */
811            phdr = ph[i].p_vaddr;
812            break;

819        }
820    }

838    /*
839     * Check if we found a dynamically linked binary and arrange to load
840     * its interpreter
841     */
842    if (interp) {
843        u_int nused = epp->ep_vmcmds.evs_used;
844        u_long interp_offset = 0;
845
846        if ((error = elf_load_interp(l, epp, interp,
847            &epp->ep_vmcmds, &interp_offset, &pos)) != 0) {
848            goto bad;
849        }

857        ap = kmem_alloc(sizeof(*ap), KM_SLEEP);
858        ap->arg_interp = epp->ep_vmcmds.evs_cmds[nused].ev_addr;
859        epp->ep_entryoffset = interp_offset;
860        epp->ep_entry = ap->arg_interp + interp_offset;

863    } else {

871    }
872
873    if (ap) {
874        ap->arg_phaddr = phdr ? phdr : computed_phdr;
875        ap->arg_phentsize = eh->e_phentsize;
876        ap->arg_phnum = eh->e_phnum;
877        ap->arg_entry = eh->e_entry;
878        epp->ep_emul_arg = ap;
879        epp->ep_emul_arg_free = elf_free_emul_arg;
880    }
```
:::
:::spoiler elf_load_psection
W zależności od rozmiaru używa [`vmcmd_map_pagedvn`] lub [`vmcmd_map_readvn`] (ewentualnie vmcmd_map_zero).

[`vmcmd_map_pagedvn`]: http://bxr.su/NetBSD/sys/kern/exec_subr.c#vmcmd_map_pagedvn
[`vmcmd_map_readvn`]: http://bxr.su/NetBSD/sys/kern/exec_subr.c#vmcmd_map_readvn

```c
341    if (ph->p_align > 1) {
342        /*
343         * Make sure we are virtually aligned as we are supposed to be.
344         */
345        diff = ph->p_vaddr - ELF_TRUNC(ph->p_vaddr, ph->p_align);

357    } else
358        diff = 0;
359
360    vmprot |= (ph->p_flags & PF_R) ? VM_PROT_READ : 0;
361    vmprot |= (ph->p_flags & PF_W) ? VM_PROT_WRITE : 0;
362    vmprot |= (ph->p_flags & PF_X) ? VM_PROT_EXECUTE : 0;
363
364    /*
365     * Adjust everything so it all starts on a page boundary.
366     */
367    *addr -= diff;
368    offset = ph->p_offset - diff;
369    *size = ph->p_filesz + diff;
370    msize = ph->p_memsz + diff;
371
372    if (ph->p_align >= PAGE_SIZE) {
373        if ((ph->p_flags & PF_W) != 0) {
374            /*
375             * Because the pagedvn pager can't handle zero fill
376             * of the last data page if it's not page aligned we
377             * map the last page readvn.
378             */
379            psize = trunc_page(*size);
380        } else {
381            psize = round_page(*size);
382        }
383    } else {
384        psize = *size;
385    }
386
387    if (psize > 0) {
388        NEW_VMCMD2(vcset, ph->p_align < PAGE_SIZE ?
389            vmcmd_map_readvn : vmcmd_map_pagedvn, psize, *addr, vp,
390            offset, vmprot, flags);
391        flags &= VMCMD_RELATIVE;
392    }
393    if (psize < *size) {
394        NEW_VMCMD2(vcset, vmcmd_map_readvn, *size - psize,
395            *addr + psize, vp, offset + psize, vmprot, flags);
396    }
397
398    /*
399     * Check if we need to extend the size of the segment (does
400     * bss extend page the next page boundary)?
401     */
402    rm = round_page(*addr + msize);
403    rf = round_page(*addr + *size);
404
405    if (rm != rf) {
406        NEW_VMCMD2(vcset, vmcmd_map_zero, rm - rf, rf, NULLVP,
407            0, vmprot, flags & VMCMD_RELATIVE);
408        *size = msize;
409    }
```
:::
:::spoiler elf_load_interp
```c
445    /*
446     * 1. open file
447     * 2. read filehdr
448     * 3. map text, data, and bss out of it using VM_*
449     */
450    vp = epp->ep_interp;
451    if (vp == NULL) {
452        error = emul_find_interp(l, epp, path);
453        if (error != 0)
454            return error;
455        vp = epp->ep_interp;
456    }

489    error = vn_marktext(vp);
490    if (error)
491        goto bad;
492
493    error = exec_read(l, vp, 0, &eh, sizeof(eh), IO_NODELOCKED);
494    if (error != 0)
495        goto bad;

499    if (eh.e_type != ET_DYN || eh.e_phnum == 0) {
500        DPRINTF("bad interpreter type %#x", eh.e_type);
501        error = ENOEXEC;
502        goto bad;
503    }
504
505    phsize = eh.e_phnum * sizeof(Elf_Phdr);
506    ph = kmem_alloc(phsize, KM_SLEEP);
507
508    error = exec_read(l, vp, eh.e_phoff, ph, phsize, IO_NODELOCKED);
509    if (error != 0)
510        goto bad;

524    /*
525     * If no position to load the interpreter was set by a probe
526     * function, pick the same address that a non-fixed mmap(0, ..)
527     * would (i.e. something safely out of the way).
528     */
529    if (*last == ELFDEFNNAME(NO_ADDR)) {
530        u_long limit = 0;
531        /*
532         * Find the start and ending addresses of the psections to
533         * be loaded.  This will give us the size.
534         */
535        for (i = 0, base_ph = NULL; i < eh.e_phnum; i++) {
536            if (ph[i].p_type == PT_LOAD) {
537                u_long psize = ph[i].p_vaddr + ph[i].p_memsz;
538                if (base_ph == NULL)
539                    base_ph = &ph[i];
540                if (psize > limit)
541                    limit = psize;
542            }
543        }

551        /*
552         * Now compute the size and load address.
553         */
554        addr = (*epp->ep_esch->es_emul->e_vm_default_addr)(p,
555            epp->ep_daddr,
556            round_page(limit) - trunc_page(base_ph->p_vaddr),
557            use_topdown);
558        addr += (Elf_Addr)pax_aslr_rtld_offset(epp, base_ph->p_align,
559            use_topdown);
560    } else {
561        addr = *last; /* may be ELF_LINK_ADDR */
562    }
563
564    /*
565     * Load all the necessary sections
566     */
567    for (i = 0, base_ph = NULL, last_ph = NULL; i < eh.e_phnum; i++) {
568        switch (ph[i].p_type) {
569        case PT_LOAD: {
570            u_long size;
571            int flags;
572
573            if (base_ph == NULL) {
574                /*
575                 * First encountered psection is always the
576                 * base psection.  Make sure it's aligned
577                 * properly (align down for topdown and align
578                 * upwards for not topdown).
579                 */
580                base_ph = &ph[i];
581                flags = VMCMD_BASE;
582                if (addr == ELF_LINK_ADDR)
583                    addr = ph[i].p_vaddr;
584                if (use_topdown)
585                    addr = ELF_TRUNC(addr, ph[i].p_align);
586                else
587                    addr = ELF_ROUND(addr, ph[i].p_align);
588            } else {
589                u_long limit = round_page(last_ph->p_vaddr
590                    + last_ph->p_memsz);
591                u_long base = trunc_page(ph[i].p_vaddr);
592
593                /*
594                 * If there is a gap in between the psections,
595                 * map it as inaccessible so nothing else
596                 * mmap'ed will be placed there.
597                 */
598                if (limit != base) {
599                    NEW_VMCMD2(vcset, vmcmd_map_zero,
600                        base - limit,
601                        limit - base_ph->p_vaddr, NULLVP,
602                        0, VM_PROT_NONE, VMCMD_RELATIVE);
603                }
604
605                addr = ph[i].p_vaddr - base_ph->p_vaddr;
606                flags = VMCMD_RELATIVE;
607            }
608            last_ph = &ph[i];
609            if ((error = elf_load_psection(vcset, vp, &ph[i], &addr,
610                &size, flags)) != 0)
611                goto bad;
612            /*
613             * If entry is within this psection then this
614             * must contain the .text section.  *entryoff is
615             * relative to the base psection.
616             */
617            if (eh.e_entry >= ph[i].p_vaddr &&
618                eh.e_entry < (ph[i].p_vaddr + size)) {
619                *entryoff = eh.e_entry - base_ph->p_vaddr;
620            }
621            addr += size;
622            break;
623        }

627        }
628    }
```
:::
:::spoiler execve_dovmcmds
[`execve_dovmcmds`]
```c
949    for (i = 0; i < epp->ep_vmcmds.evs_used && !error; i++) {
950        struct exec_vmcmd *vcp;
951
952        vcp = &epp->ep_vmcmds.evs_cmds[i];
953        if (vcp->ev_flags & VMCMD_RELATIVE) {
954            KASSERTMSG(base_vcp != NULL,
955                "%s: relative vmcmd with no base", __func__);
956            KASSERTMSG((vcp->ev_flags & VMCMD_BASE) == 0,
957                "%s: illegal base & relative vmcmd", __func__);
958            vcp->ev_addr += base_vcp->ev_addr;
959        }
960        error = (*vcp->ev_proc)(l, vcp);
961        if (error)
962            DUMPVMCMDS(epp, i, error);
963        if (vcp->ev_flags & VMCMD_BASE)
964            base_vcp = vcp;
965    }
```
:::

[`exec_elf64_execsw`]: http://bxr.su/NetBSD/sys/kern/exec_elf64.c#exec_elf64_execsw
[`exec_elf_makecmds`]: http://bxr.su/NetBSD/sys/kern/exec_elf.c#exec_elf_makecmds
[`execve_dovmcmds`]: http://bxr.su/NetBSD/sys/kern/kern_exec.c#execve_dovmcmds
[`exec_vmcmd`]: http://bxr.su/NetBSD/sys/sys/exec.h#exec_vmcmd


> [name=pj] [mądry opis](https://www.netbsd.org/docs/internals/en/chap-processes.html)
> [name=Mateusz Maciejewski] najważniejszy(?) fragment powyższego [![](https://i.imgur.com/o6T29pD.png)](https://www.netbsd.org/docs/internals/en/chap-processes.html#table-execsw-fields)

## Zadanie 8-4

:::warning
Autor: Michał Błaszczyk
:::

**[Właściwości sygnałów uniksowych](http://bxr.su/NetBSD/sys/sys/signalvar.h#113)**

```c
/*
* Signal properties and actions.
* The array below categorizes the signals and their default actions
* according to the following properties:
*/
#define SA_KILL     0x0001      /* terminates process by default */
#define SA_CORE     0x0002      /* ditto and coredumps */
#define SA_STOP     0x0004      /* suspend process */
#define SA_TTYSTOP  0x0008      /* ditto, from tty */
#define SA_IGNORE   0x0010      /* ignore by default */
#define SA_CONT     0x0020      /* continue if suspended */
#define SA_CANTMASK 0x0040      /* non-maskable, catchable */
#define SA_NORESET  0x0080      /* not reset when caught */
#define SA_TOLWP    0x0100      /* to LWP that generated, if local */
#define SA_TOALL    0x0200      /* always to all LWPs */
```

**[sigprop](http://bxr.su/NetBSD/sys/sys/signalvar.h#254)**

- **SIGTRAP** - SA_KILL | SA_CORE | SA_NORESET | SA_TOLWP
- **SIGKILL** - SA_KILL | SA_CANTMASK | SA_TOALL
- **SIGSEGV** - SA_KILL | SA_CORE | SA_TOLWP
- **SIGSTOP** - SA_STOP | SA_CANTMASK | SA_TOALL
- **SIGCONT** - SA_IGNORE | SA_CONT | SA_TOALL

**[kpsignal2()](http://bxr.su/NetBSD/sys/kern/kern_sig.c#1297)**

Scenariusz: sterownik terminala wczytał znak «Ctrl+C» i w wyniku jego przetwarzania zdecydował wysłać sygnał SIGINT do grupy pierwszoplanowej przy pomocy kpgsignal(9). Wielowątkowy proces, do którego wysyłamy sygnał, nie jest śledzony, jest aktywny, nie blokuje sygnału i posiada zarejestrowaną procedurę obsługi.

1. Ustalamy numer sygnału.
2. Jeżeli docelowy proces jest w fazie tworzenia przez fork, jest zombiakiem, lub wychodzi, wówczas kończymy w tym miejscu i zwracamy 0.
3. Sprawdzamy czy sygnał jest adresowany do konkretnego wątku, czy do wszystkich wątków w procesie.
4. Jeżeli sygnał jest ignorowany, wówczas odrzucamy taki sygnał.
5. Jeżeli sygnał jest łapany ustawiamy zmienną **action** na **SIG_CATCH**.
6. W p.p. ustawiamy action na **SIG_DFL**.
7. Jeżeli zatrzymujemy lub kontynuujemy proces, wówczas odrzucamy wszystkie oczekujące sygnały, których akcja ma efekt odwrotny.
8. Obsługujemy przypadek sygnału maskowalnego na który czekają jakieś wątki procesu.
9. Alokujemy pamięć na strukturę [ksiginfo_t](http://bxr.su/NetBSD/sys/sys/siginfo.h#99) (będzie ona kopią argumentu ksi).
10. Jeżeli sygnał jest adresowany do konkretnego wątku, wówczas:
    - ustalamy wskaźnik na ten wątek,
    - dodajemy nową (stworzoną w poprzednim kroku) strukturę ksiginfo_t do listy oczekujący struktur tego typu dla danego wątku,
    - oznaczamy sygnał jako oczekujący,
    - zwalniamy zaalokowaną pamięć i wychodzimy z procedury.
11. Jeżeli sygnał nie jest adresowany do wszystkich wątków i argument ksi nie identyfikuje konkretnego wątku, wówczas:
    - dodajemy nową (stworzoną wcześniej) strukturę ksiginfo_t do listy oczekujący struktur tego typu dla danego procesu,
    - próbujemy znaleźć wątek mogący obsłużyć sygnał,
    - oznaczamy sygnał jako oczekujący (dla znalezionego właśnie wątku),
    - zwalniamy zaalokowaną pamięć i wychodzimy z procedury.

## Dyskusja

> [name=Samuel Li] Notatki: https://hackmd.io/@lasamlai/SkGSeRyTL
