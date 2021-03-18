# Ćwiczenia 13 (24 czerwca 2020)

## Zadanie 9-7

:::warning
Autor: Paweł Jasiak
:::

Zacznijmy od obrazków, które pomogą nam zrozumieć jak wygląda organizacja pamięci.

![](https://i.imgur.com/VrWAqOY.png)

![](https://i.imgur.com/bvI42lj.png)

![](https://i.imgur.com/RgGNsGh.png)

![](https://i.imgur.com/hKNCaO2.png)

W magazynkach (będę używał zamiennie z kubełkami, wiaderkami) trzymamy po prostu dane per CPU. No bo co się tutaj dzieje. Mamy jakąś globalną listę tych kubełków no i jest fajne, ale co jeśli wszystkie CPU nagle zaczynają alokować pamięć? No jest nam niestety smutno i trzeba by jakoś temu zaradzić. Mamy blokady. Ale jak robimy w cholerę takiej alokacji no to smutek. Dlatego trzymamy wiaderka per CPU. No bo CPU na raz wykona jedną alokację -- no nie dokona rozmnożenia w dwa CPU -- więc nie musi zakładać locka na globalną pulę. W razie potrzeby, jak już nie będzie miał niczego w kubełku to pójdzie po więcej i wtedy założy sobie locka. Przez więcej rozumiem -- mamy globalną listę, która trzyma nam **pełne** oraz **puste** kubełki. Analogicznie do alokacji wygląda bowiem zwalnianie pamięci. Będziemy po prostu wrzucać elementy do kubełka no i może nadejść chwila, że wypełnimy go tak, że zacznie nam się przelewać, a powodzi nikt nie lubi.

W pojedynczym wiaderku mamy _M_ sztuk objektów do zaalokowania. Po takich _M_ alokacjach trzeba przeładować magazynek (haha, broń, przeładowanie, strzelanie :smile: ).


No i fajne, wydaje się, że jest git, ale czy na 100%? Otóż, mamy złośliwego algorytmika, który uwali nasze rozwiązanie (gdybyśmy mieli jeden kubełek) i będzie zadawał ciąg złośliwych alokacji i dealokacji, powodujących "szamotanie" się.

![](https://i.imgur.com/W6IGUWw.png)

Teraz rozwiązanie, to posiadanie dwóch takich kubełków. Jeden będzie aktywnym na którym będziemy robić operacje. A drugi to taki zapasowy, że jak już pierwszy nie da rady to próbujemy na nim. Jeśli oba nie dają rady, to wtedy aktywny staje się nieaktywnym, nieaktywny zwracamy do zbrojowni, a aktywnym staje się nowy ze zbrojowni. No i jakaś tam kluczowa obserwacja, to że jesteśmy w ten sposób przetrwać _M_ alokacji i _M_ zwolnień bez wymiany kubełków.

Cały algorytm ukradziony z artykułu
![](https://i.imgur.com/W5cge2d.png)

Jak zarządzać rozmiarem magazynku? Ustalamy sobie jakieś małe początkowe _M_. I mamy dwie funkcje, _lock_ i _trylock_ których będziemy używali w przypadku wycieczki do zbrojowni. Jeśli _trylock_ zawodzi, to zwiększamy licznik zawodzenia i robimy _lock_. Jeśli nasz licznik przekroczy ustalony próg, to będziemy zwiększać rozmiar magazynku.


## Zadanie 9-8

:::warning
Autor: Samuel Li
:::

### do czego służą vm_object?

[`vm_object`] jest zbiorem stron i służą za reprezentacje grupy pamięci.

> VM objects can be associated with backing store of various types—unbacked, swap-backed, physical device-backed, or file-backed storage. 
> Obiekty maszyn wirtualnych można powiązać z magazynem kopii zapasowych różnego typu - bez kopi zapasowej, z wymienną, z fizyczną pamięcią urządzenia lub z pamięcią pliku.
> ~ FreeBSD Handbook

### Co się dzieje w trakcie [fork(2)]

Zostaną utowrzone nowe objety przesłaniające dla każdego z procesów. Owe objekty będą wskazywały na objekt na który wcześniej wskazywał rodzic. 

Jeżeli nastąpi modyfikacja pamięci prywatnej to zmieniana strona zostanie skopiowana do najbliższego objektu i tam zmodyfikowana.

Współdzielonej to nie dotyczy

![](https://i.imgur.com/5PwAhlr.png)


:::warning
> FreeBSD uses a lower-cost heuristic to reduce the copying of shadow pages. When a page of a top-level shadow object is faulted, the kernel checks whether a lower-level shadow object contains a copy of the page. If that lower-level shadow object has the page and is referenced only by the top-level shadow object, (i.e., in principle the chain could be collapsed) the page is moved rather than copied from the lower-level shadow object to the top-level shadow object and mapped with write access.
> ~ The Design and Implementation
:::

### Co się dzieje w trakcie [mmap(2)] w prywatnym mapowaniu

> The kernel uses shadow vm_objects to prevent changes made by a process from being reflected back to the underlying vm_object. The use of a shadow vm_object is shown in Figure 6.13. When the initial private mapping is requested, the file vm_object is mapped into the requesting-process address space, with copy-on-write semantics.
> ~ The Design and Implementation of the FreeBSD(R) Operating System
> > Jądro używa shadow vm_objects, aby zapobiec odbiciu zmian dokonanych przez proces z powrotem do bazowego vm_object. Zastosowanie cienia vm_object pokazano na rysunku 6.13. Kiedy wymagane jest wstępne prywatne mapowanie, plik vm_object jest mapowany do przestrzeni adresowej procesu wysyłającego żądanie, z semantyką [kopiowania przy zapisie](http://bxr.su/FreeBSD/sys/vm/vm_map.h#122).

![](https://i.imgur.com/5ZrHfVc.png)

Kiedy nastąpi modyfikacja stony to zostanie utworzony shawdow object do którego zostanie skopiowna strona którą będziemy modyfikować.

:::info
Być może warto zamiast kopiować stronę, przenieść ją ale tego FreeBSD nie robi:

> If free memory is limited, it would be better simply to move the modified page from the file vm_object to the shadow vm_object. The move would reduce the immediate demand on the free memory, because a new page would not have to be allocated. The drawback to this optimization is that, if there is a later access to the file vm_object by some other process, the kernel will have to allocate a new page. The kernel will also have to pay the cost of doing an I/O operation to reload the page contents. In FreeBSD, the virtual-memory system never moves a page from a file vm_object rather than copying it.
> ~ The Design and Implementation of the FreeBSD(R) Operating System
> > Jeśli wolna pamięć jest ograniczona, lepiej byłoby po prostu przenieść zmodyfikowaną stronę z pliku vm_object do cienia vm_object. Przeniesienie to zmniejszy bezpośrednie zapotrzebowanie na wolną pamięć, ponieważ nie trzeba będzie przydzielać nowej strony. Wadą tej optymalizacji jest to, że jeśli późniejszy dostęp do pliku vm_object będzie miał inny proces, jądro będzie musiało przydzielić nową stronę. Jądro będzie również musiało ponieść koszty wykonania operacji we / wy w celu ponownego załadowania zawartości strony. W FreeBSD system pamięci wirtualnej nigdy nie przenosi strony z pliku vm_object, zamiast go kopiować.
:::

> When a process with a private mapping removes that mapping either explicitly with an munmap system call or implicitly by exiting, its parent or child process may be left with a chain of shadow vm_objects. Usually, these chains of shadow vm_objects can be collapsed into a single shadow vm_object, often freeing up memory as part of the collapse. Consider what happens when process A exits in Figure 6.14. First, shadow vm_object 3 can be freed along with its associated page of memory. This deallocation leaves shadow vm_objects 1 and 2 in a chain with no intervening references. Thus, these two vm_objects can be collapsed into a single shadow vm_object. Since they both contain a copy of page 0, and since only the page 0 in shadow vm_object 2 can be accessed by the remaining child process, the page 0 in shadow vm_object 1 can be freed along with shadow vm_object 1 itself.

[`vm_object`]: http://bxr.su/FreeBSD/sys/vm/vm_object.h#vm_object
[fork(2)]: https://netbsd.gw.com/cgi-bin/man-cgi?fork+2+NetBSD-current
[mmap(2)]: https://netbsd.gw.com/cgi-bin/man-cgi?mmap+2+NetBSD-current

## Zadanie 10-1

:::warning
Autor: Patrycja Balik
:::

> Zaprezentuj przebieg obsługi wywołania systemowego `read(2)` na pliku reprezentowanym przez **v-węzeł**. Zacznij od implementacji [`sys_read`](http://bxr.su/NetBSD/sys/kern/sys_generic.c#sys_read), poprzez procedurę [`dofileread(9)`](http://bxr.su/FreeBSD/sys/kern/sys_generic.c#dofilewrite), zestaw operacji `vnfileops(9)`, a kończąc na wywołaniu wirtualnego systemu plików `VOP_READ(9)`.


**v-węzeł** (*vnode*) -- obiekt w jądrze reprezentujący plik/katalog, abstrakcja nad detalami poszczególnych systemów plików.

:::spoiler [Definicja `struct vnode`](http://bxr.su/NetBSD/sys/sys/vnode.h#141)
```c=
struct vnode {
    /*
     * VM system related items.
     */
    struct uvm_object v_uobj;       /* u   the VM object */
    voff_t      v_size;         /* i+u size of file */
    voff_t      v_writesize;        /* i+u new size after write */

    /*
     * Unstable items get their own cache line.
     * On _LP64 this fills the space nicely.
     */
    kcondvar_t  v_cv            /* i   synchronization */
        __aligned(COHERENCY_UNIT);
    int     v_iflag;        /* i+u VI_* flags */
    int     v_uflag;        /* k   VU_* flags */
    int     v_usecount;     /* i   reference count */
    int     v_numoutput;        /* i   # of pending writes */
    int     v_writecount;       /* i   ref count of writers */
    int     v_holdcnt;      /* i   page & buffer refs */
    struct buflists v_cleanblkhd;       /* i+b clean blocklist head */
    struct buflists v_dirtyblkhd;       /* i+b dirty blocklist head */

    /*
     * The remaining items are largely stable.
     */
    int     v_vflag         /* v   VV_* flags */
        __aligned(COHERENCY_UNIT);
    kmutex_t    *v_interlock;       /* -   vnode interlock */
    struct mount    *v_mount;       /* v   ptr to vfs we are in */
    int     (**v_op)(void *);   /* :   vnode operations vector */
    union {
        struct mount    *vu_mountedhere;/* v   ptr to vfs (VDIR) */
        struct socket   *vu_socket; /* v   unix ipc (VSOCK) */
        struct specnode *vu_specnode;   /* v   device (VCHR, VBLK) */
        struct fifoinfo *vu_fifoinfo;   /* v   fifo (VFIFO) */
        struct uvm_ractx *vu_ractx; /* u   read-ahead ctx (VREG) */
    } v_un;
    enum vtype  v_type;         /* -   vnode type */
    enum vtagtype   v_tag;          /* -   type of underlying data */
    void        *v_data;        /* -   private data for fs */
    struct klist    v_klist;        /* i   notes attached to vnode */
    void        *v_segvguard;       /* e   for PAX_SEGVGUARD */
};
```
:::

`sys_read` wyciąga poszczególne argumenty `read`a, odnajduje plik odpowiadający deskryptorowi (i zwiększa licznik referencji), robi wstępne sprawdzenia poprawności, i woła `dofileread`.
:::spoiler Procedura `sys_read`
```c=99
int
sys_read(struct lwp *l, const struct sys_read_args *uap, register_t *retval)
{
    /* {
        syscallarg(int)     fd;
        syscallarg(void *)  buf;
        syscallarg(size_t)  nbyte;
    } */
    file_t *fp;
    int fd;

    fd = SCARG(uap, fd);

    if ((fp = fd_getfile(fd)) == NULL)
        return (EBADF);

    if ((fp->f_flag & FREAD) == 0) {
        fd_putfile(fd);
        return (EBADF);
    }

    /* dofileread() will unuse the descriptor for us */
    return (dofileread(fd, fp, SCARG(uap, buf), SCARG(uap, nbyte),
        &fp->f_offset, FOF_UPDATE_OFFSET, retval));
}
```
:::

:::spoiler Procedura [`dofileread`](http://bxr.su/NetBSD/sys/kern/sys_generic.c#dofileread)
```c=125
int
dofileread(int fd, struct file *fp, void *buf, size_t nbyte,
    off_t *offset, int flags, register_t *retval)
{
    struct iovec aiov;
    struct uio auio;
    size_t cnt;
    int error;
    lwp_t *l;

    l = curlwp;

    aiov.iov_base = (void *)buf;
    aiov.iov_len = nbyte;
    auio.uio_iov = &aiov;
    auio.uio_iovcnt = 1;
    auio.uio_resid = nbyte;
    auio.uio_rw = UIO_READ;
    auio.uio_vmspace = l->l_proc->p_vmspace;

    /*
     * Reads return ssize_t because -1 is returned on error.  Therefore
     * we must restrict the length to SSIZE_MAX to avoid garbage return
     * values.
     */
    if (auio.uio_resid > SSIZE_MAX) {
        error = EINVAL;
        goto out;
    }

    cnt = auio.uio_resid;
    error = (*fp->f_ops->fo_read)(fp, offset, &auio, fp->f_cred, flags);
    if (error)
        if (auio.uio_resid != cnt && (error == ERESTART ||
            error == EINTR || error == EWOULDBLOCK))
            error = 0;
    cnt -= auio.uio_resid;
    ktrgenio(fd, UIO_READ, buf, cnt, error);
    *retval = cnt;
 out:
    fd_putfile(fd);
    return (error);
}
```
:::

Procedura `dofileread` tworzy nam `struct uio` i woła `file::f_ops::fo_read`. Musimy więc zajrzeć do `fileops`ów dla `vnode`ów. Odpowiednie definicje mamy w [`sys/kern/vfs_vnops.c`](http://bxr.su/NetBSD/sys/kern/vfs_vnops.c).

:::spoiler Procedura [`vn_read`](http://bxr.su/NetBSD/sys/kern/vfs_vnops.c#vn_read)
```c=551
static int
vn_read(file_t *fp, off_t *offset, struct uio *uio, kauth_cred_t cred,
    int flags)
{
    struct vnode *vp = fp->f_vnode;
    int error, ioflag, fflag;
    size_t count;

    ioflag = IO_ADV_ENCODE(fp->f_advice);
    fflag = fp->f_flag;
    if (fflag & FNONBLOCK)
        ioflag |= IO_NDELAY;
    if ((fflag & (FFSYNC | FRSYNC)) == (FFSYNC | FRSYNC))
        ioflag |= IO_SYNC;
    if (fflag & FALTIO)
        ioflag |= IO_ALTSEMANTICS;
    if (fflag & FDIRECT)
        ioflag |= IO_DIRECT;
    vn_lock(vp, LK_SHARED | LK_RETRY);
    uio->uio_offset = *offset;
    count = uio->uio_resid;
    error = VOP_READ(vp, uio, ioflag, cred);
    if (flags & FOF_UPDATE_OFFSET)
        *offset += count - uio->uio_resid;
    VOP_UNLOCK(vp);
    return (error);
}
```
:::

Tutaj dokonujemy ostatnich poprawek w kwestii `uio`, ustawiamy flagi, wołamy `VOP_READ` i opcjonalnie aktualizujemy offset. Dalej wykonamy operację zależną od systemu plików.

## Zadanie 10-2

:::warning
Autor: Tomasz Stachowski
:::

Zaczynamy od sys_write ---> kern_writev ---> dofilewrite ---> fo_write (pipe_write).

Krótki opis pipe_write:
*    dynamicznie okreslamy za każdym razem wielkość bufora potoku
*    dopóki uio->uio_resid jest większe od zera (czyli mamy bajty do przetrasportowania)
        * czekamy aż nie będzie żadnych rzeczy do transportu direct (niebuforowanego), ewentualnie budzimy proces po drugiej stronie
        * chcemy, żeby przesyłanie paczek mniejszych niż PIPE_BUF było atomowe, więc czekamy, aż w buforze będzie dość miejsca na atomowy zapis
        * kopiujemy dane do bufora, uważając na to, że możemy się "zawinąć" (potencjalnie musimy wykonać dwie kopie)
        * budzimy czytelnika jak coś zrobiliśmy
* budzimy czytelnika jak coś zrobiliśmy


Rozpatrywanie wysłania SIGPIPE znajduje się w dofilewrite.

Procedura uiomove służy do kopiowania danych z pamięci użytkownika do pamieci jądra lub odwrotnie.

Zapis do bufora potoku następuje w liniach 1194 oraz 1209.

Atomowe zapisywanie rozpatrywane jest w linii 1164.

Wybudzamy procesy oczekujące na select/poll w liniach 1255 oraz 1300.

Kod porcedury dofilewrite:
```c
static int
dofilewrite(struct thread *td, int fd, struct file *fp, struct uio *auio,
    off_t offset, int flags)
{
	ssize_t cnt;
	int error;
	AUDIT_ARG_FD(fd);
	auio->uio_rw = UIO_WRITE;
	auio->uio_td = td;
	auio->uio_offset = offset;

	cnt = auio->uio_resid;
	if ((error = fo_write(fp, auio, td->td_ucred, flags, td))) {
		if (auio->uio_resid != cnt && (error == ERESTART ||
		    error == EINTR || error == EWOULDBLOCK))
			error = 0;
		/* Socket layer is responsible for issuing SIGPIPE. */
		if (fp->f_type != DTYPE_SOCKET && error == EPIPE) {
			PROC_LOCK(td->td_proc);
			tdsignal(td, SIGPIPE);
			PROC_UNLOCK(td->td_proc);
		}
	}
	cnt -= auio->uio_resid;
	td->td_retval[0] = cnt;
	return (error);
}
```

<details>
<summary>Kod procedury pipe_write</summary>


```c=1040
static int
pipe_write(struct file *fp, struct uio *uio, struct ucred *active_cred,
    int flags, struct thread *td)
{
	struct pipe *wpipe, *rpipe;
	ssize_t orig_resid;
	int desiredsize, error;

	rpipe = fp->f_data;
	wpipe = PIPE_PEER(rpipe);
	PIPE_LOCK(rpipe);
	error = pipelock(wpipe, 1);
	if (error) {
		PIPE_UNLOCK(rpipe);
		return (error);
	}
	/*
	 * detect loss of pipe read side, issue SIGPIPE if lost.
	 */
	if (wpipe->pipe_present != PIPE_ACTIVE ||
	    (wpipe->pipe_state & PIPE_EOF)) {
		pipeunlock(wpipe);
		PIPE_UNLOCK(rpipe);
		return (EPIPE);
	}
#ifdef MAC
	error = mac_pipe_check_write(active_cred, wpipe->pipe_pair);
	if (error) {
		pipeunlock(wpipe);
		PIPE_UNLOCK(rpipe);
		return (error);
	}
#endif
	++wpipe->pipe_busy;

	/* Choose a larger size if it's advantageous */
	desiredsize = max(SMALL_PIPE_SIZE, wpipe->pipe_buffer.size);
	while (desiredsize < wpipe->pipe_buffer.cnt + uio->uio_resid) {
		if (piperesizeallowed != 1)
			break;
		if (amountpipekva > maxpipekva / 2)
			break;
		if (desiredsize == BIG_PIPE_SIZE)
			break;
		desiredsize = desiredsize * 2;
	}

	/* Choose a smaller size if we're in a OOM situation */
	if (amountpipekva > (3 * maxpipekva) / 4 &&
	    wpipe->pipe_buffer.size > SMALL_PIPE_SIZE &&
	    wpipe->pipe_buffer.cnt <= SMALL_PIPE_SIZE &&
	    piperesizeallowed == 1)
		desiredsize = SMALL_PIPE_SIZE;

	/* Resize if the above determined that a new size was necessary */
	if (desiredsize != wpipe->pipe_buffer.size &&
	    (wpipe->pipe_state & PIPE_DIRECTW) == 0) {
		PIPE_UNLOCK(wpipe);
		pipespace(wpipe, desiredsize);
		PIPE_LOCK(wpipe);
	}
	MPASS(wpipe->pipe_buffer.size != 0);

	pipeunlock(wpipe);

	orig_resid = uio->uio_resid;

	while (uio->uio_resid) {
		int space;

		pipelock(wpipe, 0);
		if (wpipe->pipe_state & PIPE_EOF) {
			pipeunlock(wpipe);
			error = EPIPE;
			break;
		}
#ifndef PIPE_NODIRECT
		/*
		 * If the transfer is large, we can gain performance if
		 * we do process-to-process copies directly.
		 * If the write is non-blocking, we don't use the
		 * direct write mechanism.
		 *
		 * The direct write mechanism will detect the reader going
		 * away on us.
		 */
		if (uio->uio_segflg == UIO_USERSPACE &&
		    uio->uio_iov->iov_len >= PIPE_MINDIRECT &&
		    wpipe->pipe_buffer.size >= PIPE_MINDIRECT &&
		    (fp->f_flag & FNONBLOCK) == 0) {
			pipeunlock(wpipe);
			error = pipe_direct_write(wpipe, uio);
			if (error)
				break;
			continue;
		}
#endif

		/*
		 * Pipe buffered writes cannot be coincidental with
		 * direct writes.  We wait until the currently executing
		 * direct write is completed before we start filling the
		 * pipe buffer.  We break out if a signal occurs or the
		 * reader goes away.
		 */
		if (wpipe->pipe_map.cnt != 0) {
			if (wpipe->pipe_state & PIPE_WANTR) {
				wpipe->pipe_state &= ~PIPE_WANTR;
				wakeup(wpipe);
			}
			pipeselwakeup(wpipe);
			wpipe->pipe_state |= PIPE_WANTW;
			pipeunlock(wpipe);
			error = msleep(wpipe, PIPE_MTX(rpipe), PRIBIO | PCATCH,
			    "pipbww", 0);
			if (error)
				break;
			else
				continue;
		}

		space = wpipe->pipe_buffer.size - wpipe->pipe_buffer.cnt;

		/* Writes of size <= PIPE_BUF must be atomic. */
		if ((space < uio->uio_resid) && (orig_resid <= PIPE_BUF))
			space = 0;

		if (space > 0) {
			int size;	/* Transfer size */
			int segsize;	/* first segment to transfer */

			/*
			 * Transfer size is minimum of uio transfer
			 * and free space in pipe buffer.
			 */
			if (space > uio->uio_resid)
				size = uio->uio_resid;
			else
				size = space;
			/*
			 * First segment to transfer is minimum of
			 * transfer size and contiguous space in
			 * pipe buffer.  If first segment to transfer
			 * is less than the transfer size, we've got
			 * a wraparound in the buffer.
			 */
			segsize = wpipe->pipe_buffer.size -
				wpipe->pipe_buffer.in;
			if (segsize > size)
				segsize = size;

			/* Transfer first segment */

			PIPE_UNLOCK(rpipe);
			error = uiomove(&wpipe->pipe_buffer.buffer[wpipe->pipe_buffer.in],
					segsize, uio);
			PIPE_LOCK(rpipe);

			if (error == 0 && segsize < size) {
				KASSERT(wpipe->pipe_buffer.in + segsize ==
					wpipe->pipe_buffer.size,
					("Pipe buffer wraparound disappeared"));
				/*
				 * Transfer remaining part now, to
				 * support atomic writes.  Wraparound
				 * happened.
				 */

				PIPE_UNLOCK(rpipe);
				error = uiomove(
				    &wpipe->pipe_buffer.buffer[0],
				    size - segsize, uio);
				PIPE_LOCK(rpipe);
			}
			if (error == 0) {
				wpipe->pipe_buffer.in += size;
				if (wpipe->pipe_buffer.in >=
				    wpipe->pipe_buffer.size) {
					KASSERT(wpipe->pipe_buffer.in ==
						size - segsize +
						wpipe->pipe_buffer.size,
						("Expected wraparound bad"));
					wpipe->pipe_buffer.in = size - segsize;
				}

				wpipe->pipe_buffer.cnt += size;
				KASSERT(wpipe->pipe_buffer.cnt <=
					wpipe->pipe_buffer.size,
					("Pipe buffer overflow"));
			}
			pipeunlock(wpipe);
			if (error != 0)
				break;
		} else {
			/*
			 * If the "read-side" has been blocked, wake it up now.
			 */
			if (wpipe->pipe_state & PIPE_WANTR) {
				wpipe->pipe_state &= ~PIPE_WANTR;
				wakeup(wpipe);
			}

			/*
			 * don't block on non-blocking I/O
			 */
			if (fp->f_flag & FNONBLOCK) {
				error = EAGAIN;
				pipeunlock(wpipe);
				break;
			}

			/*
			 * We have no more space and have something to offer,
			 * wake up select/poll.
			 */
			pipeselwakeup(wpipe);

			wpipe->pipe_state |= PIPE_WANTW;
			pipeunlock(wpipe);
			error = msleep(wpipe, PIPE_MTX(rpipe),
			    PRIBIO | PCATCH, "pipewr", 0);
			if (error != 0)
				break;
		}
	}

	pipelock(wpipe, 0);
	--wpipe->pipe_busy;

	if ((wpipe->pipe_busy == 0) && (wpipe->pipe_state & PIPE_WANT)) {
		wpipe->pipe_state &= ~(PIPE_WANT | PIPE_WANTR);
		wakeup(wpipe);
	} else if (wpipe->pipe_buffer.cnt > 0) {
		/*
		 * If we have put any characters in the buffer, we wake up
		 * the reader.
		 */
		if (wpipe->pipe_state & PIPE_WANTR) {
			wpipe->pipe_state &= ~PIPE_WANTR;
			wakeup(wpipe);
		}
	}

	/*
	 * Don't return EPIPE if any byte was written.
	 * EINTR and other interrupts are handled by generic I/O layer.
	 * Do not pretend that I/O succeeded for obvious user error
	 * like EFAULT.
	 */
	if (uio->uio_resid != orig_resid && error == EPIPE)
		error = 0;

	if (error == 0)
		vfs_timestamp(&wpipe->pipe_mtime);

	/*
	 * We have something to offer,
	 * wake up select/poll.
	 */
	if (wpipe->pipe_buffer.cnt)
		pipeselwakeup(wpipe);

	pipeunlock(wpipe);
	PIPE_UNLOCK(rpipe);
	return (error);
}
```
</details>

## Zadanie 10-3

:::warning
Autor: Michał Błaszczyk
:::

**[kqueue](http://bxr.su/NetBSD/sys/sys/eventvar.h#48)**

Kqueue to obiekt jądra osiągalny przez deskryptor pliku.Będziemy z nim wiązać stan, tj. nasłuchiwane zdarzenia,a potem czekać na jego modyfikację.

kqueue() provides a generic method of notifying the user when an event happens or a condition holds, based on the results of small pieces of kernel code termed filters.  A kevent is identified by the (ident, filter) pair; there may only be one unique kevent per kqueue.

**[knote](http://bxr.su/NetBSD/sys/sys/event.h#186)**

Każde zdarzenie którym zainteresowana jest aplikacja, jest odnotowywane w pojedynczej strukturze knote.

**[sys_kfilters](http://bxr.su/NetBSD/sys/kern/kern_event.c#186)**

**[kqueue_register()](http://bxr.su/NetBSD/sys/kern/kern_event.c#1004)**

1. Alokujemy nowy knote.
2. Ustalamy filtr identyfikowany przez dostarczone jako argument zdarzenie ([kfilter_byfilter()](http://bxr.su/NetBSD/sys/kern/kern_event.c#316)).
3. Nasze zdarzenie jest powiązane z deskryptorem pliku.
4. Sprawdzamy poprawność dostarczonego (jako identyfikator zdarzenia) deskryptora oraz ustalamy wskaźnik na otwarty plik powiązany z tym deskryptorem ([fd_getfile()](http://bxr.su/NetBSD/sys/kern/kern_descrip.c#360)).
5. Ustalamy wskaźnik na strukturę deskryptora pliku ([fdfile_t](http://bxr.su/NetBSD/sys/sys/filedesc.h#106)) odpowiadającą rozpatrywanemu deskryptorowi (tablicę deskryptorów odczytujemy ze struktury kqueue).
6. Obsługujemy przypadek flagi **FR_CLOSING**.
7. Jeżeli rozpatrywany deskryptor jest mniejszy od maksymalnego deskryptora dla kqueue (z aktualnie wskazywanej (przez kqueue) tablicy deskryptorów), wówczas przeglądamy wszystkie knote powiązane z danym deskryptorem i szukamy takiego, które wpięte jest na rozpatrywaną listę kqueue z ustalonym (w kroku 2) filtrem.
8. W naszym scenariuszu zdarzenie jest dodawane (a więc flaga **EV_ADD** jest obecna). Ponadto, dane kqueue nie posiada jeszcze rozpatrywanego zdarzenia (tj. zdarzenia z identyczną parą <identyfikator, filtr>), a więc w tym momencie kn == NULL.
9. Inicjalizujemy nową strukturę knote (zdarzenie przekazujemy przez kopię a nie referencję).
10. Wkładamy knote na listę knote-ów powiązanych z rozpatrywanym deskryptorem pliku.
11. Wywołujemy funkcję f_attach (charakterystyczną dla rozpatrywanego filtru).
12. Inkrementujemy licznik referencji powiązany z wykorzystywanym filtrem.
13. Wywołanie f_attach podmieni zawartość pola kn_fop (operacje filtru). Wywołujemy funkcję f_event (z drugim argumentem równym 0) (faktycznie wywołamy tu procedurę [filt_piperead()](http://bxr.su/NetBSD/sys/kern/sys_pipe.c#1383)).
14. Aktywujemy knote ([knote_activate()](http://bxr.su/NetBSD/sys/kern/kern_event.c#1768)).
15. Aktywujemy (lub dezaktywujemy) knote.
16. Oddajemy deskryptor pliku (dekrementacja licznika referencji [fd_putfile()](http://bxr.su/NetBSD/sys/kern/kern_descrip.c#418)).

**[file_filtops](http://bxr.su/NetBSD/sys/kern/kern_descrip.c#418)**

W naszym scenariuszu, wywołanie f_attach sprowadza się do wywołania funkcji [filt_fileattach()](http://bxr.su/NetBSD/sys/kern/kern_event.c#450), zaś to wywołanie sprowadza się do wywołania fo_kqfilter, czyli dla potoku - [pipe_kqfilter()](http://bxr.su/NetBSD/sys/kern/sys_pipe.c#1456) (patrz: [pipeops](http://bxr.su/NetBSD/sys/kern/sys_pipe.c#122)).

**[pipe_kqfilter()](http://bxr.su/NetBSD/sys/kern/sys_pipe.c#1456)**

1. Ustalamy wskaźnik na potok.
2. Ustawiamy pole kn_fop na adres [pipe_rfiltops](http://bxr.su/NetBSD/sys/kern/sys_pipe.c#1441).
3. Wstawiamy knote na listę knote-ów powiązanych ze strukturą pipe_sel.
4. Zwracamy 0.

## Zadanie 10-4

:::success
Autor: Mikołaj Kowalik
:::
# zadanie 4
Ciekawsze rzeczy dzieją się niżej -- tam też jest więcej komentarzy.
```c
/*
 * Select/poll wakup. This also sends SIGIO to peer connected to
 * 'sigpipe' side of pipe.
 */
static void
pipeselwakeup(struct pipe *selp, struct pipe *sigp, int code)
{
    int band;

 //ustawienie odpowiedniej maski
    switch (code) {
    case POLL_IN:
        band = POLLIN|POLLRDNORM;
        break;
    case POLL_OUT:
        band = POLLOUT|POLLWRNORM;
        break;
    case POLL_HUP:
        band = POLLHUP;
        break;
    case POLL_ERR:
        band = POLLERR;
        break;
    default:
        band = 0;
#ifdef DIAGNOSTIC
        printf("bad siginfo code %d in pipe notification.\n", code);
#endif
        break;
    }
    // wywołujemy selnotify z oznaczonym bandem (eventami)
    selnotify(&selp->pipe_sel, band, NOTE_SUBMIT);

    if (sigp == NULL || (sigp->pipe_state & PIPE_ASYNC) == 0)
        return;

    fownsignal(sigp->pipe_pgid, SIGIO, code, band, selp);
}

```

```c
/*
 * Do a wakeup when a selectable event occurs.  Concurrency issues:
 *
 * As per selrecord(), the caller's object lock is held.  If there
 * is a named waiter, we must acquire the associated selcluster's lock
 * in order to synchronize with selclear() and pollers going to sleep
 * in sel_do_scan().
 *
 * sip->sel_cluser cannot change at this point, as it is only changed
 * in selrecord(), and concurrent calls to selrecord() are locked
 * out by the caller.
 */
void
selnotify(struct selinfo *sip, int events, long knhint)
{
    selcluster_t *sc;
    uint64_t mask;
    int index, oflag;
    lwp_t *l;
    kmutex_t *lock;

    //tutaj doszliśmy do knote z zadania
    // KNOTE to makro uruchamiające knote() gdy lista knote jest niepusta
    // ten knhint służy do podpowiedzi czy mamy założyć mutex.
    KNOTE(&sip->sel_klist, knhint);

    // Cięcie 

}

```

```c
/*
 * Walk down a list of knotes, activating them if their event has
 * triggered.  The caller's object lock (e.g. device driver lock)
 * must be held.
 */
void
knote(struct klist *list, long hint)
{
    struct knote *kn, *tmpkn;

    //przechodzimy całą listę knote i na każdej strukturce kn wywołujemy nasz filter (f_event)
    SLIST_FOREACH_SAFE(kn, list, kn_selnext, tmpkn) {
        KASSERT(kn->kn_fop != NULL);
        KASSERT(kn->kn_fop->f_event != NULL);
// tutaj jest nasz f_event
        if ((*kn->kn_fop->f_event)(kn, hint))
            knote_activate(kn);
    }
}
```

```c

// funkcja odpowiada na pytanie czy mamy aktywować dany knote (return 1 or more)
static int
filt_piperead(struct knote *kn, long hint)
{
    struct pipe *rpipe = ((file_t *)kn->kn_obj)->f_pipe;
    struct pipe *wpipe;

    if ((hint & NOTE_SUBMIT) == 0) {
        mutex_enter(rpipe->pipe_lock);
    }
    wpipe = rpipe->pipe_peer;
    kn->kn_data = rpipe->pipe_buffer.cnt;

    if ((kn->kn_data == 0) && (rpipe->pipe_state & PIPE_DIRECTW))
        kn->kn_data = rpipe->pipe_map.cnt;

    if ((rpipe->pipe_state & PIPE_EOF) ||
        (wpipe == NULL) || (wpipe->pipe_state & PIPE_EOF)) {
        kn->kn_flags |= EV_EOF;
        if ((hint & NOTE_SUBMIT) == 0) {
            mutex_exit(rpipe->pipe_lock);
        }
// powiadamiamy o eof/ lub nie mamy gdzie pisać
        return (1);
    }

    if ((hint & NOTE_SUBMIT) == 0) {
        mutex_exit(rpipe->pipe_lock);
    }
// jeśli kn_data to powiadamiamy
    return (kn->kn_data > 0);
}
```

```c
/*
 * Queue new event for knote.
 */
static void
knote_activate(struct knote *kn)
{
    // jeśli mamy aktywować knote to w skrócie musimy go wrzucić do odpowiedniej kolejki
    struct kqueue *kq;

    // sanity check
    KASSERT((kn->kn_status & KN_MARKER) == 0);
    // szukana kolejka
    kq = kn->kn_kq;

    mutex_spin_enter(&kq->kq_lock);
    // ustawianie statusu
    kn->kn_status |= KN_ACTIVE;
    // jeśli nie zakolejkowaliśmy lub nie jest wyłączony to kolejkujemy
    if ((kn->kn_status & (KN_QUEUED | KN_DISABLED)) == 0) {
        // sprawdzenie kolejki
        kq_check(kq);
        // oznaczamy, że zakolejkowaliśmy
        kn->kn_status |= KN_QUEUED;
        TAILQ_INSERT_TAIL(&kq->kq_head, kn, kn_tqe);
        // zwiększamy liczbe elementów w kolejce
        kq->kq_count++;
        kq_check(kq);
        // rozgłaszamy conditional variable
        cv_broadcast(&kq->kq_cv);
        selnotify(&kq->kq_sel, 0, NOTE_SUBMIT);
    }
    mutex_spin_exit(&kq->kq_lock);
}
```

## przykład kevent
```c
#include <sys/event.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   /* for strerror() */
#include <unistd.h>

/* function prototypes */
void diep(const char *s);

int main(void)
{
   struct kevent change;    /* event we want to monitor */
   struct kevent event;     /* event that was triggered */
   pid_t pid;
   int kq, nev;

   /* create a new kernel event queue */
   if ((kq = kqueue()) == -1)
      diep("kqueue()");

   /* initalise kevent structure */
   // ustawiamy, że chcemy się wybudzać co 5 sekund (5000)
   // EV_ADD - dodajemy event do kqueue
   // EV_ENABLE - oznaczamy, że event ma być włączony
   EV_SET(&change, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, 5000, 0);

   /* loop forever */
   for (;;) {
      nev = kevent(kq, &change, 1, &event, 1, NULL);

      if (nev < 0)
      // zasze może wystąpić jakiś kevent error
         diep("kevent()");

      else if (nev > 0) {
        // sprawdzamy czy nie ma jakiegoś błędu
         if (event.flags & EV_ERROR) {   /* report any error */
            fprintf(stderr, "EV_ERROR: %s\n", strerror(event.data));
            exit(EXIT_FAILURE);
         }

        // mało to nas obchodzi
         if ((pid = fork()) < 0)         /* fork error */
            diep("fork()");

         else if (pid == 0)     /* child */
         // wypisujemy czas jaki mamy
            if (execlp("date", "date", (char *)0) < 0)
               diep("execlp()");
      }
   }

    // kiedyś tu dojdziemy :)
    // zamykamy kolejke
   close(kq);
   return EXIT_SUCCESS;
}

void diep(const char *s)
{
   perror(s);
   exit(EXIT_FAILURE);
}
```
