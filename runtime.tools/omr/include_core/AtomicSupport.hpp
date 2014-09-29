/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#if !defined(ATOMIC_SUPPORT_HPP_)
#define ATOMIC_SUPPORT_HPP_

/* Hide the compiler specific bits of this file from DDR since it parses with GCC. */
/* TODO : there should be a better #define for this! */
#if defined(TYPESTUBS_H)
#define ATOMIC_SUPPORT_STUB
#endif /* defined(TYPESTUBS_H) */


#include <stdlib.h>
#if !defined(ATOMIC_SUPPORT_STUB)
#if defined(J9ZOS39064)
/* need this for __csg() */
#include <builtins.h>
#endif
#if defined(AIXPPC)||defined(LINUXPPC)
#if defined(__xlC__)
#include <builtins.h>
/* 
   Bytecode for: or 27,27,27 
   provides a hint that performance will probably be improved if shared resources dedicated
   to the executing processor are released for use by other processors. 
*/
#pragma mc_func __ppc_yield  {"7F7BDB78"}
/*
   Bytecode for: or 1,1,1
   Lower SMT thread priority.
*/
#pragma mc_func __ppc_dropSMT  {"7C210B78"}
/*
   Bytecode for: or 2,2,2
   Restore SMT thread priority
*/
#pragma mc_func __ppc_restoreSMT  {"7C421378"}
/* Bytecode for: nop */
#pragma mc_func __ppc_nop  {"60000000"}
#endif /* #if defined(__xlC__) */
#endif /* defined(AIXPPC)||defined(LINUXPPC) */

#if defined(_MSC_VER)
#include <intrin.h>
#endif /* defined(_MSC_VER) */

#endif /* !defined(ATOMIC_SUPPORT_STUB) */

#include "util_core_api.h"

#if defined(J9ZOS390)
extern "C" void J9ZOSRWB(void);
#endif /* defined(J9ZOS390) */

/*
 * Platform dependent synchronization functions.
 * Do not call directly, use the methods from VM_AtomicSupport.
 */
#if !defined(ATOMIC_SUPPORT_STUB)
#if defined(LINUXPPC) || defined(AIXPPC)
#if defined(__xlC__)
		inline void __yield() { __ppc_yield(); }
		inline void __dropSMT() { __ppc_dropSMT(); }
		inline void __restoreSMT() { __ppc_restoreSMT(); }
#else
		inline void __yield() { __asm__ volatile ("or 27,27,27"); }
		inline void __dropSMT() {  __asm__ volatile ("or 1,1,1"); }
		inline void __restoreSMT() {  __asm__ volatile ("or 2,2,2"); }
#endif
#elif defined(_MSC_VER)
		inline void __yield() { _mm_pause(); }
#elif defined(__GNUC__) && (defined(J9X86) || defined(J9HAMMER))
		inline void __yield() { __asm volatile ("pause"); }
#else
		inline void __yield() { __asm volatile ("# AtomicOperations::__yield"); }
#endif /* __GNUC__ && (J9X86 || J9HAMMER) */

#if defined(_MSC_VER)
		/* use compiler intrinsic */
#elif defined(LINUXPPC) || defined(AIXPPC)
#if defined(__xlC__)
		/* XL compiler complained about generated assembly, use machine code instead. */
		inline void __nop() { __ppc_nop(); }
#else
		inline void __nop() { __asm__ volatile ("nop"); }
#endif
#elif defined(LINUX) && (defined(S390) || defined(S39064))
		/*
		 * nop instruction requires operand https://bugzilla.redhat.com/show_bug.cgi?id=506417
		 */
		inline void __nop() { __asm__ volatile ("nop 0"); }
#else /* GCC && XL */
		inline void __nop() { __asm__ volatile ("nop"); }
#endif

