 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */

#include "ibmras/monitoring/plugins/common/cpu/cputime.h"
#include "ibmras/common/logging.h"
#include <cstdio>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>

#if defined(_LINUX)
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#if defined(_WINDOWS)
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#endif

#if defined(_AIX)
#include <libperfstat.h>
#endif

using namespace std;

extern IBMRAS_DECLARE_LOGGER;

extern "C" {

#if defined(_LINUX) || defined(_AIX)

// FIXME should probably move this to common along side getMilliseconds()
#define USECS_PER_SEC (1000000)
static uint64 time_microseconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	time_t seconds = tv.tv_sec;
	suseconds_t microseconds = tv.tv_usec;

	return (static_cast<uint64>(seconds) * USECS_PER_SEC) + microseconds;
}

#endif

#if defined(_LINUX)

static bool read_total_cpu_time(uint64* totaltime, const uint32 NS_PER_HZ) {
	uint64 user = 0, nice = 0, system = 0;

	std::ifstream filestream("/proc/stat");

	if (!filestream.is_open()) {
		IBMRAS_DEBUG(warning, "Failed to open /proc/stat");
		return false;
	}

	std::string prompt; // "cpu"
	filestream >> prompt >> user >> nice >> system;
	bool parsedSuccessfully = filestream.good();
	filestream.close();

	if (!parsedSuccessfully) {
		IBMRAS_DEBUG(warning, "Failed to parse /proc/stat");
		return false;
	}
		
	(*totaltime) = (user + nice + system) * NS_PER_HZ;
	
	return true;
}

static bool read_process_cpu_time(uint64* proctime, const uint32 NS_PER_HZ) {
	uint64 user = 0, kernel = 0;

	std::stringstream filenamess;
	filenamess << "/proc/" << getpid() << "/stat";
	std::string filename = filenamess.str();
	
	std::ifstream filestream(filename.c_str());

	if (!filestream.is_open()) {
		IBMRAS_DEBUG_1(warning, "Failed to open %s", filename.c_str());
		return false;
	}

	int32 dummyInt;
	uint32 dummyUInt;
	std::string dummyStr;
	filestream >> dummyInt >> dummyStr >> dummyStr >> dummyInt >> dummyInt;
	filestream >> dummyInt >> dummyInt >> dummyInt >> dummyUInt >> dummyUInt;
	filestream >> dummyUInt >> dummyUInt >> dummyUInt;
	filestream >> user >> kernel;
	bool parsedSuccessfully = filestream.good();
	filestream.close();

	if (!parsedSuccessfully) {
		IBMRAS_DEBUG_1(warning, "Failed to parse %s", filename.c_str());
		return false;
	}
		
	(*proctime) = (user + kernel) * NS_PER_HZ;
	
	return true;
}
 
struct CPUTime* getCPUTime() {
	static const uint32 userHz = sysconf(_SC_CLK_TCK);
	static const uint32 NS_PER_HZ = 1000000000 / userHz;
	struct CPUTime* cputime = new CPUTime;
	uint64 nsStart, nsEnd;
	
	nsStart = time_microseconds() * 1000;

	if (!read_total_cpu_time(&cputime->total, NS_PER_HZ)) {
		delete cputime;
		return NULL;
	}
	if (!read_process_cpu_time(&cputime->process, NS_PER_HZ)) {
		delete cputime;
		return NULL;
	}
	
	nsEnd = time_microseconds() * 1000;
	
	cputime->nprocs = get_nprocs();
	cputime->total /= cputime->nprocs;
	cputime->process /= cputime->nprocs;
	cputime->time = nsStart + ((nsEnd - nsStart) / 2);
		 
	return cputime;
}

#endif

#if defined(_WINDOWS)

static inline uint64 FILETIME_to_ns(FILETIME wintime) {
	DWORD high = wintime.dwHighDateTime;
	DWORD low = wintime.dwLowDateTime;
	return ((static_cast<uint64>(high) << 32) + low) * 100;
}

#define NSEC_TO_UNIX_EPOCH 11644473600000000000ULL
static inline bool FILETIME_to_unixtimestamp(FILETIME wintime, uint64* unixtimestamp) {
	// ns since Windows epoch 1601-01-01T00:00:00Z
	uint64 ns = FILETIME_to_ns(wintime);
	if (ns < NSEC_TO_UNIX_EPOCH) {
		// error, time is before unix epoch
		IBMRAS_DEBUG(warning, "Failed to convert Windows time to UNIX timestamp (before UNIX epoch)");
		return false; 
	}
	// convert to ns since UNIX epoch 1970-01-01T00:00:00Z
	(*unixtimestamp) =  ns - NSEC_TO_UNIX_EPOCH;
	return true; 
}

static bool read_process_cpu_time(uint64* proctime) {
	FILETIME create;
	FILETIME exit;
	FILETIME kernel;
	FILETIME user;
	HANDLE process = GetCurrentProcess();
	BOOL rc = GetProcessTimes(process, &create, &exit, &kernel, &user);
	
	if (!rc) {
		IBMRAS_DEBUG(warning, "Failed to get process cpu time");
		return false;
	}

	(*proctime) = FILETIME_to_ns(kernel) + FILETIME_to_ns(user); 
	return true;
}

