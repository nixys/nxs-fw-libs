#ifndef _INCLUDE_NXS_PIPESEM_H
#define _INCLUDE_NXS_PIPESEM_H

#include <nxs-core/nxs-core.h>

#define NXS_PIPESEM_E_OK			0
#define NXS_PIPESEM_E_ERR			1	// Общая ошибка
#define NXS_PIPESEM_E_TYPE			2	// Ошибка типа (операция не позволяется для pipesem указанного типа)
#define NXS_PIPESEM_E_LOCK			3	// Действует блокировка
#define NXS_PIPESEM_E_FD			4	// Дескриптор имеет не верное значение
#define NXS_PIPESEM_E_READ			5	// Ошибка чтения pipe
#define NXS_PIPESEM_E_WRITE			6	// Ошибка записи pipe
#define NXS_PIPESEM_E_TIMEOUT		7	// Тайм-аут
#define NXS_PIPESEM_E_NO_DATA		8	// Данных в канале нет
#define NXS_PIPESEM_E_HAVE_DATA		9	// В канале имеются данные
#define NXS_PIPESEM_E_PIPE_CLOSED	10	// Канал закрыт удалённой стороной
#define NXS_PIPESEM_E_PTR			11	// Ошибка указателя на "nxs_process_t *"

#define NXS_PIPESEM_FD_NOT_USED		-1

#define	NXS_PIPESEM_TYPE_OUT		0
#define	NXS_PIPESEM_TYPE_IN			1
#define	NXS_PIPESEM_TYPE_CLOSED		2
#define	NXS_PIPESEM_TYPE_FREED		3

#define NXS_PIPESEM_LOCK_CLOSE		0	// Замок закрыт
#define NXS_PIPESEM_LOCK_OPEN		1	// Замок открыт
#define NXS_PIPESEM_LOCK_OFF		2	// Замок выключен (на этапе инициализации)

#define NXS_PIPESEM_ON_FORCE_NO		0
#define NXS_PIPESEM_ON_FORCE_YES	1

struct nxs_pipesem_s
{
	int			fd;
	char		*lock;
	char		type;

	nxs_poll_t	*fd_poll;
	nxs_poll_t	*poll_close;

	size_t		bytes;

	nxs_buf_t	tmp_buf;
};

nxs_pipesem_t	*nxs_pipesem_malloc_el			(nxs_process_t *proc);
int				nxs_pipesem_malloc				(nxs_process_t *proc, nxs_pipesem_t **in, nxs_pipesem_t **out);
nxs_pipesem_t	*nxs_pipesem_destroy_el			(nxs_process_t *proc, nxs_pipesem_t *psem);
int				nxs_pipesem_init				(nxs_process_t *proc, nxs_pipesem_t *in, nxs_pipesem_t *out);
void			nxs_pipesem_close				(nxs_pipesem_t *el);
void			nxs_pipesem_free				(nxs_process_t *proc, nxs_pipesem_t *el);
int				nxs_pipesem_on					(nxs_process_t *proc, nxs_pipesem_t *el, int force);
int				nxs_pipesem_write				(nxs_process_t *proc, nxs_pipesem_t *out, size_t size, void *data);
int				nxs_pipesem_read				(nxs_process_t *proc, nxs_pipesem_t *in);

int				nxs_pipesem_get_type			(nxs_pipesem_t *el);
size_t			nxs_pipesem_get_bytes			(nxs_pipesem_t *el);
int				nxs_pipesem_get_fd				(nxs_pipesem_t *el);
int				nxs_pipesem_get_lock			(nxs_pipesem_t *el);
size_t			nxs_pipesem_get_buf_len			(nxs_pipesem_t *el);
u_char			*nxs_pipesem_get_buf			(nxs_pipesem_t *el);

#endif /* _INCLUDE_NXS_PIPESEM_H */