#if defined(AIXPPC) || defined(LINUXPPC)
#if !defined(__xlC__)
	/* GCC compiler does not provide the same intrinsics. */

	/**
	 * Synchronize
	 * Ensures that all instructions preceding the function the call to __sync complete before any instructions following
	 * the function call can execute.
	 */
	inline void __sync() { asm volatile ("sync"); }
	/**
	 * Instruction Synchronize
	 * Waits for all previous instructions to complete and then discards any prefetched instructions, causing subsequent
	 * instructions to be fetched (or refetched) and executed in the context established by previous instructions.
	 */
	inline void __isync() {	asm volatile ("isync");	}
	/**
	 * Load Word Synchronize
	 * Ensures that all store instructions preceding the call to __lwsync complete before any new instructions can be executed
	 * on the processor that executed the function. This allows you to synchronize between multiple processors with minimal
	 * performance impact, as __lwsync does not wait for confirmation from each processor.
	 */
	inline void __lwsync() { asm volatile ("lwsync"); }
#endif /* !defined(__xlC__) */
#endif /* AIXPPC || LINUXPPC */
#endif /* !defined(ATOMIC_SUPPORT_STUB) */

/**
 * Provide atomic access to data store.
 */
class VM_AtomicSupport
{
public:

	/**
	 * If the CPU supports it, emit an instruction to yield the CPU to another thread.
	 */
	VMINLINE static void
	yieldCPU()
	{
#if !defined(ATOMIC_SUPPORT_STUB)
		__yield();
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

	/**
	 * Generates platform-specific machine code that performs no operation.
	 */
	VMINLINE static void
	nop()
	{
#if !defined(ATOMIC_SUPPORT_STUB)
		__nop();
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

	/**
	 * Creates a memory barrier.
	 * On a given processor, any load or store instructions ahead
	 * of the sync instruction in the program sequence must complete their accesses to memory
	 * first, and then any load or store instructions after sync can begin.
	 */
	VMINLINE static void
	readWriteBarrier()
	{
#if !defined(ATOMIC_SUPPORT_STUB)
#if defined(AIXPPC) || defined(LINUXPPC)
		__sync();
#elif defined(_MSC_VER)
		_ReadWriteBarrier();
#elif defined(__GNUC__)
#if defined(J9X86)
		asm volatile("lock orl $0x0,(%%esp)" ::: "memory");
#elif defined(J9HAMMER)
		asm volatile("lock orq $0x0,(%%rsp)" ::: "memory");
#elif defined(S390) /* defined(J9HAMMER) */
		asm volatile("bcr 15,0":::"memory");
#else /* defined(S390) */
		asm volatile("":::"memory");
#endif /* defined(J9X86) || defined(J9HAMMER) */
#elif defined(J9ZOS390)
		/* Replace this with an inline "bcr 15,0" whenever possible */
		J9ZOSRWB();
		__fence();
#endif /* defined(AIXPPC) || defined(LINUXPPC) */
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

	/**
	 * Creates a store barrier.
	 * Provides the same ordering function as the sync instruction, except that a load caused
	 * by an instruction following the storeSync may be performed before a store caused by
	 * an instruction that precedes the storeSync, and the ordering does not apply to accesses
	 * to I/O memory (memory-mapped I/O).
	 */
	VMINLINE static void
	writeBarrier()
	{
#if !defined(ATOMIC_SUPPORT_STUB)
#if defined(AIXPPC) || defined(LINUXPPC)
		__lwsync();
#elif defined(_MSC_VER)
		_WriteBarrier();
#elif defined(__GNUC__)
#if defined(J9X86) || defined(J9HAMMER)
		asm volatile("":::"memory");
		/* TODO investigate whether or not we should call this
		asm volatile("sfence" ::: "memory");
		*/
#else /* defined(J9X86) || defined(J9HAMMER) */
		asm volatile("":::"memory");
#endif /* defined(J9X86) || defined(J9HAMMER) */
#elif defined(J9ZOS390)
		__fence();
#endif /* defined(AIXPPC) || defined(LINUXPPC) */
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

	/**
	 * Creates a load barrier.
	 * Causes the processor to discard any prefetched (and possibly speculatively executed)
	 * instructions and refetch the next following instructions. It is used to ensure that
	 * no loads following entry into a critical section can access data (because of aggressive
	 * out-of-order and speculative execution in the processor) before the lock is acquired.
	 */
	VMINLINE static void
	readBarrier()
	{
#if !defined(ATOMIC_SUPPORT_STUB)
#if defined(AIXPPC) || defined(LINUXPPC)
		__isync();
#elif defined(_MSC_VER)
		_ReadBarrier();
#elif defined(__GNUC__)
#if defined(J9X86) || defined(J9HAMMER)
		asm volatile("mfence":::"memory");
		/* TODO investigate whether or not we should call this instead
		asm volatile("lfence":::"memory");
		*/
#else /* defined(J9X86) || defined(J9HAMMER) */
		asm volatile("":::"memory");
#endif /* defined(J9X86) || defined(J9HAMMER) */
#elif defined(J9ZOS390)
		__fence();
#endif /* defined(AIXPPC) || defined(LINUXPPC) */
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

	/**
	 * Creates a load barrier for use in monitor enter.
	 *
	 * Causes the processor to discard any prefetched (and possibly speculatively executed)
	 * instructions and refetch the next following instructions. It is used to ensure that
	 * no loads following entry into a critical section can access data (because of aggressive
	 * out-of-order and speculative execution in the processor) before the lock is acquired.
	 *
	 * This differs from readBarrier() in that it will break any memory transaction which is being
	 * used to avoid locking a monitor.
	 */
	VMINLINE static void
	monitorEnterBarrier()
	{
#if defined(AIXPPC) || defined(LINUXPPC)
		readWriteBarrier();
#else /* defined(AIXPPC) || defined(LINUXPPC) */
		readBarrier();
#endif /* defined(AIXPPC) || defined(LINUXPPC) */
	}

	/**
	 * Store unsigned 32 bit value at memory location as an atomic operation.
	 * Compare the unsigned 32 bit value at memory location pointed to by <b>address</b>.  If it is
	 * equal to <b>oldValue</b> then update this memory location with <b>newValue</b>
	 * else retain the <b>oldValue</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param oldValue The expected value at memory address
	 * @param newValue The new value to be stored at memory address
	 *
	 * @return the value at memory location <b>address</b> BEFORE the store was attempted
	 */
	VMINLINE static U_32
	lockCompareExchangeU32(volatile U_32 *address, U_32 oldValue, U_32 newValue)
	{
#if !defined(ATOMIC_SUPPORT_STUB)
		/* These are all 32bit specializations */
#if defined(_MSC_VER)
		/* Note: This did not work correctly under MSVC6 using debug.  It appeared that not all of the lock cmpxchg's were emitted.
		 * They looked to be optimized out, regardless of the lower opt level, and the volatile variables.
		 */
		return (U_32)_InterlockedCompareExchange((volatile long *)address, (long)newValue, (long)oldValue);
#elif defined(__GNUC__) && (defined(J9X86) || defined(J9HAMMER)) /* defined(_MSC_VER) */
#if __GNUC__ >= 4
		return __sync_val_compare_and_swap(address, oldValue, newValue);
#else /* __GNUC__ >= 4 */
		/* HRT still uses a 3.x version of GCC */
		__asm ("lock cmpxchgl %2, %3"
			: "=a" (oldValue), "+m" (*address)
			: "q" (newValue), "m" (*address), "0" (oldValue)
			: "cc");
		return oldValue;
#endif  /* __GNUC__ >= 4 */
#elif defined(J9ZOS390) /* defined(__GNUC__) && (defined(J9X86) || defined(J9HAMMER)) */
		/* V1.R13 has a compiler bug and if you pass a constant as oldValue it will cause c-stack corruption */
		volatile U_32 old = oldValue;
		/* 390 cs() function defined in <stdlib.h>, doesn't expand properly to __cs1() which correctly deals with aliasing */
		__cs1((U_32 *)&old, (U_32 *)address, (U_32 *)&newValue);
		return old;
#elif defined(LINUX) && defined(S390) /* defined J9ZOS390 */
		return __sync_val_compare_and_swap(address, oldValue, newValue);
#elif defined(AIXPPC) || defined(LINUXPPC) /* defined(LINUX) && defined(S390) */
#if defined(__xlC__)
		__compare_and_swap((volatile int*)address, (int*)&oldValue, (int)newValue);
		return oldValue;
#else /* defined(__xlC__) */
		return __sync_val_compare_and_swap(address, oldValue, newValue);
#endif /* defined(__xlC__) */
#else /* defined(AIXPPC) || defined(LINUXPPC) */
#error "lockCompareExchangeU32(): unsupported platform!"
#endif /* defined(_MSVC_VER) */
#else  /* !defined(ATOMIC_SUPPORT_STUB) */
		return 0;
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

	/**
	 * Store value at memory location as an atomic operation.
	 * Compare the value at memory location pointed to by <b>address</b>.  If it is
	 * equal to <b>oldValue</b> then update this memory location with <b>newValue</b>
	 * else retain the <b>oldValue</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param oldValue The expected value at memory address
	 * @param newValue The new value to be stored at memory address
	 *
	 * @return the value at memory location <b>address</b> BEFORE the store was attempted
	 */
#if defined(_MSC_VER)
/* Implicit return in eax, not seen by compiler.  Disable compile warning C4035: no return value */
#pragma warning(disable:4035)
#endif /* _MSC_VER */
	VMINLINE static UDATA
	lockCompareExchange(volatile UDATA * address, UDATA oldValue, UDATA newValue)
	{
#if !defined(ATOMIC_SUPPORT_STUB)
		/* If we aren't 64 bit (ie: UDATA == U_32) then use the 32 bit specializations */
#if !defined(J9VM_ENV_DATA64)
		return (UDATA)lockCompareExchangeU32((volatile U_32 *)address, (U_32)oldValue, (U_32)newValue);
#else /* !defined(J9VM_ENV_DATA64) */

		/* These are only non 32bit UDATA specializations */
#if defined(_MSC_VER)
		/* Note: This did not work correctly under MSVC6 using debug.  It appeared that not all of the lock cmpxchg's were emitted.
		 * They looked to be optimized out, regardless of the lower opt level, and the volatile variables.
		 */
		return (UDATA)_InterlockedCompareExchange64((volatile IDATA *)address, (IDATA)newValue, (IDATA)oldValue);
#elif defined(__GNUC__) && defined(J9HAMMER) /* defined(_MSC_VER) */
		 return __sync_val_compare_and_swap(address, oldValue, newValue);
#elif defined(J9ZOS39064) /* defined(__GNUC__) && defined(J9HAMMER) */
		 /* V1.R13 has a compiler bug and if you pass a constant as oldValue it will cause c-stack corruption */
		 volatile UDATA old = oldValue;
		 /* Call __csg directly as csg() does not exist */
		__csg((UDATA *)&old, (UDATA *)address, (UDATA *)&newValue);
		return old;
#elif defined(LINUX) && defined(S39064)  /* defined(J9ZOS39064) */
		return __sync_val_compare_and_swap(address, oldValue, newValue);
#elif (defined(AIXPPC) || defined(LINUXPPC)) && defined(PPC64) /* defined(LINUX) && defined(S39064) */
#if defined(__xlC__)
		__compare_and_swaplp((volatile long*)address, (long*)&oldValue, (long)newValue);
		return oldValue;
#else /* defined(__xlC__) */
		return __sync_val_compare_and_swap(address, oldValue, newValue);
#endif /* defined(__xlC__) */
#else /* (defined(AIXPPC) || defined(LINUXPPC)) && defined(PPC64) */
#error "lockCompareExchange(): unsupported platform!"
#endif /* defined(_MSC_VER) */
#endif /* !defined(J9VM_ENV_DATA64) */
#else  /* !defined(ATOMIC_SUPPORT_STUB) */
		return 0;
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

	/**
	 * Store unsigned 64 bit value at memory location as an atomic operation.
	 * Compare the unsigned 64 bit value at memory location pointed to by <b>address</b>.  If it is
	 * equal to <b>oldValue</b> then update this memory location with <b>newValue</b>
	 * else retain the <b>oldValue</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param oldValue The expected value at memory address
	 * @param newValue The new value to be stored at memory address
	 *
	 * @return the value at memory location <b>address</b> BEFORE the store was attempted
	 */
	VMINLINE static U_64
	lockCompareExchangeU64(volatile U_64 *address, U_64 oldValue, U_64 newValue)
	{
#if !defined(ATOMIC_SUPPORT_STUB)
		/* On 64-bit platforms, call the UDATA-sized operation. It should probably be the other
		 * way around, but it shouldn't be changed until there is a complete inline implementation
		 * here
		 */
#if defined(J9VM_ENV_DATA64)
		return lockCompareExchange((volatile UDATA *)address, (UDATA)oldValue, (UDATA)newValue);
#else /* J9VM_ENV_DATA64 */
#if defined(_MSC_VER)
		return (U_64)_InterlockedCompareExchange64((volatile __int64 *)address, (__int64)newValue, (__int64)oldValue);
#elif defined(__GNUC__) && defined(J9X86) /* defined(_MSC_VER) */
#if ( __GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 2))
		return __sync_val_compare_and_swap(address, oldValue, newValue);
#else /* gcc >= 4.2 */
		/* Need to put newValue in ECX:EBX, but EBX is the PIC register
		 * so the compiler will not allow direct assignment to it.
		 * Manually preserve it, load to it, and restore.
		 */
		 __asm__ __volatile__(
				 "push %%ebx\n\t"
				 "movl (%1), %%ebx\n\t"
				 "movl 4(%1), %%ecx\n\t"
				 "lock; cmpxchg8b %2\n\t"
				 "pop %%ebx"

			/* outParam oldValue passed in EDX:EAX */
			: "=A" (oldValue)

			/* inParam &newValue passed in any register
			 * inParam address passed as an EA
			 * inParam oldValue as described above
			 */
			: "r" (&newValue), "m" (*address), "0" (oldValue)

			/* operation destroys ECX
		     * operation destroys EFLAGS
			 * operation affects memory
			 */
			: "ecx", "cc", "memory");
		return oldValue;
#endif /* gcc >= 4.2 */
#elif defined(J9ZOS390) /* defined(__GNUC__) && (defined(J9X86) */
		/* V1.R13 has a compiler bug and if you pass a constant as oldValue it will cause c-stack corruption */
		volatile U_64 old = oldValue;
		/* __cds1 does not write the swap value correctly, cds does the correct thing */
		cds((cds_t*)&old, (cds_t*)address, *(cds_t*)&newValue);
		return old;
#elif defined(LINUX) && defined(S390) /* defined(J9ZOS390) */
		return __sync_val_compare_and_swap(address, oldValue, newValue);
#elif defined(AIXPPC) || defined(LINUXPPC) /* defined(WIN32) */
		return J9CAS8Helper(address, ((U_32*)&oldValue)[1], ((U_32*)&oldValue)[0], ((U_32*)&newValue)[1], ((U_32*)&newValue)[0]);
#else /* defined(AIXPPC) || defined(LINUXPPC) */
#error "lockCompareExchangeU64(): unsupported platform!"
#endif /* defined(_MSC_VER) && defined(WIN32) */
#endif /* J9VM_ENV_DATA64 */
#else  /* !defined(ATOMIC_SUPPORT_STUB) */
		return 0;
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

#if defined(_MSC_VER)
#pragma warning(default:4035)/*re-enable warning C4035*/
#endif /* _MSC_VER */

	/**
	 * Add a number to the value at a specific memory location as an atomic operation.
	 * Adds the value <b>addend</b> to the value stored at memory location pointed
	 * to by <b>address</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param addend The value to be added
	 *
	 * @return The value at memory location <b>address</b> AFTER the add is completed
	 */
	VMINLINE static UDATA
	add(volatile UDATA *address, UDATA addend)
	{
		/* Stop compiler optimizing away load of oldValue */
		volatile UDATA *localAddr = address;
		UDATA oldValue;

		oldValue = (UDATA)*localAddr;
		while ((lockCompareExchange(localAddr, oldValue, oldValue + addend)) != oldValue) {
			oldValue = (UDATA)*localAddr;
		}
		return oldValue + addend;
	}

	/**
	 * AND a mask with the value at a specific memory location as an atomic operation.
	 * ANDs the value <b>mask</b> with the value stored at memory location pointed
	 * to by <b>address</b>.
	 *
	 * @param address The memory location to be updated
	 * @param mask The value to be added
	 *
	 * @return The value at memory location <b>address</b> BEFORE the AND is completed
	 */
	VMINLINE static UDATA
	bitAnd(volatile UDATA *address, UDATA mask)
	{
		/* Stop compiler optimizing away load of oldValue */
		volatile UDATA *localAddr = address;
		UDATA oldValue;

		oldValue = (UDATA)*localAddr;
		while ((lockCompareExchange(localAddr, oldValue, oldValue & mask)) != oldValue) {
			oldValue = (UDATA)*localAddr;
		}
		return oldValue;
	}

	/**
	 * OR a mask with the value at a specific memory location as an atomic operation.
	 * ORs the value <b>mask</b> with the value stored at memory location pointed
	 * to by <b>address</b>.
	 *
	 * @param address The memory location to be updated
	 * @param mask The value to be added
	 *
	 * @return The value at memory location <b>address</b> BEFORE the OR is completed
	 */
	VMINLINE static UDATA
	bitOr(volatile UDATA *address, UDATA mask)
	{
		/* Stop compiler optimizing away load of oldValue */
		volatile UDATA *localAddr = address;
		UDATA oldValue;

		oldValue = (UDATA)*localAddr;
		while ((lockCompareExchange(localAddr, oldValue, oldValue | mask)) != oldValue) {
			oldValue = (UDATA)*localAddr;
		}
		return oldValue;
	}

	/**
	 * Add a 32 bit number to the value at a specific memory location as an atomic operation.
	 * Adds the value <b>addend</b> to the value stored at memory location pointed
	 * to by <b>address</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param addend The value to be added
	 *
	 * @return The value at memory location <b>address</b>
	 */
	VMINLINE static UDATA
	addU32(volatile U_32 *address, U_32 addend)
	{
		/* Stop compiler optimizing away load of oldValue */
		volatile U_32 *localAddr = address;
		U_32 oldValue;

		oldValue = (U_32)*localAddr;
		while ((lockCompareExchangeU32(localAddr, oldValue, oldValue + addend)) != oldValue) {
			oldValue = (U_32)*localAddr;
		}
		return oldValue + addend;
	}

	/**
	 * Add a 64 bit number to the value at a specific memory location as an atomic operation.
	 * Adds the value <b>addend</b> to the value stored at memory location pointed
	 * to by <b>address</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param addend The value to be added
	 *
	 * @return The value at memory location <b>address</b>
	 */
	VMINLINE static U_64
	addU64(volatile U_64 *address, U_64 addend)
	{
		/* Stop compiler optimizing away load of oldValue */
		volatile U_64 *localAddr = address;
		U_64 oldValue;

		oldValue = (U_64)*localAddr;
		while ((lockCompareExchangeU64(localAddr, oldValue, oldValue + addend)) != oldValue) {
			oldValue = (U_64)*localAddr;
		}
		return oldValue + addend;
	}

	/**
	 * Add double float to the value at a specific memory location as an atomic operation.
	 * Adds the value <b>addend</b> to the value stored at memory location pointed
	 * to by <b>address</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param addend The value to be added
	 *
	 * @return The value at memory location <b>address</b>
	 */
	VMINLINE static double
	addDouble(volatile double *address, double addend)
	{
		/* double is stored as 64bit */
		/* Stop compiler optimizing away load of oldValue */
		volatile U_64 *localAddr = (volatile U_64 *)address;

		double oldValue = *address;
		double newValue =  oldValue + addend;

		while (lockCompareExchangeU64(localAddr, *(U_64 *)&oldValue, *(U_64 *)&newValue) != *(U_64 *)&oldValue) {
			oldValue = *address;
			newValue =  oldValue + addend;
		}
		return newValue;
	}

	/**
	 * Subtracts a number from the value at a specific memory location asn an atomic operation.
	 * Subtracts the value <b>value</b> from the value stored at memory location pointed
	 * to by <b>address</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param value The value to be subtracted
	 *
	 * @return The value at memory location <b>address</b>
	 */
	VMINLINE static UDATA
	subtract(volatile UDATA *address, UDATA value)
	{
		/* Stop compiler optimizing away load of oldValue */
		volatile UDATA *localAddr = address;
		UDATA oldValue;

		oldValue = (UDATA)*localAddr;
		while ((lockCompareExchange(localAddr, oldValue, oldValue - value)) != oldValue) {
			oldValue = (UDATA)*localAddr;
		}
		return oldValue - value;
	}

	/**
	 * Subtracts a 32 bit number from the value at a specific memory location asn an atomic operation.
	 * Subtracts the value <b>value</b> from the value stored at memory location pointed
	 * to by <b>address</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param value The value to be subtracted
	 *
	 * @return The value at memory location <b>address</b>
	 */
	VMINLINE static UDATA
	subtractU32(volatile U_32 *address, U_32 value)
	{
		/* Stop compiler optimizing away load of oldValue */
		volatile U_32 *localAddr = address;
		U_32 oldValue;

		oldValue = (U_32)*localAddr;
		while ((lockCompareExchangeU32(localAddr, oldValue, oldValue - value)) != oldValue) {
			oldValue = (U_32)*localAddr;
		}
		return oldValue - value;
	}

	/**
	 * Store value at memory location.
	 * Stores <b>value</b> at memory location pointed to be <b>address</b>.
	 *
	 * @param address The memory location to be updated
	 * @param lockingWord The location used to lock the slot for exchange
	 * @param value The value to be stored
	 *
	 * @return The value at memory location <b>address</b>
	 *
	 * @note This method can spin indefinately while attempting to write the new value.
	 */
	VMINLINE static UDATA
	set(volatile UDATA *address, UDATA value)
	{
		/* Stop compiler optimizing away load of oldValue */
		volatile UDATA *localAddr = address;
		UDATA oldValue;

		oldValue = (UDATA)*localAddr;
		while ((lockCompareExchange(localAddr, oldValue, value)) != oldValue) {
			oldValue = (UDATA)*localAddr;
		}
		return oldValue;
	}

	/**
	 * On PPC processors, lower the SMT thread priority.
	 */
	VMINLINE static void
	dropSMTThreadPriority()
	{
#if !defined(ATOMIC_SUPPORT_STUB)
#if defined(AIXPPC) || defined(LINUXPPC)
		__dropSMT();
#endif /* defined(AIXPPC) || defined(LINUXPPC) */
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

	/**
	 * On PPC processors, restore the SMT thread priority.
	 */
	VMINLINE static void
	restoreSMTThreadPriority()
	{
#if !defined(ATOMIC_SUPPORT_STUB)
#if defined(AIXPPC) || defined(LINUXPPC)
		__restoreSMT();
#endif /* defined(AIXPPC) || defined(LINUXPPC) */
#endif /* !defined(ATOMIC_SUPPORT_STUB) */
	}

	/**
	 * Return true approximately one time in frequency, which must be a power of 2.
	 * Processors which do not support timestamp reading always return true.
	 *
	 * @param[in] frequency
	 *
	 * @return see above
	 */
	VMINLINE static bool
	sampleTimestamp(UDATA frequency)
	{
		// TODO: https://jazz103.hursley.ibm.com:9443/jazz/resource/itemName/com.ibm.team.workitem.WorkItem/51150
		return true;
	}

};

#endif /* ATOMIC_SUPPORT_HPP_ */
