/*
 * MethodLookupProvider.cpp
 *
 *  Created on: 21 July 2014
 *      Author: stalleyj
 */

#include "ibmras/monitoring/Plugin.h"
#include "ibmras/vm/java/healthcenter.h"
#include "ibmras/monitoring/plugins/j9/methods/MethodLookupProvider.h"
#include "ibmras/common/util/memUtils.h"
#include "ibmjvmti.h"
#include "jni.h"
#include "jvmti.h"
#include "ibmras/common/util/strUtils.h"
#include "ibmras/common/logging.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <stdlib.h>

#define JNI_VERSION JNI_VERSION_1_4

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace j9 {
namespace methods {

IBMRAS_DEFINE_LOGGER("MethodLookup")
;

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

				if (parameters.size() > 0) {
					getMethodIDs(parameters);
				} else {
					sendMethodDictionary();
				}
			}
		}
	} else if (id == "headless") {
		getAllMethodIDs();
	}
}

void MethodLookupProvider::getMethodIDs(std::vector<std::string> &jsMethodIds) {
	JNIEnv *env;
	void *ramMethods = NULL;
	void **ramMethodsPtr = NULL;
	void *descriptorBuffer = NULL;

	int numberOfMethods = jsMethodIds.size();

	vmFunctions.theVM->AttachCurrentThread((void **) &env, NULL);

	if (vmFunctions.jvmtiGetMethodAndClassNames != 0 && numberOfMethods > 0) {
		jvmtiError error;

		/* Allocate memory for the method identifiers */
		ramMethods = (char**) hc_alloc(sizeof(void*) * numberOfMethods);
		if (ramMethods == NULL) {
			goto cleanup;
		}

		descriptorBuffer = (unsigned char**) hc_alloc(
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

		while (numberOfMethods > 0) {

			IBMRAS_DEBUG_1(debug, "%d methods to process", numberOfMethods);
			/**
			 * Queries the VM for the method and class names for one or more method identifiers. The results
			 * of calling this method are only valid if isSupported() returns true.
			 *
			 * The methodIds, results, classNameOffsets and methodNameOffsets should all be large enough to
			 * contain numberOfMethods elements. For these arrays, data at any given index will correspond
			 * to the the data in the other arrays at the same index.
			 *
			 * For each method identifier at index i in the first numberOfMethod elements in methodIds, this
			 * function will set results[i] to be:
			 *
			 * 0 if, and only if, the class and method names were found and written into the stringData
			 * array. In this case, classNameOffsets[i] will point to the offset into stringData where the
			 * class name starts and methodNameOffsets[i] will point to the offset into stringData where the
			 * method name starts.
			 *
			 * 23 if, and only if, methodIds[i] is not a valid method identifier.
			 *
			 * 110 if, and only if, methodIds[i] is a valid method identifier but there is not enough
			 * remaining space in stringData to write the method and class names.
			 *
			 * @param methodIds
			 *            An array of longs representing the method identifiers to lookup. This array should
			 *            be large enough to contain numberOfMethods elements.
			 * @param results
			 *            An array of integers that should be large enough to contain numberOfMethods
			 *            elements.
			 * @param classNameOffsets
			 *            An array of integers that should be large enough to contain numberOfMethods
			 *            elements.
			 * @param methodNameOffsets
			 *            An array of integers that should be large enough to contain numberOfMethods
			 *            elements.
			 * @param numberOfMethods
			 *            The number of methods to lookup.
			 * @param stringData
			 *            A byte array to contain the returned method and class names in UTF8.
			 */
			int stringBytesLength = 200000;
			char stringBytes[200000];

			error = vmFunctions.jvmtiGetMethodAndClassNames(vmFunctions.pti,
					ramMethods, numberOfMethods, descriptorBuffer, stringBytes,
					&stringBytesLength);
			if (error == JVMTI_ERROR_NONE) {
				jvmtiExtensionRamMethodData * descriptors =
						(jvmtiExtensionRamMethodData *) descriptorBuffer;

				std::stringstream ss;
				int j = 0;
				i=numberOfMethods;
				numberOfMethods = 0;
				for (int k=0; k<i;k++) {
					if (descriptors[j].reasonCode == JVMTI_ERROR_NONE) {

						std::stringstream ss2;
						ss2 << ramMethodsPtr[j];
						std::string method = ss2.str();
						if (ibmras::common::util::startsWith(method, "0x")) {
							method = method.substr(2);
						}
						ss << method << "=" << (char*) descriptors[j].className
								<< "." << (char*) descriptors[j].methodName
								<< "\n";
					} else if (descriptors[j].reasonCode
							== JVMTI_ERROR_OUT_OF_MEMORY) {
						ramMethodsPtr[numberOfMethods] = ramMethodsPtr[j];
						numberOfMethods++;
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
		IBMRAS_DEBUG_1(debug, "%d methods processed", jsMethodIds.size());

	}

	cleanup:

	vmFunctions.theVM->DetachCurrentThread();
	hc_dealloc((unsigned char**) &ramMethods);
	hc_dealloc((unsigned char**) &descriptorBuffer);
}

void MethodLookupProvider::getAllMethodIDs() {

	if (!vmFunctions.getJ9method) {
		return;
	}

	JNIEnv *env;
	vmFunctions.theVM->AttachCurrentThread((void **) &env, NULL);

	jclass *classes = NULL;
	jint count = 0;
	jvmtiError err = vmFunctions.pti->GetLoadedClasses(&count, &classes);

	std::vector<std::string> methods;
	for (int i = 0; i < count; i++) {
		jint methodcount;

		jmethodID *mids = NULL;
		err = vmFunctions.pti->GetClassMethods(classes[i], &methodcount, &mids);

		if (err == JVMTI_ERROR_NONE) {
			jmethodID *midPtr = mids;


			for (int j = 0; j < methodcount; j++) {
				/* Convert the object pointer to a j9methodPtr as that */
				/* is what health center uses to lookup missing method names */

				void *j9method_ptr;
				err = vmFunctions.getJ9method(vmFunctions.pti, *midPtr,
						&j9method_ptr);
				if (err == JVMTI_ERROR_NONE) {
					std::stringstream ss;
					ss << j9method_ptr;
					std::string method = ss.str();
					if (ibmras::common::util::startsWith(method, "0x")) {
						method = method.substr(2);
					}
					methods.push_back(method);
				} else {
					IBMRAS_DEBUG(fine, "getJ9method failed");
				}
				midPtr++;
			}
			env->DeleteLocalRef(classes[i]);
		}
		hc_dealloc((unsigned char**) &mids);
	}

	hc_dealloc((unsigned char**) &classes);
	vmFunctions.theVM->DetachCurrentThread();
	getMethodIDs(methods);
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

/*******************************
 * MEMORY MANAGEMENT FUNCTIONS *
 *******************************/

unsigned char* MethodLookupProvider::hc_alloc(int size) {
	jvmtiError rc;
	void* buffer = NULL;
	rc = (vmFunctions.pti)->Allocate(size, (unsigned char**) &buffer);
	if (rc != JVMTI_ERROR_NONE) {
		//fprintf(stderr,"OutOfMem : hc_alloc failed to allocate %d bytes.", size);
		return NULL;
	} else {
		//fprintf(stderr,"hc_alloc: allocated %d bytes at %p", size, buffer);
		memset(buffer, 0, size);
		return (unsigned char*) buffer;
	}

}

void MethodLookupProvider::hc_dealloc(unsigned char** buffer) {
	jvmtiError rc;

	if (*buffer == NULL) {
		//fprintf(stderr,"hc_dealloc buffer == NULL");
		return;
	}
	rc = (vmFunctions.pti)->Deallocate(*buffer);
	if (rc != JVMTI_ERROR_NONE) {
		//fprintf(stderr,"hc_dealloc failed to deallocate. rc=%d", rc);
	} else {
		*buffer = NULL;
	}
}

}
}
}
}
} /* end namespace methods */

