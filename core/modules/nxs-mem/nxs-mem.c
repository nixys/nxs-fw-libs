#include <nxs-core/nxs-core.h>

void *nxs_memzero(void *ptr, size_t size)
{
	if(ptr != NULL){

		return memset(ptr, 0, size);
	}

	return NULL;
}

void *nxs_memcpy(void *dst, void *src, size_t size)
{

	if(dst == NULL || src == NULL){

		return NULL;
	}

	return memcpy(dst, src, size);
}

void *nxs_malloc(void *old_ptr, size_t size)
{

	if(old_ptr != NULL){

		exit(1);
	}

	return malloc(size);
}

void *nxs_calloc(void *old_ptr, size_t size)
{
	void	*ptr = NULL;

	if((ptr = nxs_malloc(old_ptr, size)) != NULL){

		nxs_memzero(ptr, size);
	}

	return ptr;
}

void *nxs_realloc(void *old_ptr, size_t size)
{

	return realloc(old_ptr, size);
}

void *nxs_free(void *ptr)
{
	if(ptr != NULL){

		free(ptr);
	}

	return NULL;
}
