#ifndef _INCLUDE_NXS_ARRAY_H
#define _INCLUDE_NXS_ARRAY_H

/** @addtogroup nxs-array
 *
 * @brief Module 'nxs-array' is a basic module providing functions to work with arrays.
 *
 * In Nixys Framework an array is a pointer to a block of memory. Elements are distinguished by
 * setting nxs_array_s#size which defines amount of memory allocated for a single element.
 *
 * The size of the array itself can be counted as nxs_array_s#nalloc * nxs_array_s#size.
 *
 * The length of an array is defined by nxs_array_s#count. 
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_ARRAY_STEP_DEFAULT	10

struct nxs_array_s
{
	u_int		nalloc; /**< max allowed number of array elements*/
	size_t		size; /**< size of an element stored in array in bytes*/
	u_int		count; /**< number of elements stored in array*/
	u_int		step; /**< a number by which the nxs_array_nalloc can be extended at once*/
	void		*el; /**< pointer to the data*/
};


/**
* @brief Allocates memory for a new array and initializes it by allocating \b size * \b nalloc bytes of memory set to zero for its elements.
*
* @param nalloc Maximum number of elements in a new array.
* @param size Amount of memory allocated for a single element of the array.
* @param step A value by which the size of the array is extended when it's necessary.
*
* @return
* * Pointer to the new array.
*/
nxs_array_t		*nxs_array_malloc		(u_int nalloc, size_t size, u_int step);

/**
 * @brief Destroys an array.
 *
 * Deallocates memory for the contents of \b array and then frees the pointer to it.
 *
 * @param buf Pointer to the array to be destroyed.
 *
 * @return
 * * \b NULL
 */
void			*nxs_array_destroy		(nxs_array_t *array);

/**
 * @brief Initializes an array by allocating \b size * \b nalloc bytes of memory set to zero for its elements.
 *
 * @param array Pointer to the array.
 * @param nalloc Maximum number of elements for the array.
 * @param size Amount of memory allocated for a single element of the array.
 * @param step A value by which the size of the array is extended when it's necessary.
 *
 * @return
 * * none
 */
void			nxs_array_init			(nxs_array_t *array, u_int nalloc, size_t size, u_int step);

/**
 * @brief Deallocates memory for the data stored in an array and sets the number of its elements to 0.
 *
 * @param array Pointer to the array.
 *
 * @return
 * * none
 */
void			nxs_array_free			(nxs_array_t *array);

/**
 * @brief Clears an array by setting its nxs_array_s#count to 0.
 *
 * As a result the old data will be rewritten when new elements are added.
 *
 * @param array Pointer to the array
 *
 * @return
 * * none
 */
void			nxs_array_clear			(nxs_array_t *array);

/**
 * @brief Adds an element to the end of an array.
 *
 * If the new element exceeds nxs_array_s#nalloc, additional memory will be allocated for it.
 *
 * The memory allocated for the element is filled with zeroes.
 *
 * @param Pointer to the array
 *
 * @return
 * * Pointer to the new element of \b array.
 */
void			*nxs_array_add			(nxs_array_t *array);

/**
 * @brief Adds an element at the specified index of an array.
 *
 * If new element exceeds nxs_array_s#nalloc, additional memory will be allocated for it.
 * If \b i is beyond array's size, it will be extended and the allocated memory will be filled with zeroes.
 *
 * @param array Pointer to the array.
 * @param i Index to add the new element at
 *
 * @return
 * * Pointer to the new element of \b array.
 */
void			*nxs_array_add_i		(nxs_array_t *array, u_int i);

/**
 * @brief Gets an element at the specified index of an array.
 *
 * @param array Pointer to the array.
 * @param i Index of the element.
 *
 * @return
 * * On success, a pointer to the element at \b i position.
 * * NULL		- If \b array is a null pointer or \b is beyond the index of the last element of \b array.
 */
void			*nxs_array_get			(nxs_array_t *array, u_int i);

/**
 * @brief Returns the maximum number of elements for an array.
 *
 * @param array Pointer to the array
 *
 * @return
 * * 0		- If \b array is a null pointer.
 * * nxs_array_s#nalloc value for \b array.
 */
u_int			nxs_array_nalloc		(nxs_array_t *array);

/**
 * @brief Returns the number of elements currently stored in an array.
 *
 * @param array Pointer to the array.
 *
 * @return
 * * 0		- If \b array is a null pointer.
 * * nxs_array_count value for \b array.
 */
u_int			nxs_array_count			(nxs_array_t *array);

/**
 * @brief Returns the amount of memory allocated for a single element of an array.
 *
 * @param array Pointer to the array.
 *
 * @return
 * * 0		- If \b array is a null pointer.
 * * nxs_array_size value for \b array.
 */
u_int			nxs_array_size			(nxs_array_t *array);

/**
 * @brief Returns the value of nxs_array_s#step for an array.
 *
 * nxs_array_s#step defines how much more elements can be stored in an array once it is extended.
 *
 * @param array Pointer to the array.
 *
 * @return
 * * 0		- If \b array is a null pointer.
 * * nxs_array_s#step value for \b array.
 */
u_int			nxs_array_step			(nxs_array_t *array);

/** @} */ //end of nxs-array
#endif /* _INCLUDE_NXS_ARRAY_H */
