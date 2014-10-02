/* Auto-generated public header file */
#ifndef MMOMRHOOK_H
#define MMOMRHOOK_H
#include "j9hookable.h"

/* Begin declarations block */

	
#include "omr.h"
#include "objectdescription.h"

typedef UDATA (*condYieldFromGCFunctionPtr) (OMR_VMThread *omrVMThread, UDATA componentType);

	
/* End declarations block */

/* J9HOOK_MM_OMR_GLOBAL_GC_START

			DEPRECATED:  use J9HOOK_MM_OMR_GC_CYCLE_START or J9HOOK_MM_GLOBAL_GC_INCREMENT_START, instead.
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_GLOBAL_GC_START, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_GlobalGCStartEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_GLOBAL_GC_START 1
typedef struct MM_GlobalGCStartEvent {
	struct OMR_VMThread* currentThread;
	U_64 timestamp;
	UDATA eventid;
	UDATA globalGCCount;
	UDATA localGCCount;
	UDATA systemGC;
	UDATA aggressive;
	UDATA bytesRequested;
} MM_GlobalGCStartEvent;

/* J9HOOK_MM_OMR_GLOBAL_GC_END

			DEPRECATED:  use J9HOOK_MM_OMR_GC_CYCLE_END or J9HOOK_MM_GLOBAL_GC_INCREMENT_END, instead.
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_GLOBAL_GC_END, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_GlobalGCEndEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_GLOBAL_GC_END 2
typedef struct MM_GlobalGCEndEvent {
	struct OMR_VMThread* currentThread;
	U_64 timestamp;
	UDATA eventid;
	UDATA workStackOverflowOccured;
	UDATA workStackOverflowCount;
	UDATA workpacketCount;
	UDATA nurseryFreeBytes;
	UDATA nurseryTotalBytes;
	UDATA tenureFreeBytes;
	UDATA tenureTotalBytes;
	UDATA loaEnabled;
	UDATA tenureLOAFreeBytes;
	UDATA tenureLOATotalBytes;
	UDATA immortalFreeBytes;
	UDATA immortalTotalBytes;
	UDATA fixHeapForWalkReason;
	U_64 fixHeapForWalkTime;
} MM_GlobalGCEndEvent;

/* J9HOOK_MM_OMR_LOCAL_GC_START

			Triggered when a local GC (e.g. scavenge) is about to start.
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_LOCAL_GC_START, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_LocalGCStartEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_LOCAL_GC_START 3
typedef struct MM_LocalGCStartEvent {
	struct OMR_VMThread* currentThread;
	U_64 timestamp;
	UDATA eventid;
	UDATA globalGCCount;
	UDATA localGCCount;
} MM_LocalGCStartEvent;

/* J9HOOK_MM_OMR_LOCAL_GC_END

			Triggered when a local GC (e.g. scavenge) is completed.
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_LOCAL_GC_END, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_LocalGCEndEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_LOCAL_GC_END 4
typedef struct MM_LocalGCEndEvent {
	struct OMR_VMThread* currentThread;
	U_64 timestamp;
	UDATA eventid;
	void* subSpace;
	UDATA globalGCCount;
	UDATA localGCCount;
	UDATA rememberedSetOverflowed;
	UDATA causedRememberedSetOverflow;
	UDATA scanCacheOverflow;
	UDATA failedFlipCount;
	UDATA failedFlipBytes;
	UDATA failedTenureCount;
	UDATA failedTenureBytes;
	UDATA backout;
	UDATA flipCount;
	UDATA flipBytes;
	UDATA tenureCount;
	UDATA tenureBytes;
	UDATA tilted;
	UDATA nurseryFreeBytes;
	UDATA nurseryTotalBytes;
	UDATA tenureFreeBytes;
	UDATA tenureTotalBytes;
	UDATA loaEnabled;
	UDATA tenureLOAFreeBytes;
	UDATA tenureLOATotalBytes;
	UDATA tenureAge;
	UDATA totalMemorySize;
} MM_LocalGCEndEvent;

/* J9HOOK_MM_OMR_OOM_DUE_TO_SOFTMX

			Triggered when an OOM is about to occur due to the current softmx 
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_OOM_DUE_TO_SOFTMX, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_SoftmxOOMEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_OOM_DUE_TO_SOFTMX 5
typedef struct MM_SoftmxOOMEvent {
	struct OMR_VMThread* currentThread;
	U_64 timestamp;
	UDATA maxHeapSize;
	UDATA currentHeapSize;
	UDATA currentSoftMX;
	UDATA bytesRequired;
} MM_SoftmxOOMEvent;

/* J9HOOK_MM_OMR_COMPACT_END

			Triggered when a compact phase is completed.
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_COMPACT_END, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_CompactEndEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_COMPACT_END 6
typedef struct MM_CompactEndEvent {
	struct OMR_VMThread* omrVMThread;
	U_64 timestamp;
	UDATA eventid;
} MM_CompactEndEvent;

/* J9HOOK_MM_OMR_GC_CYCLE_START

			Triggered when the first increment of a global GC is about to start.  Note that this hook is triggered before the first corresponding phase start or increment start events.  This hook is still used on collectors which are not incremental.
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_GC_CYCLE_START, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_GCCycleStartEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_GC_CYCLE_START 7
typedef struct MM_GCCycleStartEvent {
	struct OMR_VMThread* omrVMThread;
	U_64 timestamp;
	UDATA eventid;
	struct MM_CommonGCData* commonData;
	UDATA cycleType;
} MM_GCCycleStartEvent;

/* J9HOOK_MM_OMR_GC_CYCLE_CONTINUE

			Triggered when cycle type changes.
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_GC_CYCLE_CONTINUE, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_GCCycleContinueEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_GC_CYCLE_CONTINUE 8
typedef struct MM_GCCycleContinueEvent {
	struct OMR_VMThread* omrVMThread;
	U_64 timestamp;
	UDATA eventid;
	struct MM_CommonGCData* commonData;
	UDATA oldCycleType;
	UDATA newCycleType;
} MM_GCCycleContinueEvent;

/* J9HOOK_MM_OMR_GC_CYCLE_END

			Triggered when the last increment of a global GC has completed.  Note that this hook is triggered after the last corresponding phase end or increment end events.  This hook is still used on collectors which are not incremental.
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_GC_CYCLE_END, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_GCCycleEndEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_GC_CYCLE_END 9
typedef struct MM_GCCycleEndEvent {
	struct OMR_VMThread* omrVMThread;
	U_64 timestamp;
	UDATA eventid;
	struct MM_CommonGCData* commonData;
	UDATA cycleType;
	condYieldFromGCFunctionPtr condYieldFromGCFunction;
} MM_GCCycleEndEvent;

/* J9HOOK_MM_OMR_INITIALIZED

			Triggered as soon as the GC is fully initialized
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_INITIALIZED, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_InitializedEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_INITIALIZED 10
typedef struct MM_InitializedEvent {
	struct OMR_VMThread* currentThread;
	U_64 timestamp;
	const char* gcPolicy;
	UDATA maxHeapSize;
	UDATA initialHeapSize;
	U_64 physicalMemory;
	UDATA numCPUs;
	UDATA gcThreads;
	const char* architecture;
	const char* os;
	const char* osVersion;
	UDATA compressedPointersShift;
	UDATA beat;
	UDATA timeWindow;
	UDATA targetUtilization;
	UDATA gcTrigger;
	UDATA headRoom;
	UDATA heapPageSize;
	const char* heapPageType;
	UDATA heapRequestedPageSize;
	const char* heapRequestedPageType;
	UDATA numaNodes;
	UDATA regionSize;
	UDATA regionCount;
	UDATA arrayletLeafSize;
} MM_InitializedEvent;

/* J9HOOK_MM_OMR_VERBOSE_GC_OUTPUT

			Triggered when verbose GC outputs a string
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_VERBOSE_GC_OUTPUT, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_VerboseGCOutputEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_VERBOSE_GC_OUTPUT 11
typedef struct MM_VerboseGCOutputEvent {
	struct OMR_VMThread* currentThread;
	U_64 timestamp;
	const char* string;
} MM_VerboseGCOutputEvent;

/* J9HOOK_MM_OMR_EXCESSIVEGC_RAISED

			Triggered when we are spending too much time in GC and we fail to reclaim sufficient free space on a global GC.
			The current thread is guaranteed to have VMAccess and ExclusiveVMAccess.  Neither of those states can be modified
			by users of this HOOK.
		

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_EXCESSIVEGC_RAISED, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_ExcessiveGCRaisedEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_EXCESSIVEGC_RAISED 12
typedef struct MM_ExcessiveGCRaisedEvent {
	struct OMR_VMThread* currentThread;
	U_64 timestamp;
	UDATA eventid;
	UDATA gcCount;
	float reclaimedPercent;
	float triggerPercent;
	UDATA excessiveLevel;
} MM_ExcessiveGCRaisedEvent;

/* J9HOOK_MM_OMR_OBJECT_DELETE
Report the deletion of an object. Hooking this event can significantly impact GC times.

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_OBJECT_DELETE, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_ObjectDeleteEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_OBJECT_DELETE 13
typedef struct MM_ObjectDeleteEvent {
	struct OMR_VMThread * currentThread;
	omrobjectptr_t object;
	void* heap;
} MM_ObjectDeleteEvent;

/* J9HOOK_MM_OMR_OBJECT_RENAME
Report the relocation of an object. Hooking this event can significantly impact GC times.

Example usage:
	(*hookable)->J9HookRegister(hookable, J9HOOK_MM_OMR_OBJECT_RENAME, eventOccurred, NULL);

	static void VMCALL
	eventOccurred(J9HookInterface** hookInterface, UDATA eventNum, void* voidData, void* userData)
	{
		MM_ObjectRenameEvent* eventData = voidData;
		. . .
	}
 */
#define J9HOOK_MM_OMR_OBJECT_RENAME 14
typedef struct MM_ObjectRenameEvent {
	struct OMR_VMThread * currentThread;
	omrobjectptr_t oldObject;
	omrobjectptr_t newObject;
} MM_ObjectRenameEvent;

#endif /* MMOMRHOOK_H */
