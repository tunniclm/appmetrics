#ifndef CPUDATAPROVIDER_H_
#define CPUDATAPROVIDER_H_

#include "ibmras/monitoring/Plugin.h"
#include "omr.h"
#include "omragent.h"
#include "ibmras/vm/omr/healthcenter.h"

namespace plugins {
	namespace omr {
		namespace cpu {

			class CpuDataProvider : ibmras::monitoring::Plugin {
			public:
				static CpuDataProvider* getInstance(omrRunTimeProviderParameters oRTPP);
				static CpuDataProvider* getInstance();

				CpuDataProvider(omrRunTimeProviderParameters oRTPP);
				virtual ~CpuDataProvider() {};  //destructor
				static int cpustart();
				static int cpustop();
				static pullsource* registerPullSource(uint32 provID);


			private:
				static monitordata* generateData(uint32 srcid);
				static char* getCpuData();
				static monitordata* pullCallback();
				static void pullComplete(monitordata* data);

			};

		} /* end namespace cpu */
	} /* end namespace omr */
} /* end namespace plugins */

#endif /* CPURDATAPROVIDER_H_ */
