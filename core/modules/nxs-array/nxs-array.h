#ifndef _INCLUDE_NXS_ARRAY_H
#define _INCLUDE_NXS_ARRAY_H

#include <nxs-core/nxs-core.h>

#define NXS_ARRAY_STEP_DEFAULT	10

struct nxs_array_s
{
	/*
	 * Количество допустимых элементов в массиве (размер массива)
	 */
	u_int		nalloc;

	/*
	 * Рзмер данных
	 */
	size_t		size;

	/*
	 * Количество использованных элементов массива
	 */
	u_int		count;

	/*
	 * Количество элементов, выделяемых при расширении массива
	 */
	u_int		step;

	/*
	 * Указатель на массив данных
	 */
	void		*el;
};

#define nxs_array_init2(array, type) \
		nxs_array_init(array, 0, sizeof(type), 1)

nxs_array_t		*nxs_array_malloc		(u_int nalloc, size_t size, u_int step);
void			*nxs_array_destroy		(nxs_array_t *array);
void			nxs_array_init			(nxs_array_t *array, u_int nalloc, size_t size, u_int step);
void			nxs_array_free			(nxs_array_t *array);
void			nxs_array_clear			(nxs_array_t *array);
void			*nxs_array_add			(nxs_array_t *array);
void			*nxs_array_add_i		(nxs_array_t *array, u_int i);
void			*nxs_array_get			(nxs_array_t *array, u_int i);
u_int			nxs_array_nalloc		(nxs_array_t *array);
u_int			nxs_array_count			(nxs_array_t *array);
u_int			nxs_array_size			(nxs_array_t *array);
u_int			nxs_array_step			(nxs_array_t *array);

#endif /* _INCLUDE_NXS_ARRAY_H */
