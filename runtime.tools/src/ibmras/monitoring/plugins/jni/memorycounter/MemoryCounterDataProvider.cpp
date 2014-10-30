 /**
 * IBM Confidential
 * OCO Source Materials
 * IBM Monitoring and Diagnostic Tools - Health Center
 * (C) Copyright IBM Corp. 2007, 2014 All Rights Reserved.
 * The source code for this program is not published or otherwise
 * divested of its trade secrets, irrespective of what has
 * been deposited with the U.S. Copyright Office.
 */


#if defined(_ZOS)
#define _XOPEN_SOURCE_EXTENDED 1
#undef _ALL_SOURCE
#endif

#include "ibmras/monitoring/plugins/jni/memorycounter/MemoryCounterDataProvider.h"
#include "ibmras/monitoring/agent/Agent.h"

#include <cstring>
#include <string>
#include <sstream>
#include "jni.h"
#include <ctime>
#include <stdio.h>
#include "ibmras/common/util/memUtils.h"
#include "ibmras/common/util/strUtils.h"
#include "ibmras/common/util/sysUtils.h"
#include "ibmras/common/logging.h"


#if defined(WINDOWS)
#define JLONG_FMT_STR "%I64d"
#else /* Unix platforms */
#define JLONG_FMT_STR "%lld"
#endif

#if defined(WINDOWS)
#include <windows.h>
#include <Psapi.h>
#elif defined(LINUX)
#include <sys/time.h>
#elif defined(_ZOS)
#include <sys/time.h>
#endif

