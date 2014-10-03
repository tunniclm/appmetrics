/*
 * HLConnector.cpp
 *
 *  Created on: 20 Aug 2014
 *      Author: Admin
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <cstdio>
#include <cstdlib>
#include <errno.h>


#if defined(WINDOWS)
#include <sys/stat.h>
#include "windows.h"
#include "WinBase.h"
#include "direct.h"
#include "io.h"
#define PATHSEPARATOR "\\"
#else  /* Unix platforms */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define PATHSEPARATOR "/"
#endif

#include "ibmras/monitoring/connector/headless/HLConnector.h"
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/logging.h"
#include "ibmras/common/util/strUtils.h"

namespace ibmras {
namespace monitoring {
namespace connector {
namespace headless {

IBMRAS_DEFINE_LOGGER("HeadlessConnector");

static HLConnector* instance = NULL;
static bool running = true;
static bool collect = true;


HLConnector* HLConnector::getInstance(JavaVM* theVM) {
	if (!instance) {
		instance = new HLConnector(theVM);
	}
	return instance;
}

HLConnector* HLConnector::getInstance() {
	if (!instance) {
		return NULL;
	}
	return instance;
}

HLConnector::HLConnector(JavaVM* theVM) :
		enabled(false), vm(theVM), env(NULL), zipJNIclazz(NULL), zipClazzObject(
				NULL), zipMethod(NULL), seqNumber(1), lastPacked(0), times_run(
				0) {

	number_runs = 0;
	run_duration = 0;
	time_interval = 0;
	startTime = 0;
	files_to_keep = 0;
	run_pause = 0;
	upper_limit = INT_MAX;
	lock = new ibmras::common::port::Lock;
}

HLConnector::~HLConnector() {

}

int32 HLConnector::getRunDuration() {
	return run_duration;
}

int32 HLConnector::getRunPause() {
	return run_pause;
}

int32 HLConnector::getNumberOfRuns() {
	return number_runs;
}

int32 HLConnector::getTimesRun() {
	return times_run;
}

void HLConnector::incrementRuns() {
	times_run++;
}

int HLConnector::start() {

	IBMRAS_DEBUG(debug, ">>>HLConnector::start()");
	ibmras::monitoring::agent::Agent* agent =
			ibmras::monitoring::agent::Agent::getInstance();

	std::string enabledProp = agent->getAgentProperty("headless");
	if (ibmras::common::util::equalsIgnoreCase(enabledProp, "on")) {
		enabled = true;
	} else {
		enabled = false;
		collect = false;
		return 0;
	}

	std::string tiString = agent->getAgentProperty("headless.run.time.loop");
	if (tiString.length()) {
		time_interval = atoi(tiString.c_str()) * 60;
	}
	IBMRAS_DEBUG_1(debug, "Time interval = %d", time_interval);
	IBMRAS_DEBUG_1(debug, "User time interval = %d", tiString.c_str());

	std::string ulString = agent->getAgentProperty("headless.files.max.size");
	if (ulString.length()) {
		upper_limit = atoi(ulString.c_str());
	}
	IBMRAS_DEBUG_1(debug, "upper_limit = %d", upper_limit);
	IBMRAS_DEBUG_1(debug, "User upper_limit = %d", ulString.c_str());

	std::string fkString = agent->getAgentProperty("headless.files.to.keep");
	if (fkString.length()) {
		files_to_keep = atoi(fkString.c_str());
	}
	IBMRAS_DEBUG_1(debug, "files_to_keep = %d", files_to_keep);
	IBMRAS_DEBUG_1(debug, "User files_to_keep = %s", fkString.c_str());


	std::string rdString = agent->getAgentProperty("headless.run.duration");
	if (rdString.length()) {
		run_duration = atoi(rdString.c_str()) * 60;
	}
	IBMRAS_DEBUG_1(debug, "run_duration = %d", run_duration);
	IBMRAS_DEBUG_1(debug, "User run_duration = %d", rdString.c_str());

	std::string rpString = agent->getAgentProperty(
			"headless.run.pause.duration");
	if (rpString.length()) {
		run_pause = atoi(rpString.c_str()) * 60;
	}
	IBMRAS_DEBUG_1(debug, "run_pause = %d", run_pause);
	IBMRAS_DEBUG_1(debug, "User run_pause = %d", rpString.c_str());

	std::string nrString = agent->getAgentProperty(
			"headless.run.number.of.runs");
	if (nrString.length()) {
		number_runs = atoi(nrString.c_str());
	}
	IBMRAS_DEBUG_1(debug,  "number_runs = %d", number_runs);
	IBMRAS_DEBUG_1(debug,  "User number_runs = %d", nrString.c_str());

	time(&startTime);
	lastPacked = startTime;
	struct tm *startTimeStruct;
	startTimeStruct = ::localtime(&startTime);
	strftime(startDate, 20, "%d%m%y_%H%M%S_", startTimeStruct);

//The default path will be the current directory (where the soft monitored is being run)
	std::string defaultPath;
#if defined(WINDOWS)
	TCHAR cDirectory[MAX_PATH];
	DWORD dwRes;
	dwRes = GetCurrentDirectory(sizeof(cDirectory), cDirectory);
#else
	char cDirectory[FILENAME_MAX];
	getcwd(cDirectory, sizeof(cDirectory));
#endif
	defaultPath = cDirectory;

	std::string outputDir = agent->getAgentProperty(
			"headless.output.directory");
	if (!outputDir.length()) {
		userDefinedPath = defaultPath;
	} else {
		userDefinedPath = outputDir;
		if(!createDirectory(userDefinedPath)) {
			IBMRAS_DEBUG(warning,  "The directory could not be created, using default path");
			userDefinedPath = defaultPath;
		}
	}
	IBMRAS_DEBUG_1(debug,  "Path = %s", userDefinedPath.c_str());

	//The temporary files will be written at a temporary directory under the user defined path
	//(or the current directory if the one requested by user could not be created.)
	tmpPath = userDefinedPath;
	tmpPath.append(PATHSEPARATOR);
	tmpPath.append("tmp_");
	tmpPath.append(startDate);
	createDirectory(tmpPath);

	std::string filePrefix = agent->getAgentProperty("headless.filename");
	if (!filePrefix.length()) {
		userDefinedPrefix = "";
	} else {
		IBMRAS_DEBUG_1(debug, "Prefix = %s", filePrefix.c_str());
		userDefinedPrefix = filePrefix;
	}
	IBMRAS_DEBUG_1(debug, "Prefix = %s", userDefinedPrefix.c_str());


	/***
	 * First we create a vector<string> which will contain the IDs of the datasources,
	 * these names will match the names of the files created by createFile
	 */
	std::vector<std::string> sourceIDs = agent->getBucketList()->getIDs();

	for (std::vector<std::string>::iterator it = sourceIDs.begin();
			it != sourceIDs.end(); ++it) {
		createFile(*it);
	}

	if (createdFiles.size() != sourceIDs.size()) {
		return -1;
	}

	/***
	 * If we are running Java we will have to pass the parameters using JNI,
	 * to do so we will create a UTF8-String array and pass it to create
	 * an object of the java class used to compress and archive the files.
	 * This parameter is the list of files that will have to be compressed.
	 * The Java class has a member-method to compress and pack each of
	 * the files in a zip archive whose name is provided as argument of the
	 * mentioned method.
	 */

	JavaVMAttachArgs threadArgs;

	memset(&threadArgs, 0, sizeof(threadArgs));
	threadArgs.version = JNI_VERSION_1_6;
	threadArgs.name = (char *) "HLCThread";
	threadArgs.group = NULL;

	IBMRAS_DEBUG(debug, "Attaching to thread");
	jint result =
			vm ? vm->AttachCurrentThread((void**) &env, (void*) &threadArgs) : -1;
	if (JNI_OK != result) {
		IBMRAS_DEBUG(warning, "Cannot set environment");
		IBMRAS_DEBUG(debug, "<<HLConector [NOATTACH]");
		return -1;
	} IBMRAS_DEBUG(info, "Environment set");

	/***
	 * We will create the String[] now. Creating an object array with NewObjectArray requires us to
	 * provide the specific class from which the instances are created.
	 */
	jint size = createdFiles.size();
	jint i = 0;
	IBMRAS_DEBUG(debug, "Discovering String class");
	jclass javaStringClazz = env->FindClass("java/lang/String");
	if (env->ExceptionOccurred()) {
		IBMRAS_DEBUG(warning, "Failed to find java/lang/String");
		env->ExceptionDescribe();
		//clear the exception if we are not handling it in Java
		env->ExceptionClear();

		return -1;
	}

	if (!javaStringClazz) {
		IBMRAS_DEBUG(warning, "Failed to find java/lang/String");
		return -1;
	} IBMRAS_DEBUG(debug, "String class found");

	jobjectArray fileNames = env->NewObjectArray(size, javaStringClazz, 0);

	/***
	 * Here the string array is populated with the values
	 */
	jstring currentFileName;

	for (std::map<std::string, std::string>::iterator it = expandedIDs.begin();
			it != expandedIDs.end(); ++it) {

		currentFileName = env->NewStringUTF((it->second).c_str());
		if (env->ExceptionOccurred()) {
			IBMRAS_DEBUG(warning, "Failed to create UTF string");
			env->ExceptionDescribe();
			//clear the exception if we are not handling it in Java
			env->ExceptionClear();

			return -1;
		}

		env->SetObjectArrayElement(fileNames, i, currentFileName);
		if (env->ExceptionCheck()) {
			IBMRAS_DEBUG_1(warning, "Failed to set object %d in array fileNames", i);
			env->ExceptionDescribe();
			//clear the exception if we are not handling it in Java
			env->ExceptionClear();

			return -1;
		}
		i++;
	} IBMRAS_DEBUG(debug, "jobjectArray created succesfully");

	/***
	 * Finally, the object that will zip the files is instantiated here.
	 */

	IBMRAS_DEBUG(debug, "Discovering HeadlessZipUtils class");
	jclass localzipJNIclazz = env->FindClass(
			"runtime/tools/java/connectors/headless/HeadlessZipUtils");
	zipJNIclazz = reinterpret_cast<jclass>(env->NewGlobalRef(localzipJNIclazz));
	env->DeleteLocalRef(localzipJNIclazz);
	if (env->ExceptionOccurred()) {
		IBMRAS_DEBUG(warning, "Failed to find HeadlessZipUtils");
		env->ExceptionDescribe();
		//clear the exception if we are not handling it in Java
		env->ExceptionClear();
		return -1;
	}

	IBMRAS_DEBUG(debug, "Getting ID of HeadlessZipUtils constructor");
	jmethodID constructor = env->GetMethodID(zipJNIclazz, "<init>",
			"([Ljava/lang/String;)V");
	if (!constructor) {
		IBMRAS_DEBUG(warning, "Failed to get constructor method ID");
		return -1;
	}

	IBMRAS_DEBUG(debug, "Creating HLZiputils instance");
	jobject localzipClazzObject = env->NewObject(zipJNIclazz, constructor,
			fileNames);
	zipClazzObject = reinterpret_cast<jobject>(env->NewGlobalRef(
			localzipClazzObject));
	env->DeleteLocalRef(localzipClazzObject);
	if (!zipClazzObject) {
		IBMRAS_DEBUG(warning, "Failed to instantiate HeadlessZipUtils class");
		return -1;
	}

	IBMRAS_DEBUG(debug, "Discovering zip method");
	zipMethod = env->GetMethodID(zipJNIclazz, "packFiles",
			"(Ljava/lang/String;)V");
	if (!zipMethod) {
		IBMRAS_DEBUG(warning, "Failed to find zip mehtod");
		return -1;
	}

	ibmras::common::port::ThreadData* data =
			new ibmras::common::port::ThreadData(runCounterThread);
	data->setArgs(this);
	int tResult = ibmras::common::port::createThread(data);

	std::stringstream ss;
	ss << userDefinedPath;
	ss << PATHSEPARATOR;
	if(userDefinedPrefix.length()) {
		ss << userDefinedPrefix;
		ss << "_";
	}
	ss << "healthcenter";
	ss << startDate;
	ss << ibmras::common::port::getProcessId() << "_";

	hcdName = ss.str();

	IBMRAS_DEBUG(debug, "<<<HLConnector::start()");

	return 0;
}

void HLConnector::createFile(const std::string &fileName) {
	IBMRAS_DEBUG(debug, ">>>HLConnector::createFile()");
	std::fstream* file = new std::fstream;

	std::string fullPath = tmpPath;
	fullPath.append(PATHSEPARATOR);
	fullPath.append(fileName);

	file->open(fullPath.c_str(), std::ios::out | std::ios::app);

	createdFiles[fullPath] = file;
	expandedIDs[fileName] = fullPath;
	IBMRAS_DEBUG(debug, "<<<HLConnector::createFile()");
}

bool HLConnector::createDirectory(std::string& path) {
	IBMRAS_DEBUG(debug, ">>>HLConnector::createDirectory");
	bool created = false;

	const char* pathName = path.c_str();

#if defined(WINDOWS)
	DWORD dirAttr;
	IBMRAS_DEBUG_1(debug, "Creating directory: %s", pathName);
	dirAttr = GetFileAttributes(reinterpret_cast<LPCTSTR>(pathName));

	if(INVALID_FILE_ATTRIBUTES == dirAttr) {
		switch (GetLastError()) {
			case ERROR_PATH_NOT_FOUND:
				IBMRAS_DEBUG(warning, "The directory was not found");
				IBMRAS_DEBUG_1(debug, "Creating directory: %s", pathName);
				if(!CreateDirectory(reinterpret_cast<LPCTSTR>(pathName), NULL)) {
					switch (GetLastError()) {
						//if the directory already exists we will use it instead of the current one.
						case ERROR_ALREADY_EXISTS:
							IBMRAS_DEBUG(warning, "The specified directory already exists.");
							created = true;
							break;
						case ERROR_PATH_NOT_FOUND:
							IBMRAS_DEBUG(warning, "The system cannot find the path specified.");
							break;
					}
				} else {
					created = true;
				}
				break;
			case ERROR_INVALID_NAME:
			IBMRAS_DEBUG(warning, "The filename, directory name, or volume label syntax is incorrect");
			break;
			case ERROR_BAD_NETPATH:
			IBMRAS_DEBUG(warning, "The network path was not found.");
			break;
			default:
			IBMRAS_DEBUG(warning, "The directory could not be found, permissions?.");
			IBMRAS_DEBUG_1(debug, "Creating directory: %s", pathName);
			if(!CreateDirectory(reinterpret_cast<LPCTSTR>(pathName), NULL)) {
				switch (GetLastError()) {
					case ERROR_ALREADY_EXISTS:
					IBMRAS_DEBUG(warning, "The specified directory already exists.");
					created = true;
					break;
					case ERROR_PATH_NOT_FOUND:
					IBMRAS_DEBUG(warning, "The system cannot find the path specified.");
					break;
				}
			} else {
				created = true;
			}
		}
	}

#elif defined(_LINUX)
	struct stat dir;
	IBMRAS_DEBUG_1(debug, "Pathname...%s\n", pathName);
	if (stat(pathName, &dir)) {
		IBMRAS_DEBUG_1(debug, "Directory does not exist, creating...%s\n", pathName);
		if (mkdir(pathName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) {
			IBMRAS_DEBUG_1(debug, "Directory could not be created: ", strerror(errno));
			if(EEXIST == errno) {
				IBMRAS_DEBUG_1(debug, "Directory % already existed", pathName);
				created = true;
			}
		} else {
			IBMRAS_DEBUG_1(debug, "Directory %s was created: ", pathName);
			created = true;
		}
	} else {
		IBMRAS_DEBUG(debug, "stat() returned 0, we'll check whether it was an existing directory");
		if(S_ISDIR(dir.st_mode)) {
			created = true;
		}
	}
#endif
	IBMRAS_DEBUG(debug, "<<<HLConnector::createDirectory()");

	return created;
}

int HLConnector::stop() {
	IBMRAS_DEBUG(debug, ">>>HLConnector::stop()");

	if (enabled == false) {
		return 0;
	}

	packFiles();

	for (std::map<std::string, std::fstream*>::iterator it =
			createdFiles.begin(); it != createdFiles.end(); ++it) {

		std::string fileName = it->first;
		std::fstream* currentSource = it->second;

		if (currentSource->is_open()) {
			currentSource->close();
			if (remove(fileName.c_str())) {
				IBMRAS_DEBUG_1(debug, "Deletion failed: %s\n", strerror(errno));
				delete currentSource;
				return -1;
			}
		} else {
			if (remove(fileName.c_str())) {
				IBMRAS_DEBUG_1(debug, "Deletion failed: %s\n", strerror(errno));
				delete currentSource;
				return -1;
			}
		}

	}
#if defined(WINDOWS)
	if(_rmdir(tmpPath.c_str())) {
		IBMRAS_DEBUG_1(debug, "Deletion failed: %s\n", strerror(errno));
	}
#else
	if(remove(tmpPath.c_str())) {
			IBMRAS_DEBUG_1(debug, "Deletion failed: %s\n", strerror(errno));
	}
#endif
	running = false;

	IBMRAS_DEBUG(debug, "<<<HLConnector::stop()");
	return 0;
}

int HLConnector::sendMessage(const std::string &sourceId, uint32 size,
		void* data) {

	if (!collect) {
		return 0;
	}
	IBMRAS_DEBUG(debug, ">>>HLConnector::sendMessage()");

	std::map<std::string, std::string>::iterator it;
	it = expandedIDs.find(sourceId);

	if (it == expandedIDs.end()) {
		return -1;
	}

	std::string currentKey = it->second;
	std::fstream* currentSource = createdFiles[currentKey];

	const char* cdata = reinterpret_cast<const char*>(data);

	if (!lock->acquire()) {
		if (!lock->isDestroyed()) {
			if (currentSource->is_open()) {
				uint32 length = currentSource->tellg();
				currentSource->write(cdata, size);

				std::time_t currentTime;
				time(&currentTime);

				if ((length + size > upper_limit)
						|| (time_interval
								&& ((currentTime - lastPacked) >= time_interval))) {

					time(&lastPacked);

					IBMRAS_DEBUG_1(debug,  "SendMessage from = %s", sourceId.c_str());
					IBMRAS_DEBUG_1(debug,  "MAX_FILE_SIZE = %d", upper_limit);
					IBMRAS_DEBUG_1(debug,  "Current time = %d", currentTime);

					packFiles();
				}

			} else {
				if (remove(currentKey.c_str())) {
					IBMRAS_DEBUG_1(debug, "Deletion failed: %s\n", strerror(errno));
				} else {
					currentSource->open(currentKey.c_str(),
							std::ios::out | std::ios::app);
					currentSource->write(cdata, size);
				}
			}
			lock->release();
		}
	}
	IBMRAS_DEBUG(debug, "<<<HLConnector::sendMessage()");
	return 0;
}

int HLConnector::packFiles() {
	IBMRAS_DEBUG(debug, ">>>HLConnector::packFiles()");

	std::stringstream ss;

	ss << hcdName;
	ss << seqNumber;
	ss << ".hcd";

	std::string hcdFileName = ss.str();

	IBMRAS_DEBUG_1(debug, "User defined path is %s", userDefinedPath.c_str());
	IBMRAS_DEBUG_1(debug, "The full path to the .hcd file is: %s", hcdFileName.c_str());


	jstring hcdJavaFileName;

	JavaVMAttachArgs threadArgs;

	memset(&threadArgs, 0, sizeof(threadArgs));
	threadArgs.version = JNI_VERSION_1_6;
	threadArgs.name = (char *) "HLCThread";
	threadArgs.group = NULL;

	jint result =
			vm ? vm->AttachCurrentThread((void**) &env, (void*) &threadArgs) : -1;
	if (JNI_OK != result) {
		IBMRAS_DEBUG(warning, "Cannot set environment");
		return -1;

	}

	IBMRAS_DEBUG(debug, "Creating hcd name jstring");
	hcdJavaFileName = env->NewStringUTF(hcdFileName.c_str());

	IBMRAS_DEBUG(debug, "Closing files");
	for (std::map<std::string, std::fstream*>::iterator it =
			createdFiles.begin(); it != createdFiles.end(); it++) {
		(it->second)->close();
	}

	IBMRAS_DEBUG(debug, "Calling zipping method");
	env->CallVoidMethod(zipClazzObject, zipMethod, hcdJavaFileName);
	if (env->ExceptionOccurred()) {
		IBMRAS_DEBUG(warning, "Failed to call packfiles method");
		env->ExceptionDescribe();
		//clear the exception if we are not handling it in Java
		env->ExceptionClear();
		return -1;
	} else {
		std::fstream* hcdZipStream = new std::fstream(hcdFileName.c_str());
		hcdZipStream->close();
	}

	IBMRAS_DEBUG_2(debug, "files to keep = %d, seqNumber = %d", files_to_keep, seqNumber);
	if (files_to_keep && (seqNumber - files_to_keep) > 0) {
		std::stringstream hcdRemoveName;
		hcdRemoveName << hcdName << (seqNumber - files_to_keep);
		hcdRemoveName << ".hcd";

		std::fstream* hcdStream = new std::fstream(hcdRemoveName.str().c_str());

		if (hcdStream->good())
			hcdStream->close();

		if (remove(hcdRemoveName.str().c_str())) {
			IBMRAS_DEBUG_1(debug, "Deletion failed: %s\n", strerror(errno));
			seqNumber++;
			delete hcdStream;
			return -1;
		}

		delete hcdStream;

	}


	seqNumber++;
	IBMRAS_DEBUG(debug, "<<<HLConnector::packFiles()");
	return 0;
}

void* runCounterThread(ibmras::common::port::ThreadData* tData) {

	IBMRAS_DEBUG(debug, ">>>runCounterThread");

	HLConnector* hlc = HLConnector::getInstance();
	if (hlc->getNumberOfRuns()) {
		IBMRAS_DEBUG_1(debug, "Produce HCDs for %d seconds", hlc->getRunDuration());
		while (running) {
			if ((hlc->getTimesRun() < hlc->getNumberOfRuns())) {
				collect = true;
				IBMRAS_DEBUG_2(debug,  "We've run %d times and have to run %d in total", hlc->getTimesRun()+1, hlc->getNumberOfRuns());
				IBMRAS_DEBUG_1(debug,  "Produce HCDs for %d seconds", hlc->getRunDuration());
				ibmras::common::port::sleep(hlc->getRunDuration());
				hlc->incrementRuns();
			}
			collect = false;
			IBMRAS_DEBUG_1(warning, "Not producing HCDs for %d seconds", hlc->getRunPause());
			ibmras::common::port::sleep(hlc->getRunPause());

		}
	} else if (hlc->getRunDuration() || hlc->getRunPause()) {
		while (running) {
			collect = true;
			IBMRAS_DEBUG_1(debug, "Produce HCDs for %d seconds", hlc->getRunDuration());
			ibmras::common::port::sleep(hlc->getRunDuration());

			collect = false;
			IBMRAS_DEBUG_1(warning, "Rest for %d seconds", hlc->getRunPause());
			ibmras::common::port::sleep(hlc->getRunPause());
		}
	}

	IBMRAS_DEBUG(debug, "<<<runCounterThread");

	return NULL;
}


} /*end namespace headless*/
} /*end namespace connector*/
} /*end namespace monitoring*/
} /*end namespace ibmras*/
