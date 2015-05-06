 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2015 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */
#include "ibmras/monitoring/AgentExtensions.h"
#include "ibmras/monitoring/plugins/common/cpu/CpuPlugin.h"
#include <cstdio>
#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <cstring>
#if defined(_AIX)
#include <unistd.h>
#include <libperfstat.h>
#endif

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

#define CPUSOURCE_PULL_INTERVAL 2
#define DEFAULT_CAPACITY 1024*10

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace common {
namespace cpuplugin {

CpuPlugin* CpuPlugin::instance = 0;
agentCoreFunctions CpuPlugin::aCF;

	CpuPlugin::CpuPlugin(uint32 provID):
			provID(provID), noFailures(false), current(NULL), last(NULL){
	}

	CpuPlugin::~CpuPlugin(){}

	int CpuPlugin::start() {
		aCF.logMessage(ibmras::common::logging::debug, ">>>CpuPlugin::start()");
		noFailures = true;
		aCF.logMessage(ibmras::common::logging::debug, "<<<CpuPlugin::start()");
		return 0;
	}

	int CpuPlugin::stop() {
			aCF.logMessage(ibmras::common::logging::debug, ">>>CpuPlugin::stop()");
			aCF.logMessage(ibmras::common::logging::debug, "<<<CpuPlugin::stop()");
			return 0;
	}

	pullsource* CpuPlugin::createSource(agentCoreFunctions aCF, uint32 provID) {
		aCF.logMessage(ibmras::common::logging::fine, "[cpu_os] Registering pull source");
		if(!instance) {
			CpuPlugin::aCF = aCF;
			instance = new CpuPlugin(provID);
		}
		return instance->createPullSource(0, "common_cpu");
	}

	CpuPlugin* CpuPlugin::getInstance() {
		return instance;
	}

	char* CpuPlugin::NewCString(const std::string& s) {
		char *result = new char[s.length() + 1];
		std::strcpy(result, s.c_str());
		return result;
	}

	double CpuPlugin::clamp(double value, double min, double max) {
		if (value > max) return max;
		if (value < min) return min;
		return value;
	}

	double CpuPlugin::CalculateTotalCPU(struct CPUTime* start, struct CPUTime* finish) {
		double cpu = (double)(finish->total - start->total) / (double)(finish->time - start->time);
		if (cpu < 0.0 || cpu > 1.0) {
			std::stringstream cpuss;
			cpuss <<  "[cpu_os] Total CPU reported is out of range 0.0 to 1.0 ("<<cpu<<")";
			aCF.logMessage(ibmras::common::logging::debug, cpuss.str().c_str());
		}
		cpu = clamp(cpu, 0.0, 1.0);
		return cpu;
	}

	double CpuPlugin::CalculateProcessCPU(struct CPUTime* start, struct CPUTime* finish) {
		double cpu = (double)(finish->process - start->process) / (double)(finish->time - start->time);
		if (cpu < 0.0 || cpu > 1.0) {
			std::stringstream cpuss;
			cpuss <<  "[cpu_os] Process CPU reported is out of range 0.0 to 1.0 ("<<cpu<<")";
			aCF.logMessage(ibmras::common::logging::debug, cpuss.str().c_str());
		}
		cpu = clamp(cpu, 0.0, 1.0);
		return cpu;
	}

	void CpuPlugin::AppendCPUTime(std::stringstream& contentss) {
		contentss << "startCPU";
		contentss << "@#" << (instance->current->time / 1000000); // time in ms
		contentss << "@#" << CalculateProcessCPU(instance->last, instance->current);
		contentss << "@#" << CalculateTotalCPU(instance->last, instance->current);
		contentss << '\n';
	}

	bool CpuPlugin::IsValidData(struct CPUTime* cputime) {
		aCF.logMessage(ibmras::common::logging::debug, ">>>CpuPlugin::IsValidData");
		aCF.logMessage(ibmras::common::logging::debug, "<<<CpuPlugin::IsValidData");
		return cputime != NULL;
	}


	bool CpuPlugin::TimesAreDifferent(struct CPUTime* start, struct CPUTime* finish) {
		return finish->time != start->time;
	}

	monitordata* CpuPlugin::OnRequestData() {
		aCF.logMessage(ibmras::common::logging::debug, ">>>CpuPlugin::OnRequestData");
		monitordata *data = new monitordata;
		data->provID = provID;
		data->sourceID = 0;
		data->size = 0;
		data->data = NULL;
		data->persistent = false;

		if (last != NULL) {
			delete last;
		}
		last = current;
		current = getCPUTime();

		if (IsValidData(instance->last) && IsValidData(instance->current)
			&& TimesAreDifferent(instance->last, instance->current)) {

			std::stringstream contentss;
			contentss << "#CPUSource\n";
			AppendCPUTime(contentss);

			std::string content = contentss.str();
			data->size = static_cast<uint32>(content.length()); // should data->size be a size_t?
			data->data = NewCString(content);
		} else {
			if (!IsValidData(instance->current)) {
				aCF.logMessage(ibmras::common::logging::debug, "[cpu_os] Skipped sending data (reason: invalid data)");
				if (noFailures) {
					aCF.logMessage(ibmras::common::logging::warning, "[cpu_os] At least one data gathering failure occurred");
					noFailures = false;
				}
			} else if (IsValidData(last) && !TimesAreDifferent(last, current)) {
				aCF.logMessage(ibmras::common::logging::debug, "[cpu_os] Skipped sending data (reason: time did not advance)");
				if (noFailures) {
					aCF.logMessage(ibmras::common::logging::warning, "[cpu_os] At least one data gathering failure occurred");
					noFailures = false;
				}
			}
		}


		aCF.logMessage(ibmras::common::logging::debug, "<<<CpuPlugin::OnRequestData");

		return data;
	}

	void CpuPlugin::OnComplete(monitordata* data) {
		if (data != NULL) {
			if (data->data != NULL) {
				delete[] data->data;
			}
			delete data;
		}
	}

	pullsource* CpuPlugin::createPullSource(uint32 srcid, const char* name) {
		pullsource *src = new pullsource();
		src->header.name = name;
		std::string desc("Description for ");
		desc.append(name);
		src->header.description = NewCString(desc);
		src->header.sourceID = srcid;
		src->next = NULL;
		src->header.capacity = DEFAULT_CAPACITY;
		src->callback = pullWrapper;
		src->complete = pullCompleteWrapper;
		src->pullInterval = CPUSOURCE_PULL_INTERVAL; // seconds
		return src;
	}
	/*****************************************************************************
	 * CALLBACK WRAPPERS
	 *****************************************************************************/

	extern "C" monitordata* pullWrapper() {
			return CpuPlugin::getInstance()->OnRequestData();
	}

	extern "C" void pullCompleteWrapper(monitordata* data) {
		CpuPlugin::getInstance()->OnComplete(data);
	}

	/*****************************************************************************
	 * FUNCTIONS EXPORTED BY THE LIBRARY
	 *****************************************************************************/

	extern "C" {
	pullsource* ibmras_monitoring_registerPullSource(agentCoreFunctions aCF, uint32 provID) {
		aCF.logMessage(ibmras::common::logging::fine, ">>>ibmras_monitoring_registerPullSource");
		pullsource *source = CpuPlugin::createSource(aCF, provID);
		aCF.logMessage(ibmras::common::logging::fine, "<<<ibmras_monitoring_registerPullSource");
		return source;
	}

	int ibmras_monitoring_plugin_init(const char* properties) {
		// NOTE(tunniclm): We don't have the agentCoreFunctions yet, so we can't do any init that requires
		//                 calling into the API (eg getting properties.)
		return 0;
	}

	int ibmras_monitoring_plugin_start() {
		return CpuPlugin::getInstance()->start();
	}

	int ibmras_monitoring_plugin_stop() {

		return CpuPlugin::getInstance()->stop();
	}

	const char* ibmras_monitoring_getVersion() {
		return PLUGIN_API_VERSION;
	}
	}


	/*****************************************************************************
	 * PLATFORM DEPENDENT FUNCTIONS
	 *****************************************************************************/
#if defined(_AIX) || defined(_LINUX)
#define USECS_PER_SEC (1000000)
uint64 CpuPlugin::time_microseconds() {
	struct timeval tv;
	gettimeofday(&tv, NULL);

	time_t seconds = tv.tv_sec;
	suseconds_t microseconds = tv.tv_usec;

	return (static_cast<uint64>(seconds) * USECS_PER_SEC) + microseconds;
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
		CpuPlugin::aCF.logMessage(ibmras::common::logging::debug, "[cpu_os] Failed to convert Windows time to UNIX timestamp (before UNIX epoch)");
		return false;
	}
	// convert to ns since UNIX epoch 1970-01-01T00:00:00Z
	(*unixtimestamp) =  ns - NSEC_TO_UNIX_EPOCH;
	return true;
}
#endif

struct CPUTime* CpuPlugin::getCPUTime() {

	aCF.logMessage(ibmras::common::logging::debug, ">>>CpuPlugin::getCPUTime");

#if defined(_AIX)
	static const uint32 NS_PER_CPU_TICK = 10000000;
	static const uint32 NS_PER_MS = 1000000;
	struct CPUTime* cputime = new CPUTime;
	uint64 nsStart, nsEnd;
	perfstat_cpu_total_t stats;
	perfstat_process_t pstats;
	perfstat_id_t psid;

	nsStart = time_microseconds() * 1000;

	if (perfstat_cpu_total(NULL, &stats, sizeof(perfstat_cpu_total_t), 1) == -1) {
		std::stringstream ss;
		ss << "[cpu_os] Failed to read total CPU (errno=" << errno << ")";
		aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());

		delete cputime;
		return NULL;
	}

	// psid.name is char[IDENTIIFER_LENGTH] (64); see libperfstat.h
	sprintf(psid.name, "%d", getpid());
	if (perfstat_process(&psid, &pstats, sizeof(perfstat_process_t), 1) == -1) {
		std::stringstream ss;
		ss << "[cpu_os] Failed to read process CPU (errno=" << errno << ")";
		aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());

		delete cputime;
		return NULL;
	}

	nsEnd = time_microseconds() * 1000;

	cputime->nprocs = stats.ncpus;
	cputime->total = (stats.user + stats.sys) * NS_PER_CPU_TICK / cputime->nprocs;
	cputime->process = (pstats.ucpu_time + pstats.scpu_time) * NS_PER_MS / cputime->nprocs;
	cputime->time = nsStart + ((nsEnd - nsStart) / 2);

	return cputime;