using namespace ibmras::monitoring::plugins::jni;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {
namespace memorycounter {

IBMRAS_DEFINE_LOGGER("DataProviderSources");

char* getMemoryCounters(JNIEnv *env);

MCPullSource* src = NULL;
bool enabled = true;

PullSource* getMCPullSource() {
	if (!src) {
		src = new MCPullSource;
	}
	return src;
}

bool MCPullSource::isEnabled() {
	return enabled;
}

void MCPullSource::publishConfig() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	ibmras::monitoring::connector::ConnectorManager *conMan =
			agent->getConnectionManager();

	std::string msg = "memorycounters_subsystem=";
	if (isEnabled()) {
		msg += "on";
	} else {
		msg += "off";
	}
	conMan->sendMessage("configuration/memorycounters", msg.length(),
			(void*) msg.c_str());
}

void MCPullSource::setState(const std::string &newState) {
	enabled = ibmras::common::util::equalsIgnoreCase(newState, "on");
	getMCPullSource()->publishConfig();
}

monitordata* callback() {
	return src->PullSource::generateData();
}

uint32 MCPullSource::getSourceID() {
	return MC;
}

pullsource* MCPullSource::getDescriptor() {

	pullsource* src = new pullsource();
	src->header.name = "memorycounters";
	src->header.description = "Memory-counter information";
	src->header.sourceID = MC;
	src->header.capacity = 8 * 1024;
	src->next = NULL;
	src->callback = callback;
	src->complete = ibmras::monitoring::plugins::jni::complete;
	src->pullInterval = 5;

	return src;
}

monitordata* MCPullSource::sourceData(jvmFunctions* tdpp, JNIEnv* env) {

	IBMRAS_DEBUG(debug, ">>MCPullSource::sourceData");
	monitordata* data = new monitordata;
	data->size = 0;
	data->data = NULL;

	if (isEnabled()) {
		data->persistent = false;
		data->provID = getProvID();
		data->sourceID = MC;

		data->data = getMemoryCounters(env);
		if (data->data) {
			data->size = strlen(data->data);
		}
	}
	return data;

}

char* getMemoryCounters(JNIEnv *env) {

	jint total_categories;
	jint written_count = 0;
	jvmtiMemoryCategory* categories_buffer = NULL;
	char* report;
	jvmtiError err;
	jint i;
	char *finalReport = NULL;
	char **memcounterarray = NULL;
	char* category_name = NULL;
	jvmFunctions* tdpp = getTDPP();
	if (!enabled) {
		return NULL;
	}

	if (tdpp->jvmtiGetMemoryCategories == NULL) {
		enabled = false;
		return NULL;
	}


	const char* memCounterFormatString =
			"memcounterinfo, %s, "JLONG_FMT_STR", "JLONG_FMT_STR", "JLONG_FMT_STR", "JLONG_FMT_STR", %ld, %ld, %ld\n";
	char buffer[1000];

	/* Find out how much categories buffer to allocate */
	err = tdpp->jvmtiGetMemoryCategories(tdpp->pti,
			COM_IBM_GET_MEMORY_CATEGORIES_VERSION_1, 0, NULL, NULL,
			&total_categories);
	if (JVMTI_ERROR_NONE != err) {
		goto cleanup;
	}

	categories_buffer =
			(jvmtiMemoryCategory*) ibmras::monitoring::plugins::jni::hc_alloc(
					total_categories * sizeof(jvmtiMemoryCategory));
	if (categories_buffer == NULL) {
		goto cleanup;
	}

	err = tdpp->jvmtiGetMemoryCategories(tdpp->pti,
			COM_IBM_GET_MEMORY_CATEGORIES_VERSION_1, total_categories,
			categories_buffer, &written_count, &total_categories);
	if (JVMTI_ERROR_NONE != err) {
		goto cleanup;
	}

	if (written_count < total_categories) {
		goto cleanup;
	}

	memcounterarray = (char**) ibmras::monitoring::plugins::jni::hc_alloc(
			(written_count) * sizeof(char *));
	if (memcounterarray == NULL) {
		goto cleanup;
	}

	for (i = 0; i < written_count; i++) {

		long firstChildNumber = 0;
		long nextSiblingNumber = 0;
		long parentNumber = 0;

		if (NULL != categories_buffer[i].firstChild) {
			firstChildNumber = (long) categories_buffer[i].firstChild
					- (long) &categories_buffer[0];
			if (firstChildNumber > 0) {
				firstChildNumber = firstChildNumber
						/ sizeof(jvmtiMemoryCategory);
			}
		} else {
			firstChildNumber = -1;
		}

		if (NULL != categories_buffer[i].nextSibling) {
			nextSiblingNumber = (long) categories_buffer[i].nextSibling
					- (long) &categories_buffer[0];
			if (nextSiblingNumber > 0) {
				nextSiblingNumber = nextSiblingNumber
						/ sizeof(jvmtiMemoryCategory);
			}
		} else {
			nextSiblingNumber = -1;
		}

		if (NULL != categories_buffer[i].parent) {
			parentNumber = (long) categories_buffer[i].parent
					- (long) &categories_buffer[0];
			if (parentNumber > 0) {
				parentNumber = parentNumber / sizeof(jvmtiMemoryCategory);
			}
		} else {
			parentNumber = -1;
		}
#if defined(_ZOS)
#pragma convlit(suspend)
#endif
		sprintf(buffer, memCounterFormatString, categories_buffer[i].name,
				categories_buffer[i].liveBytesShallow,
				categories_buffer[i].liveBytesDeep,
				categories_buffer[i].liveAllocationsShallow,
				categories_buffer[i].liveAllocationsDeep, firstChildNumber,
				nextSiblingNumber, parentNumber);
		memcounterarray[i] =
				(char *) ibmras::monitoring::plugins::jni::hc_alloc(
						strlen(buffer) + 1);
		if (memcounterarray[i] == NULL) {
			goto cleanup;
		}
		strcpy(memcounterarray[i], buffer);
	}

	finalReport = ibmras::monitoring::plugins::jni::join_strings(
			memcounterarray, i);

	cleanup: ibmras::monitoring::plugins::jni::hc_dealloc(
			(unsigned char**) &category_name);
	ibmras::monitoring::plugins::jni::hc_dealloc(
			(unsigned char**) &categories_buffer);
	ibmras::monitoring::plugins::jni::dealloc_report_lines(memcounterarray,
			written_count);
	ibmras::monitoring::plugins::jni::hc_dealloc(
			(unsigned char**) &memcounterarray);

	if (finalReport != NULL) {

		unsigned long long millisecondsSinceEpoch; // = ibmras::common::util::getMilliseconds();
#if defined(WINDOWS)

		SYSTEMTIME st;
		GetSystemTime(&st);

		millisecondsSinceEpoch = time(NULL)*1000+st.wMilliseconds;

#else
		struct timeval tv;
		gettimeofday(&tv, NULL);

		millisecondsSinceEpoch = (unsigned long long) (tv.tv_sec) * 1000
				+ (unsigned long long) (tv.tv_usec) / 1000;
#endif
		char tstamp[20];
		sprintf(tstamp, "%llu", millisecondsSinceEpoch);
		std::stringstream ss;
		ss << "smc\ntime stamp=";
		ss << tstamp << "\n";
		ss << finalReport;
		ss << "emc\n";

		std::string data = ss.str();
		report = reinterpret_cast<char*>(hc_alloc(data.length() + 1));
		if (report) {
			strcpy(report, data.c_str());
		}
	}
#if defined(_ZOS)
#pragma convlit(suspend)
#endif
	ibmras::monitoring::plugins::jni::hc_dealloc(
			(unsigned char**) &finalReport);

	return report;
}

} /* end namespace memorycounter */
} /* end namespace jni */
} /* end namespace plugins */
} /* end namespace monitoring */
} /* end namespace ibmras */
