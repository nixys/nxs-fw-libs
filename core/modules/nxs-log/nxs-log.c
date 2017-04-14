#include <nxs-core/nxs-core.h>

#define NXS_LOG_MSG_BUF_SIZE			16384

#define NXS_LOG_TIME_STR_LEN			20
#define NXS_LOG_TIME_STR_FULL_LEN		27

#define _NXS_LOG_LEVEL_MEM				"MEMORY:\t\t"
#define _NXS_LOG_LEVEL_INFO				"INFO:\t\t"
#define _NXS_LOG_LEVEL_WARN				"WARNING:\t"
#define _NXS_LOG_LEVEL_ERROR			"ERROR:\t\t"
#define _NXS_LOG_LEVEL_DEBUG			"DEBUG:\t\t"
#define _NXS_LOG_LEVEL_DEBUG_NET		"NET-DEBUG:\t"
#define _NXS_LOG_LEVEL_ALL				"ALL:\t\t\t"
#define _NXS_LOG_LEVEL_UNKNWN			"UNKNOWN:\t"

#define NXS_LOG_FORMAT_FILE				"[%s]\t(%d)\t%s%s\n"
#define NXS_LOG_FORMAT_CONSOLE			"%s\n"
#define NXS_LOG_FORMAT_CONSOLE_PSNAME	"%s: %s\n"
#define	NXS_LOG_FORMAT_TIME				"%d.%m.%Y %T"
#define	NXS_LOG_FORMAT_TIME_FULL		"%s.%.6lu"

static	int 	nxs_log_open			(nxs_log_t *log);
static	int		nxs_log_close			(nxs_log_t *log);
static	void	nxs_log_str_prepare		(nxs_log_t *log, const char *fmt, va_list args);
static	char	*nxs_log_str_level		(int level);
static	void	nxs_log_str_time		(char *time_str);

static int nxs_log_open(nxs_log_t *log)
{

	if(log->fp == NULL){

		log->fp = fopen((const char *)nxs_string_str(log->path), "a");

		if(log->fp == NULL){

			printf("log write error: %s (log-path: \"%s\")\n", strerror(errno), (const char *)nxs_string_str(log->path));

			return NXS_LOG_FP_ERROR_OPEN;
		}
	}

	return NXS_LOG_FP_OK;
}

static int nxs_log_close(nxs_log_t *log)
{

	fclose(log->fp);
	log->fp = NULL;

	return NXS_LOG_FP_OK;
}

static void nxs_log_str_prepare(nxs_log_t *log, const char *fmt, va_list args)
{
	size_t 		n;
	va_list		msg_args;

	va_copy(msg_args, args);
	n = vsnprintf((char *)log->tmp_msg_buf->str, log->tmp_msg_buf->size, fmt, msg_args);
	va_end(msg_args);

	if(n >= log->tmp_msg_buf->size){

		nxs_string_resize(log->tmp_msg_buf, n + 1);

		va_copy(msg_args, args);
		n = vsnprintf((char *)log->tmp_msg_buf->str, log->tmp_msg_buf->size, fmt, msg_args);
		va_end(msg_args);
	}

	log->tmp_msg_buf->len = n;
}

static char *nxs_log_str_level(int level)
{

	switch(level){

		case NXS_LOG_LEVEL_MEM:
			return _NXS_LOG_LEVEL_MEM;

		case NXS_LOG_LEVEL_INFO:
			return _NXS_LOG_LEVEL_INFO;

		case NXS_LOG_LEVEL_WARN:
			return _NXS_LOG_LEVEL_WARN;

		case NXS_LOG_LEVEL_ERROR:
			return _NXS_LOG_LEVEL_ERROR;

		case NXS_LOG_LEVEL_DEBUG:
			return _NXS_LOG_LEVEL_DEBUG;

		case NXS_LOG_LEVEL_DEBUG_NET:
			return _NXS_LOG_LEVEL_DEBUG_NET;

		case NXS_LOG_LEVEL_ALL:
			return _NXS_LOG_LEVEL_ALL;
	}

	return _NXS_LOG_LEVEL_UNKNWN;
}

static void nxs_log_str_time(char *time_str)
{
	time_t			now;
	struct tm		*t;
	struct timeval	micro_s;
	char			t_s[NXS_LOG_TIME_STR_LEN];

	now = time(NULL);
	t = localtime(&now);
	strftime(t_s, NXS_LOG_TIME_STR_LEN, NXS_LOG_FORMAT_TIME, t);

	gettimeofday(&micro_s, NULL);

	snprintf(time_str, NXS_LOG_TIME_STR_FULL_LEN, NXS_LOG_FORMAT_TIME_FULL, t_s, micro_s.tv_usec);
}

nxs_log_t *nxs_log_malloc(int level, int mode, int use_syslog, nxs_string_t *path)
{
	nxs_log_t *log = NULL;

	log = nxs_malloc(log, sizeof(nxs_log_t));

	nxs_log_init(log, level, mode, use_syslog, path);

	return log;
}

