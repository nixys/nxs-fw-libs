#ifndef _INCLUDE_NXS_MEM_H
#define _INCLUDE_NXS_MEM_H

/** @addtogroup nxs-mem
 *
 * @brief Module 'nxs-mem' is a basic module representing wrap-arounds for standard memory functions.
 *
 * Its purpose is to simplify the usage of memory functions by preventing possible exceptions and segfaults with included NULL-checks and similar routine.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

/**
* @brief Fills a block of memory with zeroes.
* 
* Sets the first \b size bytes of the block of memory pointed by \b ptr to zero.
*
* Does nothing if \b ptr is a null pointer.
*
* @param ptr Pointer to the block of memory to fill.
* @param size Number of bytes to be set to zero.
*
* @return
* * On success, \b ptr is returned.
* * \b NULL if \b ptr is a null pointer.
*/
void				nxs_memzero				(void *ptr, size_t size);

/**
* @brief Copies block of memory
*
* Copies the values of \b size bytes from the location pointed to by \b src to the memory block pointed to by \b dst.
*
* Does nothing if either \b dst or \b src  is a null pointer.
*
* @param dst Pointer to the destination array, casted to a pointer of type void*.
* @param src Pointer to the source of data to be copied, casted to a pointer of type void*.
* @param size Number of bytes to copy.
*
* @return
* * On success, \b dst is returned
* * \b NULL if either \b dst or \b src is a null pointer.
*/
void				nxs_memcpy				(void *dst, void *src, size_t size);

/**
* @brief Allocates memory block
*
* Allocates a block of \b size bytes of memory, returning a pointer to the beginning of the block. 
* 
* The \b old_ptr parameter is used to avoid memory leaks by allocating a new memory block for a pointer which is already pointing to an occupied memory block.
*
* @warning If it is the case described avobe the program will exit with status code 1.
*
* @param old_ptr Pointer to which the returned value is going to be assigned.
* @param size Size of the memory block in bytes.
*
* @return
* * On success, a pointer to the allocated memory block.
* * Null pointer on failure
*
*/
void				*nxs_malloc				(void *old_ptr, size_t size);

/**
 * @brief Allocates and zero-initializes an array.
 *
 * Allocates a block of memory with the size of \b size bytes and initializes all its bits to zero.
 *
 * The \b old_ptr parameter is used to avoid memory leaks by allocating a new memory block for a pointer which is already pointing to an occupied memory block.
 *
 * @warning If it is the case described avobe the program will exit with status code 1.
 *
 * @param old_ptr Pointer to which the returned value is going to be assigned.
 * @param size Size of the memory block in bytes.
 *
 * @return
 * * On success, a pointer to the allocated memory block.
 * * Null pointer on failure.
 */
void				*nxs_calloc				(void *old_ptr, size_t size);

/**
 * @brief Reallocates memory block
 *
 * Changes the size of the memory block pointed to by \b ptr.
 *
 * The function may move the memory block to a new location (its address is returned by the function).
 *
 * In case that ptr is a null pointer, the function behaves like nxs_malloc(), assigning a new block of \b 	size bytes and returning a pointer to its beginning.
 *
 * If the function fails to allocate the requested block of memory, a null pointer is returned, and the memory block pointed to by argument \b ptr is not deallocated (it is still valid, and with its contents unchanged).
 *
 * @param old_ptr Pointer to previously allocated memory block.
 * @param size New size of the memory block.
 *
 * @return A pointer to the reallocated memory block, which can be either the same as \b or a new location
 */
void				*nxs_realloc			(void *old_ptr, size_t size);

/**
 * @brief Deallocates memory block
 *
 * Deallocates a previously allocated memory block, making it available for further allocations
 *
 * @param ptr Pointer to a previously allocated memory block.
 *
 * @return \b NULL
 */
void				*nxs_free				(void *ptr);

/** @} */
#endif /* _INCLUDE_NXS_MEM_H */