#elif defined (_LINUX)
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
#elif defined(_WINDOWS)
		struct CPUTime* cputime = new CPUTime;
		SYSTEM_INFO sysinfo;

		GetSystemInfo(&sysinfo);
		cputime->nprocs = sysinfo.dwNumberOfProcessors;

		if (!read_process_cpu_time(&cputime->process, 0)) {
			aCF.logMessage(ibmras::common::logging::debug, "[cpu_os] Failed to read process CPU");
			delete cputime;
			return NULL;
		}
		if (!read_total_cpu_time(&cputime->time, &cputime->total)) {
			aCF.logMessage(ibmras::common::logging::debug, "[cpu_os] Failed to read total CPU");
			delete cputime;
			return NULL;
		}

		cputime->process /= cputime->nprocs; // process cpu is %age of 1 core (system cpu is %age of all cores)
		return cputime;
#else
		return NULL;
#endif
}

#if defined(_WINDOWS)
bool CpuPlugin::read_total_cpu_time(uint64* unixtimestamp, uint64* totaltime) {
#else
bool CpuPlugin::read_total_cpu_time(uint64* totaltime, const uint32 NS_PER_HZ) {
#endif
	aCF.logMessage(ibmras::common::logging::debug, ">>>read_total_cpu_time");
#if defined(_LINUX)
	uint64 user = 0, nice = 0, system = 0;

	std::ifstream filestream("/proc/stat");

	if (!filestream.is_open()) {
		aCF.logMessage(ibmras::common::logging::debug, "[cpu_os] Failed to open /proc/stat");
		return false;
	}

	std::string prompt; // "cpu"
	filestream >> prompt >> user >> nice >> system;
	bool parsedSuccessfully = filestream.good();
	filestream.close();

	if (!parsedSuccessfully) {
		aCF.logMessage(ibmras::common::logging::debug, "[cpu_os] Failed to parse /proc/stat");
		return false;
	}

	(*totaltime) = (user + nice + system) * NS_PER_HZ;

	return true;
#elif defined(_WINDOWS)
		LONGLONG user, kernel;
		FILETIME utcTimeStamp;
		HQUERY Query = NULL;
		HCOUNTER userCounter = NULL;
		HCOUNTER privilegedCounter = NULL;
		PDH_RAW_COUNTER counterValue;
		PDH_STATUS Status = ERROR_SUCCESS;

		Status = PdhOpenQuery(NULL, (DWORD_PTR) NULL, &Query);
		if (ERROR_SUCCESS != Status) {
	        std::stringstream ss;
	        ss << "[cpu_os] Failed to open pdh query for total cpu (status=" << Status << ")";
			aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());
			return false;
		}

		Status = PdhAddCounter(Query, (LPCTSTR) (WCHAR *)
			"\\Processor(_Total)\\% User Time", 0, &userCounter);

		if (ERROR_SUCCESS != Status) {
	        PdhCloseQuery(Query);
	        std::stringstream ss;
	        ss << "[cpu_os] Failed to add user time pdh counter for total cpu (status=" << Status << ")";
			aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());
			return false;
	  	}

		Status = PdhAddCounter(Query, (LPCTSTR) (WCHAR *)
			"\\Processor(_Total)\\% Privileged Time", 0, &privilegedCounter);
		if (ERROR_SUCCESS != Status) {
	        PdhCloseQuery(Query);
	        std::stringstream ss;
	        ss << "[cpu_os] Failed to add kernel time pdh counter for total cpu (status=" << Status << ")";
			aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());
			return false;
		}

		Status = PdhCollectQueryData(Query);
		if (ERROR_SUCCESS != Status) {
			PdhCloseQuery(Query);
	        std::stringstream ss;
	        ss << "[cpu_os] Failed to collect pdh query data for total cpu (status=" << Status << ")";
			aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());
			return false;
		}

		Status = PdhGetRawCounterValue(privilegedCounter, NULL, &counterValue);
		if (ERROR_SUCCESS != Status) {
			PdhCloseQuery(Query);
	        std::stringstream ss;
	        ss << "[cpu_os] Failed to get kernel time counter value for total cpu (status=" << Status << ")";
			aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());
			return false;
		}
		user = counterValue.FirstValue;

		Status = PdhGetRawCounterValue(userCounter, NULL, &counterValue);
		if (ERROR_SUCCESS != Status) {
			PdhCloseQuery(Query);
	        std::stringstream ss;
	        ss << "[cpu_os] Failed to get user time counter value for total cpu (status=" << Status << ")";
			aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());
			return false;
		}
		kernel = counterValue.FirstValue;

		PdhCloseQuery(Query);

		(*totaltime) = (static_cast<uint64>(user) + static_cast<uint64>(kernel)) * 100; // to ns
		if (!LocalFileTimeToFileTime(&counterValue.TimeStamp, &utcTimeStamp)) {
			aCF.logMessage(ibmras::common::logging::debug, "[cpu_os] Failed to convert local time to UTC");
			return false;
		}
		if (!FILETIME_to_unixtimestamp(utcTimeStamp, unixtimestamp)) {
			// message already logged
			return false;
		}

		return true;
