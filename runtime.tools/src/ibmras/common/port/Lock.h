 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#ifndef ibmras_common_port_lock_h
#define ibmras_common_port_lock_h

#include <vector>
#include <string>
#include "ibmras/export.h"

#ifndef NULL
#define NULL 0
#endif

#if defined(_WINDOWS)
#define LOCK_FAIL -1

#elif defined(_LINUX)
#define LOCK_FAIL -1

#elif defined(_AIX)

#elif defined(_ZOS)
#define LOCK_FAIL -1
#endif

namespace ibmras {
namespace common {
namespace port {

/* different type of lock functionality required by threads */
class DECL Lock {
public:
	Lock();													/* default constructor */
	int acquire();											/* acquire the lock associated with this class */
	int release();											/* release the lock */
	void destroy();											/* Detroy / release the platform lock */
	bool isDestroyed();										/* true if the underlygin platform lock has been destroyed */
	~Lock();												/* destructor to allow lock release */
private:
	void* lock;												/* platform lock structure */
};

}
}
}	/*end of namespace port */

#endif /* ibmras_common_port_lock_h */
