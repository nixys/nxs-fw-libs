// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */



/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

/* Module global functions */

// clang-format on

nxs_stack_t *nxs_stack_malloc(size_t size)
{
	nxs_stack_t *stack = NULL;

	stack = (nxs_stack_t *)nxs_malloc(stack, sizeof(nxs_stack_t));

	nxs_stack_init(stack, size);

	return stack;
}

nxs_stack_t *nxs_stack_destroy(nxs_stack_t *stack)
{

	if(stack == NULL) {

		return NULL;
	}

	nxs_stack_free(stack);

	return nxs_free(stack);
}

void nxs_stack_init(nxs_stack_t *stack, size_t size)
{

	if(stack == NULL) {

		return;
	}

	stack->count = 0;
	stack->head  = NULL;
	stack->size  = size;
}

void nxs_stack_free(nxs_stack_t *stack)
{

	while(nxs_stack_pull(stack)) {
	};
}

void *nxs_stack_push(nxs_stack_t *stack)
{
	nxs_stack_el_t *p = NULL;

	if(stack == NULL) {

		return NULL;
	}

	p = (nxs_stack_el_t *)nxs_calloc(p, sizeof(nxs_stack_el_t));

	p->data = nxs_calloc(p->data, stack->size);

	p->next = stack->head;

	stack->head = p;

	stack->count++;

	return p->data;
}

void *nxs_stack_pull(nxs_stack_t *stack)
{
	nxs_stack_el_t *p;

	if(stack == NULL) {

		return NULL;
	}

	if(stack->head == NULL) {

		return NULL;
	}

	p = stack->head->next;

	stack->head->data = nxs_free(stack->head->data);
	stack->head       = nxs_free(stack->head);

	stack->head = p;

	stack->count--;

	if(p == NULL) {

		return NULL;
	}
	else {

		return p->data;
	}
}

size_t nxs_stack_count(nxs_stack_t *stack)
{

	if(stack == NULL) {

		return 0;
	}

	return stack->count;
}

size_t nxs_stack_size(nxs_stack_t *stack)
{

	if(stack == NULL) {

		return 0;
	}

	return stack->size;
}

void *nxs_stack_get(nxs_stack_t *stack)
{

	if(stack == NULL) {

		return NULL;
	}

	return stack->head->data;
}

/* Module internal (static) functions */
