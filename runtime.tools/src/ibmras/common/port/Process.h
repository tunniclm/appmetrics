/**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
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
