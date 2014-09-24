#ifndef MEMORYCOUNTERDATAPROVIDER_H_
#define MEMORYCOUNTERDATAPROVIDER_H_

#include "ibmras/monitoring/Plugin.h"
#include "omr.h"
#include "omragent.h"
#include "ibmras/vm/omr/healthcenter.h"




namespace plugins {
	namespace omr {
		namespace memorycounters {



			class MemoryCountersDataProvider : ibmras::monitoring::Plugin {
			public:
				MemoryCountersDataProvider(omrRunTimeProviderParameters oRTPP);
				virtual ~MemoryCountersDataProvider() {};  //destructor
				static int memstart();
				static int memstop();
				static pullsource* registerPullSource(uint32 provID);


			private:
				static monitordata* generateData(uint32 srcid);
				static void dealloc_report_lines(char *lines[], int count);
				static void hc_dealloc(char* buffer);
				static char* getMemoryCounters();
				static char * join_strings(char *strings[], int count);
				static monitordata* pullCallback();
				static void pullComplete(monitordata* data);

			};

		} /* end namespace memorycounters */
	} /* end namespace omr */
} /* end namespace plugins */

#endif /* MEMORYCOUNTERDATAPROVIDER_H_ */
