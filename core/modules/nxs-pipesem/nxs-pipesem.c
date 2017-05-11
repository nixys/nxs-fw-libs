// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define NXS_PIPESEM_SHMSIZE		1
#define NXS_PIPESEM_BUF_SIZE_INIT	1024
#define _NXS_PIPESEM_FD_TIMEOUT		1000

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static int nxs_pipesem_write_core(nxs_process_t *proc, nxs_pipesem_t *out, time_t timeout);
static int nxs_pipesem_read_core(nxs_process_t *proc, nxs_pipesem_t *in, time_t timeout, size_t size);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

/*
 * Выделение памяти только под один конец канала.
 * Для корректной работы функция должна быть вызывана дважды (для каждого из концов), после чего оба конца должны быть инициализированы.
 * Рекомендуется использовать функцию nxs_pipesem_malloc(), которая производит все эти действия.
 */
nxs_pipesem_t *nxs_pipesem_malloc_el(nxs_process_t *proc)
{
	nxs_pipesem_t *p = NULL;

	p = nxs_calloc(p, sizeof(nxs_pipesem_t));

	return p;
}

/*
 * Выделение памяти и инициализация канала (оба конца)
 */
int nxs_pipesem_malloc(nxs_process_t *proc, nxs_pipesem_t **in, nxs_pipesem_t **out)
{
	int            rc;
	nxs_pipesem_t *i = NULL, *o = NULL;

	if(*in != NULL || *out != NULL) {

		return NXS_PIPESEM_E_PTR;
	}

	i = nxs_calloc(i, sizeof(nxs_pipesem_t));
	o = nxs_calloc(o, sizeof(nxs_pipesem_t));

	if((rc = nxs_pipesem_init(proc, i, o)) == NXS_PIPESEM_E_OK) {

		*in  = i;
		*out = o;
	}

	return rc;
}

/*
 * Освобождение памяти для элемента psem.
 * Конец канала, которому соответствует данный элемент должен быть либо закрыт, либо освобождён. Иначе не понятно что делать с разделяемой
 * памятью, которая используется в psem.
 */
nxs_pipesem_t *nxs_pipesem_destroy_el(nxs_process_t *proc, nxs_pipesem_t *psem)
{

	if(psem == NULL) {

		return NULL;
	}

	if(psem->type == NXS_PIPESEM_TYPE_CLOSED || psem->type == NXS_PIPESEM_TYPE_FREED) {

		psem = nxs_free(psem);
	}
	else {

		nxs_log_write_error(proc, "can't destroy pipesem: pipe is active, close or free it first");
	}

	return psem;
}

/*
 * Пока не понятно потребуется ли эта функция.
 * Сомнения в следующем: в качестве входных аргументов передаются указатели на элементы pipesem, которые являются двумя частями одного
 * целого
 * (в один канал читать, изу другого читать), но после создания этих каналов почти всегда производися fork и один из концов закрывается,
 * по этому в одном процессе оба конца не встречаются.
 */
/*
int nxs_pipesem_destroy(nxs_process_t *proc, nxs_pipesem_t **in, nxs_pipesem_t **out)
{


}
*/

