 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/plugins/jni/memory/MemoryDataProvider.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/vm/java/healthcenter.h"
#include <cstring>
#include <string>
#include <sstream>
#include "jni.h"
#include "ibmras/common/util/memUtils.h"
#include "ibmras/common/util/strUtils.h"
#include "ibmras/common/logging.h"

#include <ctime>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <sys/types.h>
#include <fcntl.h>

#include <iostream>

#if defined(WINDOWS)
#include <windows.h>
#include <Psapi.h>
#else /* Unix platforms */
#define _OE_SOCKETS
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

typedef size_t UDATA;
typedef long IDATA;

/*########################################################################################################################*/
/*########################################################################################################################*/
/*########################################################################################################################*/

typedef size_t UDATA;
typedef long IDATA;
#if defined(WINDOWS)
#include <windows.h>
#include <winbase.h>
#include <psapi.h>
#pragma comment(lib,"psapi.lib")
#endif /* defined(WIN32) || defined(WIN64) */

#if defined(LINUX)
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/prctl.h>
#endif

#if defined(AIXPPC)
#include <unistd.h>
#include <alloca.h>
#include <procinfo.h>
#include <sys/vminfo.h>
#include <sys/procfs.h>
#include <sys/resource.h>
#include <sys/types.h>
#if !defined(VMINFO_GETPSIZES)

#define VMINFO_GETPSIZES  102 /* report a system's supported page sizes */
#define VMINFO_PSIZE      103 /* report statistics for a page size */

struct vminfo_psize
{
	psize_t psize; /* IN: page size                        */

	/* The rest of this struct is output from vmgetinfo()           */

	uint64_t attr; /* bitmap of page size's attributes     */

	/* Page size attributes reported in the vminfo_psize.attr field: */
#define VM_PSIZE_ATTR_PAGEABLE       0x1  /* page size supports paging  */

	uint64_t pgexct; /* count of page faults                 */
	uint64_t pgrclm; /* count of page reclaims               */
	uint64_t lockexct; /* count of lockmisses                  */
	uint64_t backtrks; /* count of backtracks                  */
	uint64_t pageins; /* count of pages paged in              */
	uint64_t pageouts; /* count of pages paged out             */
	uint64_t pgspgins; /* count of page ins from paging space  */
	uint64_t pgspgouts; /* count of page outs from paging space */
	uint64_t numsios; /* count of start I/Os                  */
	uint64_t numiodone; /* count of iodones                     */
	uint64_t zerofills; /* count of zero filled pages           */
	uint64_t exfills; /* count of exec filled pages           */
	uint64_t scans; /* count of page scans by clock         */
	uint64_t cycles; /* count of clock hand cycles           */
	uint64_t pgsteals; /* count of page steals                 */
	uint64_t freewts; /* count of free frame waits            */
	uint64_t extendwts; /* count of extend XPT waits            */
	uint64_t pendiowts; /* count of pending I/O waits           */

	/*
	 * the next fields need to be computed by vmgetinfo
	 * system call, else their value will be inaccurate.
	 */
	rpn64_t numframes; /* # of real memory frames of this psize */
	rpn64_t numfrb; /* number of pages on free list */
	rpn64_t numclient; /* number of client frames */
	rpn64_t numcompress; /* no of frames in compressed segments */
	rpn64_t numperm; /* number frames non-working segments */
	rpn64_t numvpages; /* accessed virtual pages */
	rpn64_t minfree; /* minimun pages free list (fblru) */
	rpn64_t maxfree; /* maxfree pages free list (fblru) */
#ifndef RPTYPES
#define RPTYPES         2
#endif
	rpn64_t rpgcnt[RPTYPES];/* repaging cnt */
	rpn64_t numpout; /* number of fblru page-outs        */

	rpn64_t numremote; /* number of fblru remote page-outs */
	rpn64_t numwseguse; /* count of pages in use for working seg */
	rpn64_t numpseguse; /* count of pages in use for persistent seg */
	rpn64_t numclseguse; /* count of pages in use for client seg */
	rpn64_t numwsegpin; /* count of pages pinned for working seg */
	rpn64_t numpsegpin; /* count of pages pinned for persistent seg */
	rpn64_t numclsegpin; /* count of pages pinned for client seg */
	rpn64_t numpgsp_pgs; /* # of wseg pages with allocated paging space */

