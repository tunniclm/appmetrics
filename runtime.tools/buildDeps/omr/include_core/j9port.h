/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#if !defined(J9PORT_H_)
#define J9PORT_H_

/* NOTE:  j9port_generated.h include is at the bottom of this file until its dependencies on this file can be relaxed */

/* fix for linux s390 32bit stdint vs unistd.h definition of intptr_t (see CMVC 73850) */
#if defined(LINUX) && defined(S390)
#include <stdint.h>
#endif

#include <stdarg.h>	/* for va_list */
#include <signal.h>
#include "j9comp.h"
#include "j9cfg.h"
#include "j9thread.h"
#include "j9socket.h"
#include "gp.h"	/* for typedefs of function arguments to gp functions */
#include "omrmemcategories.h"
#include "shchelp.h"


#if defined(J9VM_INTERP_SIG_QUIT_THREAD) && (defined(LINUX) || defined(RS6000) || defined(SOLARIS) || defined(DECUNIX) || defined(OSX) || defined(IRIX))
#include <unistd.h>
#endif

#if defined(J9ZOS390)
#define PORT_ABEND_CODE	0xDED
#define PORT_ABEND_REASON_CODE 20
#define PORT_ABEND_CLEANUP_CODE 1 	/* allow for normal enclave termination/cleanup processing */
#endif

/**
 * @name Port library access
 * @anchor PortAccess
 * Macros for accessing port library.
 * @{
 */
#define PORT_ACCESS_FROM_PORT(portLibrary) J9PortLibrary *privatePortLibrary = (portLibrary)
/** @} */

#define J9_STR_(x) #x
#define J9_STR(x) J9_STR_(x)
#define J9_GET_CALLSITE() __FILE__ ":" J9_STR(__LINE__)

#define PORTLIB privatePortLibrary

/**
 * @name File Operations
 * @anchor PortFileOperations
 * File operation modifiers.
 * @{
 */
#ifdef BREW
#include "AEEFile.h"
#define	EsSeekSet	_SEEK_START
#define	EsSeekCur	_SEEK_CURRENT
#define	EsSeekEnd	_SEEK_END
#else
#ifdef		SEEK_SET
#define	EsSeekSet	SEEK_SET	/* Values for EsFileSeek */
#else
#define	EsSeekSet	0
#endif
#ifdef 		SEEK_CUR
#define	EsSeekCur	SEEK_CUR
#else
#define	EsSeekCur	1
#endif
#ifdef		SEEK_END
#define	EsSeekEnd	SEEK_END
#else
#define	EsSeekEnd	2
#endif
#endif

#define	EsOpenRead			0x1	/* Values for EsFileOpen */
#define	EsOpenWrite			0x2
#define	EsOpenCreate		0x4
#define	EsOpenTruncate		0x8
#define	EsOpenAppend		0x10
#define	EsOpenText			0x20
#define	EsOpenCreateNew 	0x40		/* Use this flag with EsOpenCreate, if this flag is specified then trying to create an existing file will fail */
#define	EsOpenSync			0x80
#define EsOpenForMapping	0x100 /* Required for WinCE for file memory mapping, ignored on other platforms. WINCE is not supported so this flag is obsolete now. */
#define	EsOpenForInherit 	0x200	/* Use this flag such that returned handle can be inherited by child processes */
#define	EsOpenCreateAlways 	0x400	/* Always creates a new file, an existing file will be overwritten */
#define EsOpenCreateNoTag 	0x800	/* Used for zOS only, to disable USS file tagging on JVM-generated files */
#define EsOpenShareDelete 	0x1000  /* used only for windows to allow a file to be renamed while it is still open */
#define EsOpenAsynchronous 	0x2000  /* used only for windows to allow a file to be opened asynchronously */

#define EsIsDir 	0	/* Return values for EsFileAttr */
#define EsIsFile 	1
/** EsMaxPath was chosen from unix MAXPATHLEN.  Override in platform
  * specific j9file implementations if needed.
  */
#define EsMaxPath 	1024
/** @} */

/**
 * @name Shared Semaphore Success flags
 * @anchor PortSharedSemaphoreSuccessFlags
 * Success codes related to shared semaphore  operations.
 * @{
 * @internal J9PORT_INFO_SHSEM* range from at 100 to 109 to avoid overlap 
 */
#define J9PORT_INFO_SHSEM_BASE 100
#define J9PORT_INFO_SHSEM_CREATED (J9PORT_INFO_SHSEM_BASE)
#define J9PORT_INFO_SHSEM_OPENED (J9PORT_INFO_SHSEM_BASE+1)
#define J9PORT_INFO_SHSEM_OPEN_UNLINKED (J9PORT_INFO_SHSEM_BASE+2)
#define J9PORT_INFO_SHSEM_OPENED_STALE (J9PORT_INFO_SHSEM_BASE+3)
#define J9PORT_INFO_SHSEM_PARTIAL (J9PORT_INFO_SHSEM_BASE+4)
#define J9PORT_INFO_SHSEM_STAT_PASSED (J9PORT_INFO_SHSEM_BASE+5)

/** @} */

/**
 * @name Shared Memory Success flags
 * @anchor PortSharedMemorySuccessFlags
 * Success codes related to shared memory semaphore operations.
 * @{
 * @internal J9PORT_INFO_SHMEM* range from at 110 to 119 to avoid overlap
 */
#define J9PORT_INFO_SHMEM_BASE 110
#define J9PORT_INFO_SHMEM_CREATED (J9PORT_INFO_SHMEM_BASE)
#define J9PORT_INFO_SHMEM_OPENED (J9PORT_INFO_SHMEM_BASE+1)
#define J9PORT_INFO_SHMEM_OPEN_UNLINKED (J9PORT_INFO_SHMEM_BASE+2)
#define J9PORT_INFO_SHMEM_OPENED_STALE (J9PORT_INFO_SHMEM_BASE+3)
#define J9PORT_INFO_SHMEM_PARTIAL (J9PORT_INFO_SHMEM_BASE+4)
#define J9PORT_INFO_SHMEM_STAT_PASSED (J9PORT_INFO_SHMEM_BASE+5)

/** @} */

/**
 * @name Sysinfo get limit success flags
 * @anchor PortSharedMemorySuccessFlags
 * Return codes related to sysinfo get limit operations.
 * @{
 * @internal J9PORT_LIMIT* range from at 120 to 129 to avoid overlap
 */
#define J9PORT_LIMIT_BASE 120
#define J9PORT_LIMIT_UNLIMITED (J9PORT_LIMIT_BASE)
#define J9PORT_LIMIT_UNKNOWN (J9PORT_LIMIT_BASE+1)
#define J9PORT_LIMIT_LIMITED (J9PORT_LIMIT_BASE+2)
/** @} */

/**
 * @name Shared Memory Eyecatcher
 * @anchor PortSharedMemoryEyecatcher
 * Eyecatcher written to start of a shared classes cache to identify the shared memory segment as such a cache
 * @{
 */
#define J9PORT_SHMEM_EYECATCHER "J9SC"
#define J9PORT_SHMEM_EYECATCHER_LENGTH 4
/** @} */

/**
 * @name Sysinfo Limits 
 * Flags used to indicate type of operation for j9sysinfo_get_limit
 * @{
 */
#define J9PORT_LIMIT_SOFT ((uintptr_t) 0x0)
#define J9PORT_LIMIT_HARD ((uintptr_t) 0x80000000)
#define J9PORT_RESOURCE_SHARED_MEMORY ((uintptr_t) 1)
#define J9PORT_RESOURCE_ADDRESS_SPACE ((uintptr_t) 2)
#define J9PORT_RESOURCE_CORE_FILE ((uintptr_t) 3)
#define J9PORT_RESOURCE_CORE_FLAGS ((uintptr_t) 4)
/** @} */

/**
 * @name Sysinfo Limits - return values 
 * These values are returned by j9sysinfo_get_limit in the limit parameter for the corresponding return codes.
 * If a value has been determined for a limit, it is the value reurned in the limit parameter.
 * @{
 */
#define J9PORT_LIMIT_UNLIMITED_VALUE (J9CONST64(0xffffffffffffffff))
#define J9PORT_LIMIT_UNKNOWN_VALUE (J9CONST64(0xffffffffffffffff))
/** @} */

/**
 * @name Control file unlink status
 * Flags used to indicate unlink status of control files used by semaphore set or shared memory
 * These flags are used to store value in J9ControlFileStatus.status
 * @{
 */
#define J9PORT_INFO_CONTROL_FILE_NOT_UNLINKED			0
#define J9PORT_INFO_CONTROL_FILE_UNLINK_FAILED			1
#define J9PORT_INFO_CONTROL_FILE_UNLINKED				2
/** @} */

/**
 * @name JSIG support (optional)
 * JSIG
 * @{ 
 */
#ifdef J9PORT_JSIG_SUPPORT
#define J9JSIG_SIGNAL(signum, handler)	jsig_primary_signal(signum, handler)
#define J9JSIG_SIGACTION(signum, act, oldact)	jsig_primary_sigaction(signum, act, oldact)
#else
#define J9JSIG_SIGNAL(signum, handler)	signal(signum, handler)
#define J9JSIG_SIGACTION(signum, act, oldact)	sigaction(signum, act, oldact)
#endif /* J9PORT_JSIG_SUPPORT */
/** @} */

/**
 * @name OS Exception Handling
 * OS Exceptions
 * @{
 */
