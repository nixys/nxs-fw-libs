#ifndef _INCLUDE_NXS_LOG_H
#define _INCLUDE_NXS_LOG_H

#include <nxs-core/nxs-core.h>

#define NXS_LOG_FP_OK					0
#define NXS_LOG_FP_ERROR_OPEN			-1

#define NXS_LOG_MODE_REOPEN				0
#define NXS_LOG_MODE_KEEP_OPEN			1

#define NXS_LOG_SYSLOG_DONT_USE			0
#define NXS_LOG_SYSLOG_USE				1

#define NXS_LOG_LEVEL_MEM				1
#define NXS_LOG_LEVEL_INFO				2
#define NXS_LOG_LEVEL_WARN				4
#define NXS_LOG_LEVEL_ERROR				8
#define NXS_LOG_LEVEL_DEBUG				16
#define NXS_LOG_LEVEL_DEBUG_NET			32
#define NXS_LOG_LEVEL_ALL				63

struct nxs_log_s
{
	FILE			*fp;

	int				level;
	int				mode;
	int				use_syslog;

	nxs_string_t	*path;
	nxs_string_t	*tmp_msg_buf;
};

#define nxs_log_write_mem(proc, msg...)				nxs_log_write(proc, NXS_LOG_LEVEL_MEM, msg)
#define nxs_log_write_info(proc, msg...)			nxs_log_write(proc, NXS_LOG_LEVEL_INFO, msg)
#define nxs_log_write_warn(proc, msg...)			nxs_log_write(proc, NXS_LOG_LEVEL_WARN, msg)
#define nxs_log_write_error(proc, msg...)			nxs_log_write(proc, NXS_LOG_LEVEL_ERROR, msg)
#define nxs_log_write_debug(proc, msg...)			nxs_log_write(proc, NXS_LOG_LEVEL_DEBUG, msg)
#define nxs_log_write_debug_net(proc, msg...)		nxs_log_write(proc, NXS_LOG_LEVEL_DEBUG_NET, msg)

nxs_log_t		*nxs_log_malloc				(int level, int mode, int use_syslog, nxs_string_t *path);
nxs_log_t		*nxs_log_destroy			(nxs_log_t *log);
void			nxs_log_free				(nxs_log_t *log);
void			nxs_log_init				(nxs_log_t *log, int level, int mode, int use_syslog, nxs_string_t *path);
void			nxs_log_reopen				(nxs_process_t *proc);

void			nxs_log_write				(nxs_process_t *proc, int level, const char *msg, ...);
void			nxs_log_write_console		(nxs_process_t *proc, const char *msg, ...);
void			nxs_log_write_raw			(nxs_process_t *proc, const char *msg, ...);

int				nxs_log_set_level			(nxs_log_t *log, int level);
int				nxs_log_get_level			(nxs_log_t *log);
int				nxs_log_set_path			(nxs_log_t *log, nxs_string_t *path);
nxs_string_t	*nxs_log_get_path			(nxs_log_t *log);

#endif /* _INCLUDE_NXS_LOG_H */