	rpn64_t numralloc; /* number of remote allocations */
	rpn64_t pfrsvdblks; /* number of system reserved blocks */
	rpn64_t pfavail; /* number of pages available for pinning */
	rpn64_t pfpinavail; /* app-level num pages avail for pinning */
	rpn64_t numpermio; /* number of fblru non-w.s. pageouts    */

	rpn64_t system_pgs; /* pages on SCBs marked V_SYSTEM        */
	rpn64_t nonsys_pgs; /* pages on SCBs not marked V_SYSTEM    */
};

#endif /* !defined(VMINFO_GETPSIZES) */
#endif

using namespace ibmras::monitoring::plugins::jni;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {
namespace memory {

IBMRAS_DEFINE_LOGGER("MemoryDataProvider");

jlong getProcessPhysicalMemorySize(JNIEnv* env);
jlong getProcessPrivateMemorySize(JNIEnv* env);
jlong getProcessVirtualMemorySize(JNIEnv* env);
jlong getFreePhysicalMemorySize(JNIEnv* env);

const std::string COMMA = ","; //$NON-NLS-1$
const std::string EQUALS = "="; //$NON-NLS-1$

const std::string PHYSICAL_MEMORY = "physicalmemory";
const std::string PRIVATE_MEMORY = "privatememory";
const std::string VIRTUAL_MEMORY = "virtualmemory"; //$NON-NLS-1$
const std::string FREE_PHYSICAL_MEMORY = "freephysicalmemory"; //$NON-NLS-1$
const std::string TOTAL_PHYSICAL_MEMORY = "totalphysicalmemory"; //$NON-NLS-1$

MEMPullSource* src = NULL;
bool enabled = true;
bool available = true;

PullSource* getMEMPullSource() {
	if (!src) {
		src = new MEMPullSource;
	}
	return src;
}


MEMPullSource::MEMPullSource() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();
	std::string osName = agent->getProperty("os.name");

	// Avoid crash on System i V5R4
	if (ibmras::common::util::equalsIgnoreCase(osName, "i5/OS") || ibmras::common::util::equalsIgnoreCase(osName, "OS/400")) {
	/*
	 * For now no version of IBM i supports the call we make to get native memory
	 * information. If this changes in the future we'll need to add a further check for the
	 * specific os version here
	 */
		available = false;
	}
}

monitordata* callback() {
	return src->PullSource::generateData();
}

bool MEMPullSource::isEnabled() {
	return enabled && available;
}


void MEMPullSource::publishConfig() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	ibmras::monitoring::connector::ConnectorManager *conMan =
			agent->getConnectionManager();

	std::string msg = "memory_subsystem=";
	if (isEnabled()) {
		msg += "on";
	} else {
		msg += "off";
	}

	conMan->sendMessage("configuration/memory", msg.length(),
			(void*) msg.c_str());
}

void MEMPullSource::setState(const std::string &newState) {
	enabled = ibmras::common::util::equalsIgnoreCase(newState, "on");

	// publish config when state changes
	getMEMPullSource()->publishConfig();
}

uint32 MEMPullSource::getSourceID() {
	return MEM;
}

pullsource* MEMPullSource::getDescriptor() {

	pullsource* src = new pullsource();
	src->header.name = "memory";
	src->header.description = "Memory information";
	src->header.sourceID = MEM;
	src->header.capacity = 8 * 1024;
	src->next = NULL;
	src->callback = callback;
	src->complete = ibmras::monitoring::plugins::jni::complete;
	src->pullInterval = 5;

	return src;
}

