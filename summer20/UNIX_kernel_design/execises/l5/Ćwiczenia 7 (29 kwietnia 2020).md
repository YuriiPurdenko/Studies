# Ćwiczenia 7 (29 kwietnia 2020)

## Zadanie 5-3

:::warning
Autor: Michał Błaszczyk
:::

**Restartowanie wywołań systemowych**

Restart syscalla to ponowne wykonanie tego syscalla w przypadku przerwania poprzedniego wywołania złapaniem sygnału.

Następujące wywołania systemowe są wrażliwe na restarty:
- open, read, write, sendto, recvfrom, sendmsg, oraz recvmsg na kanałach komunikacji lub "wolnych" urządzeniach
- wait oraz ioctl

**[syscall()](http://bxr.su/NetBSD/sys/arch/aarch64/aarch64/syscall.c#90)**
1. Ustalamy bieżący wątek oraz proces powiązany z tym wątkiem.
2. Ustalamy offset wywołania systemowego (odczytany z [ramki stosu](http://bxr.su/NetBSD/sys/arch/aarch64/include/frame.h#39)) oraz pozycję argumentów wywołania systemowego (tych przekazywanych w rejestrach).
3. Jeśli dane wywołanie systemowe wymaga więcej argumentów niż można dostarczyć w rejestrach (> NARGREG), wówczas ustalamy resztę argumentów z ramki sostu (do 10).
4. Jeśli syscall używa innego rozmiaru rejestrów, wówczas konstruujemy argumenty 64-bitowe.
5. Wywołujemy funkcję sy_invoke (wykonanie odpowiedniego wywołania systemowego).
6. Jeżeli wywołanie powiodło się (error == 0), wówczas wartość zwracaną syscalla przekazujemy przez pierwsze dwa rejestry w ramce stosu, oraz zerowana jest flaga **carry** w zapisanym rejestrze stanu.
7. Jeżeli wystąpił błąd o kodzie **ERESTART**, wówczas cofamy zapisany wskaźnik programu tak, aby syscall został wywołany ponownie.
8. Jeżeli kod będu jet różny od ERESTART i EJUSTRETURN, wówczas w pierwszym rejestrze w ramce stosu zapisywana jest wartość **errno** odpowiadająca danemu kodowi błędu oraz zapalany jest bit carry w zapisanym rejestrze stanu.
9. Wywoływana jest funkcja userret().

## Zadanie 5-4

:::warning
Autor: Mateusz Maciejewski
:::

Głównym zadaniem procedury obsługi pułapki jest identyfikacja zdarzenia i delegacja obsługi dalej, na przykład tłumacząc pułapkę na sygnał.

:::spoiler
```c
249 void
250 trap_el0_sync(struct trapframe *tf)
251{
252    struct lwp * const l = curlwp;
// ESR = Exception Syndrome Register
253    const uint32_t esr = tf->tf_esr;
254    const uint32_t eclass = __SHIFTOUT(esr, ESR_EC); /* exception class */
255
256    /* disable trace */
257    reg_mdscr_el1_write(reg_mdscr_el1_read() & ~MDSCR_SS);
258    /* enable traps and interrupts */
259    daif_enable(DAIF_D|DAIF_A|DAIF_I|DAIF_F);
260
261    switch (eclass) {
// Abort oznacza błąd przy dostępie do pamięci danych lub instrukcji
262    case ESR_EC_INSN_ABT_EL0: // AXX: Instruction Abort (EL0)
263    case ESR_EC_DATA_ABT_EL0: // AXX: Data Abort (EL0)
264        data_abort_handler(tf, eclass);
265        userret(l);
266        break;
267
268    case ESR_EC_SVC_A64: // A64: SVC Instruction Execution (supervisor calls = syscall)
269        (*l->l_proc->p_md.md_syscall)(tf);
270        break;
271    case ESR_EC_FP_ACCESS: // AXX: Access to SIMD/FP Registers
272        fpu_load(l);
273        userret(l);
274        break;
275    case ESR_EC_FP_TRAP_A64: // A64: FP Exception
276        do_trapsignal(l, SIGFPE, FPE_FLTUND, NULL, esr); /* XXX */
277        userret(l);
278        break;
279
280    case ESR_EC_PC_ALIGNMENT: // AXX: Misaligned PC
281        do_trapsignal(l, SIGBUS, BUS_ADRALN, (void *)tf->tf_pc, esr);
282        userret(l);
283        break;
284    case ESR_EC_SP_ALIGNMENT: // AXX: Misaligned SP
285        do_trapsignal(l, SIGBUS, BUS_ADRALN, (void *)tf->tf_sp, esr);
286        userret(l);
287        break;
288
289    case ESR_EC_BKPT_INSN_A64: // A64: BKPT (=breakpoint) Instruction Execution
290    case ESR_EC_BRKPNT_EL0: // AXX: Breakpoint Exception (EL0)
291    case ESR_EC_WTCHPNT_EL0: // AXX: Watchpoint (EL0) 
292        do_trapsignal(l, SIGTRAP, TRAP_BRKPT, (void *)tf->tf_pc, esr);
293        userret(l);
294        break;
295    case ESR_EC_SW_STEP_EL0: // AXX: Software Step (EL0) 
296        /* disable trace, and send trace trap */
297        tf->tf_spsr &= ~SPSR_SS;
298        do_trapsignal(l, SIGTRAP, TRAP_TRACE, (void *)tf->tf_pc, esr);
299        userret(l);
300        break;
301
302    default:
303    case ESR_EC_UNKNOWN:
315        /* illegal or not implemented instruction */
316        do_trapsignal(l, SIGILL, ILL_ILLTRP, (void *)tf->tf_pc, esr);
317        userret(l);
318        break;
319    }
320}
```
:::

Implementacja [`data_abort_handler`](http://bxr.su/NetBSD/sys/arch/aarch64/aarch64/fault.c#127):
::: spoiler
```c
126 void
127 data_abort_handler(struct trapframe *tf, uint32_t eclass)
128{
129    struct proc *p;
130    struct lwp *l;
131    struct vm_map *map;
132    struct faultbuf *fb;
133    vaddr_t va;
134    uint32_t esr, fsc, rw;
135    vm_prot_t ftype;
136    int error = 0, len;
137    const bool user = IS_SPSR_USER(tf->tf_spsr) ? true : false;
138
139    bool fatalabort;
140    const char *faultstr;
141    static char panicinfo[256];
142
147
148    l = curlwp;
149
150    esr = tf->tf_esr;
151    rw = __SHIFTOUT(esr, ESR_ISS_DATAABORT_WnR); /* 0 if IFSC */
152
153    fatalabort = is_fatal_abort(esr);
154    if (fatalabort)
155        goto do_fault;
156
157    p = l->l_proc;
158    va = trunc_page((vaddr_t)tf->tf_far);
159
160    /* eliminate addresss tag if ECR_EL1.TBI[01] is enabled */
161    va = aarch64_untag_address(va);
162
 // Sprawdzenie w której przestrzeni znajduje się adres
163    if ((VM_MIN_KERNEL_ADDRESS <= va) && (va < VM_MAX_KERNEL_ADDRESS)) {
164        map = kernel_map;
166    } else if (VM_MIN_ADDRESS <= va && va <= VM_MAX_ADDRESS) {
167        map = &p->p_vmspace->vm_map;
170    } else {
171        error = EINVAL;
172        goto do_fault;
173    }
174
// Która ochrona pamięci została naruszona? (X/R/W)? 
175    if ((eclass == ESR_EC_INSN_ABT_EL0) || (eclass == ESR_EC_INSN_ABT_EL1))
176        ftype = VM_PROT_EXECUTE;
177    else if (__SHIFTOUT(esr, ESR_ISS_DATAABORT_CM))
178        ftype = VM_PROT_READ;
179    else
180        ftype = (rw == 0) ? VM_PROT_READ : VM_PROT_WRITE;
181
192
198
199    fb = cpu_disable_onfault();
200    error = uvm_fault(map, va, ftype);
201    cpu_enable_onfault(fb);
202    if (__predict_true(error == 0)) {
203        if (user)
204            uvm_grow(p, va);
205
208        return;
209    }
210
211
212 do_fault:
213    /* faultbail path? */
214    fb = cpu_disable_onfault();
215    if (fb != NULL) {
216        cpu_jump_onfault(tf, fb, EFAULT);
217        return;
218    }
219
220    fsc = __SHIFTOUT(esr, ESR_ISS_DATAABORT_DFSC); /* also IFSC */
221    if (user) {
222        if (!fatalabort) {
223            switch (error) {
224            case ENOMEM:
225                printf("UVM: pid %d (%s), uid %d killed: "
226                    "out of swap\n",
227                    l->l_proc->p_pid, l->l_proc->p_comm,
228                    l->l_cred ?
229                    kauth_cred_geteuid(l->l_cred) : -1);
230                do_trapsignal(l, SIGKILL, 0,
231                    (void *)tf->tf_far, esr);
232                break;
233            case EACCES:
234                do_trapsignal(l, SIGSEGV, SEGV_ACCERR,
235                    (void *)tf->tf_far, esr);
236                break;
237            case EINVAL:
238                do_trapsignal(l, SIGBUS, BUS_ADRERR,
239                    (void *)tf->tf_far, esr);
240                break;
241            default:
242                do_trapsignal(l, SIGSEGV, SEGV_MAPERR,
243                    (void *)tf->tf_far, esr);
244                break;
245            }
246        } else {
247            /*
248             * fatal abort in usermode
249             */
250            switch (fsc) {
251            case ESR_ISS_FSC_TLB_CONFLICT_FAULT:
252            case ESR_ISS_FSC_LOCKDOWN_ABORT:
253            case ESR_ISS_FSC_UNSUPPORTED_EXCLUSIVE:
254            case ESR_ISS_FSC_FIRST_LEVEL_DOMAIN_FAULT:
255            case ESR_ISS_FSC_SECOND_LEVEL_DOMAIN_FAULT:
256            default:
257                do_trapsignal(l, SIGSEGV, SEGV_MAPERR,
258                    (void *)tf->tf_far, esr);
259                break;
260            case ESR_ISS_FSC_ADDRESS_SIZE_FAULT_0:
261            case ESR_ISS_FSC_ADDRESS_SIZE_FAULT_1:
262            case ESR_ISS_FSC_ADDRESS_SIZE_FAULT_2:
263            case ESR_ISS_FSC_ADDRESS_SIZE_FAULT_3:
264                do_trapsignal(l, SIGBUS, BUS_ADRERR,
265                    (void *)tf->tf_far, esr);
266                break;
267            case ESR_ISS_FSC_SYNC_EXTERNAL_ABORT:
268            case ESR_ISS_FSC_SYNC_EXTERNAL_ABORT_TTWALK_0:
269            case ESR_ISS_FSC_SYNC_EXTERNAL_ABORT_TTWALK_1:
270            case ESR_ISS_FSC_SYNC_EXTERNAL_ABORT_TTWALK_2:
271            case ESR_ISS_FSC_SYNC_EXTERNAL_ABORT_TTWALK_3:
272            case ESR_ISS_FSC_SYNC_PARITY_ERROR:
273            case ESR_ISS_FSC_SYNC_PARITY_ERROR_ON_TTWALK_0:
274            case ESR_ISS_FSC_SYNC_PARITY_ERROR_ON_TTWALK_1:
275            case ESR_ISS_FSC_SYNC_PARITY_ERROR_ON_TTWALK_2:
276            case ESR_ISS_FSC_SYNC_PARITY_ERROR_ON_TTWALK_3:
277                do_trapsignal(l, SIGBUS, BUS_OBJERR,
278                    (void *)tf->tf_far, esr);
279                break;
280            case ESR_ISS_FSC_ALIGNMENT_FAULT:
281                do_trapsignal(l, SIGBUS, BUS_ADRALN,
282                    (void *)tf->tf_far, esr);
283                break;
284            }
285        }
286
294        return;
296    }
357}
358
```
:::
Wywołanie `do_trapsignal` generuje sygnał dla wątku `l` o ustalonym typie (numerze) i powodzie. Parametry bezpośrednio odpowiadają informacjom do których ma dostęp użytkownik poprzez strukturę [`siginfo`](https://netbsd.gw.com/cgi-bin/man-cgi?siginfo+2+NetBSD-current), to znaczy numer sygnału (`int si_signo`) oraz jego powód (`si_code`).


## Zadanie 5-5

:::warning
Autor: Patrycja Balik
:::

> Załóżmy, że do procedury [`copyout(9)`](https://netbsd.gw.com/cgi-bin/man-cgi?copyout+9+NetBSD-current) przekazano adres, pod który nie odwzorowano pamięci. Przeprowadź uczestników zajęć od początku procedury [`copyout`](http://bxr.su/FreeBSD/sys/riscv/riscv/copyinout.S#copyout) aż do [`copyio_fault_nopcb`](http://bxr.su/FreeBSD/sys/riscv/riscv/copyinout.S#copyio_fault_nopcb), w którym zwróci ona «`EFAULT`». Odpowiednia procedura obsługi pułapki nazywa się [`data_abort`](http://bxr.su/FreeBSD/sys/riscv/riscv/trap.c#data_abort).
> 
> **Wskazówka:** O tym co się dzieje można myśleć, jak o nielokalnych skokach przy użyciu procedury obsługi pułapki.

Sygnatura `copyout` to:
```c
copyout(const void *kaddr, void *uaddr, size_t len);
```

Podczas gdy implementacja, w asemblerze, wygląda następująco:
```=117
ENTRY(copyout)
    beqz    a2, copyout_end /* If len == 0 then skip loop */
    add a3, a1, a2
    li  a4, VM_MAXUSER_ADDRESS
    bgt a3, a4, copyio_fault_nopcb /* Czemu porównanie ze znakiem? */

    copycommon

copyout_end:
    li  a0, 0       /* return 0 */
    ret
END(copyout)
```

Najpierw odrobinę ABI RISC-V:

| Rejestr | ABI  | Zastosowanie                  |
|---------|------|-------------------------------|
| x10–11  | a0–1 | Argumenty i zwracane wartości |
| x12–17  | a2–7 | Argumenty                     |

Może się zdarzyć, że `uaddr + len` (liczony w linii 119) jest dalej, niż maksymalny adres pamięci wirtualnej użytkownika. Wtedy od razu skaczemy do `copyio_fault_nopcb` i zwracamy `EFAULT`.

```=50
copyio_fault_nopcb:
    li  a0, EFAULT
    ret
```

Druga opcja jest taka, że `uaddr` jest odrobinę bardziej rozsądny. Musimy rozwinąć makro [`copycommon`](copyinout).

:::spoiler **Makro `copycommon`**
```=55
/*
 * copycommon - common copy routine
 *
 * a0 - Source address
 * a1 - Destination address
 * a2 - Size of copy
 */
    .macro copycommon
    la  a6, copyio_fault    /* Get the handler address */
    SET_FAULT_HANDLER(a6, a7)   /* Set the handler */
    ENTER_USER_ACCESS(a7)

    li  t2, XLEN_BYTES
    blt a2, t2, 4f      /* Byte-copy if len < XLEN_BYTES */

    /*
     * Compare lower bits of src and dest.
     * If they are aligned with each other, we can do word copy.
     */
    andi    t0, a0, (XLEN_BYTES-1)  /* Low bits of src */
    andi    t1, a1, (XLEN_BYTES-1)  /* Low bits of dest */
    bne t0, t1, 4f      /* Misaligned. Go to byte copy */
    beqz    t0, 2f          /* Already word-aligned, skip ahead */

    /* Byte copy until the first word-aligned address */
1:  lb  a4, 0(a0)       /* Load byte from src */
    addi    a0, a0, 1
    sb  a4, 0(a1)       /* Store byte in dest */
    addi    a1, a1, 1
    addi    a2, a2, -1      /* len-- */
    andi    t0, a0, (XLEN_BYTES-1)
    bnez    t0, 1b
    j   3f

    /* Copy words */
2:  ld  a4, 0(a0)       /* Load word from src */
    addi    a0, a0, XLEN_BYTES
    sd  a4, 0(a1)       /* Store word in dest */
    addi    a1, a1, XLEN_BYTES
    addi    a2, a2, -XLEN_BYTES /* len -= XLEN_BYTES */
3:  bgeu    a2, t2, 2b      /* Again if len >= XLEN_BYTES */

    /* Check if we're finished */
    beqz    a2, 5f

    /* Copy any remaining bytes */
4:  lb  a4, 0(a0)       /* Load byte from src */
    addi    a0, a0, 1
    sb  a4, 0(a1)       /* Store byte in dest */
    addi    a1, a1, 1
    addi    a2, a2, -1      /* len-- */
    bnez    a2, 4b

5:  EXIT_USER_ACCESS(a7)
    SET_FAULT_HANDLER(x0, a7)   /* Clear the handler */
    .endm
```
:::

Kilka pojęć w RISC-V:
* CSR -- Control & Status Register.
* `csrs` -- CSR set.
* `csrc` -- CSR clear.
* `sstatus` -- Supervisor status register.
* SUM -- Super User Memory. Bit w `sstatus` opisujący, jak zachowują się dostępy do pamięci trybu użytkownika. Jeśli ma wartość 0, to dostajemy błąd strony przy próbie dostępu do pamięci użytkownika w trybie uprzywilejowanym.
* `tp` -- Thread pointer. Pod względem zastosowania wydaje się, że u nas zawiera adres struktury `pcpu`.

Zobaczmy, co robi [`SET_FAULT_HANDLER`](http://bxr.su/FreeBSD/sys/riscv/include/asm.h#61).

```=61
#define SET_FAULT_HANDLER(handler, tmp)                 \
    ld  tmp, PC_CURTHREAD(tp);                          \
    ld  tmp, TD_PCB(tmp);           /* Load the pcb */  \
    sd  handler, PCB_ONFAULT(tmp)   /* Set the handler */
```

W PCB zapisujemy adres handlera błędu strony (w naszym przypadku `copyio_fault`).

>Mam pytanie co do handlera błędu strony. Gdy mamy błąd strony, to wykonujemy copyio_fault i z niego wracamy 'ret', ale wydaje się, że po 'ret' znajdujemy się znowu gdzieś w środku procedury copyout, w dodatku bez uprawnień do pamięci użytkownika i ogólnie w dość popsutym stanie. Zgaduję, że my w takim razie nie wracamy do copyout, ale jak to działa w takim razie?
>[name=Tomasz Stachowski]
> > To jest związane z wskazówką. My nie wołamy `copyio_fault`, tylko tam skaczemy za pośrednictwem trap handlera (przez zmianę adresu powrotu). Więc ten `ret` nam wraca z `copyout`.

Interesuje nas też [`ENTER_USER_ACCESS`](http://bxr.su/FreeBSD/sys/riscv/include/asm.h#66).

```=66
#define ENTER_USER_ACCESS(tmp)    \
    li  tmp, SSTATUS_SUM;         \
    csrs    sstatus, tmp
```

Tutaj po prostu ustawiamy wspomniany bit SUM.

Sam sposób kopiowania danych nie jest bardzo interesujący. W naszym przypadku adres nie jest poprawny, więc przy pierwszej próbie skopiowania do przestrzeni użytkownika trafiamy do procedury obsługi pułapki [`data_abort`](http://bxr.su/FreeBSD/sys/riscv/riscv/trap.c#166).

:::spoiler **Procedura `data_abort`**
```c=166
static void
data_abort(struct trapframe *frame, int usermode)
{
    struct vm_map *map;
    uint64_t stval;
    struct thread *td;
    struct pcb *pcb;
    vm_prot_t ftype;
    vm_offset_t va;
    struct proc *p;
    int error, sig, ucode;

#ifdef KDB
    if (kdb_active) {
        kdb_reenter();
        return;
    }
#endif

    td = curthread;
    p = td->td_proc;
    pcb = td->td_pcb;
    stval = frame->tf_stval;

    if (td->td_critnest != 0 || td->td_intr_nesting_level != 0 ||
        WITNESS_CHECK(WARN_SLEEPOK | WARN_GIANTOK, NULL,
        "Kernel page fault") != 0)
        goto fatal;

    if (usermode)
        map = &td->td_proc->p_vmspace->vm_map;
    else if (stval >= VM_MAX_USER_ADDRESS)
        map = kernel_map;
    else {
        if (pcb->pcb_onfault == 0)
            goto fatal;
        map = &td->td_proc->p_vmspace->vm_map;
    }

    va = trunc_page(stval);

    if ((frame->tf_scause == EXCP_FAULT_STORE) ||
        (frame->tf_scause == EXCP_STORE_PAGE_FAULT)) {
        ftype = VM_PROT_WRITE;
    } else if (frame->tf_scause == EXCP_INST_PAGE_FAULT) {
        ftype = VM_PROT_EXECUTE;
    } else {
        ftype = VM_PROT_READ;
    }

    if (pmap_fault_fixup(map->pmap, va, ftype))
        goto done;

    error = vm_fault_trap(map, va, ftype, VM_FAULT_NORMAL, &sig, &ucode);
    if (error != KERN_SUCCESS) {
        if (usermode) {
            call_trapsignal(td, sig, ucode, (void *)stval);
        } else {
            if (pcb->pcb_onfault != 0) {
                frame->tf_a[0] = error;
                frame->tf_sepc = pcb->pcb_onfault;
                return;
            }
            goto fatal;
        }
    }

done:
    if (usermode)
        userret(td, frame);
    return;

fatal:
    dump_regs(frame);
    panic("Fatal page fault at %#lx: %#016lx", frame->tf_sepc, stval);
}
```
:::

`data_abort` wykonuje pewne próby naprawienia sytuacji (`pmap_fault_fixup`, `vm_fault_trap`), ale w naszym scenariuszu nie powiodą się one. W linii 224 sprawdzimy, że pole `pcb_onfault` jest niezerowe, po czym ustawimy `sepc` na adres handlera.

`sepc` -- Supervisor exception program counter. Tam wrócimy po procedurze obsługi pułapki.

U nas handlerem jest procedura [`copyio_fault`](http://bxr.su/FreeBSD/sys/riscv/riscv/copyinout.S#47):

```=47
ENTRY(copyio_fault)
    SET_FAULT_HANDLER(x0, a1) /* Clear the handler */
    EXIT_USER_ACCESS(a1)
copyio_fault_nopcb:
    li  a0, EFAULT
    ret
END(copyio_fault)
```

```=70
#define EXIT_USER_ACCESS(tmp)    \
    li  tmp, SSTATUS_SUM;        \
    csrc    sstatus, tmp
```

Odwracamy zmiany, które zrobiliśmy w PCB i `sstatus`, po czym zwracamy
`EFAULT`.

## Zadanie 5-6

:::warning
Autor: Franciszek Zdobylak
:::

Jak wygląda `sleepqueue`?
```c 
struct sleepqueue {
    struct threadqueue sq_blocked[NR_SLEEPQS]; /* (c) Blocked threads. */
    u_int sq_blockedcnt[NR_SLEEPQS];    /* (c) N. of blocked threads. */
    LIST_ENTRY(sleepqueue) sq_hash;     /* (c) Chain and free list. */
    LIST_HEAD(, sleepqueue) sq_free;    /* (c) Free queues. */
    const void  *sq_wchan;      /* (c) Wait channel. */
    int sq_type;            /* (c) Queue type. */
};

struct sleepqueue_chain {
    LIST_HEAD(, sleepqueue) sc_queues;  /* List of sleep queues. */
    struct mtx sc_lock;         /* Spin lock for this chain. */
		...
} __aligned(CACHE_LINE_SIZE);
```

[`sleepq_add`]: 
1. Wyszukujemy (wg hash'u odpowiedni wpis w tablicy `sleepq_chains[]`)
2. Wyszukujemy odpowiednie sleep queue przypidane do `wchan` (`sleepq_lookup`)
3. `sleepq` przypisane do wątku zmieniamy na:
	* Jeśli wątek jest pierwszym oczekującym na danym `wchan`'ie
	  z jego `sleepq` robimy `sleepq` przypisane do tego `wchan`'u
	* Jeśli wątek jest kolejnym oczekującym na danym `wchan`'ie
	  jego `sleepq` dodajemy do listy wolnych `sleepq` (`sq_free`)
4. Dodajemy wątek do listy oczekujących na danym `wchan` (`sq_blocked`)

[`sleepq_remove_thread`]:
1. Usuwamy wątek z `sleepq` na której się znajduje
2. Znajdujemy dla niego `sleepq` z którą może "odejść"
	* Jeśli nie był ostatnim wątkiem czekającym na danym `wchan`'ie
		to dostaje wolną `sleepq` odłożoną w `sq_free`
	* Jeśli jest ostatnim wątkiem czekającym na tym `wchan`'ie
	  to dajemy mu aktualną `sleepq`, na której się znajdował
		(musimy ją oczywiście usunąć z listy `sleepq_chains[*]`)
		
**Pytania**  
*Czemu mamy dwie kolejki wątków? (tzn. `sq_blocked` jest tablicą 2 elementową)*
Korzysta z tego implementacja sx(9).

>Czemu w ogóle istnieje osobna struktura sleep_queue którą się przyczepia i odczepia od wątków, a nie jest wpisana po prostu w `thread`?
>[name=Michał Opanowicz]

[`sleepq_add`]: http://bxr.su/FreeBSD/sys/kern/subr_sleepqueue.c#309
[`sleepq_remove_thread`]: http://bxr.su/FreeBSD/sys/kern/subr_sleepqueue.c#818

## Zadanie 5-7

:::warning
Autor: Jakub Urbańczyk
:::
###### Czemu wystarczy, by jądro przypisało po jednym rekordzie sleepqueue do każdego wątku?
Zauważmy, że dla każdego `wchanu` (który chcemy użyć w danej chwili) potrzebujemy `sleepq`. Ile jest maksymalnie `wchanów` których będziemy używać w jednym momencie? Tyle ile wątków. Nie potrzbujemy mieć żadnych dodatkowych `wchan`'ów ponieważ nie mamy wątków które by miały z nich skorzystać. Dlatego skoro `wchan`'ów może być w jednym momencie tyle co wątków, to `sleepq` tak samo.

###### Jak implementacja sleepqueue unika problemu zgubionej pobudki?
Zanim wątek zostanie uśpiony jest sprawdzane czy posiada on swoje `sleepq`.
`sleepq_switch()`:
```c=575
	/*
	* If we have a sleep queue, then we've already been woken up, so
	* just return.
	*/
	if (td->td_sleepqueue != NULL) {
		mtx_unlock_spin(&sc->sc_lock);
		thread_unlock(td);
		return;
	}
```
Zauważmy, że podczas dodawania wątku do wątków zablokowanych na `sleepq` odbierana jest mu jego wolna `sleep`, a podczas usuwania dostaje wolną. W takim razie jeśli wątek dalej ma czekać to nie będzie miał wolnej `sleepq`. Wtedy od razu wracamy z procedury [`sleepq_wait`]

`sleepq_add`:
```c=375
	thread_lock(td);
	TAILQ_INSERT_TAIL(&sq->sq_blocked[queue], td, td_slpq);
	sq->sq_blockedcnt[queue]++;
	td->td_sleepqueue = NULL;
	td->td_sqqueue = queue;
	td->td_wchan = wchan;
	td->td_wmesg = wmesg;
```

[`sleepq_wait`]: http://bxr.su/FreeBSD/sys/kern/subr_sleepqueue.c#669

## Zadanie 5-8

:::warning
Autor: Arkadiusz Kozdra
:::

> A nonpageable, or wired, range has physical memory assigned at the time of the call, and this memory is not subject to replacement by the pageout daemon. Wired pages never cause a page fault as that might result in a blocking operation. Wired memory is allocated from either the general allocator malloc(), or the zone allocator described in the last two subsections of this section.

### Czemu nie można robić dostępów do pamięci stronicowalnej trzymając spinlock

> Mutexes which do not context switch are MTX_SPIN mutexes.
> ~[mutex(9)]

Muteksy, które nie przełączają kontekstu, są muteksami MTX_SPIN.

Dostęp do pamięci stronnicowalnej grozi błędem strony, a niezmiennik mówi, że trzymając MTX_SPIN nie można oddawać procesora.
Jeśli trzymamy MTX_DEF, to i tak musimy zachować własność *forward progress*, ale stronicowanie może odbywać się na urządzeniu (może ono w ostateczności zawieść lub odpowiadać z nieograniczonym opóźnieniem). *Forward progress* oznacza, że ogranicza nas tylko czas procesora.

> Mutexes which do not context switch are MTX_SPIN mutexes.	These should
only be used to protect data shared with primary interrupt	code.  This
includes interrupt	filters	and low	level scheduling code.	In all archi-
tectures both acquiring and releasing of a	uncontested spin mutex is more
expensive than the	same operation on a non-spin mutex.  In	order to pro-
tect an interrupt service routine from blocking against itself all	inter-
rupts are either blocked or deferred on a processor while holding a spin
lock.  It is permissible to hold multiple spin mutexes.
~[mutex(9)]

> The pmap_init() function allocates a minimal amount of wired memory to use for kernel page-table pages. The page-table space is expanded dynamically by the pmap_growkernel() routine as it is needed while the kernel is running. Once allocated, it is never freed. The limit on the size of the kernel’s address space is selected at boot time. On 64-bit architectures, the kernel is typically given an address space large enough to directly map all of physical memory. On 32-bit architectures, the kernel is typically given a maximum of 1 Gbyte of address space.
> ~FreeBSD

[mutex(9)]: https://www.freebsd.org/cgi/man.cgi?query=mutex&sektion=9

> Each vmem arena is protected by a single lock as allocations from the vmem arena are infrequent. 
> ~FreeBSD
> 

W pamięci stronicowalnej jądro trzyma dane procesów użytkownika, strony używane przez procesy użytkownika i dane bardzo wysokopoziomowe (rzadko używane / nieużywane).
