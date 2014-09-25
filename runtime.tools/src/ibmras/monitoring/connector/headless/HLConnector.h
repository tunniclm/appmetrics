#ifndef ibmras_monitoring_connector_headless_hlconnector_h
#define ibmras_monitoring_connector_headless_hlconnector_h

#include <string>
#include <vector>
#include <map>
#include <ctime>

#include "jni.h"

#include "ibmras/monitoring/connector/Connector.h"
#include "ibmras/monitoring/agent/BucketList.h"
#include "ibmras/monitoring/connector/Receiver.h"
#include "ibmras/common/port/ThreadData.h"

namespace ibmras {
namespace monitoring {
namespace connector {
namespace headless {

class HLConnector: public ibmras::monitoring::connector::Connector {
public:

	static HLConnector* getInstance(JavaVM* theVM, std::string &options, const char* properties);
	static HLConnector* getInstance();

	int32 getRunDuration();
	int32 getRunPause();
	int32 getNumberOfRuns();
	int32 getTimesRun();
	void incrementRuns();

	virtual ~HLConnector();
	virtual std::string getID() {return "HLConnector"; }
	int sendMessage(const std::string &sourceID, uint32 size, void* data);

	int start();
	int stop();

private:

	JavaVM* vm;
	JNIEnv* env;
	std::string agentOptions;
	jclass zipJNIclazz;
	jobject zipClazzObject;
	jmethodID zipMethod;
	int32 seqNumber;
	time_t lastPacked;
	uint32 time_interval;
	uint32 upper_limit;
	int32 files_to_keep;
	std::map<std::string, std::fstream*> createdFiles;
	std::map<std::string, std::string> expandedIDs;
	ibmras::common::port::Lock* lock;
	int32 run_duration;
	int32 run_pause;
	int32 number_runs;
	std::string userDefinedPath;
	std::string userDefinedPrefix;
	int32 times_run;
	std::time_t startTime;
	char startDate[100];

	static const int32 default_upper_limit = INT_MAX;
	static const uint32 default_time_interval = 0;
	static const uint32 default_files_to_keep = 0;
	static const uint32 default_run_duration = 0;
	static const uint32 default_run_pause = 0;
	static const uint32 default_number_runs = 0;

	HLConnector(JavaVM* theVM, std::string &options, const char* properties);
	void createFile(const std::string &fileName);
	int packFiles();
	bool createDirectory();
};

void* runCounterThread(ibmras::common::port::ThreadData* tData);


} /*end namespace headless*/
} /*end namespace connector*/
} /*end namespace monitoring*/
} /*end namespace ibmras*/

#endif /*ibmras_monitoring_connector_headless_hlconnector_h*/