#define MAX_SIZE_TOTAL_GPINFO 2048
#define J9GP_VALUE_UNDEFINED 0
#define J9GP_VALUE_32 1 
#define J9GP_VALUE_64 2
#define J9GP_VALUE_STRING 3
#define J9GP_VALUE_ADDRESS 4
#define J9GP_VALUE_FLOAT_64 5
#define J9GP_VALUE_16 6

#define J9GP_SIGNAL 0 	/* information about the signal */
#define J9GP_GPR 1 /* general purpose registers */
#define J9GP_OTHER 2  /* other information */
#define J9GP_CONTROL 3 	/* control registers */
#define J9GP_FPR 4 		/* floating point registers */
#define J9GP_MODULE 5 	/* module information */	
#define J9GP_NUM_CATEGORIES 6

#define J9GP_CONTROL_PC (-1)
#define J9GP_MODULE_NAME (-1)
/** @} */

/**
 * @name Native Language Support 
 * Native Language Support
 * @{
 * @internal standards require that all VM messages be prefixed with JVM. 
 */
#define J9NLS_COMMON_PREFIX "JVM"
#define J9NLS_ERROR_PREFIX ""
#define J9NLS_WARNING_PREFIX ""
#define J9NLS_INFO_PREFIX ""
#define J9NLS_ERROR_SUFFIX "E"
#define J9NLS_WARNING_SUFFIX "W"
#define J9NLS_INFO_SUFFIX "I"

/** @internal these macros construct in string literals from message ids. */
#define J9NLS_MESSAGE(id, message) ("" J9NLS_COMMON_PREFIX "" id##__PREFIX " " message)
#define J9NLS_ERROR_MESSAGE(id, message) ("" J9NLS_ERROR_PREFIX "" J9NLS_COMMON_PREFIX "" id##__PREFIX "" J9NLS_ERROR_SUFFIX " " message)
#define J9NLS_INFO_MESSAGE(id, message) ("" J9NLS_INFO_PREFIX "" J9NLS_COMMON_PREFIX "" id##__PREFIX "" J9NLS_INFO_SUFFIX " " message)
#define J9NLS_WARNING_MESSAGE(id, message) ("" J9NLS_WARNING_PREFIX "" J9NLS_COMMON_PREFIX "" id##__PREFIX "" J9NLS_WARNING_SUFFIX " " message)
/** @} */

/** 
 * @name Virtual Memory Access
 * Flags used to describe type of the page for the virtual memory
 * @{
 */
#define J9PORT_VMEM_PAGE_FLAG_NOT_USED 		0x1
#define J9PORT_VMEM_PAGE_FLAG_FIXED 		0x2
#define J9PORT_VMEM_PAGE_FLAG_PAGEABLE 		0x4

#define J9PORT_VMEM_PAGE_FLAG_TYPE_MASK 	0x7
/** @} */

/**
 * @name Virtual Memory Access
 * Flags used to create bitmap indicating memory access
 * @{
 */
#define J9PORT_VMEM_MEMORY_MODE_READ 0x00000001
#define J9PORT_VMEM_MEMORY_MODE_WRITE 0x00000002
#define J9PORT_VMEM_MEMORY_MODE_EXECUTE 0x00000004
#define J9PORT_VMEM_MEMORY_MODE_COMMIT 0x00000008
#define J9PORT_VMEM_MEMORY_MODE_VIRTUAL 0x00000010
#define J9PORT_VMEM_ALLOCATE_TOP_DOWN 0x00000020
#define J9PORT_VMEM_ALLOCATE_PERSIST 0x00000040
/** @} */

/**
 * @name Timer Resolution
 * @anchor timerResolution
 * Define resolution requested in @ref j9time::j9time_hires_delta
 * @{
 */
#define J9PORT_TIME_DELTA_IN_SECONDS ((uint64_t) 1)
#define J9PORT_TIME_DELTA_IN_MILLISECONDS ((uint64_t) 1000)
#define J9PORT_TIME_DELTA_IN_MICROSECONDS ((uint64_t) 1000000)
#define J9PORT_TIME_DELTA_IN_NANOSECONDS ((uint64_t) 1000000000)
/** @} */

/**
 * @name Time Unit Conversion
 * Constants used to convert between units of time.
 * @{
 */
#define J9PORT_TIME_NS_PER_MS ((uint64_t) 1000000)	/* nanoseconds per millisecond */
#define J9PORT_TIME_US_PER_SEC ((uint64_t) 1000000) /* microseconds per second */
/** @} */

/**
 * @name Shared Semaphore
 * Flags used to indicate type of operation for j9shsem_post/j9shsem_wait
 * @{
 */
#define J9PORT_SHSEM_MODE_DEFAULT ((uintptr_t) 0)
#define J9PORT_SHSEM_MODE_UNDO ((uintptr_t) 1)
#define J9PORT_SHSEM_MODE_NOWAIT ((uintptr_t) 2)
/** @} */

/* The following defines are used by j9shmem and j9shsem */
#define J9SH_MAXPATH EsMaxPath

#define J9SH_MEMORY_ID "_memory_"
#define J9SH_SEMAPHORE_ID "_semaphore_"

#define J9SH_DIRPERM_ABSENT ((uintptr_t)-2)
#define J9SH_DIRPERM (0777)
#define J9SH_PARENTDIRPERM (01777)
#define J9SH_DIRPERM_DEFAULT (0000)
#define J9SH_DIRPERM_DEFAULT_WITH_STICKYBIT (01000)
#define J9SH_BASEFILEPERM (0644)
#define J9SH_BASEFILEPERM_GROUP_RW_ACCESS (0664)

#define J9SH_SHMEM_PERM_READ (0444)
#define J9SH_SHMEM_PERM_READ_WRITE (0644)

#define J9SH_SYSV_REGULAR_CONTROL_FILE 0
#define J9SH_SYSV_OLDER_CONTROL_FILE 1
#define J9SH_SYSV_OLDER_EMPTY_CONTROL_FILE 2

/* 
 * Flags passed to "flag" argument of j9shmem_open(). Should be of type uintptr_t.
 * High order 4 bits are reserved for passing the storage key testing value to j9shmem.
 */
#define J9SHMEM_NO_FLAGS					0x0
#define J9SHMEM_OPEN_FOR_STATS				0x1
#define J9SHMEM_OPEN_FOR_DESTROY			0x2
#define J9SHMEM_PRINT_STORAGE_KEY_WARNING	0x4
#define J9SHMEM_STORAGE_KEY_TESTING			0x8
#define J9SHMEM_OPEN_FOR_SNAPSHOT			0x10

#define J9SHMEM_STORAGE_KEY_TESTING_SHIFT	((sizeof(uintptr_t)*8)-4)
#define J9SHMEM_STORAGE_KEY_TESTING_MASK	0xF



/* Flags passed to "flag" argument of j9shsem_deprecated_open(). */
#define J9SHSEM_NO_FLAGS			0x0
#define J9SHSEM_OPEN_FOR_STATS		0x1
#define J9SHSEM_OPEN_FOR_DESTROY	0x2
#define J9SHSEM_OPEN_FOR_SNAPSHOT	0x4


/* Maximum id we should try when we do ftok */
#define J9SH_MAX_PROJ_ID 20 

#ifdef WIN32
#define J9SH_BASEDIR "javasharedresources\\"
#else
#define J9SH_BASEDIR "javasharedresources/"
#endif

typedef struct J9Permission
{
	uint32_t isUserWriteable : 1;
	uint32_t isUserReadable : 1;
	uint32_t isGroupWriteable : 1;
	uint32_t isGroupReadable : 1;
	uint32_t isOtherWriteable : 1;
	uint32_t isOtherReadable : 1;
	uint32_t : 26; /* future use */
} J9Permission;

/**
 * Holds properties relating to a file. Can be added to in the future
 * if needed.
 * Note that the ownerUid and ownerGid fields are 0 on Windows.
 */
typedef struct J9FileStat
{		
	uint32_t isFile : 1;
	uint32_t isDir : 1;
	uint32_t isFixed : 1;
	uint32_t isRemote : 1;
	uint32_t isRemovable : 1;
	uint32_t : 27; /* future use */
	J9Permission perm;
	uintptr_t ownerUid;
	uintptr_t ownerGid;
} J9FileStat;

/**
 * Holds properties relating to a file system.
 */
typedef struct J9FileStatFilesystem
{
	uint64_t freeSizeBytes;
	uint64_t totalSizeBytes;
} J9FileStatFilesystem;

/**
 * Stores information about status of control file used by j9shmem_open() or j9shsem_deprecated_open().
 */
typedef struct J9ControlFileStatus {
	uintptr_t status;
	int32_t errorCode;
	char *errorMsg;
} J9ControlFileStatus;

/* It is the responsibility of the user to create the storage for J9PortVMemParams. 
 * The structure is only needed for the lifetime of the call to j9vmem_reserve_memory_ex 
 * This structure must be initialized using @ref j9vmem_vmem_params_init
 */