int nxs_pipesem_init(nxs_process_t *proc, nxs_pipesem_t *in, nxs_pipesem_t *out)
{
	int   shm_id, p_fd[2];
	char *shm;

	if(in == NULL || out == NULL) {

		return NXS_PIPESEM_E_ERR;
	}

	if((shm_id = shmget(IPC_PRIVATE, sizeof(char) * NXS_PIPESEM_SHMSIZE, 0600 | IPC_CREAT)) < 0) {

		nxs_log_write_error(proc, "can't create shared memory: %s", strerror(errno));

		return NXS_PIPESEM_E_ERR;
	}

	if((shm = (char *)shmat(shm_id, NULL, 0)) == (char *)(-1)) {

		nxs_log_write_error(proc, "can't attach shared memory: %s", strerror(errno));

		return NXS_PIPESEM_E_ERR;
	}

	if(shmctl(shm_id, IPC_RMID, NULL) < 0) {

		nxs_log_write_error(proc, "shmctl error: %s", strerror(errno));

		return NXS_PIPESEM_E_ERR;
	}

	*shm = NXS_PIPESEM_LOCK_OFF;

	if(pipe(p_fd) < 0) {

		nxs_log_write_error(proc, "pipe error: %s", strerror(errno));

		return NXS_PIPESEM_E_ERR;
	}

	in->lock       = shm;
	in->type       = NXS_PIPESEM_TYPE_IN;
	in->fd         = p_fd[0];
	in->bytes      = 0;
	in->poll_close = NULL;
	in->fd_poll    = nxs_poll_malloc();
	nxs_poll_add(in->fd_poll, in->fd, POLLIN);
	nxs_buf_init(&in->tmp_buf, NXS_PIPESEM_BUF_SIZE_INIT);

	out->lock       = shm;
	out->type       = NXS_PIPESEM_TYPE_OUT;
	out->fd         = p_fd[1];
	out->bytes      = 0;
	out->poll_close = nxs_poll_malloc();
	out->fd_poll    = nxs_poll_malloc();
	nxs_poll_add(out->poll_close, out->fd, POLLERR);
	nxs_poll_add(out->fd_poll, out->fd, POLLOUT);
	nxs_buf_init(&out->tmp_buf, NXS_PIPESEM_BUF_SIZE_INIT);

	return NXS_PIPESEM_E_OK;
}

void nxs_pipesem_close(nxs_pipesem_t *el)
{

	if(el == NULL) {

		return;
	}

	el->type = NXS_PIPESEM_TYPE_CLOSED;

	if(el->fd != NXS_PIPESEM_FD_NOT_USED) {

		el->fd_poll    = nxs_poll_destroy(el->fd_poll);
		el->poll_close = nxs_poll_destroy(el->poll_close);

		nxs_buf_free(&el->tmp_buf);

		el->bytes = 0;

		close(el->fd);

		el->fd = NXS_PIPESEM_FD_NOT_USED;
	}
}

void nxs_pipesem_free(nxs_process_t *proc, nxs_pipesem_t *el)
{

	if(el == NULL) {

		return;
	}

	nxs_pipesem_close(el);

	if(shmdt(el->lock) < 0) {

		nxs_log_write_error(proc, "shmdt error: %s", strerror(errno));
	}

	el->type = NXS_PIPESEM_TYPE_FREED;
}

int nxs_pipesem_on(nxs_process_t *proc, nxs_pipesem_t *el, int force)
{
	int    rc;
	size_t dl;

	if(el == NULL) {

		return NXS_PIPESEM_E_ERR;
	}

	if(el->type != NXS_PIPESEM_TYPE_IN) {

		return NXS_PIPESEM_E_TYPE;
	}

	if(*el->lock == NXS_PIPESEM_LOCK_CLOSE) {

		if(ioctl(el->fd, FIONREAD, &dl) < 0) {

			nxs_log_write_error(proc, "pipe enable error (ioctl): %s", strerror(errno));

			return NXS_PIPESEM_E_ERR;
		}

		if(dl > 0) {

			if(force == NXS_PIPESEM_ON_FORCE_YES) {

				if((rc = nxs_pipesem_read_core(proc, el, _NXS_PIPESEM_FD_TIMEOUT, dl)) != NXS_PIPESEM_E_OK) {

					return rc;
				}

				nxs_buf_set_len(&el->tmp_buf, 0);
			}
			else {

				return NXS_PIPESEM_E_HAVE_DATA;
			}
		}
	}

	*el->lock = NXS_PIPESEM_LOCK_OPEN;

	return NXS_PIPESEM_E_OK;
}

