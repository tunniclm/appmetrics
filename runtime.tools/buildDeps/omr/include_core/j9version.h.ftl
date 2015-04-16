/*
	(c) Copyright IBM Corp. 1998, ${uma.year}  All Rights Reserved
	C language defines for included modules.

	File generated in stream: ${uma.buildinfo.stream}
*/


#ifndef J9VERSION_H
#define J9VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define EsBuildVersionString "${uma.buildinfo.build_date}_${uma.spec.properties.buildid.value}"
#define J9UniqueBuildID ((uint64_t)J9CONST64(${uma.buildinfo.unique_build_id}))

#ifdef __cplusplus
}
#endif

#endif /* J9VERSION_H */