typedef struct J9PortVmemParams {
	/* Upon success we will attempt to return a pointer within [startAddress, endAddress] 
	 * in a best effort manner unless the J9PORT_VMEM_STRICT_ADDRESS flag is specified
	 * endAddress is the last address at which the user wishes the returned pointer to 
	 * be assigned regardless of the byteAmount
	 * startAddress must be <= endAddress or j9vmem_reserve_memory_ex will fail
	 * and cause the generation of a trace assertion
	 */
	void *startAddress;
	void *endAddress;

	/* This value must be aligned to pageSize when passing this structure to j9vmem_reserve_memory_ex() */
	uintptr_t byteAmount;

	/* Size of the page requested, a value returned by @ref j9vmem_supported_page_sizes */
	uintptr_t pageSize;

	/* Flags describing type of the page requested */
	uintptr_t pageFlags;

	/* @mode Bitmap indicating how memory is to be reserved.  Expected values combination of:
	 * \arg J9PORT_VMEM_MEMORY_MODE_READ memory is readable
	 * \arg J9PORT_VMEM_MEMORY_MODE_WRITE memory is writable
	 * \arg J9PORT_VMEM_MEMORY_MODE_EXECUTE memory is executable
	 * \arg J9PORT_VMEM_MEMORY_MODE_COMMIT commits memory as part of the reserve
	 * \arg J9PORT_VMEM_MEMORY_MODE_VIRTUAL used only on z/OS
	 *			- used to allocate memory in 4K pages using system macros instead of malloc() or __malloc31() routines
	 *			- on 64-bit, this mode rounds up byteAmount to be aligned to 1M boundary.*
	 */
	uintptr_t mode;

	/* [Optional] Bitmap indicating direction to follow when trying to allocate memory in a range.
	 * \arg J9PORT_VMEM_ALLOC_DIR_BOTTOM_UP start at lower address and proceed towards higher one
	 * \arg J9PORT_VMEM_ALLOC_DIR_TOP_DOWN start at higher address and proceed towards lower one 
	 * \arg J9PORT_VMEM_STRICT_ADDRESS fail if requested address is unavailable 
	 * \arg J9PORT_VMEM_STRICT_PAGE_SIZE fail if requested page size is unavailable 
	 * \arg J9PORT_VMEM_ZOS_USE2TO32G_AREA 
	 * 			- applies to z/OS only, ignored on all other platforms
	 * 			- use allocator that exclusively requests memory in 2to32G region if set
	 * 			- do not use allocator that requests memory exclusively in 2to32G region if not set
	 * 			- if this flag is set and the 2to32G support is not there j9vmem_reserve_memory_ex will return failure
	 * \arg J9PORT_VMEM_ALLOC_QUICK
	 *  		- enabled for Linux only,
	 *  		- If not set, search memory in linear scan method
	 *  		- If set, scan memory in a quick way, using memory information in file /proc/self/maps. (still use linear search if failed)
	 */
	uintptr_t options;
	
	/* Memory allocation category for storage */
	uint32_t category;

	/* the lowest common multiple of alignmentInBytes and pageSize should be used to determine the base address */
	uintptr_t alignmentInBytes;
} J9PortVmemParams;

/**
 * @name J9PortShSemParameters
 * The caller is responsible creating storage for J9PortShSemParameters. 
 * The structure is only needed for the lifetime of the call to @ref j9shsem_open
 * This structure must be initialized using @ref j9shsem_params_init
 */
typedef struct  J9PortShSemParameters {
 	const char *semName; /* Unique identifier of the semaphore. */
 	uint32_t setSize; /* number of semaphores to be created in this set */
 	uint32_t permission; /* Posix-style file permissions */
 	const char* controlFileDir; /* Directory in which to create control files (SysV semaphores only) */
 	uint8_t proj_id; /* parameter used with semName to generate semaphore key */
 	uint32_t deleteBasefile : 1; /* delete the base file (used to generate the semaphore key) when destroying the semaphore */
 } J9PortShSemParameters;
/**
 * @name Process Handle
 * J9ProcessHandle is a pointer to the opaque structure J9ProcessHandleStruct.
 * 
 * J9ProcessHandle represents a J9Port Library process.
 */
typedef struct J9ProcessHandleStruct *J9ProcessHandle;


/**
 * @name Process Streams
 * Flags used to define the streams of a process.
 */
#define J9PORT_PROCESS_STDIN ((uintptr_t) 0x00000001)
#define J9PORT_PROCESS_STDOUT ((uintptr_t) 0x00000002)
#define J9PORT_PROCESS_STDERR ((uintptr_t) 0x00000004)

#define J9PORT_PROCESS_IGNORE_OUTPUT 				1
#define J9PORT_PROCESS_NONBLOCKING_IO 				2
#define J9PORT_PROCESS_PIPE_TO_PARENT 				4
#define J9PORT_PROCESS_INHERIT_STDIN 				8
#define J9PORT_PROCESS_INHERIT_STDOUT				16
#define J9PORT_PROCESS_INHERIT_STDERR 				32
#define J9PORT_PROCESS_REDIRECT_STDERR_TO_STDOUT	64
#define J9PORT_PROCESS_CREATE_NEW_PROCESS_GROUP		128

#define J9PORT_PROCESS_DO_NOT_CLOSE_STREAMS			1

#define J9PORT_INVALID_FD	-1

/**
 * @name Virtual Memory Options
 * Flags used to create bitmap indicating vmem options
 * See J9PortVmemParams.options
 * 
 */
#define J9PORT_VMEM_ALLOC_DIR_BOTTOM_UP	1
#define J9PORT_VMEM_ALLOC_DIR_TOP_DOWN	2
#define J9PORT_VMEM_STRICT_ADDRESS		4
#define J9PORT_VMEM_STRICT_PAGE_SIZE	8
#define J9PORT_VMEM_ZOS_USE2TO32G_AREA 16
#define J9PORT_VMEM_ALLOC_QUICK 		32

/**
 * @name Virtual Memory Address
 * highest memory address on platform
 * 
 */
#if defined(J9ZOS390) && !defined(J9VM_ENV_DATA64)
/* z/OS 31-bit uses signed pointer comparison so this UDATA_MAX maximum address becomes -1 which is less than the minimum address of 0 so use IDATA_MAX instead */
#define J9PORT_VMEM_MAX_ADDRESS ((void *) IDATA_MAX)
#else /* defined(J9ZOS390) && !defined(J9VM_ENV_DATA64) */
#define J9PORT_VMEM_MAX_ADDRESS ((void *) UDATA_MAX)
#endif /* defined(J9ZOS390) && !defined(J9VM_ENV_DATA64) */

/**
 * @name Memory Tagging
 * Eye catcher and header/footer used for tagging j9mem allocation blocks
 *
 */
#if !defined(J9VM_SIZE_SMALL_RAM)
#define J9MEMTAG_TAG_CORRUPTION						0xFFFFFFFF

#define J9MEMTAG_VERSION							0
#define J9MEMTAG_EYECATCHER_ALLOC_HEADER			0xB1234567
#define J9MEMTAG_EYECATCHER_ALLOC_FOOTER			0xB7654321
#define J9MEMTAG_EYECATCHER_FREED_HEADER			0xBADBAD67
#define J9MEMTAG_EYECATCHER_FREED_FOOTER			0xBADBAD21
#define J9MEMTAG_PADDING_BYTE						0xDD

struct J9MemCategory;

typedef struct J9MemTag {
	uint32_t eyeCatcher;
	uint32_t sumCheck;
	uintptr_t allocSize;
	const char* callSite;
	struct J9MemCategory* category;
#if !defined(J9VM_ENV_DATA64)
	/* j9mem_allocate_memory should return addresses aligned to 8 bytes for
	 * performance reasons. On 32 bit platforms we have to pad to achieve this.
	 */
	uint8_t padding[4];
#endif
} J9MemTag;
#endif /* ifndef(J9VM_SIZE_SMALL_RAM) */

#ifdef LINUX
/**
 * @name Linux OS Dump Eyecatcher
 *
 */

#define J9OSDUMP_EYECATCHER	0x19810924
#if defined(J9VM_ENV_DATA64)
#define J9OSDUMP_SIZE	(192 * 1024)
#else
#define J9OSDUMP_SIZE	(128 * 1024)
#endif
#endif

/* j9file_chown takes unsigned arguments for user/group IDs, but uses -1 to indicate that group/user id are not to be changed */
#define J9PORT_FILE_IGNORE_ID UDATA_MAX

/* Used by j9sysinfo_limit_iterator_init/next functions */
typedef struct J9SysinfoUserLimitElement {
	
	/* Null terminated string  */
	const char *name; 
	
	uint64_t softValue;
	uint64_t hardValue;
		/** 1. If the OS indicates a limit is set to unlimited, the corresponding soft/hard value will be set to 
		 *  	J9PORT_LIMIT_UNLIMITED 
		 *  2. There may not be the notion of a hard value on some platforms (for example Windows), in which case
		 * 		the hard value will be set to J9PORT_LIMIT_UNLIMITED.
		 */ 

} J9SysinfoUserLimitElement;

typedef struct J9SysinfoLimitIteratorState {
	uint32_t count;
	uint32_t numElements;
} J9SysinfoLimitIteratorState;

/* Used by j9sysinfo_env_iterator_init/next functions */
	
typedef struct J9SysinfoEnvElement {
	/* Null terminated string in format "name=value" */
	const char *nameAndValue; 
} J9SysinfoEnvElement;

typedef struct J9SysinfoEnvIteratorState {
	void *current;		/* to be used exclusively by the port library */
	void *buffer; 		/* caller-allocated buffer. This can be freed by the caller once they have finished using the iterator. */
	uintptr_t bufferSizeBytes;  /* size of @ref buffer */
} J9SysinfoEnvIteratorState;

