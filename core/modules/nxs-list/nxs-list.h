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

enum nxs_list_init_e
{
	NXS_LIST_PTR_INIT_HEAD,
	NXS_LIST_PTR_INIT_TAIL
};

enum nxs_list_move_e
{
	NXS_LIST_MOVE_NEXT,
	NXS_LIST_MOVE_PREV
};

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

	size_t			size;
	size_t			count;

	/*
	 * Callback-функция для инициализации создаваемых элементов списка
	 */
	void			(*init_callback)(void *element);

	/*
	 * Callback-функция для удалении элементов списка
	 */
	void			(*free_callback)(void *element);
};

#define nxs_list_init2(list, type) \
		nxs_list_init(list, sizeof(type), NULL, NULL)
#define nxs_list_init3(list, type, init_callback, free_callback) \
		nxs_list_init(list, sizeof(type), init_callback, free_callback)

#define nxs_list_data_get(list) \
		nxs_list_data_get(list);

nxs_list_t			*nxs_list_malloc			(size_t size, void (*init_callback)(void *element), void (*free_callback)(void *element));
nxs_list_t			*nxs_list_destroy			(nxs_list_t *list);
void				nxs_list_init				(nxs_list_t *list, size_t size, void (*init_callback)(void *element), void (*free_callback)(void *element));
void				nxs_list_init_string			(nxs_list_t *list);
void				nxs_list_init_buf			(nxs_list_t *list);
void				nxs_list_free				(nxs_list_t *list);
void				nxs_list_clear				(nxs_list_t *list);
void				*nxs_list_add_head			(nxs_list_t *list);
void				*nxs_list_add_tail			(nxs_list_t *list);
void				*nxs_list_add_after			(nxs_list_t *list);
void				*nxs_list_add_before			(nxs_list_t *list);
void				*nxs_list_del_head			(nxs_list_t *list);
void				*nxs_list_del_tail			(nxs_list_t *list);
void				*nxs_list_del				(nxs_list_t *list, nxs_list_move_t ptr_move);
void				*nxs_list_get				(nxs_list_t *list);
void				*nxs_list_get_head			(nxs_list_t *list);
void				*nxs_list_get_tail			(nxs_list_t *list);
void				*nxs_list_ptr_init			(nxs_list_t *list, nxs_list_init_t type);
void				*nxs_list_ptr_set			(nxs_list_t *list, nxs_list_el_t *ptr);
void				*nxs_list_ptr_next			(nxs_list_t *list);
void				*nxs_list_ptr_prev			(nxs_list_t *list);
nxs_list_el_t			*nxs_list_ptr_get			(nxs_list_t *list);
size_t				nxs_list_count				(nxs_list_t *list);
size_t				nxs_list_size				(nxs_list_t *list);

/** @} */ // end of nxs-list
#endif /* _INCLUDE_NXS_LIST_H */