nxs_log_t *nxs_log_destroy(nxs_log_t *log)
{

	if(log == NULL){

		return NULL;
	}

	nxs_log_free(log);

	return nxs_free(log);
}

void nxs_log_free(nxs_log_t *log)
{

	if(log->fp != NULL){

		fclose(log->fp);

		log->fp = NULL;
	}

	log->level = 0;
	log->mode = 0;
	log->use_syslog = 0;

	log->path = nxs_string_destroy(log->path);
	log->tmp_msg_buf = nxs_string_destroy(log->tmp_msg_buf);
}

void nxs_log_init(nxs_log_t *log, int level, int mode, int use_syslog, nxs_string_t *path)
{

	log->fp = NULL;
	log->level = level;
	log->mode = mode;
	log->use_syslog = use_syslog;

	log->path = NULL;

	if(path != NULL && nxs_string_size(path) != 0 && nxs_string_len(path) != 0){

		log->path = nxs_string_malloc(0, nxs_string_str(path));
	}

	log->tmp_msg_buf = nxs_string_malloc(NXS_LOG_MSG_BUF_SIZE, NULL);
}

void nxs_log_reopen(nxs_process_t *proc)
{

	nxs_log_close(proc->log);
}

void nxs_log_write(nxs_process_t *proc, int level, const char *msg, ...)
{
	va_list		log_args;
	char 		*level_str, time_str[NXS_LOG_TIME_STR_FULL_LEN];

	if(proc->log == NULL){

		return;
	}

	if((level & proc->log->level) == 0){

		return;
	}

	va_start(log_args, msg);

	if(proc->log->path != NULL){

		if(nxs_log_open(proc->log) == NXS_LOG_FP_OK){

			nxs_log_str_time(time_str);
			level_str = nxs_log_str_level(level);
			nxs_log_str_prepare(proc->log, msg, log_args);

			fprintf(proc->log->fp, NXS_LOG_FORMAT_FILE, time_str, proc->pid, level_str, (char *)nxs_string_str(proc->log->tmp_msg_buf));

			if(proc->log->mode == NXS_LOG_MODE_REOPEN){

				nxs_log_close(proc->log);
			}
			else{

				if(fflush(proc->log->fp) < 0){

					nxs_log_close(proc->log);
				}
			}
		}
	}
	else{

		nxs_log_str_prepare(proc->log, msg, log_args);

		if(nxs_string_len(proc->ps_name) == 0){

			printf(NXS_LOG_FORMAT_CONSOLE, (char *)nxs_string_str(proc->log->tmp_msg_buf));
		}
		else{

			printf(NXS_LOG_FORMAT_CONSOLE_PSNAME, proc->ps_name->str, (char *)nxs_string_str(proc->log->tmp_msg_buf));
		}
	}

	va_end(log_args);
}

void nxs_log_write_console(nxs_process_t *proc, const char *msg, ...)
{
	va_list		log_args;

	if(proc->log == NULL){

		return;
	}

	va_start(log_args, msg);

	nxs_log_str_prepare(proc->log, msg, log_args);

	printf(NXS_LOG_FORMAT_CONSOLE, (char *)nxs_string_str(proc->log->tmp_msg_buf));

	va_end(log_args);
}

void nxs_log_write_raw(nxs_process_t *proc, const char *msg, ...)
{
	va_list		log_args;

	if(proc->log == NULL){

		return;
	}

	va_start(log_args, msg);

	if(proc->log->path != NULL){

		if(nxs_log_open(proc->log) == NXS_LOG_FP_OK){

			nxs_log_str_prepare(proc->log, msg, log_args);

			fprintf(proc->log->fp, NXS_LOG_FORMAT_CONSOLE, (char *)nxs_string_str(proc->log->tmp_msg_buf));

			if(proc->log->mode == NXS_LOG_MODE_REOPEN){

				nxs_log_close(proc->log);
			}
			else{

				if(fflush(proc->log->fp) < 0){

					nxs_log_close(proc->log);
				}
			}
		}
	}
	else{

		nxs_log_str_prepare(proc->log, msg, log_args);

		printf(NXS_LOG_FORMAT_CONSOLE, (char *)nxs_string_str(proc->log->tmp_msg_buf));
	}

	va_end(log_args);
}

int nxs_log_set_level(nxs_log_t *log, int level)
{

	if(log == NULL){

		return 1;
	}

	if(level < 0 || level > NXS_LOG_LEVEL_ALL){

		return 1;
	}

	log->level = level;

	return 0;
}

int nxs_log_get_level(nxs_log_t *log)
{

	if(log == NULL){

		return 0;
	}

	return log->level;
}

int nxs_log_set_path(nxs_log_t *log, nxs_string_t *path)
{

	log->path = nxs_string_destroy(log->path);

	if(path != NULL && nxs_string_size(path) != 0 && nxs_string_len(path) != 0){

		log->path = nxs_string_malloc(0, nxs_string_str(path));
	}

	return 0;
}

nxs_string_t *nxs_log_get_path(nxs_log_t *log)
{

	return log->path;
}