/* Used by j9sysinfo_get_CPU_utilization() */
typedef struct J9SysinfoCPUTime {
	int64_t timestamp; /* time in nanoseconds from a fixed but arbitrary point in time */
	int64_t cpuTime; /* cumulative CPU utilization (sum of system and user time in nanoseconds) of all CPUs on the system. */
	int32_t numberOfCpus; /* number of CPUs as reported by the operating system */
} J9SysinfoCPUTime;

/* Key memory categories are copied here for DDR access */
/* Special memory category for memory allocated for unknown categories */
#define J9MEM_CATEGORY_UNKNOWN 0x80000000
/* Special memory category for memory allocated for the port library itself */
#define J9MEM_CATEGORY_PORT_LIBRARY 0x80000001
/* Special memory category for *unused* sections of regions allocated for <32bit allocations on 64 bit.
 * The used sections will be accounted for under the categories they are used by. */
#define J9MEM_CATEGORY_PORT_LIBRARY_UNUSED_ALLOCATE32_REGIONS 0x80000009

typedef struct J9MemCategory {
	const char * const name;
	const uint32_t categoryCode;
	uintptr_t liveBytes;
	uintptr_t liveBytesLockWord;
	uintptr_t liveAllocations;
	uintptr_t liveAllocationsLockWord;
	const uint32_t numberOfChildren;
	const uint32_t * const children;
} J9MemCategory;

typedef struct J9MemCategorySet {
	uint32_t numberOfCategories;
	J9MemCategory **categories;
} J9MemCategorySet;

#define J9MEM_CATEGORIES_KEEP_ITERATING 0
#define J9MEM_CATEGORIES_STOP_ITERATING 1

/**
* State data for memory category walk
* @see j9mem_walk_categories(
*/
typedef struct J9MemCategoryWalkState {
	/**
	* Callback function called from j9memory_walk_categories with memory category data.
	*
	* @param [in] categoryCode               Code identifying memory category
	* @param [in] categoryName               Name of category
	* @param [in] liveBytes                  Bytes outstanding (allocated but not freed) for this category
	* @param [in] liveAllocations            Number of allocations outstanding (not freed) for this category
	* @param [in] isRoot                     True if this node is a root (i.e. does not have a parent)
	* @param [in] parentCategoryCode         Code identifying the parent of this category. Ignore if isRoot==TRUE
	* @param [in] state                      Walk state record.
	* @return J9MEM_CATEGORIES_KEEP_INTERATING or J9MEM_CATEGORIES_STOP_ITERATING
	*/
	uintptr_t (*walkFunction) (uint32_t categoryCode, const char * categoryName, uintptr_t liveBytes, uintptr_t liveAllocations, BOOLEAN isRoot, uint32_t parentCategoryCode, struct J9MemCategoryWalkState * state);
	
	
	void* userData1;
	void* userData2;
} J9MemCategoryWalkState;

typedef enum J9MemoryState {J9NUMA_PREFERRED, J9NUMA_ALLOWED, J9NUMA_DENIED} J9MemoryState;

typedef struct J9MemoryNodeDetail
{
	uintptr_t j9NodeNumber;  /**< The 1-indexed number used outside of Port and Thread to describe this node */
	J9MemoryState memoryPolicy;  /**< Whether the memory on this node is preferred, allowed, or denied use by this process under the current NUMA policy */
	uintptr_t computationalResourcesAvailable;  /**< The number of computational resources on this node which are currently available for use under the current NUMA policy */
} J9MemoryNodeDetail;

/* Stores memory usage statistics snapshot sampled at a time 'timestamp'.
 *
 * @see j9sysinfo_get_memory_info
 *
 * If one of these parameters is not available on a particular platform, this is set to the
 * default J9PORT_MEMINFO_NOT_AVAILABLE.
 */
typedef struct J9MemoryInfo {
	uint64_t totalPhysical;		/* Total physical memory in the system (in bytes). */
	uint64_t availPhysical;		/* Available physical memory in the system (in bytes). */
	uint64_t totalVirtual;		/* Total virtual memory addressable by the process (in bytes). */
	uint64_t availVirtual;		/* Virtual memory available to the process (in bytes). */
	uint64_t totalSwap;			/* Total swap memory (in bytes). */
	uint64_t availSwap;			/* Total swap memory free (in bytes). */
	uint64_t cached;			/* The physical RAM used as cache memory (in bytes). */
	uint64_t buffered;			/* The physical RAM used for file buffers (in bytes). */
	int64_t timestamp;			/* Sampling timestamp (in microseconds). */
} J9MemoryInfo;

#define J9PORT_MEMINFO_NOT_AVAILABLE ((uint64_t) -1)

/**
 * Stores usage information on a per-processor basis. These parameters are the ones that generic
 * whereas, operating system specific parameters are not saved here. If one of these parameters is
 * not available on a particular platform, this is set to J9PORT_PROCINFO_NOT_AVAILABLE.
 */
typedef struct J9ProcessorInfo {
	uint64_t userTime;			/* Time spent in user mode (in microseconds). */
	uint64_t systemTime;		/* Time spent in system mode (in microseconds). */
	uint64_t idleTime;			/* Time spent sitting idle (in microseconds). */
	uint64_t waitTime;			/* Time spent over IO wait (in microseconds). */
	uint64_t busyTime;			/* Time spent over useful work (in microseconds). */
	int32_t proc_id;			/* This processor's id. */
	/* Was current CPU online when last sampled (J9PORT_PROCINFO_PROC_ONLINE if online,
	 * J9PORT_PROCINFO_PROC_OFFLINE if not).
	 */
	int32_t online;
} J9ProcessorInfo;

/**
 * Structure collects the usage statistics for each of the processors that is online at the
 * time of sampling.
 *
 * @see j9sysinfo_get_processor_info, j9sysinfo_destroy_processor_info
 *
 * The array holds an entry for each logical processor on the system plus a one for the aggregates.
 * However, the particular record shall hold valid usage samples only if the processor were online,
 * else the individual fields shall be set to J9PORT_PROCINFO_NOT_AVAILABLE.
 */
typedef struct J9ProcessorInfos {
	int32_t totalProcessorCount;			/* Number of logical processors on the machine. */
	J9ProcessorInfo *procInfoArray;		/* Array of processors, of 'totalProcessorCount + 1'. */
	int64_t timestamp;						/* Sampling timestamp (in microseconds). */
} J9ProcessorInfos;

#define J9PORT_PROCINFO_NOT_AVAILABLE ((uint64_t) -1)

/* Processor status. */
#define J9PORT_PROCINFO_PROC_OFFLINE ((int32_t)0)
#define J9PORT_PROCINFO_PROC_ONLINE ((int32_t)1)

#define NANOSECS_PER_USEC 1000

#define J9PORT_ENABLE_ENSURE_CAP32 0
#define J9PORT_DISABLE_ENSURE_CAP32 1

#define FLG_ACQUIRE_LOCK_WAIT		0x00000001
#define FLG_ACQUIRE_LOCK_NOWAIT		0x00000002

#if defined(LINUX) 
typedef pthread_spinlock_t spinlock_t;
#else
typedef int32_t spinlock_t;
#endif /* LINUX */

/* Lock Status values - the current status of a lock.  */
#define LS_UNINITIALIZED                   0
#define LS_INITIALIZED                     1
#define LS_INITIALIZING                    2
#define LS_LOCKED                          3

/* The Lock data structure - this must be placed shared memory.
 * Structure size: 64 bytes
 */
typedef struct J9VirtReadWriteLock {
	uint64_t unused1[2];
	spinlock_t spinlock;
	volatile uint32_t lock_users; /* Reader-count */
	volatile uint32_t lock_flags;
	volatile uint32_t lock_status;
	uint64_t lock_word; /* Argument #4 to compareSwap*(). */
	volatile int32_t locking_guest_id;
	volatile int32_t locking_jvm_pid;
	uint32_t unused2;
	uint32_t unused3;
	uint64_t unused4;
} J9VirtReadWriteLock;

/* The hypervisor kind - our guest OS could be running on one of these hypervisors. */
enum hypervisor_kind {
	HK_UNDEFINED = -1,
	HK_KVM = 0,
	HK_VMWARE,
	HK_ZVM,
	HK_POWERVM
};

/* Operating Systems supported - the guest as well as host OS kind. 'OSK_UNDEFINED'
 * implies that the host is running on bare metal without an underlying OS.
 */
enum operating_system_kind {
	OSK_UNDEFINED = -1,
	OSK_WINDOWS_X86 = 0,
	OSK_WINDOWS_X86_64,
	OSK_LINUX_X86,
	OSK_LINUX_X86_64,
	OSK_AIX_PPC,
	OSK_AIX_PPC_64,
	OSK_LINUX_PPC,
	OSK_LINUX_PPC_64,
	OSK_LINUX_S390_31,
	OSK_LINUX_S390_64,
	OSK_ZOS
};

/* Reason codes shared between Xvirt and j9hypervisor. */
#define J9HYPERVISOR_RC_UNKNOWN                        0
#define J9HYPERVISOR_RC_VIRT_FLAG_SEEN                 1
#define J9HYPERVISOR_RC_INIT_VIRT_COOKIES              2

#define J9HYPERVISOR_MAX_SHARED_OBJECT_PATH_LEN        256

/* Encodings for the platform - these form the portion of the cache name that
 * follow the 'T' in the cross-guest enabled JVM.
 */