int nxs_pipesem_write(nxs_process_t *proc, nxs_pipesem_t *out, size_t size, void *data)
{
	int rc, pe;

	if(out == NULL) {

		return NXS_PIPESEM_E_ERR;
	}

	if(out->type != NXS_PIPESEM_TYPE_OUT) {

		return NXS_PIPESEM_E_TYPE;
	}

	if(out->fd == NXS_PIPESEM_FD_NOT_USED) {

		return NXS_PIPESEM_E_FD;
	}

	if(*out->lock != NXS_PIPESEM_LOCK_OPEN) {

		/*
		 * Проверка состояния канала
		 * Т.е. если канал закрыт для записи - проверяется активен ли он вообще (т.к. может получиться так, что парный процесс
		 * "закрыл" замок
		 * и завершился).
		 */

		if((pe = nxs_poll_exec(out->poll_close, 0)) != NXS_POLL_E_OK) {

			if(pe == NXS_POLL_E_TIMEOUT) {

				return NXS_PIPESEM_E_LOCK;
			}
			else {

				nxs_log_write_error(proc, "pipe write error: poll error: %d", pe);

				return NXS_PIPESEM_E_WRITE;
			}
		}

		if(nxs_poll_check_events(out->poll_close, out->fd, POLLERR) == NXS_POLL_EVENTS_TRUE) {

			return NXS_PIPESEM_E_PIPE_CLOSED;
		}

		return NXS_PIPESEM_E_LOCK;
	}

	nxs_buf_cpy_dyn(&out->tmp_buf, 0, &size, sizeof(size_t));
	nxs_buf_cat_dyn(&out->tmp_buf, data, size);

	if((rc = nxs_pipesem_write_core(proc, out, _NXS_PIPESEM_FD_TIMEOUT)) != NXS_PIPESEM_E_OK) {

		return rc;
	}

	*out->lock = NXS_PIPESEM_LOCK_CLOSE;

	return NXS_PIPESEM_E_OK;
}

int nxs_pipesem_read(nxs_process_t *proc, nxs_pipesem_t *in)
{
	int    rc, pe;
	size_t dl;

	if(in == NULL) {

		return NXS_PIPESEM_E_ERR;
	}

	if(in->type != NXS_PIPESEM_TYPE_IN) {

		return NXS_PIPESEM_E_TYPE;
	}

	if(in->fd == NXS_PIPESEM_FD_NOT_USED) {

		return NXS_PIPESEM_E_FD;
	}

	if(*in->lock != NXS_PIPESEM_LOCK_CLOSE) {

		if((pe = nxs_poll_exec(in->fd_poll, 0)) != NXS_POLL_E_OK) {

			if(pe == NXS_POLL_E_TIMEOUT) {

				return NXS_PIPESEM_E_NO_DATA;
			}
			else {

				nxs_log_write_error(proc, "pipe read error: poll error: %d", pe);

				return NXS_PIPESEM_E_READ;
			}
		}

		if(ioctl(in->fd, FIONREAD, &dl) < 0) {

			nxs_log_write_error(proc, "pipe read error: ioctl error: %s", strerror(errno));

			return NXS_PIPESEM_E_ERR;
		}

		if(dl == 0) {

			return NXS_PIPESEM_E_PIPE_CLOSED;
		}

		return NXS_PIPESEM_E_NO_DATA;
	}

	if((rc = nxs_pipesem_read_core(proc, in, _NXS_PIPESEM_FD_TIMEOUT, sizeof(size_t))) != NXS_PIPESEM_E_OK) {

		return rc;
	}

	dl = *(nxs_buf_get_subbuf(&in->tmp_buf, 0));

	if((rc = nxs_pipesem_read_core(proc, in, _NXS_PIPESEM_FD_TIMEOUT, dl)) != NXS_PIPESEM_E_OK) {

		return rc;
	}

	*in->lock = NXS_PIPESEM_LOCK_OPEN;

	return NXS_PIPESEM_E_OK;
}

int nxs_pipesem_get_type(nxs_pipesem_t *el)
{

	if(el == NULL) {

		return -1;
	}

	return el->type;
}

size_t nxs_pipesem_get_bytes(nxs_pipesem_t *el)
{

	if(el == NULL) {

		return 0;
	}

	return el->bytes;
}

int nxs_pipesem_get_fd(nxs_pipesem_t *el)
{

	if(el == NULL) {

		return -1;
	}

	return el->fd;
}

int nxs_pipesem_get_lock(nxs_pipesem_t *el)
{

	if(el == NULL) {

		return -1;
	}

	if(el->lock == NULL) {

		return -1;
	}

	return *el->lock;
}