monitordata* MEMPullSource::sourceData(jvmFunctions* tdpp, JNIEnv* env) {

	IBMRAS_DEBUG(debug, ">>MEMPullSource::sourceData");
	monitordata* data = new monitordata;
	data->size = 0;
	data->data = NULL;

	if (isEnabled()) {
		data->persistent = false;
		data->provID = getProvID();
		data->sourceID = MEM;

		std::string cp = getString(env,
				"com/ibm/java/diagnostics/healthcenter/agent/dataproviders/memory/MemoryDataProvider",
				"getJMXData", "()Ljava/lang/String;");
		std::stringstream ss;

		//ss << std::endl;
		ss << cp;
		ss << PHYSICAL_MEMORY << EQUALS << getProcessPhysicalMemorySize(env)
				<< COMMA;
		ss << PRIVATE_MEMORY << EQUALS << getProcessPrivateMemorySize(env)
				<< COMMA;
		ss << VIRTUAL_MEMORY << EQUALS << getProcessVirtualMemorySize(env)
				<< COMMA;
		ss << FREE_PHYSICAL_MEMORY << EQUALS << getFreePhysicalMemorySize(env)
				<< std::endl;

		std::string memorydata = ss.str();

		jsize len = memorydata.length();
		char* sval = reinterpret_cast<char*>(hc_alloc(len + 1));
		if (sval) {
			strcpy(sval, memorydata.c_str());
			IBMRAS_DEBUG(debug, "MEMORY REPORT\n");IBMRAS_DEBUG_1(debug, "%s", sval);
			data->size = len;
			data->data = sval;
			IBMRAS_DEBUG(debug, "<<MEMPullSource::sourceData(DATA)");
		}
	}
	return data;
}

#if defined(LINUX)
/**
 * Opens file at /proc/<pid>/<fname> for reading.
 *
 * @param[in] env    JNIEnv of the caller.
 * @param[in] fname  Name of file to open.
 *
 * @return File descriptor of the opened file or -1 on failure.
 */
static IDATA openProcFile(JNIEnv *env, const char *fname)
{
	char proc[MAXPATHLEN];

	snprintf(proc, sizeof(proc), "/proc/%d/%s", getpid(), fname);

	return open(proc,O_RDONLY);
}

/**
 * Read proc file at /proc/<pid>/<fname> into buf of size nbytes.
 * Null-terminates the buffer so it can be treated as a string.
 *
 * @param[in]  env    JNIEnv of the caller.
 * @param[in]  fname  Name of file to open.
 * @param[out] buf    Buffer to read file into.
 * @param[in]  nbytes Size of buffer.
 *
 * @return Returns number of bytes read excluding null-terminator
 *         or -1 on failure.
 */
static IDATA readProcFile(JNIEnv *env, const char *fname, char *buf, UDATA nbytes)
{
	IDATA ret = -1;
	IDATA fd = openProcFile(env, fname);

	if (-1 != fd)
	{
		ret = 0;
		/* Read up to (nbytes - 1) bytes to save space for the null terminator. */
		while (nbytes - ret > 1)
		{
			IDATA nread = read(fd, buf + ret, nbytes - ret - 1);

			if (nread <= 0)
			break;
			ret += (UDATA)nread;
		}
		buf[ret] = '\0';
		close(fd);
	}
	return ret;
}

/**
 * Gets the running process name as null-terminated string.
 *
 * @param[in]  env         JNIEnv of the caller.
 * @param[out] name        Buffer to store name of the process.
 * @param[in]  nameLength  Length of the name buffer.
 *
 * @return Pointer to name on success or NULL on error.
 */
static char* getProcessName(JNIEnv *env, char *name, UDATA nameLength)
{
	/*
	 * Read the first line from /proc/<pid>/status and parse
	 * the process name from it.
	 *
	 * It would be nice to use prctl() with PR_GET_NAME - but
	 * it is only supported on kernel version 2.6.11 and later.
	 */
#define PROC_NAME_PREFIX "Name:\t"
	char *ret = NULL;
	char buf[128];

	if (-1 != readProcFile(env, "status", buf, sizeof(buf)))
	{
		if (0 == strncmp(buf, PROC_NAME_PREFIX, sizeof(PROC_NAME_PREFIX) - 1))
		{
			UDATA i;
			char *from = buf + sizeof(PROC_NAME_PREFIX) - 1;

			for (i = 0; (i < nameLength - 1) && ('\0' != from[i]) && ('\n' != from[i]); i++)
			{
				name[i] = from[i];
			}
			name[i] = '\0';
			ret = name;
		}
	}
	return ret;
#undef PROC_NAME_PREFIX
}