#define J9HYPERVISOR_TARGET_KIND_WIN_I386_32           "wi"
#define J9HYPERVISOR_TARGET_KIND_WIN_AMD_64            "wa"
#define J9HYPERVISOR_TARGET_KIND_LINUX_I386_32         "xi"
#define J9HYPERVISOR_TARGET_KIND_LINUX_AMD_64          "xa"
#define J9HYPERVISOR_TARGET_KIND_LINUX_PPC             "xp"
#define J9HYPERVISOR_TARGET_KIND_LINUX_ZOS             "xz"
#define J9HYPERVISOR_TARGET_KIND_AIX_PPC               "ap"
#define J9HYPERVISOR_TARGET_KIND_ZOS                   "mz"

/* The platform encoding strings are just two bytes wide. */
#define J9HYPERVISOR_SIZEOF_PLATFORM_ENCODING          2

#define J9SH_OSCACHE_VIRT_LOCK_COUNT				   5

/* Macros related to KVM virtualization. */
#define J9HYPERVISOR_KVM_GUEST_DEVICE_NAME_PREFIX       "uio"
#define J9HYPERVISOR_KVM_GUEST_DEVICE_NAME_PREFIX_LEN   (sizeof(J9HYPERVISOR_KVM_GUEST_DEVICE_NAME_PREFIX) - 1)
#define J9HYPERVISOR_KVM_GUEST_DEVICE_PATH_PREFIX       "/dev/%s"

/* The magic signature on crossguest shared memory - J9CGMEM. */
#define J9HYPERVISOR_SHARED_OBJECT_SIGNATURE_LENGTH     8
#define J9HYPERVISOR_SHARED_OBJECT_SIGNATURE            { 0x4A, 0x39, 0x43, 0x47, 0x4D, 0x45, 0x4D, 0x00 }

/* The shared object header - in order to have these locks in place, the cache may not
 * itself be created. This means that the shared object representing the cache file has
 * to have a minimum size: sizeof(struct J9HypervisorSharedObjectHeader), so that it may
 * be mapped with a constant size irrespective of the actual shared object as obtained
 * from the file system.
 */
typedef struct J9HypervisorSharedObjectHeader {
	uint64_t sharedObjectMagic;
	J9VirtReadWriteLock headerLock;
	J9VirtReadWriteLock attachLock;
	J9VirtReadWriteLock dataLocks[J9SH_OSCACHE_VIRT_LOCK_COUNT];
	uint64_t sharedObjectSize;
	uint32_t unused[12]; /* reserved for future use. */
} J9HypervisorSharedObjectHeader;

/* Size of the above structure - 512 bytes. */
#define J9HYPERVISOR_SHARED_OBJECT_HEADER_SIZE    sizeof(struct J9HypervisorSharedObjectHeader)

/* These should be global -- possibly part of vm struct itself.
 * Allowed hypervisors are defined by 'enum hypervisor_kind'.
 */
typedef struct J9VirtConfig {
	uint64_t sharedObjectSize;
	intptr_t hypervisorKind;
	char sharedObjectName[J9HYPERVISOR_MAX_SHARED_OBJECT_PATH_LEN];
	intptr_t guestId;
	intptr_t initialized;
	int32_t guestMemUsed;
	int32_t guestMemUseThreshold;
	int32_t currentMemUseThreshold;
	uint64_t guestCpuUsed;
	uint64_t guestCpuUseThreshold;
	uint64_t currentCpuUseThreshold;
	BOOLEAN notifierShutdown;
} J9VirtConfig;

/* Structure to hold the Hypervisor vendor details.
 * Currently we only provide the name of the vendor
 */
typedef struct J9HypervisorVendorDetails {
	const char* hypervisorName;
} J9HypervisorVendorDetails;

/* Currently Supported Hypervisor Vendor Names */
#define HYPE_NAME_HYPERV	"Hyper-V"
#define HYPE_NAME_KVM		"KVM"
#define HYPE_NAME_POWERVM	"PowerVM"
#define HYPE_NAME_PRSM		"PR/SM"
#define HYPE_NAME_VMWARE	"VMWare"
#define HYPE_NAME_ZVM		"z/VM"
#define HYPE_NAME_POWERKVM	"PowerKVM"

/* Structure to hold the Processor usage details of the Guest OS
 * as seen by the hypervisor
 */
typedef struct J9GuestProcessorUsage {
	/* The time during which the virtual machine has used the CPU, in microseconds */
	int64_t cpuTime;
	/* The time stamp of the system, in microseconds */
	int64_t timestamp;
	/* cpu entitlement for this VM */
	SYS_FLOAT cpuEntitlement;
	/* host CPU speed (in platform-dependent units) */
	int64_t hostCpuClockSpeed;
} J9GuestProcessorUsage;

/* Structure to hold the Memory usage details of the Guest OS
 * as seen by the hypervisor
 */
typedef struct J9GuestMemoryUsage {
	/* Current memory used by the Guest OS in MB */
	int64_t memUsed;
	/* The time stamp of the system, in microseconds */
	int64_t timestamp;
	/* Maximum memory that has been allocated to the guest OS in MB */
	int64_t maxMemLimit;
} J9GuestMemoryUsage;

/* j9sysinfo_get_number_CPUs_by_type flags */
#define J9PORT_CPU_PHYSICAL 1
#define J9PORT_CPU_ONLINE 2
#define J9PORT_CPU_BOUND 3
#define J9PORT_CPU_ENTITLED 4
#define J9PORT_CPU_TARGET 5

/* j9sysinfo_get_processor_description
 */
typedef enum J9ProcessorArchitecture {

	PROCESSOR_UNDEFINED,

	PROCESSOR_S390_UNKNOWN,
	PROCESSOR_S390_GP6,
	PROCESSOR_S390_GP7,
	PROCESSOR_S390_GP8,
	PROCESSOR_S390_GP9,
	PROCESSOR_S390_GP10,
	PROCESSOR_S390_GP11,

	PROCESSOR_PPC_UNKNOWN,
	PROCESSOR_PPC_7XX,
	PROCESSOR_PPC_GP,
	PROCESSOR_PPC_GR,
	PROCESSOR_PPC_NSTAR,
	PROCESSOR_PPC_PULSAR,
	PROCESSOR_PPC_PWR403,
	PROCESSOR_PPC_PWR405,
	PROCESSOR_PPC_PWR440,
	PROCESSOR_PPC_PWR601,
	PROCESSOR_PPC_PWR602,
	PROCESSOR_PPC_PWR603,
	PROCESSOR_PPC_PWR604,
	PROCESSOR_PPC_PWR620,
	PROCESSOR_PPC_PWR630,
	PROCESSOR_PPC_RIOS1,
	PROCESSOR_PPC_RIOS2,
	PROCESSOR_PPC_P6,
	PROCESSOR_PPC_P7,
	PROCESSOR_PPC_P8,

	PROCESSOR_X86_UNKNOWN,
	PROCESSOR_X86_INTELPENTIUM,
	PROCESSOR_X86_INTELP6,
	PROCESSOR_X86_INTELPENTIUM4,
	PROCESSOR_X86_INTELCORE2,
	PROCESSOR_X86_INTELTULSA,
	PROCESSOR_X86_INTELNEHALEM,
	PROCESSOR_X86_INTELWESTMERE,
	PROCESSOR_X86_INTELSANDYBRIDGE,
	PROCESSOR_X86_INTELHASWELL,
	PROCESSOR_X86_AMDK5,
	PROCESSOR_X86_AMDK6,
	PROCESSOR_X86_AMDATHLONDURON,
	PROCESSOR_X86_AMDOPTERON,

	PROCESSOR_DUMMY = 0x40000000 /* force wide enums */

} J9ProcessorArchitecture;


/* Holds processor type and features used with j9sysinfo_get_processor_description
 * and j9sysinfo_processor_has_feature
 */
#define J9PORT_SYSINFO_FEATURES_SIZE 5
typedef struct J9ProcessorDesc {
	J9ProcessorArchitecture processor;
	J9ProcessorArchitecture physicalProcessor;
	uint32_t features[J9PORT_SYSINFO_FEATURES_SIZE];
} J9ProcessorDesc;


/* PowerPC features
 * Auxlliary Vector Hardware Capability (AT_HWCAP) features for PowerPC.
 */
