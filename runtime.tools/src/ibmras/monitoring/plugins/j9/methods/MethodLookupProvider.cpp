/*
 * MethodLookupProvider.cpp
 *
 *  Created on: 21 July 2014
 *      Author: stalleyj
 */

#include "ibmras/monitoring/Plugin.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/monitoring/plugins/j9/methods/MethodLookupProvider.h"
#include "ibmras/monitoring/plugins/jni/CFacade.h"
#include "ibmras/common/util/memUtils.h"
#include "ibmjvmti.h"
#include "jni.h"
#include "jvmti.h"
#include "ibmras/common/util/strUtils.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include<stdlib.h>

#define JNI_VERSION JNI_VERSION_1_4

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace methods {

PUSH_CALLBACK sendMethodData;
uint32 MethodLookupProvider::providerID = 0;

int startReceiver() {
	MethodLookupProvider *mlp =
			(MethodLookupProvider*) MethodLookupProvider::getInstance();
	mlp->sendMethodDictionary();
	return 0;
}

int stopReceiver() {
	return 0;
}

pushsource* MethodLookupProvider::registerPushSource(
		void (*callback)(monitordata* data), uint32 provID) {
	pushsource *src = new pushsource();
	src->header.name = "methoddictionary";
	src->header.description =
			"Method lookup data which maps hex value to method data";
	src->header.sourceID = 0;
	src->next = NULL;
	src->header.capacity = 1048576; /* 1MB bucket capacity */
	src->header.config = "methoddictionary_subsystem=on";
	MethodLookupProvider::providerID = provID;
	ibmras::monitoring::plugins::j9::methods::sendMethodData = callback;

	return src;
}

MethodLookupProvider::MethodLookupProvider(jvmFunctions functions) {
	vmFunctions = functions;
	name = "Method Lookup";
	pull = NULL;
	push = registerPushSource;
	start = ibmras::monitoring::plugins::j9::methods::startReceiver;
	stop = ibmras::monitoring::plugins::j9::methods::stopReceiver;
	handle = NULL;
	type = ibmras::monitoring::plugin::data
			| ibmras::monitoring::plugin::receiver;
	recvfactory = (RECEIVER_FACTORY) MethodLookupProvider::getInstance;
	confactory = NULL;
}

MethodLookupProvider::~MethodLookupProvider() {
}

MethodLookupProvider* instance = NULL;

MethodLookupProvider* MethodLookupProvider::getInstance(
		jvmFunctions functions) {
	if (!instance) {
		instance = new MethodLookupProvider(functions);
	}
	return instance;
}

void* MethodLookupProvider::getInstance() {
	if (!instance) {
		return NULL;
	}
	return instance;
}

void MethodLookupProvider::receiveMessage(const std::string &id, uint32 size,
		void *data) {
	// Send the initial empty dictionary
	if (id == "methoddictionary") {
		if (size == 0 || data == NULL) {
			sendMethodDictionary();
		} else {
			std::string message((const char*) data, size);
			std::size_t found = message.find(',');
			if (found != std::string::npos) {
				std::string command = message.substr(0, found);
				std::string rest = message.substr(found + 1);
				std::vector<std::string> parameters =
						ibmras::common::util::split(rest, ',');

				getMethodIDs(parameters);
			}
		}
	}
}

void MethodLookupProvider::getMethodIDs(std::vector<std::string> &jsMethodIds) {
	JNIEnv *env;
	void *ramMethods = NULL;
	void **ramMethodsPtr = NULL;
	void *descriptorBuffer = NULL;

	int numberOfMethods = jsMethodIds.size();

	int rc = vmFunctions.theVM->GetEnv((void **) &env, JNI_VERSION);
	vmFunctions.theVM->AttachCurrentThread((void **) &env, NULL);

	if (vmFunctions.jvmtiGetMethodAndClassNames != 0 && numberOfMethods > 0) {
		jvmtiError error;
		char stringBytes[200000];
		int stringBytesLength = 200000;

		/* Allocate memory for the method identifiers */
		ramMethods = (char**) ibmras::monitoring::plugins::jni::hc_alloc(
				sizeof(void*) * numberOfMethods);
		if (ramMethods == NULL) {
			goto cleanup;
		}

		descriptorBuffer =
				(unsigned char**) ibmras::monitoring::plugins::jni::hc_alloc(
						sizeof(jvmtiExtensionRamMethodData) * numberOfMethods);
		if (descriptorBuffer == NULL) {
			goto cleanup;
		}

		/* Set up the array of method identifierss */
		ramMethodsPtr = (void**) ramMethods;
		char * pEnd;
		int i = 0;
		for (std::vector<std::string>::iterator it = jsMethodIds.begin();
				it != jsMethodIds.end(); ++it) {
			ramMethodsPtr[i] = (void*) strtol(((std::string) *it).c_str(),
					&pEnd, 16);
			i++;
		}

		error = vmFunctions.jvmtiGetMethodAndClassNames(vmFunctions.pti,
				ramMethods, numberOfMethods, descriptorBuffer, stringBytes,
				&stringBytesLength);
		if (error == JVMTI_ERROR_NONE) {
			jvmtiExtensionRamMethodData * descriptors =
					(jvmtiExtensionRamMethodData *) descriptorBuffer;

			std::stringstream ss;
			int j = 0;
			for (std::vector<std::string>::iterator it = jsMethodIds.begin();
					it != jsMethodIds.end(); ++it) {
				if (descriptors[j].reasonCode == JVMTI_ERROR_NONE) {
					ss << *it << "=" << (char*) descriptors[j].className
							<< "." << (char*) descriptors[j].methodName<< "\n" ;
				}
				j++;
			}

			std::string data = ss.str();
			monitordata *mdata = generateData(0, data.c_str(),
					data.length());
			sendMethodData(mdata);

			delete mdata;
		}

	}

	cleanup: vmFunctions.theVM->DetachCurrentThread();
	ibmras::monitoring::plugins::jni::hc_dealloc((unsigned char**) &ramMethods);
	ibmras::monitoring::plugins::jni::hc_dealloc(
			(unsigned char**) &descriptorBuffer);
}

void MethodLookupProvider::sendMethodDictionary() {
	const char header[] = "#MethodDictionarySource\n";
	monitordata *mdata = generateData(0, header, strlen(header));
	mdata->persistent = true;
	sendMethodData(mdata);
	delete mdata;
}

monitordata* MethodLookupProvider::generateData(uint32 sourceID,
		const char *dataToSend, int size) {
	monitordata* data = new monitordata;
	data->provID = MethodLookupProvider::providerID;
	data->data = dataToSend;
	data->size = size;
	data->sourceID = sourceID;
	data->persistent = false;
	return data;
}

}
}
}
}
} /* end namespace methods */

