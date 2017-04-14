#include <nxs-core/nxs-core.h>

nxs_poll_t *nxs_poll_malloc()
{
	nxs_poll_t *p = NULL;

	p = (nxs_poll_t *)nxs_malloc(p, sizeof(nxs_poll_t));

	nxs_poll_init(p);

	return p;
}

nxs_poll_t *nxs_poll_destroy(nxs_poll_t *p)
{

	if(p == NULL){

		return NULL;
	}

	nxs_poll_free(p);

	return (nxs_poll_t *)nxs_free(p);
}

int nxs_poll_init(nxs_poll_t *p)
{

	if(p == NULL){

		return NXS_POLL_E_INIT;
	}

	p->fds = NULL;
	p->nfds = 0;
	p->fds_ready = 0;

	return NXS_POLL_E_OK;
}

int nxs_poll_free(nxs_poll_t *p)
{

	if(p == NULL){

		return NXS_POLL_E_INIT;
	}

	p->fds = (struct pollfd	*)nxs_free(p->fds);
	p->nfds = 0;
	p->fds_ready = 0;

	return NXS_POLL_E_OK;
}

int nxs_poll_exec(nxs_poll_t *p, int timeout)
{
	int i, rc;

	if(p == NULL){

		return NXS_POLL_E_INIT;
	}

	p->fds_ready = 0;

	for(i = 0; i < p->nfds; i++){

		p->fds[i].revents = 0;
	}

	if((rc = poll(p->fds, p->nfds, timeout)) == -1){

		return errno;
	}

	if(rc == 0){

		return NXS_POLL_E_TIMEOUT;
	}

	p->fds_ready = rc;

	return NXS_POLL_E_OK;
}

int nxs_poll_add(nxs_poll_t *p, int fd, short events)
{
	int i;

	if(p == NULL){

		return NXS_POLL_E_INIT;
	}

	if(fd < 0){

		return NXS_POLL_E_INDEX;
	}

	for(i = 0; i < p->nfds; i++){

		if(p->fds[i].fd == fd){

			p->fds[i].events |= events;

			p->fds[i].revents = 0;

			return NXS_POLL_E_OK;
		}
	}

	p->nfds++;
	p->fds = (struct pollfd	*)nxs_realloc(p->fds, sizeof(struct pollfd) * p->nfds);

	p->fds[p->nfds - 1].fd = fd;
	p->fds[p->nfds - 1].events = events;
	p->fds[p->nfds - 1].revents = 0;

	return NXS_POLL_E_OK;
}

int nxs_poll_del(nxs_poll_t *p, int fd)
{
	int i, j;

	if(p == NULL){

		return NXS_POLL_E_INIT;
	}

	if(fd < 0){

		return NXS_POLL_E_INDEX;
	}

	for(i = 0; i < p->nfds; i++){

		if(p->fds[i].fd == fd){

			for(j = i + 1; j < p->nfds; j++){

				p->fds[j - 1].fd = p->fds[j].fd;
				p->fds[j - 1].events = p->fds[j].events;
			}

			p->nfds--;
			p->fds = (struct pollfd *)nxs_realloc(p->fds, sizeof(struct pollfd) * p->nfds);

			return NXS_POLL_E_OK;
		}
	}

	return NXS_POLL_E_EXIST;
}

int nxs_poll_del_event(nxs_poll_t *p, int fd, short events)
{
	int i, j;

	if(p == NULL){

		return NXS_POLL_E_INIT;
	}

	if(fd < 0){

		return NXS_POLL_E_INDEX;
	}

	for(i = 0; i < p->nfds; i++){

		if(p->fds[i].fd == fd){

			p->fds[i].events &= ~events;

			if(p->fds[i].events == 0){

				for(j = i + 1; j < p->nfds; j++){

					p->fds[j - 1].fd = p->fds[j].fd;
					p->fds[j - 1].events = p->fds[j].events;
				}

				p->nfds--;
				p->fds = (struct pollfd *)nxs_realloc(p->fds, sizeof(struct pollfd) * p->nfds);
			}

			return NXS_POLL_E_OK;
		}
	}

	return NXS_POLL_E_EXIST;
}

short nxs_poll_get_res(nxs_poll_t *p, int fd)
{
	int i;

	if(p == NULL){

		return NXS_POLL_E_INIT;
	}

	if(fd < 0){

		return NXS_POLL_E_INDEX;
	}

	for(i = 0; i < p->nfds; i++){

		if(p->fds[i].fd == fd){

			return p->fds[i].revents;
		}
	}

	return NXS_POLL_E_EXIST;
}

short nxs_poll_check_events(nxs_poll_t *p, int fd, short events)
{
	int i;

	if(p == NULL){

		return NXS_POLL_E_INIT;
	}

	if(fd < 0){

		return NXS_POLL_E_INDEX;
	}

	for(i = 0; i < p->nfds; i++){

		if(p->fds[i].fd == fd){

			if((p->fds[i].revents & events) == events){

				return NXS_POLL_EVENTS_TRUE;
			}

			return NXS_POLL_EVENTS_FALSE;
		}
	}

	return NXS_POLL_E_EXIST;
}

u_int nxs_poll_fds_ready(nxs_poll_t *p)
{

	return p->fds_ready;
}
