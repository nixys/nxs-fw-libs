#ifndef _INCLUDE_NXS_POLL_H
#define _INCLUDE_NXS_POLL_H

// clang-format off

/** @addtogroup nxs-poll
 *
 * @brief Module 'nxs-poll' is a basic module providing functions to work with polls.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_POLL_E_OK		0
#define NXS_POLL_E_TIMEOUT	-1
#define NXS_POLL_E_INIT		-2
#define NXS_POLL_E_EXIST	-3
#define NXS_POLL_E_POLL		-4
#define NXS_POLL_E_INDEX	-5

#define NXS_POLL_EVENTS_FALSE	0
#define NXS_POLL_EVENTS_TRUE	1

struct nxs_poll_s
{
	int			nfds;
	struct pollfd		*fds;
	u_int			fds_ready;
};

nxs_poll_t			*nxs_poll_malloc			(void);
nxs_poll_t			*nxs_poll_destroy			(nxs_poll_t *p);
int				nxs_poll_init				(nxs_poll_t *p);
int				nxs_poll_free				(nxs_poll_t *p);
int				nxs_poll_exec				(nxs_poll_t *p, int timeout);
int				nxs_poll_add				(nxs_poll_t *p, int fd, short events);
int				nxs_poll_del				(nxs_poll_t *p, int fd);
int				nxs_poll_del_event			(nxs_poll_t *p, int fd, short events);
short				nxs_poll_get_res			(nxs_poll_t *p, int fd);
short				nxs_poll_check_events			(nxs_poll_t *p, int fd, short events);
u_int				nxs_poll_fds_ready			(nxs_poll_t *p);

/** @} */ // end of nxs-poll
#endif /* _INCLUDE_NXS_POLL_H */