static bool read_total_cpu_time(uint64* unixtimestamp, uint64* totaltime) {
	LONGLONG user, kernel;
	FILETIME utcTimeStamp;
	HQUERY Query = NULL;
	HCOUNTER userCounter = NULL;
	HCOUNTER privilegedCounter = NULL;
	PDH_RAW_COUNTER counterValue;
	PDH_STATUS Status = ERROR_SUCCESS;

	Status = PdhOpenQuery(NULL, (DWORD_PTR) NULL, &Query);
	if (ERROR_SUCCESS != Status) {
		IBMRAS_DEBUG(warning, "Failed to open pdh query for total cpu");
		return false;
	}

	Status = PdhAddCounter(Query, (LPCTSTR) (WCHAR *)
		"\\Processor(_Total)\\% User Time", 0, &userCounter);
		
	if (ERROR_SUCCESS != Status) {
        PdhCloseQuery(Query);
		IBMRAS_DEBUG(warning, "Failed to add user time pdh counter for total cpu");
		return false;
  	}

	Status = PdhAddCounter(Query, (LPCTSTR) (WCHAR *)
		"\\Processor(_Total)\\% Privileged Time", 0, &privilegedCounter);
	if (ERROR_SUCCESS != Status) {
        PdhCloseQuery(Query);
		IBMRAS_DEBUG(warning, "Failed to add kernel time pdh counter for total cpu");
		return false;
	}
	
	Status = PdhCollectQueryData(Query);
	if (ERROR_SUCCESS != Status) {
		PdhCloseQuery(Query);
		IBMRAS_DEBUG(warning, "Failed to collect pdh query data for total cpu");
		return false;
	}

	Status = PdhGetRawCounterValue(privilegedCounter, NULL, &counterValue);
	if (ERROR_SUCCESS != Status) {
		PdhCloseQuery(Query);
		IBMRAS_DEBUG(warning, "Failed to get kernel time counter value for total cpu");
		return false;
	}
	user = counterValue.FirstValue;
	
	Status = PdhGetRawCounterValue(userCounter, NULL, &counterValue);
	if (ERROR_SUCCESS != Status) {
		PdhCloseQuery(Query);
		IBMRAS_DEBUG(warning, "Failed to get user time counter value for total cpu");
		return false;
	}
	kernel = counterValue.FirstValue;
	
	PdhCloseQuery(Query);
	
	(*totaltime) = (static_cast<uint64>(user) + static_cast<uint64>(kernel)) * 100; // to ns
	if (!LocalFileTimeToFileTime(&counterValue.TimeStamp, &utcTimeStamp)) {
		IBMRAS_DEBUG(warning, "Failed to convert local time to UTC");
		return false;
	}
	if (!FILETIME_to_unixtimestamp(utcTimeStamp, unixtimestamp)) {
		// message already logged
		return false;
	} 
	
	return true;
}

/* returns NULL if there was an error obtaining any field */
struct CPUTime* getCPUTime() {
	struct CPUTime* cputime = new CPUTime;
	SYSTEM_INFO sysinfo;
	
	GetSystemInfo(&sysinfo);
	cputime->nprocs = sysinfo.dwNumberOfProcessors;

	if (!read_process_cpu_time(&cputime->process)) {
		delete cputime;
		return NULL;
	}
	if (!read_total_cpu_time(&cputime->time, &cputime->total)) {
		delete cputime;
		return NULL;
	}
	return cputime;	
}

#endif

#if defined(_AIX)

struct CPUTime* getCPUTime() {
	static const uint32 NS_PER_CPU_TICK = 10000000;
	static const uint32 NS_PER_MS = 1000000;
	struct CPUTime* cputime = new CPUTime;
	uint64 nsStart, nsEnd;
	perfstat_cpu_total_t stats;
	perfstat_process_t pstats;
	perfstat_id_t psid;
	
	nsStart = time_microseconds() * 1000;

	if (perfstat_cpu_total(NULL, &stats, sizeof(perfstat_cpu_total_t), 1) == -1) {
		delete cputime;
		return NULL;
	}
	
	// psid.name is char[IDENTIIFER_LENGTH] (64); see libperfstat.h
	sprintf(psid.name, "%d", getpid());
	if (perfstat_process(&psid, &pstats, sizeof(perfstat_process_t), 1) == -1) {
		delete cputime;
		return NULL;
	}
	
	nsEnd = time_microseconds() * 1000;
	
	cputime->nprocs = stats.ncpus;
	cputime->total = (stats.user + stats.sys) * NS_PER_CPU_TICK / cputime->nprocs;
	cputime->process = (pstats.ucpu_time + pstats.scpu_time) * NS_PER_MS / cputime->nprocs;
	cputime->time = nsStart + ((nsEnd - nsStart) / 2);
		 
	return cputime;
}

#endif

}
