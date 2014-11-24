/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#ifndef ibmras_common_memorymanager_h
#define ibmras_common_memorymanager_h

/*
 * Memory management functionality
 */

#include "ibmras/common/types.h"

namespace ibmras {
namespace common {

class MemoryManager {

public:

	MemoryManager();
	virtual ~MemoryManager();

	virtual unsigned char* allocate(uint32 size);
	virtual void deallocate(unsigned char**);

protected:
private:

};

namespace memory {

MemoryManager* getDefaultMemoryManager();
bool setDefaultMemoryManager(MemoryManager* manager);

unsigned char* allocate(uint32 size);
void deallocate(unsigned char**);

}

}
}

#endif /* ibmras_common_memorymanager_h */
