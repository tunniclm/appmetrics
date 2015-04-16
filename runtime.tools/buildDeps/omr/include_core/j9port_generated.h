/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 1991, 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

/**
 * @file
 * @ingroup Port
 * @brief Port Library Header
 */

#ifndef portlibrarydefines_h
#define portlibrarydefines_h

/* Constants from J9NLSConstants */
#define J9NLS_BEGIN_MULTI_LINE 0x100
#define J9NLS_CONFIG 0x800
#define J9NLS_DO_NOT_APPEND_NEWLINE 0x10
#define J9NLS_DO_NOT_PRINT_MESSAGE_TAG 0x1
#define J9NLS_END_MULTI_LINE 0x400
#define J9NLS_ERROR 0x2
#define J9NLS_INFO 0x8
#define J9NLS_MULTI_LINE 0x200
#define J9NLS_STDERR 0x40
#define J9NLS_STDOUT 0x20
#define J9NLS_VITAL 0x1000
#define J9NLS_WARNING 0x4

typedef struct J9PortLibraryVersion {
    uint16_t majorVersionNumber;
    uint16_t minorVersionNumber;
    uint32_t padding;
    uint64_t capabilities;
} J9PortLibraryVersion;

typedef struct J9PortVmemIdentifier {
    void* address;
    void* handle;
    uintptr_t size;
    uintptr_t pageSize;
    uintptr_t pageFlags;
    uintptr_t mode;
    uintptr_t allocator;
    J9MemCategory* category;
} J9PortVmemIdentifier;

typedef struct J9PortShmemStatistic {
    uintptr_t shmid;
    uintptr_t nattach;
    uintptr_t key;
    uintptr_t ouid;
    uintptr_t ogid;
    uintptr_t cuid;
    uintptr_t cgid;
    char* file;
    uintptr_t size;
    int64_t lastAttachTime;
    int64_t lastDetachTime;
    int64_t lastChangeTime;
    char* controlDir;
    J9Permission perm;
} J9PortShmemStatistic;

typedef struct J9PortShsemStatistic {
    uintptr_t semid;
    uintptr_t ouid;
    uintptr_t ogid;
    uintptr_t cuid;
    uintptr_t cgid;
    int64_t lastOpTime;
    int64_t lastChangeTime;
    int32_t nsems;
    J9Permission perm;
} J9PortShsemStatistic;

typedef struct J9MmapHandle {
    void* pointer;
    uintptr_t size;
    void* allocPointer;
    J9MemCategory* category;
} J9MmapHandle;

typedef struct J9VirtHandle {
    void* allocHdr;
    uintptr_t allocSz;
    void* allocPtr;
    J9MemCategory* category;
} J9VirtHandle;

typedef struct J9RIParameters {
    uint32_t flags;
    void* controlBlock;
} J9RIParameters;

struct J9Heap;

#if !defined(WIN32)
#include <ucontext.h>
#endif /* !WIN32 */

#if defined(J9ZOS390)
struct __mcontext;
#endif /* J9ZOS390 */

typedef struct J9PlatformStackFrame {
    uintptr_t stack_pointer;
    uintptr_t base_pointer;
    uintptr_t instruction_pointer;
    uintptr_t register1;
    uintptr_t register2;
    uintptr_t register3;
    char* symbol;
    struct J9PlatformStackFrame* parent_frame;
} J9PlatformStackFrame;

typedef struct J9PlatformThread {
    uintptr_t thread_id;
    uintptr_t process_id;
    uintptr_t stack_base;
    uintptr_t stack_end;
    uintptr_t priority;
#if defined(WIN32)
    void* context;
#elif defined(J9ZOS390)
	/* This should really be 'struct __mcontext*' however DDR cannot parse the zos system header
	 * that we carry as part of portlib. DDR runs on a linux machine and the defines required by the edcwccwi.h
	 * header are set by both VAC and other zos system headers that DDR has no access to.
	 */
    void *context;
#else
    ucontext_t *context;
#endif
    struct J9PlatformStackFrame* callstack;
#if defined(WIN32)
    void* sigmask;
#else /* WIN32 */
    sigset_t *sigmask;
#endif /* WIN32 */
    intptr_t error;
    void* dsa;
    uintptr_t dsa_format;
    void* caa;
} J9PlatformThread;

typedef struct J9ThreadWalkState {
    struct J9PortLibrary* portLibrary;
    struct J9PlatformThread* current_thread;
    int64_t deadline1;
    int64_t deadline2;
    struct J9Heap * heap;
    void* platform_data;
    intptr_t error;
    uintptr_t error_detail;
    const char* error_string;
} J9ThreadWalkState;

typedef struct J9PortSysInfoLoadData {
	double oneMinuteAverage;
	double fiveMinuteAverage;
	double fifteenMinuteAverage;
} J9PortSysInfoLoadData;

typedef struct J9StringTokens {
	void * table;
} J9StringTokens;

/**
 * @struct J9PortLibrary
 * The port library function table
 */
