 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#include "ibmras/common/util/sysUtils.h"

#if defined(WINDOWS)
	#include <windows.h>
//    #include <winsock2.h>
	#include <Psapi.h>
#elif defined(LINUX)
#include <sys/time.h>
#elif defined(AIX)
#include <sys/time.h>
#elif defined(_ZOS)
#define _XOPEN_SOURCE_EXTENDED 1
#undef _ALL_SOURCE
#include <sys/time.h>
#endif

#include <ctime>


namespace ibmras {
namespace common {
namespace util {

unsigned long long getMilliseconds() {
	unsigned long long millisecondsSinceEpoch;
#if defined(WINDOWS)

	SYSTEMTIME st;
	GetSystemTime(&st);

	millisecondsSinceEpoch = time(NULL)*1000+st.wMilliseconds;

#else
		struct timeval tv;
	gettimeofday(&tv, NULL);

	millisecondsSinceEpoch =
	    (unsigned long long)(tv.tv_sec) * 1000 +
	    (unsigned long long)(tv.tv_usec) / 1000;
#endif
	return millisecondsSinceEpoch;
}

}/*end of namespace util*/
}/*end of namespace common*/
} /*end of namespace ibmras*/


