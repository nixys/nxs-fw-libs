#ifndef _INCLUDE_NXS_ARRAY_H
#define _INCLUDE_NXS_ARRAY_H

/** @addtogroup nxs-array
 *
 * @brief Module 'nxs-array' is a basic module providing functions to work with arrays.
 *
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
	void		*el; /**< pointer to the data array */
};

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
 * @param array Pointer to the array.
 * @param nalloc Maximum number of elements for the array.
 * @param size
 * @return
 * * none
 */
void			nxs_array_init			(nxs_array_t *array, u_int nalloc, size_t size, u_int step);

/**
 * @brief Deallocates memory for an array and sets the number of its elements to 0.
 *
 * @param array Pointer to the array.
 *
 * @return
 * * none
 */
void			nxs_array_free			(nxs_array_t *array);

/**
 * @brief Clears an array by setting its nxs_array_s#count to 0
 *
 * @param array Pointer to the array
 *
 * @return
 * * none
 */
void			nxs_array_clear			(nxs_array_t *array);

/**
 * @brief Adds an element to the end of the array.
 *
 * If new element exceeds nxs_array_s#nalloc, additional memory will be allocated for it.
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
 * @brief Return an element at the specified index of an array.
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
 * @brief Returns the size of the elements of an array.
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
 * nxs_array_s#step is the number of allocated elements when the array is being extended.
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
