/*
 * Process.h
 *
 *  Created on: 15 Oct 2014
 *      Author: robbins
 */

#ifndef ibmras_common_port_process_h
#define ibmras_common_port_process_h
#include "ibmras/export.h"

namespace ibmras {
namespace common {
namespace port {

DECL int getProcessId();                              	/* get hold of the process id */
DECL std::string getHostName();                        	/* Return the host name*/

}
}
}


#endif /* ibmras_common_port_process_h */
