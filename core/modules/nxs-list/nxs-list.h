#ifndef _INCLUDE_NXS_LIST_H
#define _INCLUDE_NXS_LIST_H

// clang-format off

/** @addtogroup nxs-list
 *
 * @brief Module 'nxs-list' is a basic module providing functions to work with lists.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_LIST_PTR_INIT_HEAD		0
#define NXS_LIST_PTR_INIT_TAIL		1

#define NXS_LIST_MOVE_NEXT		0
#define NXS_LIST_MOVE_PREV		1

struct nxs_list_el_s
{
	struct nxs_list_el_s	*next;
	struct nxs_list_el_s	*prev;

	void			*data;
};

struct nxs_list_s
{
	nxs_list_el_t		*head;
	nxs_list_el_t		*tail;
	nxs_list_el_t		*ptr;

	u_int			size;
	u_int			count;
};

nxs_list_t			*nxs_list_malloc			(u_int size);
nxs_list_t			*nxs_list_destroy			(nxs_list_t *list);
void				nxs_list_init				(nxs_list_t *list, u_int size);
void				nxs_list_free				(nxs_list_t *list);
void				*nxs_list_add_head			(nxs_list_t *list);
void				*nxs_list_add_tail			(nxs_list_t *list);
void				*nxs_list_add_after			(nxs_list_t *list);
void				*nxs_list_add_before			(nxs_list_t *list);
void				*nxs_list_del_head			(nxs_list_t *list);
void				*nxs_list_del_tail			(nxs_list_t *list);
void				*nxs_list_del				(nxs_list_t *list, u_int ptr_move);
void				*nxs_list_ptr_init			(u_int type, nxs_list_t *list);
void				*nxs_list_ptr_set			(nxs_list_t *list, nxs_list_el_t *ptr);
void				*nxs_list_ptr_next			(nxs_list_t *list);
void				*nxs_list_ptr_prev			(nxs_list_t *list);
nxs_list_el_t			*nxs_list_ptr_get			(nxs_list_t *list);
void				*nxs_list_data_get			(nxs_list_t *list);
int				nxs_list_count				(nxs_list_t *list);
int				nxs_list_size				(nxs_list_t *list);

/** @} */ // end of nxs-list
#endif /* _INCLUDE_NXS_LIST_H */