#define J9PORT_PPC_FEATURE_32                    31 /* 32-bit mode.  */
#define J9PORT_PPC_FEATURE_64                    30 /* 64-bit mode.  */
#define J9PORT_PPC_FEATURE_601_INSTR             29 /* 601 chip, Old POWER ISA.  */
#define J9PORT_PPC_FEATURE_HAS_ALTIVEC           28 /* SIMD/Vector Unit.  */
#define J9PORT_PPC_FEATURE_HAS_FPU               27 /* Floating Point Unit.  */
#define J9PORT_PPC_FEATURE_HAS_MMU               26 /* Memory Management Unit.  */
#define J9PORT_PPC_FEATURE_HAS_4xxMAC            25 /* 4xx Multiply Accumulator.  */
#define J9PORT_PPC_FEATURE_UNIFIED_CACHE         24 /* Unified I/D cache.  */
#define J9PORT_PPC_FEATURE_HAS_SPE               23 /* Signal Processing ext.  */
#define J9PORT_PPC_FEATURE_HAS_EFP_SINGLE        22 /* SPE Float.  */
#define J9PORT_PPC_FEATURE_HAS_EFP_DOUBLE        21 /* SPE Double.  */
#define J9PORT_PPC_FEATURE_NO_TB                 20 /* 601/403gx have no timebase.  */
#define J9PORT_PPC_FEATURE_POWER4                19 /* POWER4 ISA 2.01.  */
#define J9PORT_PPC_FEATURE_POWER5                18 /* POWER5 ISA 2.02.  */
#define J9PORT_PPC_FEATURE_POWER5_PLUS           17 /* POWER5+ ISA 2.03.  */
#define J9PORT_PPC_FEATURE_CELL_BE               16 /* CELL Broadband Engine */
#define J9PORT_PPC_FEATURE_BOOKE                 15 /* ISA Embedded Category.  */
#define J9PORT_PPC_FEATURE_SMT                   14 /* Simultaneous Multi-Threading.  */
#define J9PORT_PPC_FEATURE_ICACHE_SNOOP          13
#define J9PORT_PPC_FEATURE_ARCH_2_05             12 /* ISA 2.05.  */
#define J9PORT_PPC_FEATURE_PA6T                  11 /* PA Semi 6T Core.  */
#define J9PORT_PPC_FEATURE_HAS_DFP               10 /* Decimal FP Unit.  */
#define J9PORT_PPC_FEATURE_POWER6_EXT             9 /* P6 + mffgpr/mftgpr.  */
#define J9PORT_PPC_FEATURE_ARCH_2_06              8 /* ISA 2.06.  */
#define J9PORT_PPC_FEATURE_HAS_VSX                7 /* P7 Vector Scalar Extension.  */
#define J9PORT_PPC_FEATURE_PSERIES_PERFMON_COMPAT 6 /* Has ISA >= 2.05 PMU basic subset support.  */
#define J9PORT_PPC_FEATURE_TRUE_LE                1 /* Processor in true Little Endian mode.  */
#define J9PORT_PPC_FEATURE_PPC_LE                 0 /* Processor emulates Little Endian Mode.  */

#define J9PORT_PPC_FEATURE_ARCH_2_07             32 + 31
#define J9PORT_PPC_FEATURE_HTM                   32 + 30
#define J9PORT_PPC_FEATURE_DSCR                  32 + 29
#define J9PORT_PPC_FEATURE_EBB                   32 + 28
#define J9PORT_PPC_FEATURE_ISEL                  32 + 27
#define J9PORT_PPC_FEATURE_TAR                   32 + 26


/* s390 features
 * z/Architecture Principles of Operation 4-69
 * STORE FACILITY LIST EXTENDED (STFLE)
 */
#define J9PORT_S390_FEATURE_ESAN3      0 /* STFLE bit 0 */
#define J9PORT_S390_FEATURE_ZARCH      1 /* STFLE bit 2 */
#define J9PORT_S390_FEATURE_STFLE      2 /* STFLE bit 7 */
#define J9PORT_S390_FEATURE_MSA        3 /* STFLE bit 17 */
#define J9PORT_S390_FEATURE_LDISP      4 /* STFLE bit 19 */
#define J9PORT_S390_FEATURE_EIMM       5 /* STFLE bit 21 */
#define J9PORT_S390_FEATURE_DFP        6 /* STFLE bit 42 & 44 */
#define J9PORT_S390_FEATURE_HPAGE      7
#define J9PORT_S390_FEATURE_ETF3EH     8 /* STFLE bit 22 & 30 */
#define J9PORT_S390_FEATURE_HIGH_GPRS  9 /* set if 64 bit */
#define J9PORT_S390_FEATURE_TE        10 /* STFLE bit 50 & 73 */
#define J9PORT_S390_FEATURE_MSA_EXTENSION3                      11 /* STFLE bit 76 */
#define J9PORT_S390_FEATURE_MSA_EXTENSION4                      12 /* STFLE bit 77 */

#define J9PORT_S390_FEATURE_COMPARE_AND_SWAP_AND_STORE          32 + 0  /* STFLE bit 32 */
#define J9PORT_S390_FEATURE_COMPARE_AND_SWAP_AND_STORE2         32 + 1  /* STFLE bit 33 */
#define J9PORT_S390_FEATURE_GENERAL_INSTRUCTIONS_EXTENSIONS     32 + 2  /* STFLE bit 34 */
#define J9PORT_S390_FEATURE_EXECUTE_EXTENSIONS                  32 + 3  /* STFLE bit 35 */
#define J9PORT_S390_FEATURE_FPE                                 32 + 9  /* STFLE bit 41 */
#define J9PORT_S390_FEATURE_MISCELLANEOUS_INSTRUCTION_EXTENSION 32 + 17 /* STFLE bit 49 */

#define J9PORT_S390_FEATURE_RI            64 + 0 /* STFLE bit 64 */

#define J9PORT_S390_FEATURE_VECTOR_FACILITY 128 + 1	/* STFLE Bit 129 */


/* x86 features
 * INTEL INSTRUCTION SET REFERENCE, A-M
 * 3-170 Vol. 2A Table 3-21. More on Feature Information Returned in the EDX Register
 */
#define J9PORT_X86_FEATURE_FPU     0 /* Floating Point Unit On-Chip. */
#define J9PORT_X86_FEATURE_VME     1 /* Virtual 8086 Mode Enhancements. */
#define J9PORT_X86_FEATURE_DE      2 /* DE Debugging Extensions. */
#define J9PORT_X86_FEATURE_PSE     3 /* Page Size Extension. */
#define J9PORT_X86_FEATURE_TSC     4 /* Time Stamp Counter. */
#define J9PORT_X86_FEATURE_MSR     5 /* Model Specific Registers RDMSR and WRMSR Instructions. */
#define J9PORT_X86_FEATURE_PAE     6 /* Physical Address Extension. */
#define J9PORT_X86_FEATURE_MCE     7 /* Machine Check Exception. */
#define J9PORT_X86_FEATURE_CX8     8 /* Compare-and-exchange 8 bytes (64 bits) instruction */
#define J9PORT_X86_FEATURE_APIC    9 /* APIC On-Chip. */
#define J9PORT_X86_FEATURE_10     10 /* Reserved */
#define J9PORT_X86_FEATURE_SEP    11 /* SYSENTER and SYSEXIT Instructions. */
#define J9PORT_X86_FEATURE_MTRR   12 /* Memory Type Range Registers. */
#define J9PORT_X86_FEATURE_PGE    13 /* Page Global Bit. */
#define J9PORT_X86_FEATURE_MCA    14 /* Machine Check Architecture. */
#define J9PORT_X86_FEATURE_CMOV   15 /* Conditional Move Instructions. */
#define J9PORT_X86_FEATURE_PAT    16 /* Page Attribute Table. */
#define J9PORT_X86_FEATURE_PSE_36 17 /* 36-Bit Page Size Extension. */
#define J9PORT_X86_FEATURE_PSN    18 /* Processor Serial Number. */
#define J9PORT_X86_FEATURE_CLFSH  19 /* CLFLUSH Instruction. */
#define J9PORT_X86_FEATURE_20     20 /* Reserved */
#define J9PORT_X86_FEATURE_DS     21 /* Debug Store. */
#define J9PORT_X86_FEATURE_ACPI   22 /* Thermal Monitor and Software Controlled Clock Facilities. */
#define J9PORT_X86_FEATURE_MMX    23 /* Intel MMX Technology. */
#define J9PORT_X86_FEATURE_FXSR   24 /* FXSAVE and FXRSTOR Instructions. */
#define J9PORT_X86_FEATURE_SSE    25 /* The processor supports the SSE extensions. */
#define J9PORT_X86_FEATURE_SSE2   26 /* The processor supports the SSE2 extensions. */
#define J9PORT_X86_FEATURE_SS     27 /* Self Snoop. */
#define J9PORT_X86_FEATURE_HTT    28 /* Hyper Threading. */
#define J9PORT_X86_FEATURE_TM     29 /* Thermal Monitor. */
#define J9PORT_X86_FEATURE_30     30 /* Reserved */
#define J9PORT_X86_FEATURE_PBE    31 /* Pending Break Enable. */

/* INTEL INSTRUCTION SET REFERENCE, A-M
 * Vol. 2A 3-167 Table 3-20. Feature Information Returned in the ECX Register
 */
