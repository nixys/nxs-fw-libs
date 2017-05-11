// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define _NXS_PROCESS_SIG_FREE	0
#define _NXS_PROCESS_SIG_USED	1

/* Module typedefs */

typedef struct			nxs_process_sig_ctx_s			nxs_process_sig_ctx_t;

/* Module declarations */

struct nxs_process_sig_ctx_s
{
	int			used;
	void			(*handler)(int sig, void *data);
	void			*data;
};

/* Module internal (static) functions prototypes */

// clang-format on

static int  nxs_proc_signal_init(void);
static void nxs_proc_sig_handler(int sig);

// clang-format off

/* Module initializations */

nxs_array_t *sig_ctx		= NULL;

/* Module global functions */

// clang-format on

nxs_process_t *nxs_proc_malloc(u_char *ps_name)
{
	nxs_process_t *proc = NULL;

	proc = nxs_malloc(proc, sizeof(nxs_process_t));

	nxs_proc_init(proc, ps_name);

	return proc;
}

nxs_process_t *nxs_proc_destroy(nxs_process_t *proc)
{
	if(proc == NULL) {

		return NULL;
	}

	nxs_proc_free(proc);

	return nxs_free(proc);
}

void nxs_proc_init(nxs_process_t *proc, u_char *ps_name)
{

	proc->pid  = getpid();
	proc->ppid = 0;

	proc->handler = NULL;

	proc->log = nxs_log_malloc(NXS_LOG_LEVEL_ALL, NXS_LOG_MODE_KEEP_OPEN, NXS_LOG_SYSLOG_DONT_USE, NULL);

	if(ps_name == NULL) {

		proc->ps_name = nxs_string_malloc(0, NXS_STRING_EMPTY_STR);
	}
	else {

		proc->ps_name = nxs_string_malloc(0, ps_name);
	}

	nxs_proc_signal_init();
}

int nxs_proc_free(nxs_process_t *proc)
{

	if(proc == NULL) {

		return NXS_PROCESS_E_INIT;
	}

	proc->log = nxs_log_destroy(proc->log);

	proc->ps_name = nxs_string_destroy(proc->ps_name);

	return NXS_PROCESS_E_OK;
}

pid_t nxs_proc_fork(nxs_process_t *proc, int sigsave, u_char *ps_child_name)
{
	pid_t    p;
	sigset_t bset;

	if((p = fork()) == 0) {

		/*
		 * Потомок
		 */

		proc->ppid = getppid();
		proc->pid  = getpid();

		if(ps_child_name != NULL) {

			nxs_string_char_cpy_dyn(proc->ps_name, NXS_STRING_NO_OFFSET, ps_child_name);
		}

		if(sigsave == NXS_PROCESS_CHLD_SIG_CLEAR) {

			nxs_proc_signal_clear(proc);
		}

		sigemptyset(&bset);

		if(sigprocmask(SIG_SETMASK, &bset, NULL) == -1) {

			nxs_log_write_warn(proc, "failed to flush signals block: sigprocmask error: %s", strerror(errno));
		}

		return NXS_PROCESS_FORK_CHILD;
	}

	if(p == -1) {

		/*
		 * Ошибка
		 */

		nxs_log_write_error(proc, "fork error: %s", strerror(errno));

		return NXS_PROCESS_FORK_ERR;
	}

	/*
	 * Родитель
	 */

	return p;
}

/*
 * * После демонизации закрытие дескрипторов, отличных от 0, 1, 2 - находится в ведении программиста
 * * Данная функция не производит создание pid-файла
 */
