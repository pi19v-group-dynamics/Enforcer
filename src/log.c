#include "log.h"
#include <time.h>
#include <stdarg.h>
#include <stdatomic.h>

#ifndef LOG_ALLOW
#define LOG_ALLOW LOG_ALL
#endif /* LOG_ALLOW */

static FILE* log_fp; /* stderr by default */
static atomic_flag log_flag = ATOMIC_FLAG_INIT;

#define lock() while (atomic_flag_test_and_set(&log_flag))
#define unlock() atomic_flag_clear(&log_flag)

__attribute__((constructor(101))) static void log_setup(void)
{
	log_fp = stderr;
	log_debug("Log output directed to stderr");
}

void log_stream(FILE* fp)
{
	lock();
	log_fp = fp;
	unlock();
}

void log_write(int lvl, const char* restrict file, const char* restrict func, int line, const char* restrict fmt, ...)
{
	static char buf[16];
	time_t t = time(NULL);
	va_list va;
	if (LOG_ALLOW & (1 << lvl)) /* check to allowing this logging level */
	{
		lock();
		va_start(va, fmt);
		buf[strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&t))] = '\0';	
		fprintf(log_fp, "[%s] %-5s %s:%s():%d ", buf,
			((const char* restrict const[]){"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"})[lvl], file, func, line);
		vfprintf(log_fp, fmt, va);
		fprintf(log_fp, "\n");
		fflush(log_fp);
		va_end(va);
		unlock();
	}
}

