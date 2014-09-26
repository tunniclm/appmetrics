#ifndef OSCPUTIME_H_
#define OSCPUTIME_H_

struct CPUTime {
	unsigned long long time; /* ns since fixed point */
	unsigned long long total; /* cumulative total cpu time in ns */
	unsigned long long process; /* cumulative process cpu time in ns */
	int nprocs;
};

extern "C" struct CPUTime* getCPUTime();

#endif /* OSCPUTIME_H_ */