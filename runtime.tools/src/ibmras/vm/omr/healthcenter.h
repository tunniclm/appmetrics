#ifndef HEALTHCENTER_H
#define HEALTHCENTER_H

#include <omr.h>
#include <omragent.h>

struct omrRunTimeProviderParameters {
	OMR_VM *theVm;
	OMR_TI const *omrti;
};
#endif // HEALTHCENTER_H