typedef uintptr_t (*j9sig_protected_fn)(struct J9PortLibrary* portLib, void* handler_arg); /* Forward struct declaration */
typedef uintptr_t (*j9sig_handler_fn)(struct J9PortLibrary* portLib, uint32_t gpType, void* gpInfo, void* handler_arg); /* Forward struct declaration */
typedef struct J9Heap J9Heap; /* Forward struct declaration */
struct J9StringTokens; /* Forward struct declaration */
struct J9MemoryNodeDetail ; /* Forward struct declaration */
struct J9PortLibrary ; /* Forward struct declaration */
struct J9PortVmemIdentifier ; /* Forward struct declaration */
struct J9PortShmemStatistic; /* Forward struct declaration */
struct J9PortLibrary; /* Forward struct declaration */
struct J9PortLibraryVersion ; /* Forward struct declaration */
struct J9Heap; /* Forward struct declaration */
struct j9shsem_handle; /* Forward struct declaration */
struct j9shmem_handle ; /* Forward struct declaration */
struct J9FileStat ; /* Forward struct declaration */
struct j9NetworkInterfaceArray_struct; /* Forward struct declaration */
struct J9PortVmemParams ; /* Forward struct declaration */
struct J9PortShSemParameters ; /* Forward struct declaration */
struct J9RIParameters ; /* Forward struct declaration */
struct J9MemoryInfo ; /* Forward struct declaration */
struct j9shmem_handle; /* Forward struct declaration */
struct J9SysinfoCPUTime ; /* Forward struct declaration */
struct J9Heap ; /* Forward struct declaration */
struct j9shsem_handle ; /* Forward struct declaration */
struct j9NetworkInterfaceArray_struct ; /* Forward struct declaration */
struct J9PortSysInfoLoadData ; /* Forward struct declaration */
struct J9FileStatFilesystem ; /* Forward struct declaration */
struct J9ControlFileStatus ; /* Forward struct declaration */
typedef struct J9PortLibrary {
	/** portVersion*/
    struct J9PortLibraryVersion portVersion;
	/** portGlobals*/
    struct J9PortLibraryGlobalData* portGlobals;
	/** see @ref j9port.c::j9port_shutdown_library "j9port_shutdown_library"*/
    int32_t  ( *port_shutdown_library)(struct J9PortLibrary *portLibrary ) ;
	/** see @ref j9port.c::j9port_isFunctionOverridden "j9port_isFunctionOverridden"*/
    int32_t  ( *port_isFunctionOverridden)(struct J9PortLibrary *portLibrary, uintptr_t offset) ;
	/** see @ref j9port.c::j9port_tls_free "j9port_tls_free"*/
    void  ( *port_tls_free)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9error.c::j9error_startup "j9error_startup"*/
    int32_t  ( *error_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9error.c::j9error_shutdown "j9error_shutdown"*/
    void  ( *error_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9error.c::j9error_last_error_number "j9error_last_error_number"*/
    int32_t  ( *error_last_error_number)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9error.c::j9error_last_error_message "j9error_last_error_message"*/
    const char*  ( *error_last_error_message)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9error.c::j9error_set_last_error "j9error_set_last_error"*/
    int32_t  ( *error_set_last_error)(struct J9PortLibrary *portLibrary,  int32_t platformCode, int32_t portableCode) ;
	/** see @ref j9error.c::j9error_set_last_error_with_message "j9error_set_last_error_with_message"*/
    int32_t  ( *error_set_last_error_with_message)(struct J9PortLibrary *portLibrary, int32_t portableCode, const char *errorMessage) ;
	/** see @ref j9time.c::j9time_startup "j9time_startup"*/
    int32_t  ( *time_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9time.c::j9time_shutdown "j9time_shutdown"*/
    void  ( *time_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9time.c::j9time_msec_clock "j9time_msec_clock"*/
    uintptr_t  ( *time_msec_clock)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9time.c::j9time_usec_clock "j9time_usec_clock"*/
    uintptr_t  ( *time_usec_clock)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9time.c::j9time_current_time_millis "j9time_current_time_millis"*/
    int64_t  ( *time_current_time_millis)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9time.c::j9time_nano_time "j9time_nano_time"*/
    int64_t  ( *time_nano_time)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9time.c::j9time_hires_clock "j9time_hires_clock"*/
    uint64_t  ( *time_hires_clock)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9time.c::j9time_hires_frequency "j9time_hires_frequency"*/
    uint64_t  ( *time_hires_frequency)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9time.c::j9time_hires_delta "j9time_hires_delta"*/
    uint64_t  ( *time_hires_delta)(struct J9PortLibrary *portLibrary, uint64_t startTime, uint64_t endTime, uint64_t requiredResolution) ;
	/** see @ref j9sysinfo.c::j9sysinfo_startup "j9sysinfo_startup"*/
    int32_t  ( *sysinfo_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_shutdown "j9sysinfo_shutdown"*/
    void  ( *sysinfo_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_process_exists "j9sysinfo_process_exists"*/
    intptr_t  ( *sysinfo_process_exists)(struct J9PortLibrary *portLibrary, uintptr_t pid)  ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_egid "j9sysinfo_get_egid"*/
    uintptr_t  ( *sysinfo_get_egid)(struct J9PortLibrary* portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_euid "j9sysinfo_get_euid"*/
    uintptr_t  ( *sysinfo_get_euid)(struct J9PortLibrary* portLibrary) ;
 	/** see @ref j9sysinfo.c::j9sysinfo_get_groups "j9sysinfo_get_groups"*/
    intptr_t  ( *sysinfo_get_groups)(struct J9PortLibrary* portLibrary, uint32_t **gidList, uint32_t categoryCode) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_pid "j9sysinfo_get_pid"*/
    uintptr_t  ( *sysinfo_get_pid)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_ppid "j9sysinfo_get_ppid"*/
    uintptr_t  ( *sysinfo_get_ppid)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_memory_info "j9sysinfo_get_memory_info"*/
    int32_t  ( *sysinfo_get_memory_info)(struct J9PortLibrary *portLibrary, struct J9MemoryInfo *memInfo, ...) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_processor_info "j9sysinfo_get_processor_info"*/
    int32_t  ( *sysinfo_get_processor_info)(struct J9PortLibrary *portLibrary, J9ProcessorInfos *procInfo) ;
	/** see @ref j9sysinfo.c::j9sysinfo_destroy_processor_info "j9sysinfo_destroy_processor_info"*/
    void  ( *sysinfo_destroy_processor_info)(struct J9PortLibrary *portLibrary, J9ProcessorInfos *procInfos) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_physical_memory "j9sysinfo_get_physical_memory"*/
    uint64_t  ( *sysinfo_get_physical_memory)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_OS_version "j9sysinfo_get_OS_version"*/
    const char*  ( *sysinfo_get_OS_version)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_env "j9sysinfo_get_env"*/
    intptr_t  ( *sysinfo_get_env)(struct J9PortLibrary *portLibrary, const char *envVar, char *infoString, uintptr_t bufSize) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_CPU_architecture "j9sysinfo_get_CPU_architecture"*/
    const char*  ( *sysinfo_get_CPU_architecture)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_OS_type "j9sysinfo_get_OS_type"*/
    const char*  ( *sysinfo_get_OS_type)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_classpathSeparator "j9sysinfo_get_classpathSeparator"*/
    uint16_t  ( *sysinfo_get_classpathSeparator)(struct J9PortLibrary *portLibrary ) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_executable_name "j9sysinfo_get_executable_name"*/
    intptr_t  ( *sysinfo_get_executable_name)(struct J9PortLibrary *portLibrary, const char *argv0, char **result) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_username "j9sysinfo_get_username"*/
    intptr_t  ( *sysinfo_get_username)(struct J9PortLibrary* portLibrary, char* buffer, uintptr_t length) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_groupname "j9sysinfo_get_groupname"*/
    intptr_t  ( *sysinfo_get_groupname)(struct J9PortLibrary* portLibrary, char* buffer, uintptr_t length) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_load_average "j9sysinfo_get_load_average"*/
    intptr_t  ( *sysinfo_get_load_average)(struct J9PortLibrary *portLibrary, struct J9PortSysInfoLoadData *loadAverageData) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_CPU_utilization "j9sysinfo_get_CPU_utilization"*/
    intptr_t  ( *sysinfo_get_CPU_utilization)(struct J9PortLibrary *portLibrary, struct J9SysinfoCPUTime *cpuTime) ;
	/** see @ref j9sysinfo.c::j9sysinfo_limit_iterator_init "j9sysinfo_limit_iterator_init"*/
    int32_t  ( *sysinfo_limit_iterator_init)(struct J9PortLibrary *portLibrary, J9SysinfoLimitIteratorState *state) ;
	/** see @ref j9sysinfo.c::j9sysinfo_limit_iterator_hasNext "j9sysinfo_limit_iterator_hasNext"*/
    BOOLEAN  ( *sysinfo_limit_iterator_hasNext)(struct J9PortLibrary *portLibrary, J9SysinfoLimitIteratorState *state) ;
	/** see @ref j9sysinfo.c::j9sysinfo_limit_iterator_next "j9sysinfo_limit_iterator_next"*/
    int32_t  ( *sysinfo_limit_iterator_next)(struct J9PortLibrary *portLibrary, J9SysinfoLimitIteratorState *state, J9SysinfoUserLimitElement *limitElement) ;
	/** see @ref j9sysinfo.c::j9sysinfo_env_iterator_init "j9sysinfo_env_iterator_init"*/
    int32_t  ( *sysinfo_env_iterator_init)(struct J9PortLibrary *portLibrary, J9SysinfoEnvIteratorState *state, void *buffer, uintptr_t bufferSizeBytes) ;
	/** see @ref j9sysinfo.c::j9sysinfo_env_iterator_hasNext "j9sysinfo_env_iterator_hasNext"*/
    BOOLEAN  ( *sysinfo_env_iterator_hasNext)(struct J9PortLibrary *portLibrary, J9SysinfoEnvIteratorState *state) ;
	/** see @ref j9sysinfo.c::j9sysinfo_env_iterator_next "j9sysinfo_env_iterator_next"*/
    int32_t  ( *sysinfo_env_iterator_next)(struct J9PortLibrary *portLibrary, J9SysinfoEnvIteratorState *state, J9SysinfoEnvElement *envElement) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_processor_description "j9sysinfo_get_processor_description"*/
    intptr_t  ( *sysinfo_get_processor_description)(struct J9PortLibrary *portLibrary, J9ProcessorDesc *desc) ;
	/** see @ref j9sysinfo.c::j9sysinfo_processor_has_feature "j9sysinfo_processor_has_feature"*/
    BOOLEAN  ( *sysinfo_processor_has_feature)(struct J9PortLibrary *portLibrary, J9ProcessorDesc *desc, uint32_t feature) ;
	/** see @ref j9file.c::j9file_startup "j9file_startup"*/
    int32_t  ( *file_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9file.c::j9file_shutdown "j9file_shutdown"*/
    void  ( *file_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9file.c::j9file_write "j9file_write"*/
    intptr_t  ( *file_write)(struct J9PortLibrary *portLibrary, intptr_t fd, const void *buf, intptr_t nbytes) ;
	/** see @ref j9file.c::j9file_write_text "j9file_write_text"*/
    intptr_t  ( *file_write_text)(struct J9PortLibrary *portLibrary, intptr_t fd, const char *buf, intptr_t nbytes) ;
	/** see @ref j9file.c::j9file_get_text_encoding "j9file_get_text_encoding"*/
    int32_t  ( *file_get_text_encoding)(struct J9PortLibrary *portLibrary, char *charsetName, uintptr_t nbytes) ;
	/** see @ref j9file.c::j9file_vprintf "j9file_vprintf"*/
    void  ( *file_vprintf)(struct J9PortLibrary *portLibrary, intptr_t fd, const char *format, va_list args) ;
	/** see @ref j9file.c::j9file_printf "j9file_printf"*/
    void  ( *file_printf)(struct J9PortLibrary *portLibrary, intptr_t fd, const char *format, ...) ;
	/** see @ref j9file.c::j9file_open "j9file_open"*/
    intptr_t  ( *file_open)(struct J9PortLibrary *portLibrary, const char *path, int32_t flags, int32_t mode) ;
	/** see @ref j9file.c::j9file_close "j9file_close"*/
    int32_t  ( *file_close)(struct J9PortLibrary *portLibrary, intptr_t fd) ;
	/** see @ref j9file.c::j9file_seek "j9file_seek"*/
    int64_t  ( *file_seek)(struct J9PortLibrary *portLibrary, intptr_t fd, int64_t offset, int32_t whence) ;
	/** see @ref j9file.c::j9file_read "j9file_read"*/
    intptr_t  ( *file_read)(struct J9PortLibrary *portLibrary, intptr_t fd, void *buf, intptr_t nbytes) ;
	/** see @ref j9file.c::j9file_unlink "j9file_unlink"*/
    int32_t  ( *file_unlink)(struct J9PortLibrary *portLibrary, const char *path) ;
	/** see @ref j9file.c::j9file_attr "j9file_attr"*/
    int32_t  ( *file_attr)(struct J9PortLibrary *portLibrary, const char *path) ;
	/** see @ref j9file.c::j9file_chmod "j9file_chmod"*/
    int32_t  ( *file_chmod)(struct J9PortLibrary *portLibrary, const char *path,int32_t mode) ;
	/** see @ref j9file.c::j9file_chown "j9file_chown"*/
    intptr_t  ( *file_chown)(struct J9PortLibrary* portLibrary, const char *path, uintptr_t owner, uintptr_t group) ;
	/** see @ref j9file.c::j9file_lastmod "j9file_lastmod"*/
    int64_t  ( *file_lastmod)(struct J9PortLibrary *portLibrary, const char *path) ;
	/** see @ref j9file.c::j9file_length "j9file_length"*/
    int64_t  ( *file_length)(struct J9PortLibrary *portLibrary, const char *path) ;
	/** see @ref j9file.c::j9file_flength "j9file_flength"*/
    int64_t  ( *file_flength)(struct J9PortLibrary *portLibrary, intptr_t fd) ;
	/** see @ref j9file.c::j9file_set_length "j9file_set_length"*/
    int32_t  ( *file_set_length)(struct J9PortLibrary *portLibrary, intptr_t fd, int64_t newLength) ;
	/** see @ref j9file.c::j9file_sync "j9file_sync"*/
    int32_t  ( *file_sync)(struct J9PortLibrary *portLibrary, intptr_t fd) ;
	/** see @ref j9file.c::j9file_fstat "j9file_fstat"*/
    int32_t  ( *file_fstat)(struct J9PortLibrary *portLibrary, intptr_t fd, struct J9FileStat *buf) ;
	/** see @ref j9file.c::j9file_stat "j9file_stat"*/
    int32_t  ( *file_stat)(struct J9PortLibrary *portLibrary, const char *path, uint32_t flags, struct J9FileStat *buf) ;
	/** see @ref j9file.c::j9file_stat_filesystem "j9file_stat_filesystem"*/
    int32_t  ( *file_stat_filesystem)(struct J9PortLibrary *portLibrary, const char *path, uint32_t flags, struct J9FileStatFilesystem *buf) ;
	/** see @ref j9file_blockingasync.c::j9file_blockingasync_open "j9file_blockingasync_open"*/
    intptr_t  ( *file_blockingasync_open)(struct J9PortLibrary *portLibrary, const char *path, int32_t flags, int32_t mode) ;
	/** see @ref j9file_blockingasync.c::j9file_blockingasync_close "j9file_blockingasync_close"*/
    int32_t  ( *file_blockingasync_close)(struct J9PortLibrary *portLibrary, intptr_t fd) ;
	/** see @ref j9file_blockingasync.c::j9file_blockingasync_read "j9file_blockingasync_read"*/
    intptr_t  ( *file_blockingasync_read)(struct J9PortLibrary *portLibrary, intptr_t fd, void *buf, intptr_t nbytes) ;
	/** see @ref j9file_blockingasync.c::j9file_blockingasync_write "j9file_blockingasync_write"*/
    intptr_t  ( *file_blockingasync_write)(struct J9PortLibrary *portLibrary, intptr_t fd, const void *buf, intptr_t nbytes) ;
	/** see @ref j9file_blockingasync.c::j9file_blockingasync_set_length "j9file_blockingasync_set_length"*/
    int32_t  ( *file_blockingasync_set_length)(struct J9PortLibrary *portLibrary, intptr_t fd, int64_t newLength) ;
	/** see @ref j9file_blockingasync.c::j9file_blockingasync_flength "j9file_blockingasync_flength"*/
    int64_t  ( *file_blockingasync_flength)(struct J9PortLibrary *portLibrary, intptr_t fd) ;
	/** see @ref j9file.c::j9file_blockingasync_startup "j9file_blockingasync_startup"*/
    int32_t  ( *file_blockingasync_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9file.c::j9file_blockingasync_shutdown "j9file_blockingasync_shutdown"*/
    void  ( *file_blockingasync_shutdown)(struct J9PortLibrary *portLibrary) ;
    /** see @ref j9sl.c::j9sl_startup "j9sl_startup"*/
    int32_t  ( *sl_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sl.c::j9sl_shutdown "j9sl_shutdown"*/
    void  ( *sl_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sl.c::j9sl_close_shared_library "j9sl_close_shared_library"*/
    uintptr_t  ( *sl_close_shared_library)(struct J9PortLibrary *portLibrary, uintptr_t descriptor) ;
	/** see @ref j9sl.c::j9sl_open_shared_library "j9sl_open_shared_library"*/
    uintptr_t  ( *sl_open_shared_library)(struct J9PortLibrary *portLibrary, char *name, uintptr_t *descriptor, uintptr_t flags) ;
	/** see @ref j9sl.c::j9sl_lookup_name "j9sl_lookup_name"*/
    uintptr_t  ( *sl_lookup_name)(struct J9PortLibrary *portLibrary, uintptr_t descriptor, char *name, uintptr_t *func, const char *argSignature) ;
	/** see @ref j9tty.c::j9tty_startup "j9tty_startup"*/
    int32_t  ( *tty_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9tty.c::j9tty_shutdown "j9tty_shutdown"*/
    void  ( *tty_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9tty.c::j9tty_printf "j9tty_printf"*/
    void  ( *tty_printf)(struct J9PortLibrary *portLibrary, const char *format, ...) ;
	/** see @ref j9tty.c::j9tty_vprintf "j9tty_vprintf"*/
    void  ( *tty_vprintf)(struct J9PortLibrary *portLibrary, const char *format, va_list args) ;
	/** see @ref j9tty.c::j9tty_get_chars "j9tty_get_chars"*/
    intptr_t  ( *tty_get_chars)(struct J9PortLibrary *portLibrary, char *s, uintptr_t length) ;
	/** see @ref j9tty.c::j9tty_err_printf "j9tty_err_printf"*/
    void  ( *tty_err_printf)(struct J9PortLibrary *portLibrary, const char *format, ...) ;
	/** see @ref j9tty.c::j9tty_err_vprintf "j9tty_err_vprintf"*/
    void  ( *tty_err_vprintf)(struct J9PortLibrary *portLibrary, const char *format, va_list args) ;
	/** see @ref j9tty.c::j9tty_available "j9tty_available"*/
    intptr_t  ( *tty_available)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9tty.c::j9tty_daemonize "j9tty_daemonize"*/
    void  ( *tty_daemonize)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9heap.c::j9heap_create "j9heap_create"*/
    J9Heap*  ( *heap_create)(struct J9PortLibrary *portLibrary, void* heapBase, uintptr_t heapSize, uint32_t heapFlags) ;
	/** see @ref j9heap.c::j9heap_allocate "j9heap_allocate"*/
    void*  ( *heap_allocate)(struct J9PortLibrary *portLibrary, struct J9Heap* heap, uintptr_t byteAmount) ;
	/** see @ref j9heap.c::j9heap_free "j9heap_free"*/
    void  ( *heap_free)(struct J9PortLibrary *portLibrary, struct J9Heap* heap, void *address) ;
	/** see @ref j9heap.c::j9heap_reallocate "j9heap_reallocate"*/
    void*  ( *heap_reallocate)(struct J9PortLibrary *portLibrary, struct J9Heap* heap, void *address, uintptr_t byteAmount) ;
	/** see @ref j9mem.c::j9mem_startup "j9mem_startup"*/
    int32_t  ( *mem_startup)(struct J9PortLibrary *portLibrary, uintptr_t portGlobalSize) ;
	/** see @ref j9mem.c::j9mem_shutdown "j9mem_shutdown"*/
    void  ( *mem_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9mem.c::j9mem_allocate_memory "j9mem_allocate_memory"*/
    void*  ( *mem_allocate_memory)(struct J9PortLibrary *portLibrary, uintptr_t byteAmount, const char *callSite, uint32_t category) ;
	/** see @ref j9mem.c::j9mem_free_memory "j9mem_free_memory"*/
    void  ( *mem_free_memory)(struct J9PortLibrary *portLibrary, void *memoryPointer) ;
	/** see @ref j9mem.c::j9mem_advise_and_free_memory "j9mem_advise_and_free_memory"*/
    void  ( *mem_advise_and_free_memory)(struct J9PortLibrary *portLibrary, void *memoryPointer) ;
	/** see @ref j9mem.c::j9mem_reallocate_memory "j9mem_reallocate_memory"*/
    void*  ( *mem_reallocate_memory)(struct J9PortLibrary *portLibrary, void *memoryPointer, uintptr_t byteAmount, const char * callSite, uint32_t category) ;
	/** see @ref j9mem.c::j9mem_allocate_memory32 "j9mem_allocate_memory32"*/
    void*  ( *mem_allocate_memory32)(struct J9PortLibrary *portLibrary, uintptr_t byteAmount, const char *callSite,  uint32_t category) ;
	/** see @ref j9mem.c::j9mem_free_memory32 "j9mem_free_memory32"*/
    void  ( *mem_free_memory32)(struct J9PortLibrary *portLibrary, void *memoryPointer) ;
	/** see @ref j9mem.c::j9mem_ensure_capacity32 "j9mem_ensure_capacity32"*/
    uintptr_t  ( *mem_ensure_capacity32)(struct J9PortLibrary *portLibrary, uintptr_t byteAmount) ;
	/** see @ref j9cpu.c::j9cpu_startup "j9cpu_startup"*/
    int32_t  ( *cpu_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9cpu.c::j9cpu_shutdown "j9cpu_shutdown"*/
    void  ( *cpu_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9cpu.c::j9cpu_flush_icache "j9cpu_flush_icache"*/
    void  ( *cpu_flush_icache)(struct J9PortLibrary *portLibrary, void *memoryPointer, uintptr_t byteAmount) ;
	/** see @ref j9vmem.c::j9vmem_startup "j9vmem_startup"*/
    int32_t  ( *vmem_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9vmem.c::j9vmem_shutdown "j9vmem_shutdown"*/
    void  ( *vmem_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9vmem.c::j9vmem_commit_memory "j9vmem_commit_memory"*/
    void*  ( *vmem_commit_memory)(struct J9PortLibrary *portLibrary, void *address, uintptr_t byteAmount, struct J9PortVmemIdentifier *identifier) ;
	/** see @ref j9vmem.c::j9vmem_decommit_memory "j9vmem_decommit_memory"*/
    intptr_t  ( *vmem_decommit_memory)(struct J9PortLibrary *portLibrary, void *address, uintptr_t byteAmount, struct J9PortVmemIdentifier *identifier) ;
	/** see @ref j9vmem.c::j9vmem_free_memory "j9vmem_free_memory"*/
    int32_t  ( *vmem_free_memory)(struct J9PortLibrary *portLibrary, void *userAddress, uintptr_t byteAmount, struct J9PortVmemIdentifier *identifier) ;
	/** see @ref j9vmem.c::j9vmem_vmem_params_init "j9vmem_vmem_params_init"*/
    int32_t  ( *vmem_vmem_params_init)(struct J9PortLibrary *portLibrary, struct J9PortVmemParams *params) ;
	/** see @ref j9vmem.c::j9vmem_reserve_memory "j9vmem_reserve_memory"*/
    void*  ( *vmem_reserve_memory)(struct J9PortLibrary *portLibrary, void *address, uintptr_t byteAmount, struct J9PortVmemIdentifier *identifier, uintptr_t mode, uintptr_t pageSize,  uint32_t category) ;
	/** see @ref j9vmem.c::j9vmem_reserve_memory_ex "j9vmem_reserve_memory_ex"*/
    void*  ( *vmem_reserve_memory_ex)(struct J9PortLibrary *portLibrary, struct J9PortVmemIdentifier *identifier, struct J9PortVmemParams *params) ;
	/** see @ref j9vmem.c::j9vmem_get_page_size "j9vmem_get_page_size"*/
    uintptr_t  ( *vmem_get_page_size)(struct J9PortLibrary *portLibrary, struct J9PortVmemIdentifier *identifier) ;
	/** see @ref j9vmem.c::j9vmem_get_page_flags "j9vmem_get_page_flags"*/
    uintptr_t  ( *vmem_get_page_flags)(struct J9PortLibrary *portLibrary, struct J9PortVmemIdentifier *identifier) ;
	/** see @ref j9vmem.c::j9vmem_supported_page_sizes "j9vmem_supported_page_sizes"*/
    uintptr_t*  ( *vmem_supported_page_sizes)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9vmem.c::j9vmem_supported_page_flags "j9vmem_supported_page_flags"*/
    uintptr_t*  ( *vmem_supported_page_flags)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9vmem.c::j9vmem_default_large_page_size_ex "j9vmem_default_large_page_size_ex"*/
    void  ( *vmem_default_large_page_size_ex)(struct J9PortLibrary *portLibrary, uintptr_t mode, uintptr_t *pageSize, uintptr_t *pageFlags) ;
	/** see @ref j9vmem.c::j9vmem_find_valid_page_size "j9vmem_find_valid_page_size"*/
    intptr_t  ( *vmem_find_valid_page_size)(struct J9PortLibrary *portLibrary, uintptr_t mode, uintptr_t *pageSize, uintptr_t *pageFlags, BOOLEAN *isSizeSupported) ;
	/** see @ref j9vmem.c::j9vmem_numa_set_affinity "j9vmem_numa_set_affinity"*/
    intptr_t  ( *vmem_numa_set_affinity)(struct J9PortLibrary *portLibrary, uintptr_t numaNode, void *address, uintptr_t byteAmount, struct J9PortVmemIdentifier *identifier) ;
	/** see @ref j9vmem.c::j9vmem_numa_get_node_details "j9vmem_numa_get_node_details"*/
    intptr_t  ( *vmem_numa_get_node_details)(struct J9PortLibrary *portLibrary, struct J9MemoryNodeDetail *numaNodes, uintptr_t *nodeCount) ;
	/** see @ref j9sock.c::j9sock_startup "j9sock_startup"*/
    int32_t  ( *sock_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sock.c::j9sock_shutdown "j9sock_shutdown"*/
    int32_t  ( *sock_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sock.c::j9sock_htons "j9sock_htons"*/
    uint16_t  ( *sock_htons)(struct J9PortLibrary *portLibrary, uint16_t val) ;
	/** see @ref j9sock.c::j9sock_write "j9sock_write"*/
    int32_t  ( *sock_write)(struct J9PortLibrary *portLibrary, j9socket_t sock, uint8_t *buf, int32_t nbyte, int32_t flags) ;
	/** see @ref j9sock.c::j9sock_sockaddr "j9sock_sockaddr"*/
    int32_t  ( *sock_sockaddr)(struct J9PortLibrary *portLibrary, j9sockaddr_t handle, const char *addrStr, uint16_t port) ;
	/** see @ref j9sock.c::j9sock_read "j9sock_read"*/
    int32_t  ( *sock_read)(struct J9PortLibrary *portLibrary, j9socket_t sock, uint8_t *buf, int32_t nbyte, int32_t flags) ;
	/** see @ref j9sock.c::j9sock_socket "j9sock_socket"*/
    int32_t  ( *sock_socket)(struct J9PortLibrary *portLibrary, j9socket_t *handle, int32_t family, int32_t socktype,  int32_t protocol) ;
	/** see @ref j9sock.c::j9sock_close "j9sock_close"*/
    int32_t  ( *sock_close)(struct J9PortLibrary *portLibrary, j9socket_t *sock) ;
	/** see @ref j9sock.c::j9sock_connect "j9sock_connect"*/
    int32_t  ( *sock_connect)(struct J9PortLibrary *portLibrary, j9socket_t sock, j9sockaddr_t addr) ;
	/** see @ref j9sock.c::j9sock_inetaddr "j9sock_inetaddr"*/
    int32_t  ( *sock_inetaddr)(struct J9PortLibrary *portLibrary, const char *addrStr, uint32_t *addr) ;
	/** see @ref j9sock.c::j9sock_gethostbyname "j9sock_gethostbyname"*/
    int32_t  ( *sock_gethostbyname)(struct J9PortLibrary *portLibrary, const char *name, j9hostent_t handle) ;
	/** see @ref j9sock.c::j9sock_hostent_addrlist "j9sock_hostent_addrlist"*/
    int32_t  ( *sock_hostent_addrlist)(struct J9PortLibrary *portLibrary, j9hostent_t handle, uint32_t index) ;
	/** see @ref j9sock.c::j9sock_sockaddr_init "j9sock_sockaddr_init"*/
    int32_t  ( *sock_sockaddr_init)(struct J9PortLibrary *portLibrary, j9sockaddr_t handle, int16_t family, uint32_t ipAddrNetworkOrder, uint16_t portNetworkOrder) ;
	/** see @ref j9sock.c::j9sock_linger_init "j9sock_linger_init"*/
    int32_t  ( *sock_linger_init)(struct J9PortLibrary *portLibrary, j9linger_t handle, int32_t enabled, uint16_t timeout) ;
	/** see @ref j9sock.c::j9sock_setopt_linger "j9sock_setopt_linger"*/
    int32_t  ( *sock_setopt_linger)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  j9linger_t optval) ;
	/** see @ref j9gp.c::j9gp_startup "j9gp_startup"*/
    int32_t  ( *gp_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9gp.c::j9gp_shutdown "j9gp_shutdown"*/
    void  ( *gp_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9gp.c::j9gp_protect "j9gp_protect"*/
    uintptr_t  ( *gp_protect)(struct J9PortLibrary *portLibrary,  protected_fn fn, void* arg ) ;
	/** see @ref j9gp.c::j9gp_register_handler "j9gp_register_handler"*/
    void  ( *gp_register_handler)(struct J9PortLibrary *portLibrary, handler_fn fn, void *aUserData ) ;
	/** see @ref j9gp.c::j9gp_info "j9gp_info"*/
    uint32_t  ( *gp_info)(struct J9PortLibrary *portLibrary, void *info, uint32_t category, int32_t index, const char **name, void **value) ;
	/** see @ref j9gp.c::j9gp_info_count "j9gp_info_count"*/
    uint32_t  ( *gp_info_count)(struct J9PortLibrary *portLibrary, void *info, uint32_t category) ;
	/** see @ref j9gp.c::j9gp_handler_function "j9gp_handler_function"*/
    void  ( *gp_handler_function)(void *unknown) ;
	/** see @ref j9str.c::j9str_startup "j9str_startup"*/
    int32_t  ( *str_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9str.c::j9str_shutdown "j9str_shutdown"*/
    void  ( *str_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9str.c::j9str_printf "j9str_printf"*/
    uintptr_t  ( *str_printf)(struct J9PortLibrary *portLibrary, char *buf, uintptr_t bufLen, const char* format, ...) ;
	/** see @ref j9str.c::j9str_vprintf "j9str_vprintf"*/
    uintptr_t  ( *str_vprintf)(struct J9PortLibrary *portLibrary, char *buf, uintptr_t bufLen, const char *format, va_list args) ;
	/** see @ref j9str.c::j9str_create_tokens "j9str_create_tokens"*/
    J9StringTokens*  ( *str_create_tokens)(struct J9PortLibrary* portLibrary, int64_t timeMillis) ;
	/** see @ref j9str.c::j9str_set_token "j9str_set_token"*/
    intptr_t  ( *str_set_token)(struct J9PortLibrary* portLibrary, struct J9StringTokens* tokens, const char* key, const char* format, ...) ;
	/** see @ref j9str.c::j9str_subst_tokens "j9str_subst_tokens"*/
    uintptr_t  ( *str_subst_tokens)(struct J9PortLibrary *portLibrary, char *buf, uintptr_t bufLen, const char *format, struct J9StringTokens* tokens) ;
	/** see @ref j9str.c::j9str_free_tokens "j9str_free_tokens"*/
    void  ( *str_free_tokens)(struct J9PortLibrary* portLibrary, struct J9StringTokens* tokens) ;
	/** see @ref j9str.c::j9str_set_time_tokens "j9str_set_time_tokens"*/
    intptr_t  ( *str_set_time_tokens)(struct J9PortLibrary* portLibrary, struct J9StringTokens* tokens, int64_t timeMillis) ;
	/** see @ref j9str.c::j9str_convert "j9str_convert"*/
    int32_t  ( *str_convert)(struct J9PortLibrary *portLibrary, int32_t fromCode, int32_t toCode, const char *inBuffer, uintptr_t inBufferSize, char *outBuffer, uintptr_t outBufferSize) ;
	/** see @ref j9exit.c::j9exit_startup "j9exit_startup"*/
    int32_t  ( *exit_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9exit.c::j9exit_shutdown "j9exit_shutdown"*/
    void  ( *exit_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9exit.c::j9exit_get_exit_code "j9exit_get_exit_code"*/
    int32_t  ( *exit_get_exit_code)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9exit.c::j9exit_shutdown_and_exit "j9exit_shutdown_and_exit"*/
    void  ( *exit_shutdown_and_exit)(struct J9PortLibrary *portLibrary, int32_t exitCode) ;
	/** self_handle*/
    void* self_handle;
	/** see @ref j9dump.c::j9dump_create "j9dump_create"*/
    uintptr_t  ( *dump_create)(struct J9PortLibrary *portLibrary, char *filename, char *dumpType, void *userData) ;
	/** see @ref j9dump.c::j9dump_startup "j9dump_startup"*/
    int32_t  ( *dump_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9dump.c::j9dump_shutdown "j9dump_shutdown"*/
    void  ( *dump_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9nls.c::j9nls_startup "j9nls_startup"*/
    int32_t  ( *nls_startup)(struct J9PortLibrary *portLibrary ) ;
	/** see @ref j9nls.c::j9nls_free_cached_data "j9nls_free_cached_data"*/
    void  ( *nls_free_cached_data)( struct J9PortLibrary *portLibrary ) ;
	/** see @ref j9nls.c::j9nls_shutdown "j9nls_shutdown"*/
    void  ( *nls_shutdown)( struct J9PortLibrary *portLibrary ) ;
	/** see @ref j9nls.c::j9nls_set_catalog "j9nls_set_catalog"*/
    void  ( *nls_set_catalog)( struct J9PortLibrary *portLibrary, const char **paths, const int nPaths, const char* baseName, const char* extension ) ;
	/** see @ref j9nls.c::j9nls_set_locale "j9nls_set_locale"*/
    void  ( *nls_set_locale)( struct J9PortLibrary *portLibrary, const char* lang, const char* region, const char* variant ) ;
	/** see @ref j9nls.c::j9nls_get_language "j9nls_get_language"*/
    const char*  ( *nls_get_language)( struct J9PortLibrary *portLibrary ) ;
	/** see @ref j9nls.c::j9nls_get_region "j9nls_get_region"*/
    const char*  ( *nls_get_region)( struct J9PortLibrary *portLibrary ) ;
	/** see @ref j9nls.c::j9nls_get_variant "j9nls_get_variant"*/
    const char*  ( *nls_get_variant)( struct J9PortLibrary *portLibrary ) ;
	/** see @ref j9nls.c::j9nls_printf "j9nls_printf"*/
    void  ( *nls_printf)( struct J9PortLibrary *portLibrary, uintptr_t flags, uint32_t module_name, uint32_t message_num, ... ) ;
	/** see @ref j9nls.c::j9nls_vprintf "j9nls_vprintf"*/
    void  ( *nls_vprintf)( struct J9PortLibrary *portLibrary, uintptr_t flags, uint32_t module_name, uint32_t message_num, va_list args ) ;
	/** see @ref j9nls.c::j9nls_lookup_message "j9nls_lookup_message"*/
    const char*  ( *nls_lookup_message)( struct J9PortLibrary *portLibrary, uintptr_t flags, uint32_t module_name, uint32_t message_num, const char *default_string ) ;
	/** see @ref j9ipcmutex.c::j9ipcmutex_startup "j9ipcmutex_startup"*/
    int32_t  ( *ipcmutex_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9ipcmutex.c::j9ipcmutex_shutdown "j9ipcmutex_shutdown"*/
    void  ( *ipcmutex_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9ipcmutex.c::j9ipcmutex_acquire "j9ipcmutex_acquire"*/
    int32_t  ( *ipcmutex_acquire)(struct J9PortLibrary *portLibrary, const char *name) ;
	/** see @ref j9ipcmutex.c::j9ipcmutex_release "j9ipcmutex_release"*/
    int32_t  ( *ipcmutex_release)(struct J9PortLibrary *portLibrary, const char *name) ;
	/** see @ref j9j9portcontrol.c::j9port_control "j9port_control"*/
    int32_t  ( *port_control)( struct J9PortLibrary *portLibrary, const char *key, uintptr_t value) ;
	/** see @ref j9sig.c::j9sig_startup "j9sig_startup"*/
    int32_t  ( *sig_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sig.c::j9sig_shutdown "j9sig_shutdown"*/
    void  ( *sig_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sig.c::j9sig_protect "j9sig_protect"*/
    int32_t  ( *sig_protect)(struct J9PortLibrary *portLibrary,  j9sig_protected_fn fn, void* fn_arg, j9sig_handler_fn handler, void* handler_arg, uint32_t flags, uintptr_t *result ) ;
	/** see @ref j9sig.c::j9sig_can_protect "j9sig_can_protect"*/
    int32_t  ( *sig_can_protect)(struct J9PortLibrary *portLibrary,  uint32_t flags) ;
	/** see @ref j9sig.c::j9sig_set_async_signal_handler "j9sig_set_async_signal_handler"*/
    uint32_t  ( *sig_set_async_signal_handler)(struct J9PortLibrary* portLibrary,  j9sig_handler_fn handler, void* handler_arg, uint32_t flags) ;
	/** see @ref j9sig.c::j9sig_info "j9sig_info"*/
    uint32_t  ( *sig_info)(struct J9PortLibrary *portLibrary, void *info, uint32_t category, int32_t index, const char **name, void **value) ;
	/** see @ref j9sig.c::j9sig_info_count "j9sig_info_count"*/
    uint32_t  ( *sig_info_count)(struct J9PortLibrary *portLibrary, void *info, uint32_t category) ;
	/** see @ref j9sig.c::j9sig_set_options "j9sig_set_options"*/
    int32_t  ( *sig_set_options)(struct J9PortLibrary *portLibrary, uint32_t options) ;
	/** see @ref j9sig.c::j9sig_get_options "j9sig_get_options"*/
    uint32_t  ( *sig_get_options)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sig.c::j9sig_get_current_signal "j9sig_get_current_signal"*/
    intptr_t  ( *sig_get_current_signal)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sig.c::j9sig_set_reporter_priority "j9sig_set_reporter_priority"*/
    int32_t  ( *sig_set_reporter_priority)(struct J9PortLibrary *portLibrary, uintptr_t priority) ;
	/** see @ref j9sysinfo.c::j9sysinfo_DLPAR_enabled "j9sysinfo_DLPAR_enabled"*/
    uintptr_t  ( *sysinfo_DLPAR_enabled)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_DLPAR_max_CPUs "j9sysinfo_DLPAR_max_CPUs"*/
    uintptr_t  ( *sysinfo_DLPAR_max_CPUs)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_weak_memory_consistency "j9sysinfo_weak_memory_consistency"*/
    uintptr_t  ( *sysinfo_weak_memory_consistency)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9file.c::j9file_read_text "j9file_read_text"*/
    char*  ( *file_read_text)(struct J9PortLibrary *portLibrary, intptr_t fd, char *buf, intptr_t nbytes) ;
	/** see @ref j9file.c::j9file_mkdir "j9file_mkdir"*/
    int32_t  ( *file_mkdir)(struct J9PortLibrary *portLibrary, const char *path) ;
	/** see @ref j9file.c::j9file_move "j9file_move"*/
    int32_t  ( *file_move)(struct J9PortLibrary *portLibrary, const char *pathExist, const char *pathNew) ;
	/** see @ref j9file.c::j9file_unlinkdir "j9file_unlinkdir"*/
    int32_t  ( *file_unlinkdir)(struct J9PortLibrary *portLibrary, const char *path) ;
	/** see @ref j9file.c::j9file_findfirst "j9file_findfirst"*/
    uintptr_t  ( *file_findfirst)(struct J9PortLibrary *portLibrary, const char *path, char *resultbuf) ;
	/** see @ref j9file.c::j9file_findnext "j9file_findnext"*/
    int32_t  ( *file_findnext)(struct J9PortLibrary *portLibrary, uintptr_t findhandle, char *resultbuf) ;
	/** see @ref j9file.c::j9file_findclose "j9file_findclose"*/
    void  ( *file_findclose)(struct J9PortLibrary *portLibrary, uintptr_t findhandle) ;
	/** see @ref j9file.c::j9file_error_message "j9file_error_message"*/
    const char*  ( *file_error_message)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9file.c::j9file_unlock_bytes "j9file_unlock_bytes"*/
    int32_t  ( *file_unlock_bytes)(struct J9PortLibrary *portLibrary, intptr_t fd, uint64_t offset, uint64_t length) ;
	/** see @ref j9file.c::j9file_lock_bytes "j9file_lock_bytes"*/
    int32_t  ( *file_lock_bytes)(struct J9PortLibrary *portLibrary, intptr_t fd, int32_t lockFlags, uint64_t offset, uint64_t length) ;
	/** see @ref j9file.c::j9file_convert_native_fd_to_j9file_fd "j9file_convert_native_fd_to_j9file_fd"*/
    intptr_t  ( *file_convert_native_fd_to_j9file_fd)(struct J9PortLibrary *portLibrary, intptr_t nativeFD) ;
	/** see @ref j9file_blockingasync.c::j9file_blockingasync_unlock_bytes "j9file_blockingasync_unlock_bytes"*/
    int32_t  ( *file_blockingasync_unlock_bytes)(struct J9PortLibrary *portLibrary, intptr_t fd, uint64_t offset, uint64_t length) ;
	/** see @ref j9file_blockingasync.c::j9file_blockingasync_lock_bytes "j9file_blockingasync_lock_bytes"*/
    int32_t  ( *file_blockingasync_lock_bytes)(struct J9PortLibrary *portLibrary, intptr_t fd, int32_t lockFlags, uint64_t offset, uint64_t length) ;
	/** see @ref j9sock.c::j9sock_htonl "j9sock_htonl"*/
    int32_t  ( *sock_htonl)(struct J9PortLibrary *portLibrary, int32_t val) ;
	/** see @ref j9sock.c::j9sock_bind "j9sock_bind"*/
    int32_t  ( *sock_bind)(struct J9PortLibrary *portLibrary, j9socket_t sock, j9sockaddr_t addr) ;
	/** see @ref j9sock.c::j9sock_accept "j9sock_accept"*/
    int32_t  ( *sock_accept)(struct J9PortLibrary *portLibrary, j9socket_t serverSock, j9sockaddr_t addrHandle, j9socket_t *sockHandle) ;
	/** see @ref j9sock.c::j9sock_shutdown_input "j9sock_shutdown_input"*/
    int32_t  ( *sock_shutdown_input)(struct J9PortLibrary *portLibrary, j9socket_t sock) ;
	/** see @ref j9sock.c::j9sock_shutdown_output "j9sock_shutdown_output"*/
    int32_t  ( *sock_shutdown_output)(struct J9PortLibrary *portLibrary, j9socket_t sock) ;
	/** see @ref j9sock.c::j9sock_listen "j9sock_listen"*/
    int32_t  ( *sock_listen)(struct J9PortLibrary *portLibrary, j9socket_t sock, int32_t backlog ) ;
	/** see @ref j9sock.c::j9sock_ntohl "j9sock_ntohl"*/
    int32_t  ( *sock_ntohl)(struct J9PortLibrary *portLibrary, int32_t val) ;
	/** see @ref j9sock.c::j9sock_ntohs "j9sock_ntohs"*/
    uint16_t  ( *sock_ntohs)(struct J9PortLibrary *portLibrary, uint16_t val) ;
	/** see @ref j9sock.c::j9sock_getpeername "j9sock_getpeername"*/
    int32_t  ( *sock_getpeername)(struct J9PortLibrary *portLibrary, j9socket_t handle, j9sockaddr_t addrHandle) ;
	/** see @ref j9sock.c::j9sock_getsockname "j9sock_getsockname"*/
    int32_t  ( *sock_getsockname)(struct J9PortLibrary *portLibrary, j9socket_t handle, j9sockaddr_t addrHandle) ;
	/** see @ref j9sock.c::j9sock_readfrom "j9sock_readfrom"*/
    int32_t  ( *sock_readfrom)(struct J9PortLibrary *portLibrary, j9socket_t sock, uint8_t *buf, int32_t nbyte, int32_t flags, j9sockaddr_t addrHandle) ;
	/** see @ref j9sock.c::j9sock_select "j9sock_select"*/
    int32_t  ( *sock_select)(struct J9PortLibrary *portLibrary, int32_t nfds, j9fdset_t readfds, j9fdset_t writefds, j9fdset_t exceptfds, j9timeval_t timeout) ;
	/** see @ref j9sock.c::j9sock_writeto "j9sock_writeto"*/
    int32_t  ( *sock_writeto)(struct J9PortLibrary *portLibrary, j9socket_t sock, uint8_t *buf, int32_t nbyte, int32_t flags, j9sockaddr_t addrHandle) ;
	/** see @ref j9sock.c::j9sock_inetntoa "j9sock_inetntoa"*/
    int32_t  ( *sock_inetntoa)(struct J9PortLibrary *portLibrary, char **addrStr, uint32_t nipAddr) ;
	/** see @ref j9sock.c::j9sock_gethostbyaddr "j9sock_gethostbyaddr"*/
    int32_t  ( *sock_gethostbyaddr)(struct J9PortLibrary *portLibrary, char *addr, int32_t length, int32_t type, j9hostent_t handle) ;
	/** see @ref j9sock.c::j9sock_gethostname "j9sock_gethostname"*/
    int32_t  ( *sock_gethostname)(struct J9PortLibrary *portLibrary, char *buffer, int32_t length) ;
	/** see @ref j9sock.c::j9sock_hostent_aliaslist "j9sock_hostent_aliaslist"*/
    int32_t  ( *sock_hostent_aliaslist)(struct J9PortLibrary *portLibrary, j9hostent_t handle, char ***aliasList) ;
	/** see @ref j9sock.c::j9sock_hostent_hostname "j9sock_hostent_hostname"*/
    int32_t  ( *sock_hostent_hostname)(struct J9PortLibrary *portLibrary, j9hostent_t handle, char** hostName) ;
	/** see @ref j9sock.c::j9sock_sockaddr_port "j9sock_sockaddr_port"*/
    uint16_t  ( *sock_sockaddr_port)(struct J9PortLibrary *portLibrary, j9sockaddr_t handle) ;
	/** see @ref j9sock.c::j9sock_sockaddr_address "j9sock_sockaddr_address"*/
    int32_t  ( *sock_sockaddr_address)(struct J9PortLibrary *portLibrary, j9sockaddr_t handle) ;
	/** see @ref j9sock.c::j9sock_fdset_init "j9sock_fdset_init"*/
    int32_t  ( *sock_fdset_init)(struct J9PortLibrary *portLibrary, j9socket_t socketP) ;
	/** see @ref j9sock.c::j9sock_fdset_size "j9sock_fdset_size"*/
    int32_t  ( *sock_fdset_size)(struct J9PortLibrary *portLibrary, j9socket_t handle) ;
	/** see @ref j9sock.c::j9sock_timeval_init "j9sock_timeval_init"*/
    int32_t  ( *sock_timeval_init)(struct J9PortLibrary *portLibrary, uint32_t secTime, uint32_t uSecTime, j9timeval_t timeP) ;
	/** see @ref j9sock.c::j9sock_getopt_int "j9sock_getopt_int"*/
    int32_t  ( *sock_getopt_int)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  int32_t *optval) ;
	/** see @ref j9sock.c::j9sock_setopt_int "j9sock_setopt_int"*/
    int32_t  ( *sock_setopt_int)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  int32_t *optval) ;
	/** see @ref j9sock.c::j9sock_getopt_bool "j9sock_getopt_bool"*/
    int32_t  ( *sock_getopt_bool)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  BOOLEAN *optval) ;
	/** see @ref j9sock.c::j9sock_setopt_bool "j9sock_setopt_bool"*/
    int32_t  ( *sock_setopt_bool)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  BOOLEAN *optval) ;
	/** see @ref j9sock.c::j9sock_getopt_byte "j9sock_getopt_byte"*/
    int32_t  ( *sock_getopt_byte)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  uint8_t *optval) ;
	/** see @ref j9sock.c::j9sock_setopt_byte "j9sock_setopt_byte"*/
    int32_t  ( *sock_setopt_byte)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  uint8_t *optval) ;
	/** see @ref j9sock.c::j9sock_getopt_linger "j9sock_getopt_linger"*/
    int32_t  ( *sock_getopt_linger)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  j9linger_t optval) ;
	/** see @ref j9sock.c::j9sock_getopt_sockaddr "j9sock_getopt_sockaddr"*/
    int32_t  ( *sock_getopt_sockaddr)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname, j9sockaddr_t optval) ;
	/** see @ref j9sock.c::j9sock_setopt_sockaddr "j9sock_setopt_sockaddr"*/
    int32_t  ( *sock_setopt_sockaddr)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  j9sockaddr_t optval) ;
	/** see @ref j9sock.c::j9sock_setopt_ipmreq "j9sock_setopt_ipmreq"*/
    int32_t  ( *sock_setopt_ipmreq)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  j9ipmreq_t optval) ;
	/** see @ref j9sock.c::j9sock_linger_enabled "j9sock_linger_enabled"*/
    int32_t  ( *sock_linger_enabled)(struct J9PortLibrary *portLibrary, j9linger_t handle, BOOLEAN *enabled) ;
	/** see @ref j9sock.c::j9sock_linger_linger "j9sock_linger_linger"*/
    int32_t  ( *sock_linger_linger)(struct J9PortLibrary *portLibrary, j9linger_t handle, uint16_t *linger) ;
	/** see @ref j9sock.c::j9sock_ipmreq_init "j9sock_ipmreq_init"*/
    int32_t  ( *sock_ipmreq_init)(struct J9PortLibrary *portLibrary, j9ipmreq_t handle, uint32_t nipmcast, uint32_t nipinterface) ;
	/** see @ref j9sock.c::j9sock_setflag "j9sock_setflag"*/
    int32_t  ( *sock_setflag)(struct J9PortLibrary *portLibrary, int32_t flag, int32_t *arg) ;
	/** see @ref j9sock.c::j9sock_freeaddrinfo "j9sock_freeaddrinfo"*/
    int32_t  ( *sock_freeaddrinfo)(struct J9PortLibrary *portLibrary, j9addrinfo_t handle) ;
	/** see @ref j9sock.c::j9sock_getaddrinfo "j9sock_getaddrinfo"*/
    int32_t  ( *sock_getaddrinfo)(struct J9PortLibrary *portLibrary, char *name, j9addrinfo_t hints, j9addrinfo_t result) ;
	/** see @ref j9sock.c::j9sock_getaddrinfo_address "j9sock_getaddrinfo_address"*/
    int32_t  ( *sock_getaddrinfo_address)(struct J9PortLibrary *portLibrary, j9addrinfo_t handle, uint8_t *address, int index, uint32_t* scope_id) ;
	/** see @ref j9sock.c::j9sock_getaddrinfo_create_hints "j9sock_getaddrinfo_create_hints"*/
    int32_t  ( *sock_getaddrinfo_create_hints)(struct J9PortLibrary *portLibrary, j9addrinfo_t *result, int16_t family, int32_t socktype, int32_t protocol, int32_t flags) ;
	/** see @ref j9sock.c::j9sock_getaddrinfo_family "j9sock_getaddrinfo_family"*/
    int32_t  ( *sock_getaddrinfo_family)(struct J9PortLibrary *portLibrary, j9addrinfo_t handle, int32_t *family, int index) ;
	/** see @ref j9sock.c::j9sock_getaddrinfo_length "j9sock_getaddrinfo_length"*/
    int32_t  ( *sock_getaddrinfo_length)(struct J9PortLibrary *portLibrary, j9addrinfo_t handle, int32_t *length) ;
	/** see @ref j9sock.c::j9sock_getaddrinfo_name "j9sock_getaddrinfo_name"*/
    int32_t  ( *sock_getaddrinfo_name)(struct J9PortLibrary *portLibrary, j9addrinfo_t handle, char *name, int index) ;
	/** see @ref j9sock.c::j9sock_getnameinfo "j9sock_getnameinfo"*/
    int32_t  ( *sock_getnameinfo)(struct J9PortLibrary *portLibrary, j9sockaddr_t in_addr, int32_t sockaddr_size, char *name, int32_t name_length, int flags) ;
	/** see @ref j9sock.c::j9sock_ipv6_mreq_init "j9sock_ipv6_mreq_init"*/
    int32_t  ( *sock_ipv6_mreq_init)(struct J9PortLibrary *portLibrary, j9ipv6_mreq_t handle, uint8_t *ipmcast_addr, uint32_t ipv6mr_interface) ;
	/** see @ref j9sock.c::j9sock_setopt_ipv6_mreq "j9sock_setopt_ipv6_mreq"*/
    int32_t  ( *sock_setopt_ipv6_mreq)(struct J9PortLibrary *portLibrary, j9socket_t socketP, int32_t optlevel, int32_t optname,  j9ipv6_mreq_t optval) ;
	/** see @ref j9sock.c::j9sock_sockaddr_address6 "j9sock_sockaddr_address6"*/
    int32_t  ( *sock_sockaddr_address6)(struct J9PortLibrary *portLibrary, j9sockaddr_t handle, uint8_t *address, uint32_t *length, uint32_t* scope_id) ;
	/** see @ref j9sock.c::j9sock_sockaddr_family "j9sock_sockaddr_family"*/
    int32_t  ( *sock_sockaddr_family)(struct J9PortLibrary *portLibrary, int16_t *family, j9sockaddr_t handle) ;
	/** see @ref j9sock.c::j9sock_sockaddr_init6 "j9sock_sockaddr_init6"*/
    int32_t  ( *sock_sockaddr_init6)(struct J9PortLibrary *portLibrary, j9sockaddr_t handle, uint8_t *addr, int32_t addrlength, int16_t family, uint16_t portNetworkOrder, uint32_t flowinfo, uint32_t scope_id, j9socket_t sock) ;
	/** see @ref j9sock.c::j9sock_socketIsValid "j9sock_socketIsValid"*/
    int32_t  ( *sock_socketIsValid)(struct J9PortLibrary *portLibrary, j9socket_t handle) ;
	/** see @ref j9sock.c::j9sock_select_read "j9sock_select_read"*/
    int32_t  ( *sock_select_read)(struct J9PortLibrary *portLibrary, j9socket_t j9socketP, int32_t secTime, int32_t uSecTime, BOOLEAN accept) ;
	/** see @ref j9sock.c::j9sock_set_nonblocking "j9sock_set_nonblocking"*/
    int32_t  ( *sock_set_nonblocking)(struct J9PortLibrary *portLibrary, j9socket_t socketP, BOOLEAN nonblocking) ;
	/** see @ref j9sock.c::j9sock_error_message "j9sock_error_message"*/
    const char*  ( *sock_error_message)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sock.c::j9sock_get_network_interfaces "j9sock_get_network_interfaces"*/
    int32_t  ( *sock_get_network_interfaces)(struct J9PortLibrary *portLibrary, struct j9NetworkInterfaceArray_struct *array,BOOLEAN preferIPv4Stack) ;
	/** see @ref j9sock.c::j9sock_free_network_interface_struct "j9sock_free_network_interface_struct"*/
    int32_t  ( *sock_free_network_interface_struct)(struct J9PortLibrary *portLibrary, struct j9NetworkInterfaceArray_struct* array) ;
	/** see @ref j9sock.c::j9sock_connect_with_timeout "j9sock_connect_with_timeout"*/
    int32_t  ( *sock_connect_with_timeout)(struct J9PortLibrary *portLibrary, j9socket_t sock, j9sockaddr_t addr, uint32_t timeout, uint32_t step, uint8_t** context) ;
	/** see @ref j9sock.c::j9sock_fdset_zero "j9sock_fdset_zero"*/
    void  ( *sock_fdset_zero)(struct J9PortLibrary *portLibrary, j9fdset_t j9fdset) ;
	/** see @ref j9sock.c::j9sock_fdset_set "j9sock_fdset_set"*/
    void  ( *sock_fdset_set)(struct J9PortLibrary *portLibrary, j9socket_t aSocket, j9fdset_t j9fdset) ;
	/** see @ref j9sock.c::j9sock_fdset_clr "j9sock_fdset_clr"*/
    void  ( *sock_fdset_clr)(struct J9PortLibrary *portLibrary, j9socket_t aSocket, j9fdset_t j9fdset) ;
	/** see @ref j9sock.c::j9sock_fdset_isset "j9sock_fdset_isset"*/
    BOOLEAN  ( *sock_fdset_isset)(struct J9PortLibrary *portLibrary, j9socket_t aSocket, j9fdset_t j9fdset) ;
	/** see @ref j9str.c::j9str_ftime "j9str_ftime"*/
    uintptr_t  ( *str_ftime)(struct J9PortLibrary *portLibrary, char *buf, uintptr_t bufLen, const char *format, int64_t timeMillis) ;
	/** see @ref j9mmap.c::j9mmap_startup "j9mmap_startup"*/
    int32_t  ( *mmap_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9mmap.c::j9mmap_shutdown "j9mmap_shutdown"*/
    void  ( *mmap_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9mmap.c::j9mmap_capabilities "j9mmap_capabilities"*/
    int32_t  ( *mmap_capabilities)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9mmap.c::j9mmap_map_file "j9mmap_map_file"*/
    struct J9MmapHandle*  ( *mmap_map_file)(struct J9PortLibrary *portLibrary, intptr_t file, uint64_t offset, uintptr_t size, const char *mappingName, uint32_t flags,  uint32_t category) ;
	/** see @ref j9mmap.c::j9mmap_unmap_file "j9mmap_unmap_file"*/
    void  ( *mmap_unmap_file)(struct J9PortLibrary *portLibrary, J9MmapHandle *handle) ;
	/** see @ref j9mmap.c::j9mmap_msync "j9mmap_msync"*/
    intptr_t  ( *mmap_msync)(struct J9PortLibrary *portLibrary, void *start, uintptr_t length, uint32_t flags) ;
	/** see @ref j9mmap.c::j9mmap_protect "j9mmap_protect"*/
    intptr_t  ( *mmap_protect)(struct J9PortLibrary *portLibrary, void* address, uintptr_t length, uintptr_t flags) ;
	/** see @ref j9mmap.c::j9mmap_get_region_granularity "j9mmap_get_region_granularity"*/
    uintptr_t  ( *mmap_get_region_granularity)(struct J9PortLibrary *portLibrary, void *address) ;
	/** see @ref j9shsem.c::j9shsem_params_init "j9shsem_params_init"*/
    int32_t  ( *shsem_params_init)(struct J9PortLibrary *portLibrary, struct J9PortShSemParameters *params) ;
	/** see @ref j9shsem.c::j9shsem_startup "j9shsem_startup"*/
    int32_t  ( *shsem_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9shsem.c::j9shsem_shutdown "j9shsem_shutdown"*/
    void  ( *shsem_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9shsem.c::j9shsem_open "j9shsem_open"*/
    intptr_t  ( *shsem_open)(struct J9PortLibrary *portLibrary, struct j9shsem_handle **handle, const struct J9PortShSemParameters *params) ;
	/** see @ref j9shsem.c::j9shsem_post "j9shsem_post"*/
    intptr_t  ( *shsem_post)(struct J9PortLibrary *portLibrary, struct j9shsem_handle* handle, uintptr_t semset, uintptr_t flag) ;
	/** see @ref j9shsem.c::j9shsem_wait "j9shsem_wait"*/
    intptr_t  ( *shsem_wait)(struct J9PortLibrary *portLibrary, struct j9shsem_handle* handle, uintptr_t semset, uintptr_t flag) ;
	/** see @ref j9shsem.c::j9shsem_getVal "j9shsem_getVal"*/
    intptr_t  ( *shsem_getVal)(struct J9PortLibrary *portLibrary, struct j9shsem_handle* handle, uintptr_t semset) ;
	/** see @ref j9shsem.c::j9shsem_setVal "j9shsem_setVal"*/
    intptr_t  ( *shsem_setVal)(struct J9PortLibrary *portLibrary, struct j9shsem_handle* handle, uintptr_t semset, intptr_t value) ;
	/** see @ref j9shsem.c::j9shsem_close "j9shsem_close"*/
    void  ( *shsem_close)(struct J9PortLibrary *portLibrary, struct j9shsem_handle **handle) ;
	/** see @ref j9shsem.c::j9shsem_destroy "j9shsem_destroy"*/
    intptr_t  ( *shsem_destroy)(struct J9PortLibrary *portLibrary, struct j9shsem_handle **handle) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_startup "j9shsem_deprecated_startup"*/
    int32_t  ( *shsem_deprecated_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_shutdown "j9shsem_deprecated_shutdown"*/
    void  ( *shsem_deprecated_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_open "j9shsem_deprecated_open"*/
    intptr_t  ( *shsem_deprecated_open)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, struct j9shsem_handle** handle, const char* semname, int setSize, int permission, uintptr_t flags, J9ControlFileStatus *controlFileStatus) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_openDeprecated "j9shsem_deprecated_openDeprecated"*/
    intptr_t  ( *shsem_deprecated_openDeprecated)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, struct j9shsem_handle** handle, const char* semname, uintptr_t cacheFileType) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_post "j9shsem_deprecated_post"*/
    intptr_t  ( *shsem_deprecated_post)(struct J9PortLibrary *portLibrary, struct j9shsem_handle* handle, uintptr_t semset, uintptr_t flag) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_wait "j9shsem_deprecated_wait"*/
    intptr_t  ( *shsem_deprecated_wait)(struct J9PortLibrary *portLibrary, struct j9shsem_handle* handle, uintptr_t semset, uintptr_t flag) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_getVal "j9shsem_deprecated_getVal"*/
    intptr_t  ( *shsem_deprecated_getVal)(struct J9PortLibrary *portLibrary, struct j9shsem_handle* handle, uintptr_t semset) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_setVal "j9shsem_deprecated_setVal"*/
    intptr_t  ( *shsem_deprecated_setVal)(struct J9PortLibrary *portLibrary, struct j9shsem_handle* handle, uintptr_t semset, intptr_t value) ;
    /** see @ref j9shsem.c::j9shsem_deprecated_handle_stat "j9shsem_deprecated_handle_stat"*/
    intptr_t  ( *shsem_deprecated_handle_stat)(struct J9PortLibrary *portLibrary, struct j9shsem_handle *handle, struct J9PortShsemStatistic *statbuf);
	/** see @ref j9shsem.c::j9shsem_deprecated_close "j9shsem_deprecated_close"*/
    void  ( *shsem_deprecated_close)(struct J9PortLibrary *portLibrary, struct j9shsem_handle **handle) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_destroy "j9shsem_deprecated_destroy"*/
    intptr_t  ( *shsem_deprecated_destroy)(struct J9PortLibrary *portLibrary, struct j9shsem_handle **handle) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_destroyDeprecated "j9shsem_deprecated_destroyDeprecated"*/
    intptr_t  ( *shsem_deprecated_destroyDeprecated)(struct J9PortLibrary *portLibrary, struct j9shsem_handle **handle, uintptr_t cacheFileType) ;
	/** see @ref j9shsem.c::j9shsem_deprecated_getid "j9shsem_deprecated_getid"*/
    int32_t  ( *shsem_deprecated_getid)(struct J9PortLibrary *portLibrary, struct j9shsem_handle* handle) ;
	/** see @ref j9shmem.c::j9shmem_startup "j9shmem_startup"*/
    int32_t  ( *shmem_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9shmem.c::j9shmem_shutdown "j9shmem_shutdown"*/
    void  ( *shmem_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9shmem.c::j9shmem_open "j9shmem_open"*/
    intptr_t  ( *shmem_open)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, struct j9shmem_handle **handle, const char* rootname, uintptr_t size, uint32_t perm, uint32_t category, uintptr_t flags, J9ControlFileStatus *controlFileStatus) ;
	/** see @ref j9shmem.c::j9shmem_openDeprecated "j9shmem_openDeprecated"*/
    intptr_t  ( *shmem_openDeprecated)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, struct j9shmem_handle **handle, const char* rootname, uint32_t perm, uintptr_t cacheFileType, uint32_t category) ;
	/** see @ref j9shmem.c::j9shmem_attach "j9shmem_attach"*/
    void*  ( *shmem_attach)(struct J9PortLibrary *portLibrary, struct j9shmem_handle* handle,  uint32_t category) ;
	/** see @ref j9shmem.c::j9shmem_detach "j9shmem_detach"*/
    intptr_t  ( *shmem_detach)(struct J9PortLibrary *portLibrary, struct j9shmem_handle **handle) ;
	/** see @ref j9shmem.c::j9shmem_close "j9shmem_close"*/
    void  ( *shmem_close)(struct J9PortLibrary *portLibrary, struct j9shmem_handle **handle) ;
	/** see @ref j9shmem.c::j9shmem_destroy "j9shmem_destroy"*/
    intptr_t  ( *shmem_destroy)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, struct j9shmem_handle **handle) ;
	/** see @ref j9shmem.c::j9shmem_destroyDeprecated "j9shmem_destroyDeprecated"*/
    intptr_t  ( *shmem_destroyDeprecated)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, struct j9shmem_handle **handle, uintptr_t cacheFileType) ;
	/** see @ref j9shmem.c::j9shmem_findfirst "j9shmem_findfirst"*/
    uintptr_t  ( *shmem_findfirst)(struct J9PortLibrary *portLibrary, char *cacheDirName, char *resultbuf) ;
	/** see @ref j9shmem.c::j9shmem_findnext "j9shmem_findnext"*/
    int32_t  ( *shmem_findnext)(struct J9PortLibrary *portLibrary, uintptr_t findhandle, char *resultbuf) ;
	/** see @ref j9shmem.c::j9shmem_findclose "j9shmem_findclose"*/
    void  ( *shmem_findclose)(struct J9PortLibrary *portLibrary, uintptr_t findhandle) ;
	/** see @ref j9shmem.c::j9shmem_stat "j9shmem_stat"*/
    uintptr_t  ( *shmem_stat)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, const char* name, struct J9PortShmemStatistic* statbuf) ;
	/** see @ref j9shmem.c::j9shmem_statDeprecated "j9shmem_statDeprecated"*/
    uintptr_t  ( *shmem_statDeprecated)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, const char* name, struct J9PortShmemStatistic* statbuf, uintptr_t cacheFileType) ;
    /** see @ref j9shmem.c::j9shmem_handle_stat "j9shmem_handle_stat"*/
    intptr_t  ( *shmem_handle_stat)(struct J9PortLibrary *portLibrary, struct j9shmem_handle *handle, struct J9PortShmemStatistic *statbuf);
	/** see @ref j9shmem.c::j9shmem_getDir "j9shmem_getDir"*/
    intptr_t  ( *shmem_getDir)(struct J9PortLibrary* portLibrary, const char* ctrlDirName, BOOLEAN appendBaseDir, char* buffer, uintptr_t length) ;
	/** see @ref j9shmem.c::j9shmem_createDir "j9shmem_createDir"*/
    intptr_t  ( *shmem_createDir)(struct J9PortLibrary *portLibrary, char* cacheDirName, uintptr_t cacheDirPerm, BOOLEAN cleanMemorySegments) ;
	/** see @ref j9shmem.c::j9shmem_getFilepath "j9shmem_getFilepath"*/
    intptr_t  ( *shmem_getFilepath)(struct J9PortLibrary* portLibrary, char* cacheDirName, char* buffer, uintptr_t length, const char* cachename) ;
	/** see @ref j9shmem.c::j9shmem_protect "j9shmem_protect"*/
    intptr_t  ( *shmem_protect)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, void* address, uintptr_t length, uintptr_t flags) ;
	/** see @ref j9shmem.c::j9shmem_get_region_granularity "j9shmem_get_region_granularity"*/
    uintptr_t  ( *shmem_get_region_granularity)(struct J9PortLibrary *portLibrary, const char* cacheDirName, uintptr_t groupPerm, void *address) ;
    /** see @ref j9shmem.c::j9shmem_getid "j9shmem_getid"*/
    int32_t  ( *shmem_getid)(struct J9PortLibrary *portLibrary, struct j9shmem_handle* handle);
	/** see @ref j9sysinfo.c::j9sysinfo_get_limit "j9sysinfo_get_limit"*/
    uint32_t  ( *sysinfo_get_limit)(struct J9PortLibrary* portLibrary, uint32_t resourceID, uint64_t* limit) ;
	/** see @ref j9sysinfo.c::j9sysinfo_set_limit "j9sysinfo_set_limit"*/
    uint32_t  ( *sysinfo_set_limit)(struct J9PortLibrary* portLibrary, uint32_t resourceID, uint64_t limit) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_processing_capacity "j9sysinfo_get_processing_capacity"*/
    uintptr_t  ( *sysinfo_get_processing_capacity)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_number_CPUs_by_type "j9sysinfo_get_number_CPUs_by_type"*/
    uintptr_t  ( *sysinfo_get_number_CPUs_by_type)(struct J9PortLibrary *portLibrary, uintptr_t type) ;
	/** see @ref j9sysinfo.c::j9sysinfo_set_number_entitled_CPUs "j9sysinfo_set_number_entitled_CPUs"*/
    void  ( *sysinfo_set_number_entitled_CPUs)(struct J9PortLibrary *portLibrary, uintptr_t number) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_cwd "j9sysinfo_get_cwd"*/
    intptr_t  ( *sysinfo_get_cwd)(struct J9PortLibrary *portLibrary, char *buf, uintptr_t bufLen) ;
	/** see @ref j9sysinfo.c::j9sysinfo_get_tmp "j9sysinfo_get_tmp"*/
    intptr_t  ( *sysinfo_get_tmp)(struct J9PortLibrary *portLibrary, char *buf, uintptr_t bufLen, BOOLEAN ignoreEnvVariable) ;
	/** see @ref j9port.c::j9port_init_library "j9port_init_library"*/
    int32_t  ( *port_init_library)(struct J9PortLibrary *portLibrary, struct J9PortLibraryVersion *version, uintptr_t size) ;
	/** see @ref j9port.c::j9port_startup_library "j9port_startup_library"*/
    int32_t  ( *port_startup_library)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9port.c::j9port_create_library "j9port_create_library"*/
    int32_t  ( *port_create_library)(struct J9PortLibrary *portLibrary, struct J9PortLibraryVersion *version, uintptr_t size) ;
	/** see @ref j9syslog.c::j9syslog_write "j9syslog_write"*/
    uintptr_t  ( *syslog_write)(struct J9PortLibrary *portLibrary, uintptr_t flags, const char* message) ;
	/** see @ref j9virt.c::j9virt_rwlock_startup "j9virt_rwlock_startup"*/
    int32_t  ( *virt_rwlock_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9virt.c::j9virt_rwlock_shutdown "j9virt_rwlock_shutdown"*/
    void  ( *virt_rwlock_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9virt.c::j9virt_rwlock_initialize "j9virt_rwlock_initialize"*/
    intptr_t  ( *virt_rwlock_initialize)(struct J9PortLibrary *portLibrary, J9VirtReadWriteLock *lockp) ;
	/** see @ref j9virt.c::j9virt_rwlock_acquire_shared "j9virt_rwlock_acquire_shared"*/
    intptr_t  ( *virt_rwlock_acquire_shared)(struct J9PortLibrary *portLibrary, J9VirtReadWriteLock *lockp) ;
	/** see @ref j9virt.c::j9virt_rwlock_release_shared "j9virt_rwlock_release_shared"*/
    intptr_t  ( *virt_rwlock_release_shared)(struct J9PortLibrary *portLibrary, J9VirtReadWriteLock *lockp) ;
	/** see @ref j9virt.c::j9virt_rwlock_acquire_exclusive "j9virt_rwlock_acquire_exclusive"*/
    intptr_t  ( *virt_rwlock_acquire_exclusive)(struct J9PortLibrary *portLibrary, J9VirtReadWriteLock *lockp, const int32_t callerId, uint32_t flags) ;
	/** see @ref j9virt.c::j9virt_rwlock_release_exclusive "j9virt_rwlock_release_exclusive"*/
    intptr_t  ( *virt_rwlock_release_exclusive)(struct J9PortLibrary *portLibrary, J9VirtReadWriteLock *lockp, const int32_t callerId) ;
	/** see @ref j9hypervisor.c::j9hypervisor_startup "j9hypervisor_startup"*/
    int32_t  ( *hypervisor_startup)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9hypervisor.c::j9hypervisor_shutdown "j9hypervisor_shutdown"*/
    void  ( *hypervisor_shutdown)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9hypervisor.c::j9hypervisor_initialize_virtualization "j9hypervisor_initialize_virtualization"*/
    intptr_t  ( *hypervisor_initialize_virtualization)(struct J9PortLibrary *portLibrary, J9VirtConfig *virtConfig, uint32_t reason) ;
	/** see @ref j9hypervisor.c::j9hypervisor_is_virtualization_initialized "j9hypervisor_is_virtualization_initialized"*/
    BOOLEAN  ( *hypervisor_is_virtualization_initialized)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9hypervisor.c::j9hypervisor_setup_shared_object_access "j9hypervisor_setup_shared_object_access"*/
    intptr_t  ( *hypervisor_setup_shared_object_access)(struct J9PortLibrary *portLibrary, uintptr_t size, void** memHdr, const char* fullPath) ;
	/** see @ref j9hypervisor.c::j9hypervisor_destroy_shared_object_access "j9hypervisor_destroy_shared_object_access"*/
    intptr_t  ( *hypervisor_destroy_shared_object_access)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9hypervisor.c::j9hypervisor_delete_shared_object "j9hypervisor_delete_shared_object"*/
    intptr_t  ( *hypervisor_delete_shared_object)(struct J9PortLibrary *portLibrary, J9VirtHandle *memHandle) ;
	/** see @ref j9hypervisor.c::j9hypervisor_map_shared_object "j9hypervisor_map_shared_object"*/
    struct J9VirtHandle*  ( *hypervisor_map_shared_object)(struct J9PortLibrary *portLibrary, uint32_t categoryCode) ;
	/** see @ref j9hypervisor.c::j9hypervisor_unmap_shared_object "j9hypervisor_unmap_shared_object"*/
    void  ( *hypervisor_unmap_shared_object)(struct J9PortLibrary *portLibrary, J9VirtHandle *memHandle) ;
	/** see @ref j9hypervisor.c::j9hypervisor_protect "j9hypervisor_protect"*/
    intptr_t  ( *hypervisor_protect)(struct J9PortLibrary *portLibrary, void* address, uintptr_t length, uintptr_t flags) ;
	/** see @ref j9hypervisor.c::j9hypervisor_sync "j9hypervisor_sync"*/
    intptr_t  ( *hypervisor_sync)(struct J9PortLibrary *portLibrary, void *start, uintptr_t length, uint32_t flags) ;
	/** see @ref j9hypervisor.c::j9hypervisor_get_shared_object_size "j9hypervisor_get_shared_object_size"*/
    uintptr_t  ( *hypervisor_get_shared_object_size)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9hypervisor.c::j9hypervisor_get_guest_id "j9hypervisor_get_guest_id"*/
    intptr_t  ( *hypervisor_get_guest_id)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9hypervisor.c::j9hypervisor_hypervisor_present "j9hypervisor_hypervisor_present"*/
    intptr_t  ( *hypervisor_hypervisor_present)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9hypervisor.c::j9hypervisor_get_hypervisor_info "j9hypervisor_get_hypervisor_info"*/
    intptr_t  ( *hypervisor_get_hypervisor_info)(struct J9PortLibrary *portLibrary , J9HypervisorVendorDetails *vendorDetails) ;
	/** see @ref j9hypervisor.c::j9hypervisor_get_guest_processor_usage "j9hypervisor_get_guest_processor_usage"*/
    intptr_t  ( *hypervisor_get_guest_processor_usage)(struct J9PortLibrary *portLibrary, J9GuestProcessorUsage *gpUsage) ;
	/** see @ref j9hypervisor.c::j9hypervisor_get_guest_memory_usage "j9hypervisor_get_guest_memory_usage"*/
    intptr_t  ( *hypervisor_get_guest_memory_usage)(struct J9PortLibrary *portLibrary, J9GuestMemoryUsage *gmUsage) ;
	/** see @ref j9process.c::j9process_create "j9process_create"*/
    intptr_t  ( *process_create)(struct J9PortLibrary *portLibrary, const char *command[], uintptr_t commandLength, char *env[], uintptr_t envSize, const char *dir, uint32_t options, intptr_t fdInput, intptr_t fdOutput, intptr_t fdError, J9ProcessHandle *processHandle) ;
	/** see @ref j9process.c::j9process_waitfor "j9process_waitfor"*/
    intptr_t  ( *process_waitfor)(struct J9PortLibrary *portLibrary, J9ProcessHandle processHandle) ;
	/** see @ref j9process.c::j9process_terminate "j9process_terminate"*/
    intptr_t  ( *process_terminate)(struct J9PortLibrary *portLibrary, J9ProcessHandle processHandle) ;
	/** see @ref j9process.c::j9process_write "j9process_write"*/
    intptr_t  ( *process_write)(struct J9PortLibrary *portLibrary, J9ProcessHandle processHandle, void *buffer, uintptr_t numBytes) ;
	/** see @ref j9process.c::j9process_read "j9process_read"*/
    intptr_t  ( *process_read)(struct J9PortLibrary *portLibrary, J9ProcessHandle processHandle, uintptr_t flags, void *buffer, uintptr_t numBytes) ;
	/** see @ref j9process.c::j9process_get_available "j9process_get_available"*/
    intptr_t  ( *process_get_available)(struct J9PortLibrary *portLibrary, J9ProcessHandle processHandle, uintptr_t flags) ;
	/** see @ref j9process.c::j9process_close "j9process_close"*/
    intptr_t  ( *process_close)(struct J9PortLibrary *portLibrary, J9ProcessHandle *processHandle, uint32_t options) ;
	/** see @ref j9process.c::j9process_getStream "j9process_getStream"*/
    intptr_t  ( *process_getStream)(struct J9PortLibrary *portLibrary, J9ProcessHandle processHandle, uintptr_t streamFlag, intptr_t *stream) ;
	/** see @ref j9process.c::j9process_isComplete "j9process_isComplete"*/
    intptr_t  ( *process_isComplete)(struct J9PortLibrary *portLibrary, J9ProcessHandle processHandle) ;
	/** see @ref j9process.c::j9process_get_exitCode "j9process_get_exitCode"*/
    intptr_t  ( *process_get_exitCode)(struct J9PortLibrary *portLibrary, J9ProcessHandle processHandle) ;
	/** see @ref j9introspect.c::j9introspect_threads_startDo "j9introspect_threads_startDo"*/
    struct J9PlatformThread*  ( *introspect_threads_startDo)(struct J9PortLibrary *portLibrary, J9Heap *heap, J9ThreadWalkState *state) ;
	/** see @ref j9introspect.c::j9introspect_threads_startDo_with_signal "j9introspect_threads_startDo_with_signal"*/
    struct J9PlatformThread*  ( *introspect_threads_startDo_with_signal)(struct J9PortLibrary *portLibrary, J9Heap *heap, J9ThreadWalkState *state, void *signal_info) ;
	/** see @ref j9introspect.c::j9introspect_threads_nextDo "j9introspect_threads_nextDo"*/
    struct J9PlatformThread*  ( *introspect_threads_nextDo)(J9ThreadWalkState *state) ;
	/** see @ref j9introspect.c::j9introspect_backtrace_thread "j9introspect_backtrace_thread"*/
    uintptr_t  ( *introspect_backtrace_thread)(struct J9PortLibrary *portLibrary, J9PlatformThread *thread, J9Heap *heap, void *signalInfo) ;
	/** see @ref j9introspect.c::j9introspect_backtrace_symbols "j9introspect_backtrace_symbols"*/
    uintptr_t  ( *introspect_backtrace_symbols)(struct J9PortLibrary *portLibrary, J9PlatformThread *thread, J9Heap *heap) ;
	/** see @ref j9syslog.c::j9syslog_query "j9syslog_query"*/
    uintptr_t  ( *syslog_query)(struct J9PortLibrary *portLibrary) ;
	/** see @ref j9syslog.c::j9syslog_set "j9syslog_set"*/
    void  ( *syslog_set)(struct J9PortLibrary *portLibrary, uintptr_t options) ;
	/** see @ref j9mem.c::j9mem_walk_categories "j9mem_walk_categories"*/
    void  ( *mem_walk_categories)(struct J9PortLibrary *portLibrary, J9MemCategoryWalkState * state) ;
	/** see @ref j9heap.c::j9heap_query_size "j9heap_query_size"*/
    uintptr_t  ( *heap_query_size)(struct J9PortLibrary *portLibrary, struct J9Heap *heap, void *address) ;
	/** see @ref j9heap.c::j9heap_grow "j9heap_grow"*/
    BOOLEAN  ( *heap_grow)(struct J9PortLibrary *portLibrary, struct J9Heap *heap, uintptr_t growAmount) ;
#if defined(J9VM_PORT_RUNTIME_INSTRUMENTATION)
	/** see @ref j9ri.c::j9ri_params_init "j9ri_params_init"*/
    void  ( *ri_params_init)(struct J9PortLibrary *portLibrary, struct J9RIParameters *riParams, void *riControlBlock) ;
	/** see @ref j9ri.c::j9ri_initialize "j9ri_initialize"*/
    void  ( *ri_initialize)(struct J9PortLibrary *portLibrary, struct J9RIParameters *riParams) ;
	/** see @ref j9ri.c::j9ri_deinitialize "j9ri_deinitialize"*/
    void  ( *ri_deinitialize)(struct J9PortLibrary *portLibrary, struct J9RIParameters *riParams) ;
	/** see @ref j9ri.c::j9ri_enable "j9ri_enable"*/
    void  ( *ri_enable)(struct J9PortLibrary *portLibrary, struct J9RIParameters *riParams) ;
	/** see @ref j9ri.c::j9ri_disable "j9ri_disable"*/
    void  ( *ri_disable)(struct J9PortLibrary *portLibrary, struct J9RIParameters *riParams) ;
#endif /* J9VM_PORT_RUNTIME_INSTRUMENTATION */
} J9PortLibrary;

#if defined(J9VM_PORT_CAN_RESERVE_SPECIFIC_ADDRESS)
#define J9PORT_CAPABILITY_MASK_CAN_RESERVE J9PORT_CAPABILITY_CAN_RESERVE_SPECIFIC_ADDRESS
#else /* J9VM_PORT_CAN_RESERVE_SPECIFIC_ADDRESS */
#define J9PORT_CAPABILITY_MASK_CAN_RESERVE 0
#endif /* J9VM_PORT_CAN_RESERVE_SPECIFIC_ADDRESS */

#if defined(J9VM_PORT_ALLOCATE_TOP_DOWN)
#define J9PORT_CAPABILITY_MASK_TOP_DOWN J9PORT_CAPABILITY_ALLOCATE_TOP_DOWN
#else /* J9VM_PORT_ALLOCATE_TOP_DOWN */
#define J9PORT_CAPABILITY_MASK_TOP_DOWN 0
#endif /* J9VM_PORT_ALLOCATE_TOP_DOWN */


#define J9PORT_CAPABILITY_MASK ((uint64_t)(J9PORT_CAPABILITY_STANDARD | J9PORT_CAPABILITY_MASK_CAN_RESERVE | J9PORT_CAPABILITY_MASK_TOP_DOWN))


#define J9PORT_SET_VERSION(portLibraryVersion, capabilityMask) \
	(portLibraryVersion)->majorVersionNumber = J9PORT_MAJOR_VERSION_NUMBER; \
	(portLibraryVersion)->minorVersionNumber = J9PORT_MINOR_VERSION_NUMBER; \
	(portLibraryVersion)->capabilities = (capabilityMask)
#define J9PORT_SET_VERSION_DEFAULT(portLibraryVersion) \
	(portLibraryVersion)->majorVersionNumber = J9PORT_MAJOR_VERSION_NUMBER; \
	(portLibraryVersion)->minorVersionNumber = J9PORT_MINOR_VERSION_NUMBER; \
	(portLibraryVersion)->capabilities = J9PORT_CAPABILITY_MASK


/**
 * @name Port library startup and shutdown functions
 * @anchor PortStartup
 * Create, initialize, startup and shutdow the port library
 * @{
 */
/** Standard startup and shutdown (port library allocated on stack or by application)  */
extern J9_CFUNC int32_t j9port_create_library(struct J9PortLibrary *portLibrary, struct J9PortLibraryVersion *version, uintptr_t size);
extern J9_CFUNC int32_t j9port_init_library(struct J9PortLibrary*portLibrary, struct J9PortLibraryVersion *version, uintptr_t size);
extern J9_CFUNC int32_t j9port_shutdown_library(struct J9PortLibrary *portLibrary);
extern J9_CFUNC int32_t j9port_startup_library(struct J9PortLibrary *portLibrary);

/** Port library self allocation routines */
extern J9_CFUNC int32_t j9port_allocate_library(struct J9PortLibraryVersion *expectedVersion, struct J9PortLibrary **portLibrary);
/** @} */

/**
 * @name Port library version and compatability queries
 * @anchor PortVersionControl
 * Determine port library compatability and version.
 * @{
 */
extern J9_CFUNC uintptr_t j9port_getSize(struct J9PortLibraryVersion *version);
extern J9_CFUNC int32_t j9port_getVersion(struct J9PortLibrary *portLibrary, struct J9PortLibraryVersion *version);
extern J9_CFUNC int32_t j9port_isCompatible(struct J9PortLibraryVersion *expectedVersion);
/** @} */


/**
 * @name PortLibrary Access functions
 * Users should call port library functions through these macros.
 * @code
 * PORT_ACCESS_FROM_ENV(jniEnv);
 * memoryPointer = j9mem_allocate_memory(1024);
 * @endcode
 * @{
 */
#if !defined(J9PORT_LIBRARY_DEFINE)
#define j9port_shutdown_library() privatePortLibrary->port_shutdown_library(privatePortLibrary)
#define j9port_isFunctionOverridden(param1) privatePortLibrary->port_isFunctionOverridden(privatePortLibrary,param1)
#define j9port_tls_free() privatePortLibrary->port_tls_free(privatePortLibrary)
#define j9error_startup() privatePortLibrary->error_startup(privatePortLibrary)
#define j9error_shutdown() privatePortLibrary->error_shutdown(privatePortLibrary)
#define j9error_last_error_number() privatePortLibrary->error_last_error_number(privatePortLibrary)
#define j9error_last_error_message() privatePortLibrary->error_last_error_message(privatePortLibrary)
#define j9error_set_last_error(param1,param2) privatePortLibrary->error_set_last_error(privatePortLibrary,param1,param2)
#define j9error_set_last_error_with_message(param1,param2) privatePortLibrary->error_set_last_error_with_message(privatePortLibrary,param1,param2)
#define j9time_startup() privatePortLibrary->time_startup(privatePortLibrary)
#define j9time_shutdown() privatePortLibrary->time_shutdown(privatePortLibrary)
#define j9time_msec_clock() privatePortLibrary->time_msec_clock(privatePortLibrary)
#define j9time_usec_clock() privatePortLibrary->time_usec_clock(privatePortLibrary)
#define j9time_current_time_millis() privatePortLibrary->time_current_time_millis(privatePortLibrary)
#define j9time_nano_time() privatePortLibrary->time_nano_time(privatePortLibrary)
#define j9time_hires_clock() privatePortLibrary->time_hires_clock(privatePortLibrary)
#define j9time_hires_frequency() privatePortLibrary->time_hires_frequency(privatePortLibrary)
#define j9time_hires_delta(param1,param2,param3) privatePortLibrary->time_hires_delta(privatePortLibrary,param1,param2,param3)
#define j9sysinfo_startup() privatePortLibrary->sysinfo_startup(privatePortLibrary)
#define j9sysinfo_shutdown() privatePortLibrary->sysinfo_shutdown(privatePortLibrary)
#define j9sysinfo_process_exists(param1) privatePortLibrary->sysinfo_process_exists(privatePortLibrary,param1)
#define j9sysinfo_get_egid() privatePortLibrary->sysinfo_get_egid(privatePortLibrary)
#define j9sysinfo_get_euid() privatePortLibrary->sysinfo_get_euid(privatePortLibrary)
#define j9sysinfo_get_groups(param1,param2) privatePortLibrary->sysinfo_get_groups(privatePortLibrary,param1,param2)
#define j9sysinfo_get_pid() privatePortLibrary->sysinfo_get_pid(privatePortLibrary)
#define j9sysinfo_get_ppid() privatePortLibrary->sysinfo_get_ppid(privatePortLibrary)
#define j9sysinfo_get_memory_info privatePortLibrary->sysinfo_get_memory_info
#define j9sysinfo_get_processor_info(param1) privatePortLibrary->sysinfo_get_processor_info(privatePortLibrary,param1)
#define j9sysinfo_destroy_processor_info(param1) privatePortLibrary->sysinfo_destroy_processor_info(privatePortLibrary,param1)
#define j9sysinfo_get_physical_memory() privatePortLibrary->sysinfo_get_physical_memory(privatePortLibrary)
#define j9sysinfo_get_OS_version() privatePortLibrary->sysinfo_get_OS_version(privatePortLibrary)
#define j9sysinfo_get_env(param1,param2,param3) privatePortLibrary->sysinfo_get_env(privatePortLibrary,param1,param2,param3)
#define j9sysinfo_get_CPU_architecture() privatePortLibrary->sysinfo_get_CPU_architecture(privatePortLibrary)
#define j9sysinfo_get_OS_type() privatePortLibrary->sysinfo_get_OS_type(privatePortLibrary)
#define j9sysinfo_get_classpathSeparator() privatePortLibrary->sysinfo_get_classpathSeparator(privatePortLibrary)
#define j9sysinfo_get_executable_name(param1,param2) privatePortLibrary->sysinfo_get_executable_name(privatePortLibrary,param1,param2)
#define j9sysinfo_get_username(param1,param2) privatePortLibrary->sysinfo_get_username(privatePortLibrary,param1,param2)
#define j9sysinfo_get_groupname(param1,param2) privatePortLibrary->sysinfo_get_groupname(privatePortLibrary,param1,param2)
#define j9sysinfo_get_load_average(param1) privatePortLibrary->sysinfo_get_load_average(privatePortLibrary,param1)
#define j9sysinfo_get_CPU_utilization(param1) privatePortLibrary->sysinfo_get_CPU_utilization(privatePortLibrary,param1)
#define j9sysinfo_limit_iterator_init(param1) privatePortLibrary->sysinfo_limit_iterator_init(privatePortLibrary,param1)
#define j9sysinfo_limit_iterator_hasNext(param1) privatePortLibrary->sysinfo_limit_iterator_hasNext(privatePortLibrary,param1)
#define j9sysinfo_limit_iterator_next(param1,param2) privatePortLibrary->sysinfo_limit_iterator_next(privatePortLibrary,param1,param2)
#define j9sysinfo_env_iterator_init(param1,param2,param3) privatePortLibrary->sysinfo_env_iterator_init(privatePortLibrary,param1,param2,param3)
#define j9sysinfo_env_iterator_hasNext(param1) privatePortLibrary->sysinfo_env_iterator_hasNext(privatePortLibrary,param1)
#define j9sysinfo_env_iterator_next(param1,param2) privatePortLibrary->sysinfo_env_iterator_next(privatePortLibrary,param1,param2)
#define j9sysinfo_get_processor_description(param1) privatePortLibrary->sysinfo_get_processor_description(privatePortLibrary,param1)
#define j9sysinfo_processor_has_feature(param1,param2) privatePortLibrary->sysinfo_processor_has_feature(privatePortLibrary,param1,param2)
#define j9file_startup() privatePortLibrary->file_startup(privatePortLibrary)
#define j9file_shutdown() privatePortLibrary->file_shutdown(privatePortLibrary)
#define j9file_write(param1,param2,param3) privatePortLibrary->file_write(privatePortLibrary,param1,param2,param3)
#define j9file_write_text(param1,param2,param3) privatePortLibrary->file_write_text(privatePortLibrary,param1,param2,param3)
#define j9file_get_text_encoding(param1,param2) privatePortLibrary->file_get_text_encoding(privatePortLibrary,param1,param2)
#define j9file_vprintf(param1,param2,param3) privatePortLibrary->file_vprintf(privatePortLibrary,param1,param2,param3)
#define j9file_printf privatePortLibrary->file_printf
#define j9file_open(param1,param2,param3) privatePortLibrary->file_open(privatePortLibrary,param1,param2,param3)
#define j9file_close(param1) privatePortLibrary->file_close(privatePortLibrary,param1)
#define j9file_seek(param1,param2,param3) privatePortLibrary->file_seek(privatePortLibrary,param1,param2,param3)
#define j9file_read(param1,param2,param3) privatePortLibrary->file_read(privatePortLibrary,param1,param2,param3)
#define j9file_unlink(param1) privatePortLibrary->file_unlink(privatePortLibrary,param1)
#define j9file_attr(param1) privatePortLibrary->file_attr(privatePortLibrary,param1)
#define j9file_chmod(param1,param2) privatePortLibrary->file_chmod(privatePortLibrary,param1,param2)
#define j9file_chown(param1,param2,param3) privatePortLibrary->file_chown(privatePortLibrary,param1,param2,param3)
#define j9file_lastmod(param1) privatePortLibrary->file_lastmod(privatePortLibrary,param1)
#define j9file_length(param1) privatePortLibrary->file_length(privatePortLibrary,param1)
#define j9file_flength(param1) privatePortLibrary->file_flength(privatePortLibrary,param1)
#define j9file_set_length(param1,param2) privatePortLibrary->file_set_length(privatePortLibrary,param1,param2)
#define j9file_sync(param1) privatePortLibrary->file_sync(privatePortLibrary,param1)
#define j9file_fstat(param1,param2) privatePortLibrary->file_fstat(privatePortLibrary,param1,param2)
#define j9file_stat(param1,param2,param3) privatePortLibrary->file_stat(privatePortLibrary,param1,param2,param3)
#define j9file_stat_filesystem(param1,param2,param3) privatePortLibrary->file_stat_filesystem(privatePortLibrary,param1,param2,param3)
#define j9file_blockingasync_open(param1,param2,param3) privatePortLibrary->file_blockingasync_open(privatePortLibrary,param1,param2,param3)
#define j9file_blockingasync_close(param1) privatePortLibrary->file_blockingasync_close(privatePortLibrary,param1)
#define j9file_blockingasync_read(param1,param2,param3) privatePortLibrary->file_blockingasync_read(privatePortLibrary,param1,param2,param3)
#define j9file_blockingasync_write(param1,param2,param3) privatePortLibrary->file_blockingasync_write(privatePortLibrary,param1,param2,param3)
#define j9file_blockingasync_unlock_bytes(param1,param2,param3) privatePortLibrary->file_blockingasync_unlock_bytes(privatePortLibrary,param1,param2,param3)
#define j9file_blockingasync_lock_bytes(param1,param2,param3,param4) privatePortLibrary->file_blockingasync_lock_bytes(privatePortLibrary,param1,param2,param3,param4)
#define j9file_blockingasync_set_length(param1,param2) privatePortLibrary->file_blockingasync_set_length(privatePortLibrary,param1,param2)
#define j9file_blockingasync_flength(param1) privatePortLibrary->file_blockingasync_flength(privatePortLibrary,param1)
#define j9sl_startup() privatePortLibrary->sl_startup(privatePortLibrary)
#define j9sl_shutdown() privatePortLibrary->sl_shutdown(privatePortLibrary)
#define j9sl_close_shared_library(param1) privatePortLibrary->sl_close_shared_library(privatePortLibrary,param1)
#define j9sl_open_shared_library(param1,param2,param3) privatePortLibrary->sl_open_shared_library(privatePortLibrary,param1,param2,param3)
#define j9sl_lookup_name(param1,param2,param3,param4) privatePortLibrary->sl_lookup_name(privatePortLibrary,param1,param2,param3,param4)
#define j9tty_startup() privatePortLibrary->tty_startup(privatePortLibrary)
#define j9tty_shutdown() privatePortLibrary->tty_shutdown(privatePortLibrary)
#define j9tty_printf privatePortLibrary->tty_printf
#define j9tty_vprintf(param1,param2) privatePortLibrary->tty_vprintf(privatePortLibrary,param1,param2)
#define j9tty_get_chars(param1,param2) privatePortLibrary->tty_get_chars(privatePortLibrary,param1,param2)
#define j9tty_err_printf privatePortLibrary->tty_err_printf
#define j9tty_err_vprintf(param1,param2) privatePortLibrary->tty_err_vprintf(privatePortLibrary,param1,param2)
#define j9tty_available() privatePortLibrary->tty_available(privatePortLibrary)
#define j9tty_daemonize() privatePortLibrary->tty_daemonize(privatePortLibrary)
#define j9heap_create(param1,param2,param3) privatePortLibrary->heap_create(privatePortLibrary,param1,param2,param3)
#define j9heap_allocate(param1,param2) privatePortLibrary->heap_allocate(privatePortLibrary,param1,param2)
#define j9heap_free(param1,param2) privatePortLibrary->heap_free(privatePortLibrary,param1,param2)
#define j9heap_reallocate(param1,param2,param3) privatePortLibrary->heap_reallocate(privatePortLibrary,param1,param2,param3)
#define j9mem_startup(param1) privatePortLibrary->mem_startup(privatePortLibrary,param1)
#define j9mem_shutdown() privatePortLibrary->mem_shutdown(privatePortLibrary)
#define j9mem_allocate_memory(param1,param2,param3) privatePortLibrary->mem_allocate_memory(privatePortLibrary,param1,param2,param3)
#define j9mem_free_memory(param1) privatePortLibrary->mem_free_memory(privatePortLibrary,param1)
#define j9mem_advise_and_free_memory(param1) privatePortLibrary->mem_advise_and_free_memory(privatePortLibrary,param1)
#define j9mem_reallocate_memory(param1,param2,param3,param4) privatePortLibrary->mem_reallocate_memory(privatePortLibrary,param1,param2,param3,param4)
#define j9mem_allocate_memory32(param1,param2,param3) privatePortLibrary->mem_allocate_memory32(privatePortLibrary,param1,param2,param3)
#define j9mem_free_memory32(param1) privatePortLibrary->mem_free_memory32(privatePortLibrary,param1)
#define j9mem_ensure_capacity32(param1) privatePortLibrary->mem_ensure_capacity32(privatePortLibrary,param1)
#define j9cpu_startup() privatePortLibrary->cpu_startup(privatePortLibrary)
#define j9cpu_shutdown() privatePortLibrary->cpu_shutdown(privatePortLibrary)
#define j9cpu_flush_icache(param1,param2) privatePortLibrary->cpu_flush_icache(privatePortLibrary,param1,param2)
#define j9vmem_startup() privatePortLibrary->vmem_startup(privatePortLibrary)
#define j9vmem_shutdown() privatePortLibrary->vmem_shutdown(privatePortLibrary)
#define j9vmem_commit_memory(param1,param2,param3) privatePortLibrary->vmem_commit_memory(privatePortLibrary,param1,param2,param3)
#define j9vmem_decommit_memory(param1,param2,param3) privatePortLibrary->vmem_decommit_memory(privatePortLibrary,param1,param2,param3)
#define j9vmem_free_memory(param1,param2,param3) privatePortLibrary->vmem_free_memory(privatePortLibrary,param1,param2,param3)
#define j9vmem_vmem_params_init(param1) privatePortLibrary->vmem_vmem_params_init(privatePortLibrary,param1)
#define j9vmem_reserve_memory(param1,param2,param3,param4,param5,param6) privatePortLibrary->vmem_reserve_memory(privatePortLibrary,param1,param2,param3,param4,param5,param6)
#define j9vmem_reserve_memory_ex(param1,param2) privatePortLibrary->vmem_reserve_memory_ex(privatePortLibrary,param1,param2)
#define j9vmem_get_page_size(param1) privatePortLibrary->vmem_get_page_size(privatePortLibrary,param1)
#define j9vmem_get_page_flags(param1) privatePortLibrary->vmem_get_page_flags(privatePortLibrary,param1)
#define j9vmem_supported_page_sizes() privatePortLibrary->vmem_supported_page_sizes(privatePortLibrary)
#define j9vmem_supported_page_flags() privatePortLibrary->vmem_supported_page_flags(privatePortLibrary)
#define j9vmem_default_large_page_size_ex(param1,param2,param3) privatePortLibrary->vmem_default_large_page_size_ex(privatePortLibrary,param1,param2,param3)
#define j9vmem_find_valid_page_size(param1,param2,param3,param4) privatePortLibrary->vmem_find_valid_page_size(privatePortLibrary,param1,param2,param3,param4)
#define j9vmem_numa_set_affinity(param1,param2,param3,param4) privatePortLibrary->vmem_numa_set_affinity(privatePortLibrary,param1,param2,param3,param4)
#define j9vmem_numa_get_node_details(param1,param2) privatePortLibrary->vmem_numa_get_node_details(privatePortLibrary,param1,param2)
#define j9sock_startup() privatePortLibrary->sock_startup(privatePortLibrary)
#define j9sock_shutdown() privatePortLibrary->sock_shutdown(privatePortLibrary)
#define j9sock_htons(param1) privatePortLibrary->sock_htons(privatePortLibrary,param1)
#define j9sock_write(param1,param2,param3,param4) privatePortLibrary->sock_write(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_sockaddr(param1,param2,param3) privatePortLibrary->sock_sockaddr(privatePortLibrary,param1,param2,param3)
#define j9sock_read(param1,param2,param3,param4) privatePortLibrary->sock_read(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_socket(param1,param2,param3,param4) privatePortLibrary->sock_socket(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_close(param1) privatePortLibrary->sock_close(privatePortLibrary,param1)
#define j9sock_connect(param1,param2) privatePortLibrary->sock_connect(privatePortLibrary,param1,param2)
#define j9sock_inetaddr(param1,param2) privatePortLibrary->sock_inetaddr(privatePortLibrary,param1,param2)
#define j9sock_gethostbyname(param1,param2) privatePortLibrary->sock_gethostbyname(privatePortLibrary,param1,param2)
#define j9sock_hostent_addrlist(param1,param2) privatePortLibrary->sock_hostent_addrlist(privatePortLibrary,param1,param2)
#define j9sock_sockaddr_init(param1,param2,param3,param4) privatePortLibrary->sock_sockaddr_init(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_linger_init(param1,param2,param3) privatePortLibrary->sock_linger_init(privatePortLibrary,param1,param2,param3)
#define j9sock_setopt_linger(param1,param2,param3,param4) privatePortLibrary->sock_setopt_linger(privatePortLibrary,param1,param2,param3,param4)
#define j9gp_startup() privatePortLibrary->gp_startup(privatePortLibrary)
#define j9gp_shutdown() privatePortLibrary->gp_shutdown(privatePortLibrary)
#define j9gp_protect(param1,param2) privatePortLibrary->gp_protect(privatePortLibrary,param1,param2)
#define j9gp_register_handler(param1,param2) privatePortLibrary->gp_register_handler(privatePortLibrary,param1,param2)
#define j9gp_info(param1,param2,param3,param4,param5) privatePortLibrary->gp_info(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9gp_info_count(param1,param2) privatePortLibrary->gp_info_count(privatePortLibrary,param1,param2)
#define j9gp_handler_function() privatePortLibrary->gp_handler_function(privatePortLibrary)
#define j9str_startup() privatePortLibrary->str_startup(privatePortLibrary)
#define j9str_shutdown() privatePortLibrary->str_shutdown(privatePortLibrary)
#define j9str_printf privatePortLibrary->str_printf
#define j9str_vprintf(param1,param2,param3,param4) privatePortLibrary->str_vprintf(privatePortLibrary,param1,param2,param3,param4)
#define j9str_create_tokens(param1) privatePortLibrary->str_create_tokens(privatePortLibrary,param1)
#define j9str_set_token privatePortLibrary->str_set_token
#define j9str_subst_tokens(param1,param2,param3,param4) privatePortLibrary->str_subst_tokens(privatePortLibrary,param1,param2,param3,param4)
#define j9str_free_tokens(param1) privatePortLibrary->str_free_tokens(privatePortLibrary,param1)
#define j9str_set_time_tokens(param1,param2) privatePortLibrary->str_set_time_tokens(privatePortLibrary,param1,param2)
#define j9str_convert(param1,param2,param3,param4,param5,param6) privatePortLibrary->str_convert(privatePortLibrary,param1,param2,param3,param4,param5,param6)
#define j9exit_startup() privatePortLibrary->exit_startup(privatePortLibrary)
#define j9exit_shutdown() privatePortLibrary->exit_shutdown(privatePortLibrary)
#define j9exit_get_exit_code() privatePortLibrary->exit_get_exit_code(privatePortLibrary)
#define j9exit_shutdown_and_exit(param1) privatePortLibrary->exit_shutdown_and_exit(privatePortLibrary,param1)
#define j9dump_create(param1,param2,param3) privatePortLibrary->dump_create(privatePortLibrary,param1,param2,param3)
#define j9dump_startup() privatePortLibrary->dump_startup(privatePortLibrary)
#define j9dump_shutdown() privatePortLibrary->dump_shutdown(privatePortLibrary)
#define j9nls_startup() privatePortLibrary->nls_startup(privatePortLibrary)
#define j9nls_free_cached_data() privatePortLibrary->nls_free_cached_data(privatePortLibrary)
#define j9nls_shutdown() privatePortLibrary->nls_shutdown(privatePortLibrary)
#define j9nls_set_catalog(param1,param2,param3,param4) privatePortLibrary->nls_set_catalog(privatePortLibrary,param1,param2,param3,param4)
#define j9nls_set_locale(param1,param2,param3) privatePortLibrary->nls_set_locale(privatePortLibrary,param1,param2,param3)
#define j9nls_get_language() privatePortLibrary->nls_get_language(privatePortLibrary)
#define j9nls_get_region() privatePortLibrary->nls_get_region(privatePortLibrary)
#define j9nls_get_variant() privatePortLibrary->nls_get_variant(privatePortLibrary)
#define j9nls_printf privatePortLibrary->nls_printf
#define j9nls_vprintf(param1,param2,param3,param4) privatePortLibrary->nls_vprintf(privatePortLibrary,param1,param2,param3,param4)
#define j9nls_lookup_message(param1,param2,param3,param4) privatePortLibrary->nls_lookup_message(privatePortLibrary,param1,param2,param3,param4)
#define j9ipcmutex_startup() privatePortLibrary->ipcmutex_startup(privatePortLibrary)
#define j9ipcmutex_shutdown() privatePortLibrary->ipcmutex_shutdown(privatePortLibrary)
#define j9ipcmutex_acquire(param1) privatePortLibrary->ipcmutex_acquire(privatePortLibrary,param1)
#define j9ipcmutex_release(param1) privatePortLibrary->ipcmutex_release(privatePortLibrary,param1)
#define j9port_control(param1,param2) privatePortLibrary->port_control(privatePortLibrary,param1,param2)
#define j9sig_startup() privatePortLibrary->sig_startup(privatePortLibrary)
#define j9sig_shutdown() privatePortLibrary->sig_shutdown(privatePortLibrary)
#define j9sig_protect(param1,param2,param3,param4,param5,param6) privatePortLibrary->sig_protect(privatePortLibrary,param1,param2,param3,param4,param5,param6)
#define j9sig_can_protect(param1) privatePortLibrary->sig_can_protect(privatePortLibrary,param1)
#define j9sig_set_async_signal_handler(param1,param2,param3) privatePortLibrary->sig_set_async_signal_handler(privatePortLibrary,param1,param2,param3)
#define j9sig_info(param1,param2,param3,param4,param5) privatePortLibrary->sig_info(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9sig_info_count(param1,param2) privatePortLibrary->sig_info_count(privatePortLibrary,param1,param2)
#define j9sig_set_options(param1) privatePortLibrary->sig_set_options(privatePortLibrary,param1)
#define j9sig_get_options() privatePortLibrary->sig_get_options(privatePortLibrary)
#define j9sig_get_current_signal() privatePortLibrary->sig_get_current_signal(privatePortLibrary)
#define j9sig_set_reporter_priority(param1) privatePortLibrary->sig_set_reporter_priority(privatePortLibrary,param1)
#define j9sysinfo_DLPAR_enabled() privatePortLibrary->sysinfo_DLPAR_enabled(privatePortLibrary)
#define j9sysinfo_DLPAR_max_CPUs() privatePortLibrary->sysinfo_DLPAR_max_CPUs(privatePortLibrary)
#define j9sysinfo_weak_memory_consistency() privatePortLibrary->sysinfo_weak_memory_consistency(privatePortLibrary)
#define j9file_read_text(param1,param2,param3) privatePortLibrary->file_read_text(privatePortLibrary,param1,param2,param3)
#define j9file_mkdir(param1) privatePortLibrary->file_mkdir(privatePortLibrary,param1)
#define j9file_move(param1,param2) privatePortLibrary->file_move(privatePortLibrary,param1,param2)
#define j9file_unlinkdir(param1) privatePortLibrary->file_unlinkdir(privatePortLibrary,param1)
#define j9file_findfirst(param1,param2) privatePortLibrary->file_findfirst(privatePortLibrary,param1,param2)
#define j9file_findnext(param1,param2) privatePortLibrary->file_findnext(privatePortLibrary,param1,param2)
#define j9file_findclose(param1) privatePortLibrary->file_findclose(privatePortLibrary,param1)
#define j9file_error_message() privatePortLibrary->file_error_message(privatePortLibrary)
#define j9file_unlock_bytes(param1,param2,param3) privatePortLibrary->file_unlock_bytes(privatePortLibrary,param1,param2,param3)
#define j9file_lock_bytes(param1,param2,param3,param4) privatePortLibrary->file_lock_bytes(privatePortLibrary,param1,param2,param3,param4)
#define j9file_convert_native_fd_to_j9file_fd(param1) privatePortLibrary->file_convert_native_fd_to_j9file_fd(privatePortLibrary,param1)
#define j9sock_htonl(param1) privatePortLibrary->sock_htonl(privatePortLibrary,param1)
#define j9sock_bind(param1,param2) privatePortLibrary->sock_bind(privatePortLibrary,param1,param2)
#define j9sock_accept(param1,param2,param3) privatePortLibrary->sock_accept(privatePortLibrary,param1,param2,param3)
#define j9sock_shutdown_input(param1) privatePortLibrary->sock_shutdown_input(privatePortLibrary,param1)
#define j9sock_shutdown_output(param1) privatePortLibrary->sock_shutdown_output(privatePortLibrary,param1)
#define j9sock_listen(param1,param2) privatePortLibrary->sock_listen(privatePortLibrary,param1,param2)
#define j9sock_ntohl(param1) privatePortLibrary->sock_ntohl(privatePortLibrary,param1)
#define j9sock_ntohs(param1) privatePortLibrary->sock_ntohs(privatePortLibrary,param1)
#define j9sock_getpeername(param1,param2) privatePortLibrary->sock_getpeername(privatePortLibrary,param1,param2)
#define j9sock_getsockname(param1,param2) privatePortLibrary->sock_getsockname(privatePortLibrary,param1,param2)
#define j9sock_readfrom(param1,param2,param3,param4,param5) privatePortLibrary->sock_readfrom(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9sock_select(param1,param2,param3,param4,param5) privatePortLibrary->sock_select(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9sock_writeto(param1,param2,param3,param4,param5) privatePortLibrary->sock_writeto(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9sock_inetntoa(param1,param2) privatePortLibrary->sock_inetntoa(privatePortLibrary,param1,param2)
#define j9sock_gethostbyaddr(param1,param2,param3,param4) privatePortLibrary->sock_gethostbyaddr(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_gethostname(param1,param2) privatePortLibrary->sock_gethostname(privatePortLibrary,param1,param2)
#define j9sock_hostent_aliaslist(param1,param2) privatePortLibrary->sock_hostent_aliaslist(privatePortLibrary,param1,param2)
#define j9sock_hostent_hostname(param1,param2) privatePortLibrary->sock_hostent_hostname(privatePortLibrary,param1,param2)
#define j9sock_sockaddr_port(param1) privatePortLibrary->sock_sockaddr_port(privatePortLibrary,param1)
#define j9sock_sockaddr_address(param1) privatePortLibrary->sock_sockaddr_address(privatePortLibrary,param1)
#define j9sock_fdset_init(param1) privatePortLibrary->sock_fdset_init(privatePortLibrary,param1)
#define j9sock_fdset_size(param1) privatePortLibrary->sock_fdset_size(privatePortLibrary,param1)
#define j9sock_timeval_init(param1,param2,param3) privatePortLibrary->sock_timeval_init(privatePortLibrary,param1,param2,param3)
#define j9sock_getopt_int(param1,param2,param3,param4) privatePortLibrary->sock_getopt_int(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_setopt_int(param1,param2,param3,param4) privatePortLibrary->sock_setopt_int(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_getopt_bool(param1,param2,param3,param4) privatePortLibrary->sock_getopt_bool(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_setopt_bool(param1,param2,param3,param4) privatePortLibrary->sock_setopt_bool(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_getopt_byte(param1,param2,param3,param4) privatePortLibrary->sock_getopt_byte(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_setopt_byte(param1,param2,param3,param4) privatePortLibrary->sock_setopt_byte(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_getopt_linger(param1,param2,param3,param4) privatePortLibrary->sock_getopt_linger(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_getopt_sockaddr(param1,param2,param3,param4) privatePortLibrary->sock_getopt_sockaddr(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_setopt_sockaddr(param1,param2,param3,param4) privatePortLibrary->sock_setopt_sockaddr(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_setopt_ipmreq(param1,param2,param3,param4) privatePortLibrary->sock_setopt_ipmreq(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_linger_enabled(param1,param2) privatePortLibrary->sock_linger_enabled(privatePortLibrary,param1,param2)
#define j9sock_linger_linger(param1,param2) privatePortLibrary->sock_linger_linger(privatePortLibrary,param1,param2)
#define j9sock_ipmreq_init(param1,param2,param3) privatePortLibrary->sock_ipmreq_init(privatePortLibrary,param1,param2,param3)
#define j9sock_setflag(param1,param2) privatePortLibrary->sock_setflag(privatePortLibrary,param1,param2)
#define j9sock_freeaddrinfo(param1) privatePortLibrary->sock_freeaddrinfo(privatePortLibrary,param1)
#define j9sock_getaddrinfo(param1,param2,param3) privatePortLibrary->sock_getaddrinfo(privatePortLibrary,param1,param2,param3)
#define j9sock_getaddrinfo_address(param1,param2,param3,param4) privatePortLibrary->sock_getaddrinfo_address(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_getaddrinfo_create_hints(param1,param2,param3,param4,param5) privatePortLibrary->sock_getaddrinfo_create_hints(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9sock_getaddrinfo_family(param1,param2,param3) privatePortLibrary->sock_getaddrinfo_family(privatePortLibrary,param1,param2,param3)
#define j9sock_getaddrinfo_length(param1,param2) privatePortLibrary->sock_getaddrinfo_length(privatePortLibrary,param1,param2)
#define j9sock_getaddrinfo_name(param1,param2,param3) privatePortLibrary->sock_getaddrinfo_name(privatePortLibrary,param1,param2,param3)
#define j9sock_getnameinfo(param1,param2,param3,param4,param5) privatePortLibrary->sock_getnameinfo(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9sock_ipv6_mreq_init(param1,param2,param3) privatePortLibrary->sock_ipv6_mreq_init(privatePortLibrary,param1,param2,param3)
#define j9sock_setopt_ipv6_mreq(param1,param2,param3,param4) privatePortLibrary->sock_setopt_ipv6_mreq(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_sockaddr_address6(param1,param2,param3,param4) privatePortLibrary->sock_sockaddr_address6(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_sockaddr_family(param1,param2) privatePortLibrary->sock_sockaddr_family(privatePortLibrary,param1,param2)
#define j9sock_sockaddr_init6(param1,param2,param3,param4,param5,param6,param7,param8) privatePortLibrary->sock_sockaddr_init6(privatePortLibrary,param1,param2,param3,param4,param5,param6,param7,param8)
#define j9sock_socketIsValid(param1) privatePortLibrary->sock_socketIsValid(privatePortLibrary,param1)
#define j9sock_select_read(param1,param2,param3,param4) privatePortLibrary->sock_select_read(privatePortLibrary,param1,param2,param3,param4)
#define j9sock_set_nonblocking(param1,param2) privatePortLibrary->sock_set_nonblocking(privatePortLibrary,param1,param2)
#define j9sock_error_message() privatePortLibrary->sock_error_message(privatePortLibrary)
#define j9sock_get_network_interfaces(param1,param2) privatePortLibrary->sock_get_network_interfaces(privatePortLibrary,param1,param2)
#define j9sock_free_network_interface_struct(param1) privatePortLibrary->sock_free_network_interface_struct(privatePortLibrary,param1)
#define j9sock_connect_with_timeout(param1,param2,param3,param4,param5) privatePortLibrary->sock_connect_with_timeout(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9sock_fdset_zero(param1) privatePortLibrary->sock_fdset_zero(privatePortLibrary,param1)
#define j9sock_fdset_set(param1,param2) privatePortLibrary->sock_fdset_set(privatePortLibrary,param1,param2)
#define j9sock_fdset_clr(param1,param2) privatePortLibrary->sock_fdset_clr(privatePortLibrary,param1,param2)
#define j9sock_fdset_isset(param1,param2) privatePortLibrary->sock_fdset_isset(privatePortLibrary,param1,param2)
#define j9str_ftime(param1,param2,param3,param4) privatePortLibrary->str_ftime(privatePortLibrary,param1,param2,param3,param4)
#define j9mmap_startup() privatePortLibrary->mmap_startup(privatePortLibrary)
#define j9mmap_shutdown() privatePortLibrary->mmap_shutdown(privatePortLibrary)
#define j9mmap_capabilities() privatePortLibrary->mmap_capabilities(privatePortLibrary)
#define j9mmap_map_file(param1,param2,param3,param4,param5,param6) privatePortLibrary->mmap_map_file(privatePortLibrary,param1,param2,param3,param4,param5,param6)
#define j9mmap_unmap_file(param1) privatePortLibrary->mmap_unmap_file(privatePortLibrary,param1)
#define j9mmap_msync(param1,param2,param3) privatePortLibrary->mmap_msync(privatePortLibrary,param1,param2,param3)
#define j9mmap_protect(param1,param2,param3) privatePortLibrary->mmap_protect(privatePortLibrary,param1,param2,param3)
#define j9mmap_get_region_granularity(param1) privatePortLibrary->mmap_get_region_granularity(privatePortLibrary,param1)
#define j9shsem_params_init(param1) privatePortLibrary->shsem_params_init(privatePortLibrary,param1)
#define j9shsem_startup() privatePortLibrary->shsem_startup(privatePortLibrary)
#define j9shsem_shutdown() privatePortLibrary->shsem_shutdown(privatePortLibrary)
#define j9shsem_open(param1,param2) privatePortLibrary->shsem_open(privatePortLibrary,param1,param2)
#define j9shsem_post(param1,param2,param3) privatePortLibrary->shsem_post(privatePortLibrary,param1,param2,param3)
#define j9shsem_wait(param1,param2,param3) privatePortLibrary->shsem_wait(privatePortLibrary,param1,param2,param3)
#define j9shsem_getVal(param1,param2) privatePortLibrary->shsem_getVal(privatePortLibrary,param1,param2)
#define j9shsem_setVal(param1,param2,param3) privatePortLibrary->shsem_setVal(privatePortLibrary,param1,param2,param3)
#define j9shsem_close(param1) privatePortLibrary->shsem_close(privatePortLibrary,param1)
#define j9shsem_destroy(param1) privatePortLibrary->shsem_destroy(privatePortLibrary,param1)
#define j9shsem_deprecated_startup() privatePortLibrary->shsem_deprecated_startup(privatePortLibrary)
#define j9shsem_deprecated_shutdown() privatePortLibrary->shsem_deprecated_shutdown(privatePortLibrary)
#define j9shsem_deprecated_open(param1,param2,param3,param4,param5,param6,param7,param8) privatePortLibrary->shsem_deprecated_open(privatePortLibrary,param1,param2,param3,param4,param5,param6,param7,param8)
#define j9shsem_deprecated_openDeprecated(param1,param2,param3,param4,param5) privatePortLibrary->shsem_deprecated_openDeprecated(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9shsem_deprecated_post(param1,param2,param3) privatePortLibrary->shsem_deprecated_post(privatePortLibrary,param1,param2,param3)
#define j9shsem_deprecated_wait(param1,param2,param3) privatePortLibrary->shsem_deprecated_wait(privatePortLibrary,param1,param2,param3)
#define j9shsem_deprecated_getVal(param1,param2) privatePortLibrary->shsem_deprecated_getVal(privatePortLibrary,param1,param2)
#define j9shsem_deprecated_setVal(param1,param2,param3) privatePortLibrary->shsem_deprecated_setVal(privatePortLibrary,param1,param2,param3)
#define j9shsem_deprecated_handle_stat(param1,param2) privatePortLibrary->shsem_deprecated_handle_stat(privatePortLibrary,param1,param2)
#define j9shsem_deprecated_close(param1) privatePortLibrary->shsem_deprecated_close(privatePortLibrary,param1)
#define j9shsem_deprecated_destroy(param1) privatePortLibrary->shsem_deprecated_destroy(privatePortLibrary,param1)
#define j9shsem_deprecated_destroyDeprecated(param1,param2) privatePortLibrary->shsem_deprecated_destroyDeprecated(privatePortLibrary,param1,param2)
#define j9shsem_deprecated_getid(param1) privatePortLibrary->shsem_deprecated_getid(privatePortLibrary,param1)
#define j9shmem_startup() privatePortLibrary->shmem_startup(privatePortLibrary)
#define j9shmem_shutdown() privatePortLibrary->shmem_shutdown(privatePortLibrary)
#define j9shmem_open(param1,param2,param3,param4,param5,param6,param7,param8,param9) privatePortLibrary->shmem_open(privatePortLibrary,param1,param2,param3,param4,param5,param6,param7,param8,param9)
#define j9shmem_openDeprecated(param1,param2,param3,param4,param5,param6,param7) privatePortLibrary->shmem_openDeprecated(privatePortLibrary,param1,param2,param3,param4,param5,param6,param7)
#define j9shmem_attach(param1,param2) privatePortLibrary->shmem_attach(privatePortLibrary,param1,param2)
#define j9shmem_detach(param1) privatePortLibrary->shmem_detach(privatePortLibrary,param1)
#define j9shmem_close(param1) privatePortLibrary->shmem_close(privatePortLibrary,param1)
#define j9shmem_destroy(param1,param2,param3) privatePortLibrary->shmem_destroy(privatePortLibrary,param1,param2,param3)
#define j9shmem_destroyDeprecated(param1,param2,param3,param4) privatePortLibrary->shmem_destroyDeprecated(privatePortLibrary,param1,param2,param3,param4)
#define j9shmem_findfirst(param1,param2) privatePortLibrary->shmem_findfirst(privatePortLibrary,param1,param2)
#define j9shmem_findnext(param1,param2) privatePortLibrary->shmem_findnext(privatePortLibrary,param1,param2)
#define j9shmem_findclose(param1) privatePortLibrary->shmem_findclose(privatePortLibrary,param1)
#define j9shmem_stat(param1,param2,param3,param4) privatePortLibrary->shmem_stat(privatePortLibrary,param1,param2,param3,param4)
#define j9shmem_statDeprecated(param1,param2,param3,param4,param5) privatePortLibrary->shmem_statDeprecated(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9shmem_handle_stat(param1,param2) privatePortLibrary->shmem_handle_stat(privatePortLibrary,param1,param2)
#define j9shmem_getDir(param1,param2,param3,param4) privatePortLibrary->shmem_getDir(privatePortLibrary,param1,param2,param3,param4)
#define j9shmem_createDir(param1,param2,param3) privatePortLibrary->shmem_createDir(privatePortLibrary,param1,param2,param3)
#define j9shmem_getFilepath(param1,param2,param3,param4) privatePortLibrary->shmem_getFilepath(privatePortLibrary,param1,param2,param3,param4)
#define j9shmem_protect(param1,param2,param3,param4,param5) privatePortLibrary->shmem_protect(privatePortLibrary,param1,param2,param3,param4,param5)
#define j9shmem_get_region_granularity(param1,param2,param3) privatePortLibrary->shmem_get_region_granularity(privatePortLibrary,param1,param2,param3)
#define j9shmem_getid(param1) privatePortLibrary->shmem_getid(privatePortLibrary,param1)
#define j9sysinfo_get_limit(param1,param2) privatePortLibrary->sysinfo_get_limit(privatePortLibrary,param1,param2)
#define j9sysinfo_set_limit(param1,param2) privatePortLibrary->sysinfo_set_limit(privatePortLibrary,param1,param2)
#define j9sysinfo_get_processing_capacity() privatePortLibrary->sysinfo_get_processing_capacity(privatePortLibrary)
#define j9sysinfo_get_number_CPUs_by_type(param1) privatePortLibrary->sysinfo_get_number_CPUs_by_type(privatePortLibrary,param1)
#define j9sysinfo_set_number_entitled_CPUs(param1) privatePortLibrary->sysinfo_set_number_entitled_CPUs(privatePortLibrary,param1)
#define j9sysinfo_get_cwd(param1,param2) privatePortLibrary->sysinfo_get_cwd(privatePortLibrary,param1,param2)
#define j9sysinfo_get_tmp(param1,param2,param3) privatePortLibrary->sysinfo_get_tmp(privatePortLibrary,param1,param2,param3)
#define j9syslog_write(param1,param2) privatePortLibrary->syslog_write(privatePortLibrary,param1,param2)
#define j9virt_rwlock_startup() privatePortLibrary->virt_rwlock_startup(privatePortLibrary)
#define j9virt_rwlock_shutdown() privatePortLibrary->virt_rwlock_shutdown(privatePortLibrary)
#define j9virt_rwlock_initialize(param1) privatePortLibrary->virt_rwlock_initialize(privatePortLibrary,param1)
#define j9virt_rwlock_acquire_shared(param1) privatePortLibrary->virt_rwlock_acquire_shared(privatePortLibrary,param1)
#define j9virt_rwlock_release_shared(param1) privatePortLibrary->virt_rwlock_release_shared(privatePortLibrary,param1)
#define j9virt_rwlock_acquire_exclusive(param1,param2,param3) privatePortLibrary->virt_rwlock_acquire_exclusive(privatePortLibrary,param1,param2,param3)
#define j9virt_rwlock_release_exclusive(param1,param2) privatePortLibrary->virt_rwlock_release_exclusive(privatePortLibrary,param1,param2)
#define j9hypervisor_startup() privatePortLibrary->hypervisor_startup(privatePortLibrary)
#define j9hypervisor_shutdown() privatePortLibrary->hypervisor_shutdown(privatePortLibrary)
#define j9hypervisor_initialize_virtualization(param1,param2) privatePortLibrary->hypervisor_initialize_virtualization(privatePortLibrary,param1,param2)
#define j9hypervisor_is_virtualization_initialized() privatePortLibrary->hypervisor_is_virtualization_initialized(privatePortLibrary)
#define j9hypervisor_setup_shared_object_access(param1,param2,param3) privatePortLibrary->hypervisor_setup_shared_object_access(privatePortLibrary,param1,param2,param3)
#define j9hypervisor_destroy_shared_object_access() privatePortLibrary->hypervisor_destroy_shared_object_access(privatePortLibrary)
#define j9hypervisor_delete_shared_object(param1) privatePortLibrary->hypervisor_delete_shared_object(privatePortLibrary,param1)
#define j9hypervisor_map_shared_object(param1) privatePortLibrary->hypervisor_map_shared_object(privatePortLibrary,param1)
#define j9hypervisor_unmap_shared_object(param1) privatePortLibrary->hypervisor_unmap_shared_object(privatePortLibrary,param1)
#define j9hypervisor_protect(param1,param2,param3) privatePortLibrary->hypervisor_protect(privatePortLibrary,param1,param2,param3)
#define j9hypervisor_sync(param1,param2,param3) privatePortLibrary->hypervisor_sync(privatePortLibrary,param1,param2,param3)
#define j9hypervisor_get_shared_object_size() privatePortLibrary->hypervisor_get_shared_object_size(privatePortLibrary)
#define j9hypervisor_get_guest_id() privatePortLibrary->hypervisor_get_guest_id(privatePortLibrary)
#define j9hypervisor_hypervisor_present() privatePortLibrary->hypervisor_hypervisor_present(privatePortLibrary)
#define j9hypervisor_get_hypervisor_info(param1) privatePortLibrary->hypervisor_get_hypervisor_info(privatePortLibrary,param1)
#define j9hypervisor_get_guest_processor_usage(param1) privatePortLibrary->hypervisor_get_guest_processor_usage(privatePortLibrary,param1)
#define j9hypervisor_get_guest_memory_usage(param1) privatePortLibrary->hypervisor_get_guest_memory_usage(privatePortLibrary,param1)
#define j9process_create(param1,param2,param3,param4,param5,param6,param7,param8,param9,param10) privatePortLibrary->process_create(privatePortLibrary,param1,param2,param3,param4,param5,param6,param7,param8,param9,param10)
#define j9process_waitfor(param1) privatePortLibrary->process_waitfor(privatePortLibrary,param1)
#define j9process_terminate(param1) privatePortLibrary->process_terminate(privatePortLibrary,param1)
#define j9process_write(param1,param2,param3) privatePortLibrary->process_write(privatePortLibrary,param1,param2,param3)
#define j9process_read(param1,param2,param3,param4) privatePortLibrary->process_read(privatePortLibrary,param1,param2,param3,param4)
#define j9process_get_available(param1,param2) privatePortLibrary->process_get_available(privatePortLibrary,param1,param2)
#define j9process_close(param1,param2) privatePortLibrary->process_close(privatePortLibrary,param1,param2)
#define j9process_getStream(param1,param2,param3) privatePortLibrary->process_getStream(privatePortLibrary,param1,param2,param3)
#define j9process_isComplete(param1) privatePortLibrary->process_isComplete(privatePortLibrary,param1)
#define j9process_get_exitCode(param1) privatePortLibrary->process_get_exitCode(privatePortLibrary,param1)
#define j9introspect_threads_startDo(param1,param2) privatePortLibrary->introspect_threads_startDo(privatePortLibrary,param1,param2)
#define j9introspect_threads_startDo_with_signal(param1,param2,param3) privatePortLibrary->introspect_threads_startDo_with_signal(privatePortLibrary,param1,param2,param3)
#define j9introspect_threads_nextDo() privatePortLibrary->introspect_threads_nextDo(privatePortLibrary)
#define j9introspect_backtrace_thread(param1,param2,param3) privatePortLibrary->introspect_backtrace_thread(privatePortLibrary,param1,param2,param3)
#define j9introspect_backtrace_symbols(param1,param2) privatePortLibrary->introspect_backtrace_symbols(privatePortLibrary,param1,param2)
#define j9syslog_query() privatePortLibrary->syslog_query(privatePortLibrary)
#define j9syslog_set(param1) privatePortLibrary->syslog_set(privatePortLibrary,param1)
#define j9mem_walk_categories(param1) privatePortLibrary->mem_walk_categories(privatePortLibrary,param1)
#define j9heap_query_size(param1,param2) privatePortLibrary->heap_query_size(privatePortLibrary,param1,param2)
#define j9heap_grow(param1,param2) privatePortLibrary->heap_grow(privatePortLibrary,param1,param2)
#if defined(J9VM_PORT_RUNTIME_INSTRUMENTATION)
#define j9ri_params_init(param1,param2) privatePortLibrary->ri_params_init(privatePortLibrary,param1,param2)
#define j9ri_initialize(param1) privatePortLibrary->ri_initialize(privatePortLibrary,param1)
#define j9ri_deinitialize(param1) privatePortLibrary->ri_deinitialize(privatePortLibrary,param1)
#define j9ri_enable(param1) privatePortLibrary->ri_enable(privatePortLibrary,param1)
#define j9ri_disable(param1) privatePortLibrary->ri_disable(privatePortLibrary,param1)
#endif /* J9VM_PORT_RUNTIME_INSTRUMENTATION */


#undef j9nls_lookup_message
#define j9nls_lookup_message(param1,param2,param3) privatePortLibrary->nls_lookup_message(privatePortLibrary,param1,param2,param3)

#undef j9nls_vprintf
#define j9nls_vprintf(param1,param2,param3) privatePortLibrary->j9nls_vprintf(privatePortLibrary,param1,param2,param3)

#undef j9mem_allocate_memory
#define j9mem_allocate_memory(param1, category) privatePortLibrary->mem_allocate_memory(privatePortLibrary,param1, J9_GET_CALLSITE(), category)
#undef j9mem_allocate_memory32
#define j9mem_allocate_memory32(param1, category) privatePortLibrary->mem_allocate_memory32(privatePortLibrary,param1, J9_GET_CALLSITE(), category)
#undef j9mem_reallocate_memory
#define j9mem_reallocate_memory(param1, param2, category) privatePortLibrary->mem_reallocate_memory(privatePortLibrary,param1,param2,J9_GET_CALLSITE(), category)

#undef j9sysinfo_get_memory_info
#define j9sysinfo_get_memory_info(param1) privatePortLibrary->sysinfo_get_memory_info(privatePortLibrary,param1)

#define j9sysinfo_get_memory_info_with_flags(param1,param2) privatePortLibrary->sysinfo_get_memory_info(privatePortLibrary,param1,param2)



#endif /* !J9PORT_LIBRARY_DEFINE */

/** @} */

#endif
