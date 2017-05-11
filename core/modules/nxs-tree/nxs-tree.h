#ifndef _INCLUDE_NXS_TREE_H
#define _INCLUDE_NXS_TREE_H

// clang-format off

/** @addtogroup nxs-tree
 *
 * @brief Module 'nxs-tree' is a basic module providing functions to work with trees.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define	NXS_TREE_DEL_CHLD_ORDER		0
#define	NXS_TREE_DEL_CHLD_TAIL		1

#define	NXS_TREE_DEL_CHLD_OK		0
#define	NXS_TREE_DEL_CHLD_ERR_PTR	1
#define	NXS_TREE_DEL_CHLD_ERR_INDEX	2
#define	NXS_TREE_DEL_CHLD_ERR_CHLD	3

struct nxs_tree_el_s
{
	void			*data;

	struct nxs_tree_el_s	**childs;
	struct nxs_tree_el_s	*parent;

	int			childs_count;
};

struct nxs_tree_s
{
	u_int			size;
	int			count;

	nxs_tree_el_t		*root;

	/*
	 * Указатель на текущий корневой элемент
	 */
	nxs_tree_el_t		*c_root;

	/*
	 * Указатель на текущий элемент
	 */
	nxs_tree_el_t		*c_el;
};

nxs_tree_t			*nxs_tree_malloc			(u_int size);
nxs_tree_t			*nxs_tree_destroy			(nxs_tree_t *tree);
void				*nxs_tree_init				(nxs_tree_t *tree, u_int size);
/*void				nxs_tree_free				(nxs_tree_t *tree);*/
void				*nxs_tree_add				(nxs_tree_t *tree);
int				nxs_tree_del_child			(nxs_tree_t *tree, int index, int mode);
void				nxs_tree_chroot				(nxs_tree_t *tree);
void				nxs_tree_chroot_up			(nxs_tree_t *tree);
void				*nxs_tree_ptr_c_el_init			(nxs_tree_t *tree);
void				*nxs_tree_ptr_c_root_init		(nxs_tree_t *tree);
void				*nxs_tree_ptr_c_el_set			(nxs_tree_t *tree, int index);
void				*nxs_tree_get_c_root_parent		(nxs_tree_t *tree);
void				*nxs_tree_get_c_root			(nxs_tree_t *tree);
void				*nxs_tree_get_c_el			(nxs_tree_t *tree);
int				nxs_tree_get_root_cc			(nxs_tree_t *tree);
int				nxs_tree_get_c_root_cc			(nxs_tree_t *tree);
int				nxs_tree_get_c_el_cc			(nxs_tree_t *tree);

/** @} */ // end of nxs-tree
#endif /* _INCLUDE_NXS_TREE_H */