int nxs_proc_daemonize(nxs_process_t *proc, int sigsave, u_char *ps_name, int uid, int _umask, u_char *_chdir)
{
	pid_t                  pid;
	nxs_process_sig_ctx_t *el      = NULL;
	void *                 handler = NULL, *data = NULL;
	int                    sighup_used;

	if(nxs_proc_setuid(proc, uid) == NXS_PROCESS_E_SETUID) {

		return NXS_PROCESS_E_SETUID;
	}

	if((pid = nxs_proc_fork(proc, sigsave, ps_name)) != 0) {

		if(pid == NXS_PROCESS_FORK_ERR) {

			return NXS_PROCESS_E_FORK;
		}

		exit(NXS_PROCESS_E_OK);
	}

	if(setsid() == -1) {

		nxs_log_write_error(proc, "setsid error: %s", strerror(errno));

		return NXS_PROCESS_E_SETSID;
	}

	/*
	 * Временное пересохранение сигнала SIGHUP
	 */

	sighup_used = 0;

	if(sigsave == NXS_PROCESS_CHLD_SIG_SAVE && sig_ctx != NULL && (el = nxs_array_get(sig_ctx, SIGHUP)) != NULL &&
	   el->used == _NXS_PROCESS_SIG_USED) {

		sighup_used = 1;

		handler = el->handler;
		data    = el->data;
	}

	nxs_proc_signal_set(proc, SIGHUP, NXS_PROCESS_SA_FLAG_EMPTY, NXS_SIG_IGN, NULL, NXS_PROCESS_FORCE_YES);

	if((pid = nxs_proc_fork(proc, sigsave, ps_name)) != 0) {

		if(pid == NXS_PROCESS_FORK_ERR) {

			return NXS_PROCESS_E_FORK;
		}

		exit(NXS_PROCESS_E_OK);
	}

	/*
	 * Восстановление обработки сигнала SIGHUP
	 */
	if(sighup_used == 1) {

		nxs_proc_signal_set(proc, SIGHUP, NXS_PROCESS_SA_FLAG_EMPTY, handler, data, NXS_PROCESS_FORCE_YES);
	}
	else {

		nxs_proc_signal_del(proc, SIGHUP);
	}

	if(nxs_proc_chdir(proc, _chdir) == NXS_PROCESS_E_CHDIR) {

		return NXS_PROCESS_E_CHDIR;
	}

	if(nxs_proc_umask(_umask) == NXS_PROCESS_E_UMASK) {

		return NXS_PROCESS_E_UMASK;
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	if(open("/dev/null", O_RDONLY) == -1) {

		nxs_log_write_error(proc, "failed to reopen stdin while daemonising: %s ", strerror(errno));
	}

	if(open("/dev/null", O_WRONLY) == -1) {

		nxs_log_write_error(proc, "failed to reopen stdout while daemonising: %s ", strerror(errno));
	}

	if(open("/dev/null", O_RDWR) == -1) {

		nxs_log_write_error(proc, "failed to reopen stderr while daemonising: %s ", strerror(errno));
	}

	return NXS_PROCESS_E_OK;
}

int nxs_proc_setlog(nxs_process_t *proc, int level, int mode, int use_syslog, nxs_string_t *path)
{

	proc->log = nxs_log_destroy(proc->log);

	proc->log = nxs_log_malloc(level, mode, use_syslog, path);

	return NXS_PROCESS_E_OK;
}

int nxs_proc_log_level_get(nxs_process_t *proc)
{

	return nxs_log_get_level(proc->log);
}

void nxs_proc_log_level_set(nxs_process_t *proc, int level)
{

	nxs_log_set_level(proc->log, level);
}

int nxs_proc_setuid(nxs_process_t *proc, int uid)
{

	if(uid == NXS_PROCESS_NO_UID) {

		return NXS_PROCESS_E_OK;
	}

	if(setuid((uid_t)uid) == -1) {

		nxs_log_write_error(proc, "setuid error: %s", strerror(errno));

		return NXS_PROCESS_E_SETUID;
	}

	return NXS_PROCESS_E_OK;
}

int nxs_proc_chdir(nxs_process_t *proc, u_char *_chdir)
{

	if(_chdir == NULL) {

		return NXS_PROCESS_E_OK;
	}

	if(chdir((char *)_chdir) == -1) {

		nxs_log_write_error(proc, "chdir error: %s", strerror(errno));

		return NXS_PROCESS_E_CHDIR;
	}

	return NXS_PROCESS_E_OK;
}

int nxs_proc_umask(int _umask)
{

	if(_umask == NXS_PROCESS_NO_UMASK) {

		return NXS_PROCESS_E_OK;
	}

	umask((mode_t)_umask);

	return NXS_PROCESS_E_OK;
}

pid_t nxs_proc_get_pid(nxs_process_t *proc)
{

	return proc->pid;
}

pid_t nxs_proc_get_ppid(nxs_process_t *proc)
{

	return proc->ppid;
}

u_char *nxs_proc_get_name(nxs_process_t *proc)
{

	return nxs_string_str(proc->ps_name);
}

int nxs_proc_check_pid(pid_t pid)
{

	if(kill(pid, 0) == 0) {

		return NXS_YES;
	}

	if(errno == ESRCH) {

		return NXS_NO;
	}

	return -1;
}

int nxs_proc_execl(nxs_string_t *path, nxs_string_t *argv, ...)
{
	nxs_string_t *str;
	u_int         count;
	u_char **     args;
	va_list       str_args;

	count = 0;
	args  = NULL;

	str = argv;

	va_start(str_args, argv);

	while(str != NULL) {

		args = (u_char **)nxs_realloc(args, (count + 1) * sizeof(u_char *));

		args[count] = nxs_string_str(str);

		count++;

		str = va_arg(str_args, nxs_string_t *);
	}

	va_end(str_args);

	args = (u_char **)nxs_realloc(args, (count + 1) * sizeof(u_char *));

	args[count] = NULL;

	if(execv((const char *)nxs_string_str(path), (char *const *)args) == -1) {

		args = (u_char **)nxs_free(args);

		return NXS_PROCESS_E_EXEC;
	}

	return NXS_PROCESS_E_OK;
}

int nxs_proc_execlp(nxs_string_t *path, nxs_string_t *argv, ...)
{
	nxs_string_t *str;
	u_int         count;
	u_char **     args;
	va_list       str_args;

	count = 0;
	args  = NULL;

	str = argv;

	va_start(str_args, argv);

	while(str != NULL) {

		args = (u_char **)nxs_realloc(args, (count + 1) * sizeof(u_char *));

		args[count] = nxs_string_str(str);

		count++;

		str = va_arg(str_args, nxs_string_t *);
	}

	va_end(str_args);

	args = (u_char **)nxs_realloc(args, (count + 1) * sizeof(u_char *));

	args[count] = NULL;

	if(execvp((const char *)nxs_string_str(path), (char *const *)args) == -1) {

		args = (u_char **)nxs_free(args);

		return NXS_PROCESS_E_EXEC;
	}

	return NXS_PROCESS_E_OK;
}

int nxs_proc_execv(nxs_string_t *path, nxs_array_t *argv)
{
	nxs_string_t *str;
	u_int         i;
	u_char **     args;

	args = (u_char **)nxs_malloc(NULL, (nxs_array_count(argv) + 1) * sizeof(u_char *));

	for(i = 0; i < nxs_array_count(argv); i++) {

		str = nxs_array_get(argv, i);

		args[i] = nxs_string_str(str);
	}

	args[i] = NULL;

	if(execv((const char *)nxs_string_str(path), (char *const *)args) == -1) {

		args = (u_char **)nxs_free(args);

		return NXS_PROCESS_E_EXEC;
	}

	return NXS_PROCESS_E_OK;
}

int nxs_proc_execvp(nxs_string_t *path, nxs_array_t *argv)
{
	nxs_string_t *str;
	u_int         i;
	u_char **     args;

	args = (u_char **)nxs_malloc(NULL, (nxs_array_count(argv) + 1) * sizeof(u_char *));

	for(i = 0; i < nxs_array_count(argv); i++) {

		str = nxs_array_get(argv, i);

		args[i] = nxs_string_str(str);
	}

	args[i] = NULL;

	if(execvp((const char *)nxs_string_str(path), (char *const *)args) == -1) {

		args = (u_char **)nxs_free(args);

		return NXS_PROCESS_E_EXEC;
	}

	return NXS_PROCESS_E_OK;
}

int nxs_proc_signal_set(nxs_process_t *proc, int sig, int sa_flag, void (*sig_handler)(int, void *), void *data, int force)
{
	struct sigaction       sigact;
	nxs_process_sig_ctx_t *el;

	if(sig_ctx == NULL) {

		nxs_proc_signal_init();
	}

	if((el = nxs_array_get(sig_ctx, sig)) != NULL && el->used == _NXS_PROCESS_SIG_USED && force != NXS_PROCESS_FORCE_YES) {

		/*
		 * Сигнал определён, переопределение запрещено
		 */

		return NXS_PROCESS_E_EXIST;
	}

	/*
	 * Определение сигнала
	 */

	nxs_memzero(&sigact, sizeof(sigact));

	if(sig_handler == NULL) {

		nxs_log_write_error(proc, "signal add error: sig_handler is NULL");

		return NXS_PROCESS_E_SIGHANDLER;
	}

	if(sig_handler == NXS_SIG_DFL) {

		sigact.sa_handler = SIG_DFL;
	}
	else {

		if(sig_handler == NXS_SIG_IGN) {

			sigact.sa_handler = SIG_IGN;
		}
		else {

			sigact.sa_handler = nxs_proc_sig_handler;
		}
	}

	sigfillset(&sigact.sa_mask);
	sigact.sa_flags = sa_flag;

	if(sigaction(sig, &sigact, NULL) == -1) {

		nxs_log_write_error(proc, "sigaction error: %s", strerror(errno));

		return NXS_PROCESS_E_SIGACT;
	}

	/*
	 * Добавление сигнала в массив sig_ctx
	 */

	if(el == NULL) {

		/*
		 * Сигнал не определён
		 */

		el = nxs_array_add_i(sig_ctx, sig);
	}

	el->used    = _NXS_PROCESS_SIG_USED;
	el->data    = data;
	el->handler = sig_handler;

	return NXS_PROCESS_E_OK;
}

int nxs_proc_signal_del(nxs_process_t *proc, int sig)
{
	struct sigaction       sigact;
	nxs_process_sig_ctx_t *el;

	if(sig_ctx == NULL) {

		return NXS_PROCESS_E_INIT;
	}

	if((el = nxs_array_get(sig_ctx, sig)) == NULL) {

		return NXS_PROCESS_E_EXIST;
	}

	if(el->used == _NXS_PROCESS_SIG_FREE) {

		return NXS_PROCESS_E_EXIST;
	}

	nxs_memzero(&sigact, sizeof(sigact));

	sigact.sa_handler = SIG_DFL;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;

	if(sigaction(sig, &sigact, NULL) == -1) {

		nxs_log_write_error(proc, "sigaction error: %s", strerror(errno));

		return NXS_PROCESS_E_SIGACT;
	}

	nxs_memzero(el, sizeof(nxs_process_sig_ctx_t));

	return NXS_PROCESS_E_OK;
}

int nxs_proc_signal_clear(nxs_process_t *proc)
{
	int sig;

	if(sig_ctx == NULL) {

		return NXS_PROCESS_E_INIT;
	}

	for(sig = 0; sig < (int)nxs_array_count(sig_ctx); sig++) {

		if(nxs_proc_signal_del(proc, sig) == NXS_PROCESS_E_SIGACT) {

			return NXS_PROCESS_E_SIGACT;
		}
	}

	return NXS_PROCESS_E_OK;
}

/*
 * Блокировка указанных сигналов
 *
 * Списко сигналов должен обязательно заканчиваться константой NXS_PROCESS_SIG_END_ARGS
 */
int nxs_proc_signal_block(nxs_process_t *proc, int sig, ...)
{
	va_list  args;
	int      _sig;
	sigset_t bset;

	sigemptyset(&bset);

	va_start(args, sig);

	for(_sig = sig; _sig != 0; _sig = va_arg(args, int)) {

		if(sigaddset(&bset, _sig) == -1) {

			nxs_log_write_error(proc, "failed to block signal: sigaddset error (signal: %d)", _sig);

			va_end(args);

			return NXS_PROCESS_E_SIG;
		}
	}

	va_end(args);

	if(sigprocmask(SIG_BLOCK, &bset, NULL) == -1) {

		nxs_log_write_error(proc, "failed to block signal: sigprocmask error");

		return NXS_PROCESS_E_SIG;
	}

	return NXS_PROCESS_E_OK;
}

/*
 * Разблокировка указанных сигналов
 *
 * Списко сигналов должен обязательно заканчиваться константой NXS_PROCESS_SIG_END_ARGS
 */
int nxs_proc_signal_unblock(nxs_process_t *proc, int sig, ...)
{
	va_list  args;
	int      _sig;
	sigset_t bset;

	sigemptyset(&bset);

	va_start(args, sig);

	for(_sig = sig; _sig != 0; _sig = va_arg(args, int)) {

		if(sigaddset(&bset, _sig) == -1) {

			nxs_log_write_error(proc, "failed to unblock signal: sigaddset error (signal: %d)", _sig);

			va_end(args);

			return NXS_PROCESS_E_SIG;
		}
	}

	va_end(args);

	if(sigprocmask(SIG_UNBLOCK, &bset, NULL) == -1) {

		nxs_log_write_error(proc, "failed to unblock signal: sigprocmask error");

		return NXS_PROCESS_E_SIG;
	}

	return NXS_PROCESS_E_OK;
}

/* Module internal (static) functions */

static int nxs_proc_signal_init(void)
{

	if(sig_ctx != NULL) {

		return NXS_PROCESS_E_EXIST;
	}

	sig_ctx = nxs_array_malloc(0, sizeof(nxs_process_sig_ctx_t), 1);

	return NXS_PROCESS_E_OK;
}

static void nxs_proc_sig_handler(int sig)
{
	nxs_process_sig_ctx_t *el;

	if(sig_ctx == NULL) {

		return;
	}

	if((el = nxs_array_get(sig_ctx, sig)) == NULL) {

		return;
	}

	if(el->used == _NXS_PROCESS_SIG_FREE) {

		return;
	}

	el->handler(sig, el->data);
}
