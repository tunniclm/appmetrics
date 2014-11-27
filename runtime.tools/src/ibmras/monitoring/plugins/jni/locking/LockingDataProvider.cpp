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

#include "ibmras/monitoring/plugins/jni/locking/LockingDataProvider.h"
#include "ibmras/common/logging.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/util/strUtils.h"
#include "jni.h"
#include "jvmti.h"
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <ctime>
#if defined(WINDOWS)
#include <windows.h>
#include <Psapi.h>
#include <WinBase.h>
#endif

#if defined(LINUX)
#include <sys/time.h>
#endif

#if defined(_ZOS)
#include <sys/time.h>
#endif

#include "ibmras/common/util/strUtils.h"

using namespace ibmras::monitoring::plugins::jni;

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jni {
namespace locking {

IBMRAS_DEFINE_LOGGER("DataProviderSources");

char* reportJLA(JNIEnv *env);
char* monitor_dump_event(JNIEnv *env);

JLAPullSource* src = NULL;
bool enabled = true;

PullSource* getJLAPullSource(uint32 id) {
	if (!src) {
		src = new JLAPullSource(id);
	}
	return src;
}

monitordata* callback() {
	return src->generateData();
}

void complete(monitordata *mdata) {
	src->pullComplete(mdata);
}

bool JLAPullSource::isEnabled() {
	return enabled;
}

void JLAPullSource::setState(const std::string &newState) {
	enabled = ibmras::common::util::equalsIgnoreCase(newState, "on");
	if (src) {
		src->publishConfig();
	}
}

JLAPullSource::JLAPullSource(uint32 id) :PullSource(id, "Health Center (locking)") {
}

void JLAPullSource::publishConfig() {
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	ibmras::monitoring::connector::ConnectorManager *conMan =
			agent->getConnectionManager();

	std::string msg = "locking_subsystem=";
	if (isEnabled()) {
		msg += "on";
	} else {
		msg += "off";
	}

	conMan->sendMessage("configuration/locking", msg.length(),
			(void*) msg.c_str());
}

uint32 JLAPullSource::getSourceID() {
	return JLA;
}

pullsource* JLAPullSource::getDescriptor() {

	pullsource* src = new pullsource();
	src->header.name = "locking";
	src->header.description = "Locking information";
	src->header.sourceID = JLA;
	src->header.capacity = 256 * 1024;
	src->next = NULL;
	src->callback = callback;
	src->complete = complete;
	src->pullInterval = 120;

	return src;
}

monitordata* JLAPullSource::sourceData(jvmFunctions* tdpp, JNIEnv* env) {
	monitordata* data = new monitordata;
	data->size = 0;
	data->data = NULL;

	if (isEnabled()) {
		data->persistent = false;
		std::stringstream ss;

		data->provID = getProvID();
		data->sourceID = JLA;

		unsigned long long millisecondsSinceEpoch;

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

		ss << "reportTime," << millisecondsSinceEpoch << ",";

		char* value = reportJLA(env);

		ss << value;

		ibmras::monitoring::plugins::jni::hc_dealloc(reinterpret_cast<unsigned char**>(&value));

		std::string lockingdata = ss.str();

		int len = lockingdata.length();

		char* sval = ibmras::common::util::createAsciiString(lockingdata.c_str());

		if (sval) {
			data->size = len;
			data->data = sval;
		}
	}
	return data;
}

char* reportJLA(JNIEnv *env) {
	char * reportData = NULL;

	IBMRAS_DEBUG(debug, "> reportJLA");

	/* Create the report to pass to the gui */
	reportData = monitor_dump_event(env);

	IBMRAS_DEBUG(debug, "< reportJLA");
	return reportData;
}

char* monitor_dump_event(JNIEnv *env) {
	void ** q = NULL;
	void * p = 0;


	UINT64 tm_delta;
	UINT64 tm_curr;

	static int dcnt = 0;

	char * beg;
	char * end;
	char * stt;
	unsigned char ty;
	int i;
	int psz = sizeof(void *);

	int reportArraySize = 100;
	char ** reportLineArray = NULL;

	char * finalReport = NULL; /* This will contain the full report to be passed back */
	int reportLineCount = 0; /* A count of the number of lines in the report */

	int rawcnt = 0;
	int infcnt = 0;

	char buffer[1000];

	const char* repjlmfmt =
			"%s, %5.0f, %8d, %8d, %8d, %8d, %8d, %8d, %5.0f, %8.0f,  %s,";

	const char* javaMon = "javaMonitor";
	const char* systemMon = "systemMonitor";
	const char* reportEnd = "reportcomplete";

	jdata_t * jp = NULL;

	int JMONMAX;
	int SMONMAX;

	int arrj_size;
	int arrs_size;

	jdata_t ** arrjmon = NULL;
	jdata_t ** arrsmon = NULL;

	jvmtiEnv* pti = getTDPP()->pti;
	/*** code starts ***/
	IBMRAS_DEBUG(debug, "> monitor_dump_event");

	/* before we do anything else, create this - we need to be sure we can free it later. */
	finalReport = (char*) ibmras::monitoring::plugins::jni::hc_alloc(
			sizeof(char) * 500);
	if (finalReport == NULL) {
		goto cleanup;
	}

	reportLineArray = (char**) ibmras::monitoring::plugins::jni::hc_alloc(
			reportArraySize * sizeof reportLineArray);
	if (reportLineArray == NULL) {
		goto cleanup;
	}

	getTDPP()->dumpVMLockMonitor(pti, &p);
	if (0 == p) {
		/* if the dumpVMlockMonitor call has failed returned a blank string
		 * this seems to happen quite often (intermittently 50% of the time?)
		 * there doesn't seem to be an impact on our code
		 */
		goto cleanup;
	} else {
		q = (void **) p;
	}

	//int pid = ibmras::common::port::getProcessId();

	dcnt++; /* Get current report run number */

	beg = reinterpret_cast<char*>(q[0]);
	end = reinterpret_cast<char*>(q[1]);

	/* calc max entries divided by 32*/
	JMONMAX = (int) ((end - beg) / 32);
	SMONMAX = (int) (end - beg);

	arrj_size = JMONMAX * sizeof(jdata_t *);
	arrjmon = (jdata_t **) ibmras::monitoring::plugins::jni::hc_alloc(
			arrj_size);
	if (arrjmon == NULL) {
		goto cleanup;
	}

	arrs_size = SMONMAX * sizeof(jdata_t *);
	arrsmon = (jdata_t **) ibmras::monitoring::plugins::jni::hc_alloc(
			arrs_size);
	if (arrsmon == NULL) {
		goto cleanup;
	}

	tm_curr = read_cycles_on_processor(0);
	tm_delta = tm_curr - tm_stt;

	/* output interval time */
	sprintf(buffer, "IntervalTime, %" _P64 "d,", tm_delta);
	reportLineArray[reportLineCount] =
			reinterpret_cast<char*>(ibmras::monitoring::plugins::jni::hc_alloc(
					strlen(buffer) + 1));
	if (reportLineArray[reportLineCount] == NULL) {
		goto cleanup;
	}
	strcpy(reportLineArray[reportLineCount++], buffer);
	IBMRAS_DEBUG_2(fine, "%d, %s", strlen(reportLineArray[reportLineCount-1]), reportLineArray[reportLineCount-1]);

	stt = beg;

	while (stt < end) {
		unsigned char held;
		unsigned int htlo, hthi;
		double dhtm;

		jp = (jdata_t *) ibmras::monitoring::plugins::jni::hc_alloc(
				sizeof(jdata_t));
		if (jp == NULL) {
			goto cleanup;
		}

		ty = (unsigned char) *stt;
		stt++;
		held = (unsigned char) *stt;
		stt++;

		/* The VM still uses JVMPI constants (1 and 2) even though it's not JVMPI anymore
		 We replicate these in our header file. */
		if (ty == JVMPI_MONITOR_JAVA) {
			arrjmon[infcnt] = jp;
			infcnt++;
			if (infcnt >= JMONMAX) {
				sprintf(finalReport, "Stopping JLM, Java Monitor Count %d\n",
						JMONMAX);
				goto returnReport;
			}
		} else if (ty == JVMPI_MONITOR_RAW) {
			arrsmon[rawcnt] = jp;
			rawcnt++;
			if (rawcnt >= SMONMAX) {
				sprintf(finalReport, "Stopping JLM, Raw Monitor Count %d\n",
						SMONMAX);
				goto returnReport;
			}
		} else {
			sprintf(finalReport, "Stopping JLM, Bad Data\n");
			goto returnReport;
		}

		jp->gets = dump_read_u4(stt);
		stt += 4;
		jp->slow = dump_read_u4(stt);
		stt += 4;
		jp->rec = dump_read_u4(stt);
		stt += 4;
		jp->tier2 = dump_read_u4(stt);
		stt += 4;
		jp->tier3 = dump_read_u4(stt);
		stt += 4;
		hthi = (unsigned int) dump_read_u4(stt);
		stt += 4;
		htlo = (unsigned int) dump_read_u4(stt);
		stt += 4;

		jp->hthi = hthi;
		jp->htlo = htlo;

		stt += psz;

		jp->mnm = ibmras::monitoring::plugins::jni::dupJavaStr(stt);
		if (jp->mnm == NULL) {
			goto returnReport;
		}

		stt += strlen(jp->mnm) + 1;

		jp->nrec = jp->gets - jp->rec;
		jp->fast = jp->nrec - jp->slow;

		if (jp->gets > 0) {
			dhtm = (double) htlo;
			dhtm += 1024.0 * 1024.0 * 4096.0 * (double) hthi;
			jp->dhtm = dhtm;
			jp->util = 100.0 * dhtm / (double) (INT64) tm_delta;

			if (jp->nrec > 0) {
				jp->averht = dhtm / (double) (INT64) jp->nrec;
				jp->miss = 100.0 * jp->slow / (double) jp->nrec;
			} else {
				jp->averht = 0.0;
				jp->miss = 0.0;
			}
		} else {
			jp->util = jp->miss = jp->averht = 0.0;
		}
	}

	qsort((void *) arrsmon, rawcnt, sizeof(jdata_t *), qcmp_jlm);

	/*  This is the start of the block that outputs the system monitors to the report.  The calculations have all been
	 done at this point and stored in the arrsmon[] array */

	for (i = 0; i < rawcnt; i++) {
		jp = arrsmon[i];

		/* Add the reportLine to the array */
		sprintf(buffer, repjlmfmt, systemMon, jp->miss, jp->gets, jp->nrec,
				jp->slow, jp->rec, jp->tier2, jp->tier3, jp->util, jp->averht,
				jp->mnm);
		reportLineArray[reportLineCount] =
				reinterpret_cast<char*>(ibmras::monitoring::plugins::jni::hc_alloc(
						strlen(buffer) + 1));
		if (reportLineArray[reportLineCount] == NULL) {
			goto cleanup;
		}
		strcpy(reportLineArray[reportLineCount++], buffer);
		IBMRAS_DEBUG_2(fine, "%d, %s", strlen(reportLineArray[reportLineCount-1]), reportLineArray[reportLineCount-1]);

		/*
		 * check to make sure we have not grown to big for the array
		 */
		if (reportLineCount >= reportArraySize) {
			/* add another 100 lines to the report */
			reportArraySize += 100;

			reportLineArray =
					(char**) ibmras::monitoring::plugins::jni::hc_realloc_ptr_array(
							&reportLineArray, reportLineCount, reportArraySize);
			if (reportLineArray == NULL) {
				goto cleanup;
			}
		}
	}
	/*  This is the end of the system monitor block */

	qsort((void *) arrjmon, infcnt, sizeof(jdata_t *), qcmp_jlm);

	/*  This is the start of the block that outputs the java program monitors to the report.  The calculations have all been
	 done at this point and stored in the arrjmon[] array */
	for (i = 0; i < infcnt; i++) {
		jp = arrjmon[i];

		/* Add the reportLine to the array */
		sprintf(buffer, repjlmfmt, javaMon, jp->miss, jp->gets, jp->nrec,
				jp->slow, jp->rec, jp->tier2, jp->tier3, jp->util, jp->averht,
				jp->mnm);
		reportLineArray[reportLineCount] =
				reinterpret_cast<char*>(ibmras::monitoring::plugins::jni::hc_alloc(
						strlen(buffer) + 1));
		if (reportLineArray[reportLineCount] == NULL) {
			goto cleanup;
		}
		strcpy(reportLineArray[reportLineCount++], buffer);
		IBMRAS_DEBUG_2(fine, "%d, %s", strlen(reportLineArray[reportLineCount-1]), reportLineArray[reportLineCount-1]);

		/*
		 * check to make sure we have not grown to big for the array
		 */
		if (reportLineCount >= reportArraySize) {
			/* add another 100 lines to the report */
			reportArraySize += 100;

			reportLineArray =
					(char**) ibmras::monitoring::plugins::jni::hc_realloc_ptr_array(
							&reportLineArray, reportLineCount, reportArraySize);
			if (reportLineArray == NULL) {
				goto cleanup;
			}
		}
	}

	/*  This is the end of the java monitor block */
	reportLineArray[reportLineCount] =
			reinterpret_cast<char*>(ibmras::monitoring::plugins::jni::hc_alloc(
					strlen(reportEnd) + 1));
	if (reportLineArray[reportLineCount] == NULL) {
		goto cleanup;
	}
	sprintf(reportLineArray[reportLineCount++], "%s", reportEnd);
	IBMRAS_DEBUG_1(fine, "%s", reportLineArray[reportLineCount-1]);

	/* if we get this far we've not used the original finalReport that we malloced memory for, join_strings will
	 * malloc more memory so lets free the original one
	 */
	ibmras::monitoring::plugins::jni::hc_dealloc(
			(unsigned char**) &finalReport);

	/* Now that the array is complete we need to merge all those strings into one final one to pass
	 pass back */
	finalReport = ibmras::monitoring::plugins::jni::join_strings(
			reportLineArray, reportLineCount);
	if (finalReport != NULL) {
		goto returnReport;
	}

	/* cleanup - returns a NULL report */
	cleanup: IBMRAS_DEBUG(debug, "monitor_dump_event: cleanup");
	ibmras::monitoring::plugins::jni::hc_dealloc(
			(unsigned char**) &finalReport);
	/* returnReport - returns a report to be deallocated by the caller */

	returnReport:

	IBMRAS_DEBUG(debug, "monitor_dump_event: returnReport");

	if (p) {
		ibmras::monitoring::plugins::jni::hc_dealloc((unsigned char**) &q[0]);
		ibmras::monitoring::plugins::jni::hc_dealloc((unsigned char**) &p);
	}

	for (i = 0; i < rawcnt; i++) {
		ibmras::monitoring::plugins::jni::hc_dealloc(
				(unsigned char**) &arrsmon[i]->mnm);
		ibmras::monitoring::plugins::jni::hc_dealloc(
				(unsigned char**) &arrsmon[i]);
	}

	for (i = 0; i < infcnt; i++) {
		ibmras::monitoring::plugins::jni::hc_dealloc(
				(unsigned char**) &arrjmon[i]->mnm);
		ibmras::monitoring::plugins::jni::hc_dealloc(
				(unsigned char**) &arrjmon[i]);
	}

	ibmras::monitoring::plugins::jni::hc_dealloc((unsigned char**) &arrjmon);
	ibmras::monitoring::plugins::jni::hc_dealloc((unsigned char**) &arrsmon);

	ibmras::monitoring::plugins::jni::dealloc_report_lines(reportLineArray,
			reportArraySize);
	ibmras::monitoring::plugins::jni::hc_dealloc(
			(unsigned char**) &reportLineArray);

	IBMRAS_DEBUG(debug, "< monitor_dump_event");
	return finalReport;
}

} /* end namespace locking */
} /* end namespace jni */
} /* end namespace plugins */
} /* end namespace monitoring */
} /* end namespace ibmras */
