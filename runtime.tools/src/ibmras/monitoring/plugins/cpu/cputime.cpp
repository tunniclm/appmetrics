#include "ibmras/monitoring/plugins/cpu/cputime.h"
#include "ibmras/common/logging.h"
#include <cstdio>
#include <ctime>

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

#if defined(_LINUX)

// FIXME should probably move this to common along side getMilliseconds()
static unsigned long long time_microseconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((unsigned long long)tv.tv_sec * 1000000) + (unsigned long long)tv.tv_usec;
}

static bool read_total_cpu_time(unsigned long long* totaltime, const unsigned NS_PER_HZ) {
	unsigned long long user = 0, nice = 0, system = 0;
	char buffer[128];

	FILE* fp = fopen("/proc/stat" , "r");
	if (!fp) {
		IBMRAS_DEBUG(warning, "Failed to open /proc/stat");
		return false;
	}
	int bytesRead = fread(buffer, 1, sizeof(buffer) - 1, fp); // leave space for null term
	fclose(fp);
	
	if (bytesRead <= 0) {
		IBMRAS_DEBUG(warning, "Failed to read /proc/stat");
		return false;
	} 
	buffer[bytesRead] = '\0';
	if (0 == sscanf(buffer, "cpu  %llu %llu %llu", &user, &nice, &system)) {
		IBMRAS_DEBUG(warning, "Failed to parse /proc/stat");
		return false;
	}

	(*totaltime) = (user + nice + system) * NS_PER_HZ;
	
	return true;
}

static bool read_process_cpu_time(unsigned long long* proctime, const unsigned NS_PER_HZ) {
	unsigned long long user = 0, kernel = 0;
	char buffer[128];
	char statfile[128];
#if defined(_ZOS)
#pragma convlit(suspend)
#endif	
	sprintf(statfile, "/proc/%d/stat", getpid()); // FIXME bounds & error handling
#if defined(_ZOS)
#pragma convlit(resume)
#endif
	FILE* fp = fopen(statfile, "r");
	if (!fp) {
		IBMRAS_DEBUG_1(warning, "Failed to open %s", statfile);
		return false;
	}
	int bytesRead = fread(buffer, 1, sizeof(buffer) - 1, fp);
	fclose(fp);

	if (bytesRead <= 0) {
		IBMRAS_DEBUG_1(warning, "Failed to read %s", statfile);
		return false;
	} 
	buffer[bytesRead] = '\0';
	if (0 == sscanf(buffer, 
			"%*d %*s %*c %*d"
			"%*d %*d %*d %*d %*u %*u %*u %*u %*u"
			"%llu %llu",
 			&user, &kernel)) {
 		
		IBMRAS_DEBUG_1(warning, "Failed to parse %s", statfile);
		return false;
	}
	
	(*proctime) = (user + kernel) * NS_PER_HZ;
	
	return true;
}
 
// TODO check numeric types (int, long etc)
struct CPUTime* getCPUTime() {
	static const unsigned userHz = sysconf(_SC_CLK_TCK);
	const unsigned NS_PER_HZ = 1000000000 / userHz;
	struct CPUTime* cputime = new CPUTime;
	unsigned long long nsStart, nsEnd;
	
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

static inline unsigned long long FILETIME_to_ns(FILETIME wintime) {
	return ((((unsigned long long) wintime.dwHighDateTime) << 32) + wintime.dwLowDateTime) * 100;
}

#define NSEC_TO_UNIX_EPOCH 11644473600000000000ULL
static inline bool FILETIME_to_unixtimestamp(FILETIME wintime, unsigned long long* unixtimestamp) {
	// ns since Windows epoch 1601-01-01T00:00:00Z
	unsigned long long ns = ((((unsigned long long) wintime.dwHighDateTime) << 32) + wintime.dwLowDateTime) * 100;
	if (ns < NSEC_TO_UNIX_EPOCH) {
		// error, time is before unix epoch
		IBMRAS_DEBUG(warning, "Failed to convert Windows time to UNIX timestamp (before UNIX epoch)");
		return false; 
	}
	// convert to ns since UNIX epoch 1970-01-01T00:00:00Z
	(*unixtimestamp) =  ns - NSEC_TO_UNIX_EPOCH;
	return true; 
}

static bool read_process_cpu_time(unsigned long long* proctime) {
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

static bool read_total_cpu_time(unsigned long long* unixtimestamp, unsigned long long* totaltime) {
	long long user, kernel;
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
	
	(*totaltime) = ((unsigned long long)user + (unsigned long long)kernel) * 100ULL; // to ns
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

// FIXME should probably move this to common along side getMilliseconds()
static unsigned long long time_microseconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((unsigned long long)tv.tv_sec * 1000000) + (unsigned long long)tv.tv_usec;
}

struct CPUTime* getCPUTime() {
	const unsigned NS_PER_CPU_TICK = 10000000L; // TODO check data type
	const unsigned NS_PER_MS = 1000000L;
	struct CPUTime* cputime = new CPUTime;
	unsigned long long nsStart, nsEnd;
	perfstat_cpu_total_t stats;
	perfstat_process_t pstats;
	perfstat_id_t psid;
	
	nsStart = time_microseconds() * 1000;

	if (perfstat_cpu_total(NULL, &stats, sizeof(perfstat_cpu_total_t), 1) == -1) {
		delete cputime;
		return NULL;
	}
	
	// psid.name is char[IDENTIIFER_LENGTH] (64); see libperfstat.h
#if defined(_ZOS)
#pragma convlit(suspend)
#endif
	sprintf(psid.name, "%d", getpid());
#if defined(_ZOS)
#pragma convlit(resume)
#endif
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