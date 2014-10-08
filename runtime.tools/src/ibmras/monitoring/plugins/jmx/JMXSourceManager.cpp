/*
 * SourceManager.cpp
 *
 *  Created on: 31 Jan 2014
 *      Author: adam
 */

#include "ibmras/monitoring/plugins/jmx/JMXSourceManager.h"
#include "ibmras/monitoring/plugins/jmx/JMXUtility.h"
#include "ibmras/common/logging.h"
#include <cstring>
#include <stdlib.h>

namespace ibmras {
namespace monitoring {
namespace plugins {
namespace jmx {


JMXSourceManager::JMXSourceManager() {
	running = false;
	provid = -1;
	pullCount = 2;
	pullsources = NULL;
	vm = NULL;
}

pullsource* JMXSourceManager::registerPullSource(uint32 provID) {
	IBMRAS_DEBUG(fine, "Registering pull sources");
	provid = provID;
	pullsources = new JMXPullSource*[pullCount];
	pullsources[CPU] = ibmras::monitoring::plugins::jmx::os::getOSPullSource();
	pullsources[CPU]->setProvID(provID);
	pullsource* src = pullsources[CPU]->getDescriptor();
	pullsources[RT] = ibmras::monitoring::plugins::jmx::rt::getRTPullSource();
	pullsources[RT]->setProvID(provID);
	src->next = pullsources[RT]->getDescriptor();		/* create chain of pullsources */
	return src;
}

int JMXSourceManager::start() {
	IBMRAS_DEBUG(info, "Starting");
	for (uint32 i = 0; i < pullCount; i++) {
		JMXPullSource* p = pullsources[i];
		if (p) {
			p->publishConfig();
		}
	}
	return 0;
}

int JMXSourceManager::stop() {
	IBMRAS_DEBUG(info, "Stopping");
	for(uint32 i = 0; i < pullCount; i++) {
		JMXPullSource* p = pullsources[i];
		delete p;
	}
	return 0;
}

uint32 JMXSourceManager::getProvID() {
	return provid;
}

}	/* end namespace jmx */
}	/* end namespace plugins */
}	/* end namespace monitoring */
} 	/* end namespace ibmras */


