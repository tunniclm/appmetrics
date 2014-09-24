/*
 * Semaphore.h
 *
 *  Created on: 31 Jan 2014
 *      Author: adam
 */

#ifndef ibmras_common_port_semaphore_h
#define ibmras_common_port_semaphore_h

#include <vector>
#include <string>

#ifndef NULL
#define NULL 0
#endif

#include "ibmras/common/Logger.h"

namespace ibmras {
namespace common {
namespace port {

/* class to provide semaphore semantics */
class Semaphore {
public:
	Semaphore(uint32 initial, uint32 max);					/* semaphore initial and max count */
	void inc();												/* increase the semaphore count */
	bool wait(uint32 timeout);								/* decrement the semaphore count */
	~Semaphore();											/* OS cleanup of semaphore */
private:
	void* handle;											/* opaque handle to platform data structure */
};

}
}
}	/*end of namespace port */

#endif /* ibmras_common_port_semaphore_h */
