#ifndef NATIVEMEMORYDATAPROVIDER_H_
#define NATIVEMEMORYDATAPROVIDER_H_

#include "ibmras/monitoring/Plugin.h"
#include "omr.h"
#include "omragent.h"
#include "ibmras/vm/omr/healthcenter.h"

namespace plugins {
	namespace omr {
		namespace nativememory {

			class NativeMemoryDataProvider : ibmras::monitoring::Plugin {
			public:
				static NativeMemoryDataProvider* getInstance(omrRunTimeProviderParameters oRTPP);
				static NativeMemoryDataProvider* getInstance();

				NativeMemoryDataProvider(omrRunTimeProviderParameters oRTPP);
				virtual ~NativeMemoryDataProvider() {};  //destructor
				static int memstart();
				static int memstop();
				static pullsource* registerPullSource(uint32 provID);


			private:
				static monitordata* generateData(uint32 srcid);
				static char* getNativeMemoryData();
				static monitordata* pullCallback();
				static void pullComplete(monitordata* data);

			};

		} /* end namespace nativememory */
	} /* end namespace omr */
} /* end namespace plugins */

#endif /* NATIVEMEMORYDATAPROVIDER_H_ */
