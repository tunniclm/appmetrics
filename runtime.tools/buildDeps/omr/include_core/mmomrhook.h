/* Auto-generated public header file */
#ifndef MMOMRHOOK_H
#define MMOMRHOOK_H
#include "j9hookable.h"

/* Begin declarations block */

	
#include "omr.h"
#include "objectdescription.h"

typedef uintptr_t (*condYieldFromGCFunctionPtr) (OMR_VMThread *omrVMThread, uintptr_t componentType);

	
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
	uint64_t timestamp;
	uintptr_t eventid;
	uintptr_t globalGCCount;
	uintptr_t localGCCount;
	uintptr_t systemGC;
	uintptr_t aggressive;
	uintptr_t bytesRequested;
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
	uint64_t timestamp;
	uintptr_t eventid;
	uintptr_t workStackOverflowOccured;
	uintptr_t workStackOverflowCount;
	uintptr_t workpacketCount;
	uintptr_t nurseryFreeBytes;
	uintptr_t nurseryTotalBytes;
	uintptr_t tenureFreeBytes;
	uintptr_t tenureTotalBytes;
	uintptr_t loaEnabled;
	uintptr_t tenureLOAFreeBytes;
	uintptr_t tenureLOATotalBytes;
	uintptr_t immortalFreeBytes;
	uintptr_t immortalTotalBytes;
	uintptr_t fixHeapForWalkReason;
	uint64_t fixHeapForWalkTime;
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
	uint64_t timestamp;
	uintptr_t eventid;
	uintptr_t globalGCCount;
	uintptr_t localGCCount;
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
	uint64_t timestamp;
	uintptr_t eventid;
	void* subSpace;
	uintptr_t globalGCCount;
	uintptr_t localGCCount;
	uintptr_t rememberedSetOverflowed;
	uintptr_t causedRememberedSetOverflow;
	uintptr_t scanCacheOverflow;
	uintptr_t failedFlipCount;
	uintptr_t failedFlipBytes;
	uintptr_t failedTenureCount;
	uintptr_t failedTenureBytes;
	uintptr_t backout;
	uintptr_t flipCount;
	uintptr_t flipBytes;
	uintptr_t tenureCount;
	uintptr_t tenureBytes;
	uintptr_t tilted;
	uintptr_t nurseryFreeBytes;
	uintptr_t nurseryTotalBytes;
	uintptr_t tenureFreeBytes;
	uintptr_t tenureTotalBytes;
	uintptr_t loaEnabled;
	uintptr_t tenureLOAFreeBytes;
	uintptr_t tenureLOATotalBytes;
	uintptr_t tenureAge;
	uintptr_t totalMemorySize;
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
	uint64_t timestamp;
	uintptr_t maxHeapSize;
	uintptr_t currentHeapSize;
	uintptr_t currentSoftMX;
	uintptr_t bytesRequired;
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
	uint64_t timestamp;
	uintptr_t eventid;
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
	uint64_t timestamp;
	uintptr_t eventid;
	struct MM_CommonGCData* commonData;
	uintptr_t cycleType;
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
	uint64_t timestamp;
	uintptr_t eventid;
	struct MM_CommonGCData* commonData;
	uintptr_t oldCycleType;
	uintptr_t newCycleType;
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
	uint64_t timestamp;
	uintptr_t eventid;
	struct MM_CommonGCData* commonData;
	uintptr_t cycleType;
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
	uint64_t timestamp;
	const char* gcPolicy;
	uintptr_t maxHeapSize;
	uintptr_t initialHeapSize;
	uint64_t physicalMemory;
	uintptr_t numCPUs;
	uintptr_t gcThreads;
	const char* architecture;
	const char* os;
	const char* osVersion;
	uintptr_t compressedPointersShift;
	uintptr_t beat;
	uintptr_t timeWindow;
	uintptr_t targetUtilization;
	uintptr_t gcTrigger;
	uintptr_t headRoom;
	uintptr_t heapPageSize;
	const char* heapPageType;
	uintptr_t heapRequestedPageSize;
	const char* heapRequestedPageType;
	uintptr_t numaNodes;
	uintptr_t regionSize;
	uintptr_t regionCount;
	uintptr_t arrayletLeafSize;
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
	uint64_t timestamp;
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
	uint64_t timestamp;
	uintptr_t eventid;
	uintptr_t gcCount;
	float reclaimedPercent;
	float triggerPercent;
	uintptr_t excessiveLevel;
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
