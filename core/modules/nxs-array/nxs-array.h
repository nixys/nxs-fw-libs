#ifndef _INCLUDE_NXS_ARRAY_H
#define _INCLUDE_NXS_ARRAY_H

// clang-format off

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
	/*
	 * Количество допустимых элементов в массиве (размер массива)
	 */
	size_t			nalloc;

	/*
	 * Рзмер данных
	 */
	size_t			size;

	/*
	 * Количество использованных элементов массива
	 */
	size_t			count;

	/*
	 * Количество элементов, выделяемых при расширении массива
	 */
	size_t			step;

	/*
	 * Указатель на массив данных
	 */
	void			*el;

	/*
	 * Callback-функция для инициализации создаваемых элементов массива
	 */
	void			(*init_callback)(void *element);

	/*
	 * Callback-функция для удалении элементов массива
	 */
	void			(*free_callback)(void *element);
};

#define nxs_array_init2(array, type) \
		nxs_array_init(array, 0, sizeof(type), 1, NULL, NULL)
#define nxs_array_init3(array, type, init_callback, free_callback) \
		nxs_array_init(array, 0, sizeof(type), 1, init_callback, free_callback)

nxs_array_t			*nxs_array_malloc			(size_t nalloc, size_t size, size_t step, void (*init_callback)(void *element), void (*free_callback)(void *element));
void				*nxs_array_destroy			(nxs_array_t *array);
void				nxs_array_init				(nxs_array_t *array, size_t nalloc, size_t size, size_t step, void (*init_callback)(void *element), void (*free_callback)(void *element));
void				nxs_array_init_string			(nxs_array_t *array);
void				nxs_array_init_buf			(nxs_array_t *array);
void				nxs_array_free				(nxs_array_t *array);
void				nxs_array_clear				(nxs_array_t *array);
void				*nxs_array_add				(nxs_array_t *array);
void				*nxs_array_add_i			(nxs_array_t *array, size_t i);
void				*nxs_array_get				(nxs_array_t *array, size_t i);
size_t				nxs_array_nalloc			(nxs_array_t *array);
size_t				nxs_array_count				(nxs_array_t *array);
size_t				nxs_array_size				(nxs_array_t *array);
size_t				nxs_array_step				(nxs_array_t *array);

/** @} */ // end of nxs-array
#endif /* _INCLUDE_NXS_ARRAY_H */