/**
 * Skips n number of space-separator fields in str. The string
 * must not begin with whitespace.
 *
 * @param[in] str Null-terminated string that will be scanned.
 * @param[in] n   Number of fields to skip, must be positive.
 *
 * @return Pointer to the location in the string after the skipped
 *         fields, or NULL if end of string was encountered.
 */
static const char* skipFields(const char *str, UDATA n)
{
	str++;
	while (('\0' != *str) && (n > 0))
	{
		if (isspace(*str))
		{
			n--;
		}
		str++;
	}
	return(n != 0 ? NULL : str);
}

/**
 * Opens /proc/<pid>/stat file and reads the field at position index
 * from the file as sscanf would. Field index must be >= 2. Returns
 * result of sscanf (i.e. number of fields read), or -1 on failure.
 *
 * @param[in]  env    JNIEnv of the caller.
 * @param[in]  index  Index of the field to be read.
 * @param[in]  format Format string for field to be read.
 * @param[out] ...    Field(s) to be read as per format.
 *
 * @return Number of fields read, or -1 on failure.
 */
static IDATA readProcStatField(JNIEnv *env, UDATA index, const char *format, ...)
__attribute__((format(scanf,3,4)));
static IDATA readProcStatField(JNIEnv *env, UDATA index, const char *format, ...)
{
	IDATA ret = -1;
	char buf[512];

	if (-1 != readProcFile(env, "stat", buf, sizeof(buf)))
	{
		/*
		 * The second field in /proc/<pid>/stat is the process name
		 * surrounded by parentheses. Unfortunately, the process
		 * name can have both spaces and parentheses in it, neither
		 * of which are escaped. Thus, to parse the file correctly,
		 * we must get the process name in order to be able to skip
		 * it in the /proc/<pid>/stat file.
		 */
		char processName[64];

		if (NULL != getProcessName(env, processName, sizeof(processName)))
		{
			char expected[128];
			size_t length = (size_t)snprintf(expected, sizeof(expected),
					"%d (%s) ", getpid(), processName);

			/* Verify that the start of the file matches what we expected. */
			if (0 == memcmp(buf, expected, length))
			{
				const char *str = skipFields(buf + length, index - 2);

				if (NULL != str)
				{
					va_list ap;
					va_start(ap, format);
					ret = vsscanf(str, format, ap);
					va_end(ap);
				}
			}
		}
	}

	return ret;
}
#endif

jlong getProcessPhysicalMemorySize(JNIEnv* env) {
#if defined(LINUX)
	/* Read rss field from /proc/<pid>/stat as per 'man proc'. */
#define RSS_FIELD_INDEX 23
	long rss;

	if (1 == readProcStatField(env, RSS_FIELD_INDEX, "%ld", &rss))
	{
		/* NOTE: This is accurate even in the context of huge pages. */
		return(jlong)rss * sysconf(_SC_PAGESIZE);
	}
#undef RSS_FIELD_INDEX
#elif defined(AIXPPC)
	/*
	 * There is no API on AIX to get the rss of the shared memory used by this process.
	 * If such an API was available, this function should return the following:
	 *
	 *   sharedRss + (pe.pi_trss + pe.pi_drss)*4096
	 *
	 * NOTE: pi_trss and pi_drss are always in 4K units regardless of pi_text_l2psize.
	 */
#elif defined(WINDOWS)
	PROCESS_MEMORY_COUNTERS info;

	info.cb = sizeof(info);
	if (0 != GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info)))
	{
		return info.WorkingSetSize;
	}

#endif
	return -1;

}