#define J9PORT_X86_FEATURE_SSE3         32 + 0 /* Streaming SIMD Extensions 3 */
#define J9PORT_X86_FEATURE_PCLMULQDQ    32 + 1 /* PCLMULQDQ. */
#define J9PORT_X86_FEATURE_DTES64       32 + 2 /* 64-bit DS Area. */
#define J9PORT_X86_FEATURE_MONITOR      32 + 3 /* MONITOR/MWAIT. */
#define J9PORT_X86_FEATURE_DS_CPL       32 + 4 /* CPL Qualified Debug Store. */
#define J9PORT_X86_FEATURE_VMX          32 + 5 /* Virtual Machine Extensions. */
#define J9PORT_X86_FEATURE_SMX          32 + 6 /* Safer Mode Extensions. */
#define J9PORT_X86_FEATURE_EIST         32 + 7 /* Enhanced Intel SpeedStep technology. */
#define J9PORT_X86_FEATURE_TM2          32 + 8 /* Thermal Monitor 2. */
#define J9PORT_X86_FEATURE_SSSE3        32 + 9 /* Supplemental Streaming SIMD Extensions 3 */
#define J9PORT_X86_FEATURE_CNXT_ID      32 + 10 /* L1 Context ID. */
#define J9PORT_X86_FEATURE_11           32 + 11 /* Reserved */
#define J9PORT_X86_FEATURE_FMA          32 + 12 /* FMA extensions using YMM state. */
#define J9PORT_X86_FEATURE_CMPXCHG16B   32 + 13 /* CMPXCHG16B Available. */
#define J9PORT_X86_FEATURE_XTPR         32 + 14 /* xTPR Update Control. */
#define J9PORT_X86_FEATURE_PDCM         32 + 15 /* Perfmon and Debug Capability. */
#define J9PORT_X86_FEATURE_16           32 + 16 /* Reserved. */
#define J9PORT_X86_FEATURE_PCID         32 + 17 /* Process-context identifiers. */
#define J9PORT_X86_FEATURE_DCA          32 + 18 /* Processor supports the ability to prefetch data from a memory mapped device. */
#define J9PORT_X86_FEATURE_SSE4_1       32 + 19 /* Processor supports SSE4.1. */
#define J9PORT_X86_FEATURE_SSE4_2       32 + 20 /* Processor supports SSE4.2. */
#define J9PORT_X86_FEATURE_X2APIC       32 + 21 /* Processor supports x2APIC feature. */
#define J9PORT_X86_FEATURE_MOVBE        32 + 22 /* Processor supports MOVBE instruction. */
#define J9PORT_X86_FEATURE_POPCNT       32 + 23 /* Processor supports the POPCNT instruction. */
#define J9PORT_X86_FEATURE_TSC_DEADLINE 32 + 24 /* Processor's local APIC timer supports one-shot operation using a TSC deadline value. */
#define J9PORT_X86_FEATURE_AESNI        32 + 25 /* Processor supports the AESNI instruction extensions. */
#define J9PORT_X86_FEATURE_XSAVE        32 + 26 /* Processor supports the XSAVE/XRSTOR processor extended states. */
#define J9PORT_X86_FEATURE_OSXSAVE      32 + 27 /* OS has enabled XSETBV/XGETBV instructions to access XCR0, and support for processor extended state management using XSAVE/XRSTOR. */
#define J9PORT_X86_FEATURE_AVX          32 + 28 /* Processor supports the AVX instruction extensions. */
#define J9PORT_X86_FEATURE_F16C         32 + 29 /* 16-bit floating-point conversion instructions. */
#define J9PORT_X86_FEATURE_RDRAND       32 + 30 /* Processor supports RDRAND instruction. */

#define J9PORT_SL_FOUND  0
#define J9PORT_SL_NOT_FOUND  1
#define J9PORT_SL_INVALID  2
#define J9PORT_SL_UNSUPPORTED  3
#define J9PORT_SL_UNKNOWN 4					/* Unknown Shared Library related error. */

#define J9PORT_SLOPEN_DECORATE  1 			/* Note this value must remain 1, in order for legacy callers using TRUE and FALSE to control decoration */
#define J9PORT_SLOPEN_LAZY  2
#define J9PORT_SLOPEN_NO_LOOKUP_MSG_FOR_NOT_FOUND  4
#define J9PORT_SLOPEN_OPEN_EXECUTABLE 8     /* Can be ORed without affecting existing flags. */

#define J9PORT_ARCH_X86  "x86"
#define J9PORT_ARCH_PPC  "ppc" 				/* in line with IBM JDK 1.22 and above for AIX and Linux/PPC */
#define J9PORT_ARCH_PPC64  "ppc64"
#define J9PORT_ARCH_PPC64LE  "ppc64le"
#define J9PORT_ARCH_S390  "s390"
#define J9PORT_ARCH_S390X  "s390x"
#define J9PORT_ARCH_HAMMER  "amd64"

#define J9PORT_TTY_IN  0
#define J9PORT_TTY_OUT  1
#define J9PORT_TTY_ERR  2

#define J9PORT_CTLDATA_SIG_FLAGS  "SIG_FLAGS"
#define J9PORT_CTLDATA_TRACE_START  "TRACE_START"
#define J9PORT_CTLDATA_TRACE_STOP  "TRACE_STOP"
#define J9PORT_CTLDATA_HTTP_PROXY_PARMS  "HTTP_PROXY_PARMS"
#define J9PORT_CTLDATA_SHMEM_CONTROL_DIR  "SHMEM_CONTROL_DIR"
#define J9PORT_CTLDATA_VMEM_NUMA_IN_USE  "VMEM_NUMA_IN_USE"
#define J9PORT_CTLDATA_SYSLOG_OPEN  "SYSLOG_OPEN"
#define J9PORT_CTLDATA_SYSLOG_CLOSE  "SYSLOG_CLOSE"
#define J9PORT_CTLDATA_NOIPT  "NOIPT"
#define J9PORT_CTLDATA_TIME_CLEAR_TICK_TOCK  "TIME_CLEAR_TICK_TOCK"
#define J9PORT_CTLDATA_MEM_CATEGORIES_SET  "MEM_CATEGORIES_SET"
#define J9PORT_CTLDATA_AIX_PROC_ATTR  "AIX_PROC_ATTR"
#define J9PORT_CTLDATA_ALLOCATE32_COMMIT_SIZE  "ALLOCATE32_COMMIT_SIZE"
#define J9PORT_CTLDATA_NOSUBALLOC32BITMEM  "NOSUBALLOC32BITMEM"
#define J9PORT_CTLDATA_VMEM_ADVISE_OS_ONFREE  "VMEM_ADVISE_OS_ONFREE"
#define J9PORT_CTLDATA_VECTOR_REGS_SUPPORT_ON  "VECTOR_REGS_SUPPORT_ON"

#define J9PORT_MAJOR_VERSION_NUMBER  82
#define J9PORT_MINOR_VERSION_NUMBER  0

#define J9PORT_CAPABILITY_BASE  0
#define J9PORT_CAPABILITY_STANDARD  1
#define J9PORT_CAPABILITY_CAN_RESERVE_SPECIFIC_ADDRESS  2
#define J9PORT_CAPABILITY_ALLOCATE_TOP_DOWN  4

#define J9PORT_FILE_READ_LOCK  1
#define J9PORT_FILE_WRITE_LOCK  2
#define J9PORT_FILE_WAIT_FOR_LOCK  4
#define J9PORT_FILE_NOWAIT_FOR_LOCK  8

#define J9PORT_MMAP_CAPABILITY_COPYONWRITE  1
#define J9PORT_MMAP_CAPABILITY_READ  2
#define J9PORT_MMAP_CAPABILITY_WRITE  4
#define J9PORT_MMAP_CAPABILITY_UMAP_REQUIRES_SIZE  8
#define J9PORT_MMAP_CAPABILITY_MSYNC  16
#define J9PORT_MMAP_CAPABILITY_PROTECT  32
#define J9PORT_MMAP_FLAG_CREATE_FILE  1
#define J9PORT_MMAP_FLAG_READ  2
#define J9PORT_MMAP_FLAG_WRITE  4
#define J9PORT_MMAP_FLAG_COPYONWRITE  8
#define J9PORT_MMAP_FLAG_EXECUTABLE  16
#define J9PORT_MMAP_FLAG_SHARED  32
#define J9PORT_MMAP_FLAG_PRIVATE  64
#define J9PORT_MMAP_SYNC_WAIT  0x80
#define J9PORT_MMAP_SYNC_ASYNC  0x100
#define J9PORT_MMAP_SYNC_INVALIDATE  0x200


#define J9PORT_SIG_FLAG_MAY_RETURN  1
#define J9PORT_SIG_FLAG_MAY_CONTINUE_EXECUTION  2
#define J9PORT_SIG_FLAG_SIGSEGV  4
#define J9PORT_SIG_FLAG_SIGBUS  8
#define J9PORT_SIG_FLAG_SIGILL  16
#define J9PORT_SIG_FLAG_SIGFPE  32
#define J9PORT_SIG_FLAG_SIGTRAP  64
#define J9PORT_SIG_FLAG_SIGRESERVED7  0x80
#define J9PORT_SIG_FLAG_SIGRESERVED8  0x100
#define J9PORT_SIG_FLAG_SIGRESERVED9  0x200
#define J9PORT_SIG_FLAG_SIGALLSYNC  (J9PORT_SIG_FLAG_SIGSEGV | J9PORT_SIG_FLAG_SIGBUS | J9PORT_SIG_FLAG_SIGILL | J9PORT_SIG_FLAG_SIGFPE | J9PORT_SIG_FLAG_SIGTRAP)
#define J9PORT_SIG_FLAG_SIGQUIT  0x400
#define J9PORT_SIG_FLAG_SIGABRT  0x800
#define J9PORT_SIG_FLAG_SIGTERM  0x1000
#define J9PORT_SIG_FLAG_SIGRECONFIG  0x2000
#define J9PORT_SIG_FLAG_SIGINT  0x4000
#define J9PORT_SIG_FLAG_SIGXFSZ  0x8000
#define J9PORT_SIG_FLAG_SIGRESERVED16  0x10000
#define J9PORT_SIG_FLAG_SIGRESERVED17  0x20000
#define J9PORT_SIG_FLAG_SIGALLASYNC  (J9PORT_SIG_FLAG_SIGQUIT | J9PORT_SIG_FLAG_SIGABRT | J9PORT_SIG_FLAG_SIGTERM | J9PORT_SIG_FLAG_SIGRECONFIG | J9PORT_SIG_FLAG_SIGXFSZ)
#define J9PORT_SIG_FLAG_SIGFPE_DIV_BY_ZERO  (J9PORT_SIG_FLAG_SIGFPE | 0x40000)
#define J9PORT_SIG_FLAG_SIGFPE_INT_DIV_BY_ZERO  (J9PORT_SIG_FLAG_SIGFPE | 0x80000)
#define J9PORT_SIG_FLAG_SIGFPE_INT_OVERFLOW  (J9PORT_SIG_FLAG_SIGFPE | 0x100000)
#define J9PORT_SIG_FLAG_DOES_NOT_MAP_TO_POSIX  0x200000

