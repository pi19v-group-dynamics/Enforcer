#pragma once

#include <stdio.h>

enum /* log levels */
{
	LOG_TRACE = 0,
	LOG_DEBUG = 1,
	LOG_INFO  = 2,
	LOG_WARN  = 3,
	LOG_ERROR = 4,
	LOG_FATAL = 5
};

enum /* log allow bitfield values */
{
	LOG_OFF = 0x0,
	LOG_TRACE_BIT = 1 << LOG_TRACE,
	LOG_DEBUG_BIT = 1 << LOG_DEBUG,
	LOG_INFO_BIT  = 1 << LOG_INFO,
	LOG_WARN_BIT  = 1 << LOG_WARN,
	LOG_ERROR_BIT = 1 << LOG_ERROR,
	LOG_FATAL_BIT = 1 << LOG_FATAL,
	LOG_ALL = LOG_TRACE_BIT | LOG_DEBUG_BIT | LOG_INFO_BIT
	        | LOG_WARN_BIT | LOG_ERROR_BIT | LOG_FATAL_BIT
};

#define log_trace(...) log_write(LOG_TRACE, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_write(LOG_DEBUG, __FILE__, __func__, __LINE__, __VA_ARGS__) 
#define log_info(...)  log_write(LOG_INFO,  __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_write(LOG_WARN,  __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_error(...) log_write(LOG_ERROR, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_write(LOG_FATAL, __FILE__, __func__, __LINE__, __VA_ARGS__)

/* Set logging stream */
void log_stream(FILE* fp);
/* Generic function to printing logs */
void log_write(int lvl, const char* restrict file, const char* restrict func, int line, const char* restrict fmt, ...);

