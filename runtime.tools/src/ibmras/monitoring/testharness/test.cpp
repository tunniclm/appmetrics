/*
 * test.cpp
 *
 *  Created on: 31 Jan 2014
 *      Author: adam
 */

#include <iostream>
#include "ibmras/monitoring/agent/Agent.h"
#include "ibmras/common/logging.h"
#include <string>


void* stdoutlog(const std::string& msg) {
	std::cerr << "TA : ";
	std::cerr << msg;
	return NULL;
}

int main(int argc, char* argv[])
{

	IBMRAS_DEFINE_LOGGER("test program");
	ibmras::monitoring::agent::Agent* agent = ibmras::monitoring::agent::Agent::getInstance();

	printf("Starting the test run : \n");

	agent->setLogOutput(stdoutlog);

	agent->setLocalLog(true);

	int i;
	agent->setAgentProperty("plugin.path", argv[1]);
	agent->init();
	agent->start();
	std::cout << "\n\n** Press any key to stop ** \n";
	std::cin >> i;
	agent->stop();
	agent->shutdown();
    return 0;
}
