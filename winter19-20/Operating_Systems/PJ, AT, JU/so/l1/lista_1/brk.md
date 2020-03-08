brk służy do zmiany końca segmentu data dla programu

```
man brk
```

       brk()  and  sbrk() change the location of the program break, which defines the end of the process's data segment (i.e., the program break is the first location after the end of the uninitialized data
       segment).  Increasing the program break has the effect of allocating memory to the process; decreasing the break deallocates memory.

       brk() sets the end of the data segment to the value specified by addr, when that value is reasonable, the system has enough memory, and the process does not exceed its maximum data  size  (see  setr‐
       limit(2)).

       sbrk() increments the program's data space by increment bytes.  Calling sbrk() with an increment of 0 can be used to find the current location of the program break.

```
_start() 
```

```
__GI___sbrk
__GI___default_morecore
sysmalloc
_int_malloc
tcache_init
tcache_init

__alloc_dir
opendir_tail
```
