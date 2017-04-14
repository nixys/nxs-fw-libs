#ifndef _INCLUDE_NXS_STACK_H
#define _INCLUDE_NXS_STACK_H

#include <nxs-core/nxs-core.h>

struct nxs_stack_el_s
{
	void					*data;
	struct nxs_stack_el_s	*next;
};

struct nxs_stack_s
{
	nxs_stack_el_t			*head;

	int						count;
	u_int					size;
};

nxs_stack_t			*nxs_stack_malloc				(u_int size);
nxs_stack_t			*nxs_stack_destroy				(nxs_stack_t *stack);
void				nxs_stack_init					(nxs_stack_t *stack, u_int size);
void				nxs_stack_free					(nxs_stack_t *stack);
void				*nxs_stack_push					(nxs_stack_t *stack);
void				*nxs_stack_pull					(nxs_stack_t *stack);
void				*nxs_stack_get					(nxs_stack_t *stack);

#endif /* _INCLUDE_NXS_STACK_H */