#else
		return false;
#endif
}

bool CpuPlugin::read_process_cpu_time(uint64* proctime, const uint32 NS_PER_HZ) {
	aCF.logMessage(ibmras::common::logging::debug, ">>>read_process_cpu_time");

#if defined(_LINUX)
	uint64 user = 0, kernel = 0;

	std::stringstream filenamess;
	filenamess << "/proc/" << getpid() << "/stat";
	std::string filename = filenamess.str();

	std::ifstream filestream(filename.c_str());

	if (!filestream.is_open()) {
		std::stringstream ss;
		ss << "[cpu_os] Failed to open " << filename;
		aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());
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
		std::stringstream ss;
		ss << "[cpu_os] Failed to parse " << filename;
		aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());
		return false;
	}

	(*proctime) = (user + kernel) * NS_PER_HZ;

	return true;
#elif defined(_WINDOWS)
	FILETIME create;
	FILETIME exit;
	FILETIME kernel;
	FILETIME user;
	HANDLE process = GetCurrentProcess();
	BOOL rc = GetProcessTimes(process, &create, &exit, &kernel, &user);

	if (!rc) {
		std::stringstream ss;
		ss << "[cpu_os] Failed to get process cpu time (error=" << GetLastError() << ")";
		aCF.logMessage(ibmras::common::logging::debug, ss.str().c_str());
		return false;
	}
	(*proctime) = FILETIME_to_ns(kernel) + FILETIME_to_ns(user);
	return true;
#else
	return false;
#endif
}

}
}
}
}
}