#define J9PORT_SIG_EXCEPTION_CONTINUE_SEARCH  0
#define J9PORT_SIG_EXCEPTION_CONTINUE_EXECUTION  1
#define J9PORT_SIG_EXCEPTION_RETURN  2
#define J9PORT_SIG_EXCEPTION_COOPERATIVE_SHUTDOWN  3

#define J9PORT_SIG_NO_EXCEPTION 0
#define J9PORT_SIG_EXCEPTION_OCCURRED  1
#define J9PORT_SIG_ERROR  -1

#define J9PORT_SIG_SIGNAL  0
#define J9PORT_SIG_GPR  1
#define J9PORT_SIG_OTHER  2
#define J9PORT_SIG_CONTROL  3
#define J9PORT_SIG_FPR  4
#define J9PORT_SIG_MODULE  5
#define J9PORT_SIG_VR	6
#define J9PORT_SIG_NUM_CATEGORIES  (J9PORT_SIG_VR + 1)

#define J9PORT_SIG_SIGNAL_TYPE  -1
#define J9PORT_SIG_SIGNAL_CODE  -2
#define J9PORT_SIG_SIGNAL_ERROR_VALUE  -3
#define J9PORT_SIG_CONTROL_PC  -4
#define J9PORT_SIG_CONTROL_SP  -5
#define J9PORT_SIG_CONTROL_BP  -6
#define J9PORT_SIG_GPR_X86_EDI  -7
#define J9PORT_SIG_GPR_X86_ESI  -8
#define J9PORT_SIG_GPR_X86_EAX  -9
#define J9PORT_SIG_GPR_X86_EBX  -10
#define J9PORT_SIG_GPR_X86_ECX  -11
#define J9PORT_SIG_GPR_X86_EDX  -12
#define J9PORT_SIG_MODULE_NAME  -13
#define J9PORT_SIG_SIGNAL_ADDRESS  -14
#define J9PORT_SIG_SIGNAL_HANDLER  -15
#define J9PORT_SIG_SIGNAL_PLATFORM_SIGNAL_TYPE  -16
#define J9PORT_SIG_SIGNAL_INACCESSIBLE_ADDRESS  -17
#define J9PORT_SIG_GPR_AMD64_RDI  -18
#define J9PORT_SIG_GPR_AMD64_RSI  -19
#define J9PORT_SIG_GPR_AMD64_RAX  -20
#define J9PORT_SIG_GPR_AMD64_RBX  -21
#define J9PORT_SIG_GPR_AMD64_RCX  -22
#define J9PORT_SIG_GPR_AMD64_RDX  -23
#define J9PORT_SIG_GPR_AMD64_R8  -24
#define J9PORT_SIG_GPR_AMD64_R9  -25
#define J9PORT_SIG_GPR_AMD64_R10  -26
#define J9PORT_SIG_GPR_AMD64_R11  -27
#define J9PORT_SIG_GPR_AMD64_R12  -28
#define J9PORT_SIG_GPR_AMD64_R13  -29
#define J9PORT_SIG_GPR_AMD64_R14  -30
#define J9PORT_SIG_GPR_AMD64_R15  -31
#define J9PORT_SIG_CONTROL_POWERPC_LR  -32
#define J9PORT_SIG_CONTROL_POWERPC_MSR  -33
#define J9PORT_SIG_CONTROL_POWERPC_CTR  -34
#define J9PORT_SIG_CONTROL_POWERPC_CR  -35
#define J9PORT_SIG_CONTROL_POWERPC_FPSCR  -36
#define J9PORT_SIG_CONTROL_POWERPC_XER  -37
#define J9PORT_SIG_CONTROL_POWERPC_MQ  -38
#define J9PORT_SIG_CONTROL_POWERPC_DAR  -39
#define J9PORT_SIG_CONTROL_POWERPC_DSIR  -40
#define J9PORT_SIG_CONTROL_S390_FPC  -41
#define J9PORT_SIG_CONTROL_S390_GPR7  -42
#define J9PORT_SIG_CONTROL_X86_EFLAGS  -43
#define J9PORT_SIG_SIGNAL_ZOS_CONDITION_INFORMATION_BLOCK  -44
#define J9PORT_SIG_SIGNAL_ZOS_CONDITION_FACILITY_ID  -45
#define J9PORT_SIG_SIGNAL_ZOS_CONDITION_MESSAGE_NUMBER  -46
#define J9PORT_SIG_SIGNAL_ZOS_CONDITION_FEEDBACK_TOKEN  -47
#define J9PORT_SIG_SIGNAL_ZOS_CONDITION_SEVERITY  -48
#define J9PORT_SIG_MODULE_FUNCTION_NAME  -49
#define J9PORT_SIG_WINDOWS_DEFER_TRY_EXCEPT_HANDLER  -50
#define J9PORT_SIG_CONTROL_S390_BEA  -51

#define J9PORT_SIG_VALUE_UNDEFINED  1
#define J9PORT_SIG_VALUE_STRING  2
#define J9PORT_SIG_VALUE_ADDRESS  3
#define J9PORT_SIG_VALUE_32  4
#define J9PORT_SIG_VALUE_64  5
#define J9PORT_SIG_VALUE_FLOAT_64  6
#define J9PORT_SIG_VALUE_16  7
#define J9PORT_SIG_VALUE_128 8

#define J9PORT_SIG_OPTIONS_JSIG_NO_CHAIN  1
#define J9PORT_SIG_OPTIONS_REDUCED_SIGNALS_SYNCHRONOUS  2
#define J9PORT_SIG_OPTIONS_REDUCED_SIGNALS_ASYNCHRONOUS  4
#define J9PORT_SIG_OPTIONS_ZOS_USE_CEEHDLR  8
#define J9PORT_SIG_OPTIONS_COOPERATIVE_SHUTDOWN  16
#define J9PORT_SIG_OPTIONS_SIGXFSZ  32

#define J9PORT_PAGE_PROTECT_NOT_SUPPORTED  -2
#define J9PORT_PAGE_PROTECT_NONE  1
#define J9PORT_PAGE_PROTECT_READ  2
#define J9PORT_PAGE_PROTECT_WRITE  4
#define J9PORT_PAGE_PROTECT_EXEC  8

#define J9PORT_VMEM_RESERVE_USED_INVALID  0
#define J9PORT_VMEM_RESERVE_USED_J9MEM_ALLOCATE_MEMORY  1
#define J9PORT_VMEM_RESERVE_USED_MMAP  2
#define J9PORT_VMEM_RESERVE_USED_SHM  3
#define J9PORT_VMEM_RESERVE_USED_MALLOC31  4
#define J9PORT_VMEM_RESERVE_USED_J9ALLOCATE_LARGE_FIXED_PAGES_ABOVE_BAR  5
#define J9PORT_VMEM_RESERVE_USED_J9ALLOCATE_LARGE_PAGEABLE_PAGES_ABOVE_BAR  6
#define J9PORT_VMEM_RESERVE_USED_J9ALLOCATE_LARGE_PAGES_BELOW_BAR  7
#define J9PORT_VMEM_RESERVE_USED_J9ALLOCATE_4K_PAGES_IN_2TO32G_AREA  8
#define J9PORT_VMEM_RESERVE_USED_J9ALLOCATE_4K_PAGES_ABOVE_BAR 9
#define J9PORT_VMEM_RESERVE_USED_J9ALLOCATE_4K_PAGES_BELOW_BAR 10

#define J9PORT_ENSURE_CAPACITY_FAILED  0
#define J9PORT_ENSURE_CAPACITY_SUCCESS  1
#define J9PORT_ENSURE_CAPACITY_NOT_REQUIRED  2


/* j9str_convert encodings. */
/* character set currently in effect */
#define J9STR_CODE_PLATFORM 1
/* modified UTF-8 */
#define J9STR_CODE_MUTF8 2
/* UTF-16 */
#define J9STR_CODE_WIDE 3
/* EBCDIC */
#define J9STR_CODE_EBCDIC 4
/* orthodox UTF-8 */
#define J9STR_CODE_UTF8 5
/* orthodox UTF-8 */
#define J9STR_CODE_LATIN1 6
/* Windows default ANSI code page */
#define J9STR_CODE_WINDEFAULTACP 7
/* Windows current thread ANSI code page */
#define J9STR_CODE_WINTHREADACP 8

#define UNICODE_REPLACEMENT_CHARACTER 0xFFFD
#define MAX_STRING_TERMINATOR_LENGTH 4
/* worst case terminating sequence: wchar or UTF-32 */


/* include the generated Port header here (at the end of the file since it relies on some defines from within this file) */
#include "j9port_generated.h"

#endif /* !defined(J9PORT_H_) */
