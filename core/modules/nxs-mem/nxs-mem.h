#ifndef _INCLUDE_NXS_MEM_H
#define _INCLUDE_NXS_MEM_H

// clang-format off

/** @addtogroup nxs-mem
 *
 * @brief Module 'nxs-mem' is a basic module providing functions to work with memory.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

void				*nxs_memzero				(void *ptr, size_t size);
void				*nxs_memcpy				(void *dst, void *src, size_t size);
void				*nxs_malloc				(void *old_ptr, size_t size);
void				*nxs_calloc				(void *old_ptr, size_t size);
void				*nxs_realloc				(void *old_ptr, size_t size);
void				*nxs_free				(void *ptr);

/** @} */ // end of nxs-mem
#endif /* _INCLUDE_NXS_MEM_H */
