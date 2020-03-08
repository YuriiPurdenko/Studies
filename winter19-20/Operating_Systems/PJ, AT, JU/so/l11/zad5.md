Jeśli nasz kod komunikuje się bezpośrednio z hardware, procesami na innych
rdzeniach, modyfikuje page table musimy martwić się o kolejność.

weakly-ordered model of memory -  the order of memory accesses is not required
to be the same as the program order for load and store operations. The 
processor is able to re-order memory read operations with respect to each other.
Writes may also be re-ordered (for example, write combining) .As a result,
hardware optimizations, such as the use of cache and write buffer, function in
a way that improves the performance of the processor, which means that the
required bandwidth between the processor and external memory can be reduced and
the long latencies associated with such external memory accesses are hidden.

Jakie mamy możliwości re-orderingu
* Multiple issue of instructions
* Out-of-order execution
* Speculation
* Speculative loads
* Load and store optimizations
* External memory systems
* Cache coherent multi-core processing
* Optimizing compilers


Mamy dwie rodzaje pamięci na ARMie
Normal -- cachowana itd...
Device -- nie jest cachowana

Rodzaje barier

* Istruction Synchronization Barrier (ISB)
    
    This is used to guarantee that any subsequent instructions are fetched, again, so that privilege and access are checked with the current MMU configuration. It is used to ensure any previously executed context-changing operations, such as writes to system control registers, have completed by the time the ISB completes. In hardware terms, this might mean that the instruction pipeline is flushed, for example. Typical uses of this would be in memory management, cache control, and context switching code, or where code is being moved about in memory.

* Data Memory Barrier (DMB)

    This prevents re-ordering of data accesses instructions across the barrier instruction. All data accesses, that is, loads or stores, but not instruction fetches, performed by this processor before the DMB, are visible to all other masters within the specified shareability domain before any of the data accesses after the DMB.

* Data Synchronization Barrier (DSB)

    This enforces the same ordering as the Data Memory Barrier, but has the additional effect of blocking execution of any further instructions, not just loads or stores, or both, until synchronization is complete. This can be used to prevent execution of a SEV instruction, for instance, that would signal to other cores that an event occurred. It waits until all cache, TLB and branch predictor maintenance operations issued by this processor have completed for the specified shareability domain.


Dodałbym data sync barrier przed while aby nie usunął sobie flag[id]
