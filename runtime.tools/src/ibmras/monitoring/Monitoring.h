/*
 * Monitoring.h
 */

#ifndef ibmras_monitoring_monitoring_h
#define ibmras_monitoring_monitoring_h

/*
 * API definitions for data sources to connect to the monitoring
 * agent.
 */
#include "ibmras/export.h"
#include "ibmras/common/types.h"


/* data from a source */
typedef struct monitordata {
	uint32 provID;				/* provider ID, previously allocated during the source registration */
	uint32 sourceID;			/* source ID, previously supplied by the source during registration */
	uint32 size;				/* amount of data being provided */
	const char *data;					/* char array of the data to store */
	bool persistent;            /* persistent data will not be removed from the bucket */
} monitordata;

typedef monitordata* (*PULL_CALLBACK)(void);			/* shortcut definition for the pull source callback */
typedef void (*PULL_CALLBACK_COMPLETE)(monitordata*);	/* callback to indicate when the data source can free / re-use the memory */
typedef char* (*GET_CONFIG)(void);

/* common header for data sources */
typedef struct srcheader {
	uint32 sourceID;			/* ID assigned by the provider - unique by provider */
	uint32 capacity;			/* the amount of space in bytes that should be allocated for this source */
	const char *name;			/* null terminated C string */
	const char *description;	/* null terminated C string */
} srcheader;

typedef struct pushsource {
	srcheader header;			/* common source header */
	pushsource *next;			/* next source or null if this is the last one in the list */
} pushsource;


typedef pushsource* (*PUSH_SOURCE_REGISTER)(void (*callback)(monitordata* data), uint32 provID);
typedef void (*PUSH_CALLBACK)(monitordata* data);

typedef struct pullsource{
	srcheader header;			/* common source header */
	pullsource *next;			/* the next source or null if this is the last one in the list */
	uint32 pullInterval;		/* time in seconds at which data should be pulled from this source */
	PULL_CALLBACK callback;
	PULL_CALLBACK_COMPLETE complete;
} pullsource;

/* definition for connectors */
typedef void* (*CONNECTOR_FACTORY)(const char* properties);	/* short cut for the function pointer to invoke in the connector library */

/* definition for receivers */
typedef void* (*RECEIVER_FACTORY)();	/* short cut for the function pointer to invoke in the receiver library */


#endif /* ibmras_monitoring_monitoring_h */