jlong getProcessPrivateMemorySize(JNIEnv* env) {

#if defined(LINUX)
	/*
	 * Read shared field from /proc/<pid>/statm as per 'man proc'.
	 * Return difference between virtual memory size and shared.
	 */
#define SHARED_FIELD_INDEX 2
	char buf[512];

	if (-1 != readProcFile(env, "statm", buf, sizeof(buf)))
	{
		const char *str = skipFields(buf, SHARED_FIELD_INDEX);

		if (NULL != str)
		{
			long shared;

			if (1 == sscanf(str, "%ld", &shared))
			{
				jlong vsize = getProcessVirtualMemorySize(env);

				if (-1 != vsize)
				{
					jlong priv = vsize - ((jlong)shared * sysconf(_SC_PAGESIZE));

					return(priv > 0 ? priv : -1);
				}
			}
		}
	}
#undef SHARED_FIELD_INDEX
#elif defined(AIXPPC)
	struct procentry64 pe;
	pid_t pid = getpid();

	if (1 == getprocs64((struct procentry64*)&pe, sizeof(pe), NULL, 0, &pid, 1))
	{
		/* NOTE: pi_dvm is always in 4K units regardless of pi_data_l2psize. */
		jlong size = (jlong)pe.pi_tsize + (jlong)pe.pi_dvm * 4096;

		return(size > 0 ? size : -1);
	}
#elif defined(WINDOWS)

	IBMRAS_DEBUG(debug, ">>MEMPullSource::getProcessPrivateMemorySizeImpl()");

	PROCESS_MEMORY_COUNTERS_EX procMemCount;

	bool result = GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&procMemCount), sizeof(PROCESS_MEMORY_COUNTERS_EX));

	if(result) {
		return procMemCount.PrivateUsage > 0 ? procMemCount.PrivateUsage : -1;
	}
	return -1;

#endif
	IBMRAS_DEBUG(debug, "<<MEMPullSource::getProcessPrivateMemorySizeImpl()[ERROR]");
	return -1;
}

jlong getProcessVirtualMemorySize(JNIEnv* env) {
#if defined(LINUX)
	/* Read vsize field from /proc/<pid>/stat as per 'man proc'. */
#define VSIZE_FIELD_INDEX 22
	unsigned long vsize;

	if (1 == readProcStatField(env, VSIZE_FIELD_INDEX, "%lu", &vsize))
	{
		return(jlong)(vsize > 0 ? vsize : -1);
	}
#undef VSIZE_FIELD_INDEX
#elif defined(AIXPPC)
	/* There is no API on AIX to get shared memory usage for the process. If such an
	 * API existed, we could return getProcessPrivateMemorySize() + sharedSize here.
	 *
	 * Note: Iterating through /proc/<pid>/map and looking at the pages that are
	 * not marked MA_SHARED does not account for shared code pages when in fact
	 * command-line AIX utilities (such as svmon) do show that pages are shared.
	 */
#elif defined(WINDOWS)
	PROCESS_MEMORY_COUNTERS info;

	info.cb = sizeof(info);
	if (0 != GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info)))
	{
		return(jlong)info.PagefileUsage;
	}
#endif
	return -1;
}

jlong getFreePhysicalMemorySize(JNIEnv* env) {
#if defined(LINUX)
	/* NOTE: This is accurate even in the context of huge pages. */
	return(jlong)sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE);
#elif defined(AIXPPC)
	/* NOTE: This works on AIX 5.3 and later. */
	IDATA numPageSizes = vmgetinfo(NULL, VMINFO_GETPSIZES, 0);

	if (numPageSizes > 0)
	{
		psize_t *pageSizes = (psize_t*)__alloca(numPageSizes*sizeof(psize_t));
		IDATA numPageSizesRetrieved = vmgetinfo(pageSizes, VMINFO_GETPSIZES, numPageSizes);

		if (numPageSizes == numPageSizesRetrieved)
		{
			jlong size = 0;
			IDATA i;

			for (i = 0; i < numPageSizes; i++)
			{
				struct vminfo_psize pageSize;

				pageSize.psize = pageSizes[i];
				if (0 == vmgetinfo(&pageSize, VMINFO_PSIZE, sizeof(pageSize)))
				{
					size += (jlong)pageSize.psize * pageSize.numfrb;
				}
			}
			return(size > 0 ? size : -1);
		}
	}
	return -1;
#elif defined(WINDOWS)
	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof(statex);
	if (0 != GlobalMemoryStatusEx(&statex))
	{
		return statex.ullAvailPhys;
	}
	return -1;
#else
	return -1;
#endif
}

} /* end namespace env */
} /* end namespace jni */
} /* end namespace plugins */
} /* end namespace monitoring */
} /* end namespace ibmras */
