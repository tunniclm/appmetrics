#ifndef ibmras_monitoring_plugins_environment_agent_version_h
#define ibmras_monitoring_plugins_environment_agent_version_h

#include <string>
#include <sstream>

#define AGENT_VERSION "3.0.0"

static const std::string months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const std::string getAgentVersionAndDate() {
	static const std::string date = __DATE__;
	std::stringstream ss;
	
	ss << AGENT_VERSION << '.' << /* year */ date.substr(7, 4);
	
	std::string month = date.substr(0, 3);
	for (int i = 1; i <= 12; i++) { 
		if (months[i-1] == month) {
			if (i < 10) ss << '0'; 
			ss << i; 
		}
	}
	
	/* day of month */
	ss << (date[4] == ' ' ? '0' : date[4]);
	ss << date[5];
	
	return ss.str();
}

#endif /* ibmras_monitoring_plugins_environment_agent_version_h */