size_t nxs_pipesem_get_buf_len(nxs_pipesem_t *el)
{

	if(el == NULL) {

		return 0;
	}

	return nxs_buf_get_len(&el->tmp_buf);
}

u_char *nxs_pipesem_get_buf(nxs_pipesem_t *el)
{

	if(el == NULL) {

		return NULL;
	}

	return nxs_buf_get_subbuf(&el->tmp_buf, 0);
}

/* Module internal (static) functions */

static int nxs_pipesem_write_core(nxs_process_t *proc, nxs_pipesem_t *out, time_t timeout)
{
	size_t  tb, lb, size;
	ssize_t nb;
	int     pe;
	u_char *buf;

	size = nxs_buf_get_len(&out->tmp_buf);
	buf  = nxs_buf_get_subbuf(&out->tmp_buf, 0);

	tb = 0;
	lb = size;

	while(tb < size) {

		if((pe = nxs_poll_exec(out->fd_poll, timeout)) != NXS_POLL_E_OK) {

			if(pe == NXS_POLL_E_TIMEOUT) {

				nxs_log_write_error(proc, "pipe write error: timeout (timeout: %d sec)", timeout / 1000);

				return NXS_PIPESEM_E_TIMEOUT;
			}

			nxs_log_write_error(proc, "pipe write error: poll error: %s", strerror(pe));

			return NXS_PIPESEM_E_ERR;
		}

		/*
		 * Отправка данных
		 */
		nb = write(out->fd, buf + tb, lb);

		if(nb == 0) {

			nxs_log_write_error(proc, "pipe write error: wrote 0 bytes (pipe fd: %d)", out->fd);

			return NXS_PIPESEM_E_WRITE;
		}

		if(nb > 0) {

			tb += nb;
			lb -= nb;
		}
		else {

			if(errno == EPIPE) {

				nxs_log_write_error(proc, "pipe write error: peer closed pipe (pipe fd: %d)", out->fd);

				return NXS_PIPESEM_E_PIPE_CLOSED;
			}

			nxs_log_write_error(proc, "pipe write error: %s (pipe fd: %d)", strerror(errno), out->fd);

			return NXS_PIPESEM_E_WRITE;
		}
	}

	out->bytes += tb;

	return NXS_PIPESEM_E_OK;
}

static int nxs_pipesem_read_core(nxs_process_t *proc, nxs_pipesem_t *in, time_t timeout, size_t size)
{
	size_t  tb, lb;
	ssize_t nb;
	int     pe;
	u_char *buf;

	if(nxs_buf_get_size(&in->tmp_buf) < size) {

		nxs_buf_resize(&in->tmp_buf, size);
	}

	buf = nxs_buf_get_subbuf(&in->tmp_buf, 0);

	tb = 0;
	lb = size;

	while(tb < size) {

		if((pe = nxs_poll_exec(in->fd_poll, timeout)) != NXS_POLL_E_OK) {

			if(pe == NXS_POLL_E_TIMEOUT) {

				nxs_log_write_error(proc, "pipe read error: timeout (timeout: %d sec)", timeout / 1000);

				nxs_buf_set_len(&in->tmp_buf, 0);

				return NXS_PIPESEM_E_TIMEOUT;
			}

			nxs_log_write_error(proc, "pipe read error: poll error: %s", strerror(pe));

			nxs_buf_set_len(&in->tmp_buf, 0);

			return NXS_PIPESEM_E_READ;
		}

		nb = read(in->fd, buf + tb, lb);

		if(nb > 0) {

			tb += nb;
			lb -= nb;
		}
		else {

			if(nb < 0) {

				nxs_log_write_error(proc, "pipe read error: %s (pipe fd: %d)", strerror(errno), in->fd);

				nxs_buf_set_len(&in->tmp_buf, 0);

				return NXS_PIPESEM_E_READ;
			}

			nxs_log_write_debug_net(proc, "pipe read error: peer closed pipe (pipe fd: %d)", in->fd);

			nxs_buf_set_len(&in->tmp_buf, 0);

			return NXS_PIPESEM_E_PIPE_CLOSED;
		}
	}

	in->bytes += tb;

	nxs_buf_set_len(&in->tmp_buf, tb);

	return NXS_PIPESEM_E_OK;
}
