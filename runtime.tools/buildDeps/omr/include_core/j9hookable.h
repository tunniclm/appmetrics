/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef J9HOOKABLE_H
#define J9HOOKABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "j9comp.h"
#include "j9cfg.h"
#include "j9thread.h"

#define J9_EVENT_IS_HOOKED(interface, event) (((uint8_t*)(&(interface).common + 1))[(event) & J9HOOK_EVENT_NUM_MASK] & J9HOOK_FLAG_HOOKED)
#define J9_EVENT_IS_RESERVED(interface, event) (((uint8_t*)(&(interface).common + 1))[(event) & J9HOOK_EVENT_NUM_MASK] & J9HOOK_FLAG_RESERVED)

/* derives the common interface pointer from a module-specific hook interface */
#define J9_HOOK_INTERFACE(interface) (&(interface).common.hookInterface)



struct J9HookInterface; /* Forward struct declaration */
typedef void (*J9HookFunction)(struct J9HookInterface** hookInterface, uintptr_t eventNum, void* eventData, void* userData); /* Forward struct declaration */
typedef struct J9HookInterface {
    void  ( *J9HookDispatch)(struct J9HookInterface** hookInterface, uintptr_t eventNum, void* eventData) ;
    intptr_t  ( *J9HookDisable)(struct J9HookInterface** hookInterface, uintptr_t eventNum) ;
    intptr_t  ( *J9HookReserve)(struct J9HookInterface** hookInterface, uintptr_t eventNum) ;
    intptr_t  ( *J9HookRegister)(struct J9HookInterface** hookInterface, uintptr_t eventNum, J9HookFunction function, void* userData, ...) ;
    void  ( *J9HookUnregister)(struct J9HookInterface** hookInterface, uintptr_t eventNum, J9HookFunction function, void* userData) ;
    void  ( *J9HookShutdownInterface)(struct J9HookInterface** hookInterface) ;
    intptr_t  ( *J9HookIsEnabled)(struct J9HookInterface** hookInterface, uintptr_t eventNum) ;
    uintptr_t  ( *J9HookAllocateAgentID)(struct J9HookInterface** hookInterface) ;
    void  ( *J9HookDeallocateAgentID)(struct J9HookInterface** hookInterface, uintptr_t agentID) ;
} J9HookInterface;


#define J9HOOK_ERR_DISABLED  -1
#define J9HOOK_ERR_NOMEM  -2
#define J9HOOK_ERR_INVALID_AGENT_ID  -3
#define J9HOOK_TAG_REVERSE_ORDER  0x10000000
#define J9HOOK_TAG_AGENT_ID  0x20000000
#define J9HOOK_TAG_COUNTED  0x40000000
#define J9HOOK_TAG_ONCE  0x80000000
#define J9HOOK_AGENTID_FIRST  ((uintptr_t)0)
#define J9HOOK_AGENTID_DEFAULT  ((uintptr_t)1)
#define J9HOOK_AGENTID_LAST  ((uintptr_t)-1)

typedef struct J9CommonHookInterface {
    struct J9HookInterface* hookInterface;
    uintptr_t size;
    j9thread_monitor_t lock;
    struct J9Pool* pool;
    uintptr_t nextAgentID;
} J9CommonHookInterface;


#define J9HOOK_FLAG_DISABLED  4
#define J9HOOK_EVENT_NUM_MASK  0xFFFF
#define J9HOOK_FLAG_HOOKED  1
#define J9HOOK_FLAG_RESERVED  2

typedef struct J9HookRecord {
    struct J9HookRecord* next;
    J9HookFunction function;
    void* userData;
    uintptr_t count;
    uintptr_t id;
    uintptr_t agentID;
} J9HookRecord;


/* magic hooks supported by every hook interface */

/*
 * J9HOOK_REGISTRATION_EVENT
 * Invoked every time a listener is succesfully registered or unregistered for an event (including this one).
 * No event is reported if the listener was not added since it was already registered.
 */
#define J9HOOK_REGISTRATION_EVENT 0
typedef struct J9HookRegistrationEvent {
	uintptr_t eventNum;
	J9HookFunction function;
	void* userData;
	uintptr_t isRegistration; /* true for register, false for unregister */
	uintptr_t agentID;
} J9HookRegistrationEvent;



#ifdef __cplusplus
}
#endif

#endif /* J9HOOKABLE_H */
