#ifndef _INCLUDE_NXS_STACK_H
#define _INCLUDE_NXS_STACK_H

// clang-format off

/** @addtogroup nxs-stack
 *
 * @brief Module 'nxs-stack' is a basic module providing functions to work with memory stack.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

struct nxs_stack_el_s
{
	void			*data;
	struct nxs_stack_el_s	*next;
};

struct nxs_stack_s
{
	nxs_stack_el_t		*head;

	size_t			count;
	size_t			size;
};

nxs_stack_t			*nxs_stack_malloc			(size_t size);
nxs_stack_t			*nxs_stack_destroy			(nxs_stack_t *stack);
void				nxs_stack_init				(nxs_stack_t *stack, size_t size);
void				nxs_stack_free				(nxs_stack_t *stack);
void				*nxs_stack_push				(nxs_stack_t *stack);
void				*nxs_stack_pull				(nxs_stack_t *stack);
size_t				nxs_stack_count				(nxs_stack_t *stack);
size_t				nxs_stack_size				(nxs_stack_t *stack);
void				*nxs_stack_get				(nxs_stack_t *stack);

/** @} */ // end of nxs-fsstack
#endif /* _INCLUDE_NXS_STACK_H */
