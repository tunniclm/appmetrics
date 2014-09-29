/*******************************************************************************
 * Licensed Materials - Property of IBM
 * "Restricted Materials of IBM"
 *
 * (c) Copyright IBM Corp. 2014 All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 *******************************************************************************/

#ifndef TRACEFORMAT_H_
#define TRACEFORMAT_H_

/**
 * Trace formatting functions. Exposed as external symbols.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * A callback to obtain a format string for a trace point with id tracepoint in
 * component componentName.
 *
 * Typically the format string will be obtained from a trace format .dat file though
 * this is not required. Care must be taken that the format file matches the original
 * trace point data as it will contain format specifiers which will be filled in when
 * the trace data for this trace point is formatted.
 *
 * If the string cannot be found a constant string with no inserts should be returned
 * rather than null.
 * (For example "UNKNOWN TRACEPOINT ID")
 *
 * @param[in] componentName the name of the component this trace point belongs to, for example j9mm
 * @param[in] tracepoint the trace point number within the component, for example 123
 */
typedef char* (*FormatStringCallback)(const char *componentName, I_32 tracepoint);
struct UtTraceFileIterator;
typedef struct  UtTraceFileIterator UtTraceFileIterator;
struct UtTracePointIterator;
typedef struct  UtTracePointIterator UtTracePointIterator;

/**
 * Obtain a UtTraceFileIterator for the file named in fileName.
 *
 * This iterator can then be used to obtain trace point iterators over each
 * buffer in the file via calls to omr_trc_getTracePointIteratorForNextBuffer
 *
 * @param[in] portLib An initialised J9PortLibraryStructure.
 * @param[in] fileName The name of the trace file to open.
 * @param[in,out] iteratorPtr A pointer to a location where the initialised UtTraceFileIterator pointer can be stored.
 * @param[in] getFormatString A callback the formatter can use to obtain a format string for a trace point id in a named module.
 *
 * @return OMR_ERROR_NONE on success
 * @return OMR_ERROR_NOT_AVAILABLE if the specified file cannot be opened
 * @return OMR_ERROR_ILLEGAL_ARGUMENT if the specified file does not contain valid trace data.
 * @return OMR_ERROR_OUT_OF_NATIVE_MEMORY if memory for the iterator structure cannot be allocated.
 * @return OMR_ERROR_NOT_AVAILABLE if the specified file cannot be opened.
 */
omr_error_t omr_trc_getTraceFileIterator(J9PortLibrary *portLib, char* fileName, UtTraceFileIterator** iteratorPtr, FormatStringCallback getFormatString);

/**
 * Free a trace file iterator, any memory associated with it and
 * close the trace file it opened.
 *
 * @param[in] iter the UtTraceFileIterator to free
 * @return OMR_ERROR_NONE on success
 */
omr_error_t omr_trc_freeTraceFileIterator(UtTraceFileIterator* iter);

/**
 * Obtain an UtTracePointIterator for the next trace buffer in a file opened by a UtTraceFileIterator.
 * If there are no more trace buffers in the file *bufferIteratorPtr will point to NULL.
 *
 * @param[in] fileIter A pointer to the UtTraceFileIterator that will return an iterator over it's next buffer.
 * @param[in,out] bufferIteratorPtr A pointer to a location where the initialised UtTracePointIterator pointer can be stored.
 * @return OMR_ERROR_NONE on success, including if there are no more buffers in the file at EOF.
 * @return OMR_ERROR_OUT_OF_NATIVE_MEMORY if memory for the iterator structure cannot be allocated.
 * @return OMR_ERROR_INTERNAL if the file ends unexpectedly.
 */
omr_error_t omr_trc_getTracePointIteratorForNextBuffer(UtTraceFileIterator* fileIter, UtTracePointIterator **bufferIteratorPtr);

/**
 * Format the next trace point available from iter into buffer.
 * Returns a pointer to buffer on success or NULL if no more trace
 * can be formatted from this buffer.
 *
 * @param[in] iter the UtTracePointIterator to obtain the next trace point from.
 * @param[in,out] buffer the buffer to format the trace point into
 * @param[in] buffLen the length of the buffer
 * @return a pointer to buffer or NULL if there are no more trace points available
 */
char * omr_trc_formatNextTracePoint(UtTracePointIterator *iter, char *buffer, U_32 buffLen);

/**
 * Free a trace point iterator and any memory associated with it
 * All UtTracePointIterators obtained from a file iterator should
 * be free'd before the UtTraceFileIterator is free'd.
 *
 * @param[in] iter the UtTracePointIterator to free
 * @return OMR_ERROR_NONE on success
 */
omr_error_t omr_trc_freeTracePointIterator(UtTracePointIterator *iter);

/**
 * Return the thread id associated with the trace buffer this
 * UtTracePointIterator is formatting.
 *
 * @param[in] iter the UtTracePointIterator
 * @return the thread id.
 */
U_64 omr_trc_getBufferIteratorThreadId(UtTracePointIterator* iter);

/**
 * Copies the thread name associated with this trace buffer into buffer.
 * The length of the string is returned (not including the trailing NULL).
 * If this is equal to or greater than buffLen the string will have been
 * truncated.

 * @param[in] iter the UtTracePointIterator
 * @param[in,out] buffer the UtTracePointIterator
 * @param[buffLen] the size of buffer
 * @return the number of characters written to buffer
 */
U_32 omr_trc_getBufferIteratorThreadName(UtTracePointIterator* iter, char* buffer, U_32 buffLen);

#ifdef __cplusplus
}
#endif

#endif /* TRACEFORMAT_H_ */
