#ifndef _INCLUDE_NXS_PROC_H
#define _INCLUDE_NXS_PROC_H

#include <nxs-core/nxs-core.h>

typedef void (*__nxs_sighandler_t) (int, void *);

#define NXS_PROCESS_E_OK			0
#define NXS_PROCESS_E_INIT			1 // Не инициализирован (sig_ctx)
#define NXS_PROCESS_E_EXIST			2 // Уже инициализирован (sig_ctx)
#define NXS_PROCESS_E_SIGACT		3 // Ошибка задания сигнала sigaction
#define NXS_PROCESS_E_SIGHANDLER	4 // Ошибка задания сигнала sigaction
#define NXS_PROCESS_E_SETUID		5
#define NXS_PROCESS_E_FORK			6
#define NXS_PROCESS_E_SETSID		7
#define NXS_PROCESS_E_CHDIR			8
#define NXS_PROCESS_E_UMASK			9
#define NXS_PROCESS_E_SIG			10
#define NXS_PROCESS_E_KILL			11
#define NXS_PROCESS_E_EXEC			12

#define NXS_PROCESS_FORK_ERR		-1
#define NXS_PROCESS_FORK_CHILD		0

#define NXS_PROCESS_CHLD_SIG_CLEAR	0
#define NXS_PROCESS_CHLD_SIG_SAVE	1

#define NXS_PROCESS_SA_FLAG_EMPTY	0

#define NXS_PROCESS_FORCE_NO		0
#define NXS_PROCESS_FORCE_YES		1

#define NXS_SIG_DFL	((__nxs_sighandler_t) 0)
#define NXS_SIG_IGN	((__nxs_sighandler_t) 1)

#define NXS_PROCESS_NO_UID			-1
#define NXS_PROCESS_NO_UMASK		-1

#define NXS_PROCESS_SIG_END_ARGS	0

struct nxs_process_s
{
	pid_t				pid;
	pid_t				ppid;

	nxs_string_t		*ps_name;

	int					(*handler)();

	nxs_log_t			*log;
};

nxs_process_t *		nxs_proc_malloc				(u_char *ps_name);
nxs_process_t *		nxs_proc_destroy			(nxs_process_t *proc);
void				nxs_proc_init				(nxs_process_t *proc, u_char *ps_name);
int					nxs_proc_free				(nxs_process_t *proc);
pid_t				nxs_proc_fork				(nxs_process_t *proc, int sigsave, u_char *ps_child_name);
int					nxs_proc_daemonize			(nxs_process_t *proc, int sigsave, u_char *ps_name, int uid, int _umask, u_char *_chdir);
int					nxs_proc_setlog				(nxs_process_t *proc, int level, int mode, int use_syslog, nxs_string_t *path);
int					nxs_proc_log_level_get		(nxs_process_t *proc);
void				nxs_proc_log_level_set		(nxs_process_t *proc, int level);
int					nxs_proc_setuid				(nxs_process_t *proc, int uid);
int					nxs_proc_chdir				(nxs_process_t *proc, u_char *_chdir);
int					nxs_proc_umask				(int _umask);
pid_t				nxs_proc_get_pid			(nxs_process_t *proc);
pid_t				nxs_proc_get_ppid			(nxs_process_t *proc);
u_char				*nxs_proc_get_name			(nxs_process_t *proc);
int					nxs_proc_check_pid			(pid_t pid);

int					nxs_proc_execl				(nxs_string_t *path, nxs_string_t *argv, ...);
int					nxs_proc_execlp				(nxs_string_t *path, nxs_string_t *argv, ...);
int					nxs_proc_execv				(nxs_string_t *path, nxs_array_t *argv);
int					nxs_proc_execvp				(nxs_string_t *path, nxs_array_t *argv);

int					nxs_proc_signal_set			(nxs_process_t *proc, int sig, int sa_flag, void (*sig_handler)(int, void *), void *data, int force);
int					nxs_proc_signal_del			(nxs_process_t *proc, int sig);
int					nxs_proc_signal_clear		(nxs_process_t *proc);
int					nxs_proc_signal_block		(nxs_process_t *proc, int sig, ...);
int					nxs_proc_signal_unblock		(nxs_process_t *proc, int sig, ...);

#endif /* _INCLUDE_NXS_PROC_H */
