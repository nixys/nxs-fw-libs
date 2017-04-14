#ifndef _INCLUDE_NXS_MEM_H
#define _INCLUDE_NXS_MEM_H

#include <nxs-core/nxs-core.h>

void										*nxs_memzero								(void *ptr, size_t size);
void										*nxs_memcpy									(void *dst, void *src, size_t size);
void										*nxs_malloc									(void *old_ptr, size_t size);
void										*nxs_calloc									(void *old_ptr, size_t size);
void										*nxs_realloc								(void *old_ptr, size_t size);
void										*nxs_free									(void *ptr);

#endif /* _INCLUDE_NXS_MEM_H */
