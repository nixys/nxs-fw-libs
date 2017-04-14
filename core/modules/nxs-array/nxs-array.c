#include <nxs-core/nxs-core.h>

nxs_array_t *nxs_array_malloc(u_int nalloc, size_t size, u_int step)
{
	nxs_array_t	*array = NULL;

	array = nxs_calloc(array, sizeof(nxs_array_t));

	nxs_array_init(array, nalloc, size, step);

	return array;
}

void *nxs_array_destroy(nxs_array_t *array)
{

	if(array == NULL){

		return NULL;
	}

	nxs_array_free(array);

	return nxs_free(array);
}

void nxs_array_init(nxs_array_t *array, u_int nalloc, size_t size, u_int step)
{

	if(array == NULL){

		return;
	}

	if(size == 0){

		return;
	}

	array->size = size;
	array->count = 0;
	array->step = step == 0 ? NXS_ARRAY_STEP_DEFAULT : step;

	array->el = NULL;

	if(nalloc == 0){

		array->nalloc = 0;

		return;
	}

	array->nalloc = nalloc;
	array->el = nxs_calloc(array->el, array->size * array->nalloc);
}

void nxs_array_free(nxs_array_t *array)
{

	if(array == NULL){

		return;
	}

	array->el = nxs_free(array->el);

	array->count = array->nalloc = 0;
}

void nxs_array_clear(nxs_array_t *array)
{

	if(array == NULL){

		return;
	}

	array->count = 0;
}

void *nxs_array_add(nxs_array_t *array)
{

	array->count++;

	if(array->count > array->nalloc){

		array->el = nxs_realloc(array->el, array->size * (array->nalloc + array->step));

		nxs_memzero((u_char *)array->el + array->size * array->nalloc, array->size * array->step);

		array->nalloc += array->step;
	}

	return (u_char *)array->el + array->size * (array->count - 1);
}

/*
 * Добавление в массив i-го элемента.
 * Если размер массива меньше номера добавляемого элемента - он будет расширен до требуемых значений. Память, выделенная под новые элементы будет обнулена
 */
void *nxs_array_add_i(nxs_array_t *array, u_int i)
{

	if(i >= array->count){

		array->count = i + 1;
	}

	if(array->count > array->nalloc){

		array->el = nxs_realloc(array->el, array->size * array->count);

		nxs_memzero((u_char *)array->el + array->size * array->nalloc, array->size * (array->count - array->nalloc));

		array->nalloc = array->count;
	}

	return (u_char *)array->el + array->size * i;
}

/*
 * Получение элемента массива
 */
void *nxs_array_get(nxs_array_t *array, u_int i)
{

	if(array == NULL){

		return NULL;
	}

	if(i >= array->count){

		return NULL;
	}

	return (u_char *)array->el + array->size * i;
}

u_int nxs_array_nalloc(nxs_array_t *array)
{

	if(array == NULL){

		return 0;
	}

	return array->nalloc;
}

u_int nxs_array_count(nxs_array_t *array)
{

	if(array == NULL){

		return 0;
	}

	return array->count;
}

u_int nxs_array_size(nxs_array_t *array)
{

	if(array == NULL){

		return 0;
	}

	return array->size;
}

u_int nxs_array_step(nxs_array_t *array)
{

	if(array == NULL){

		return 0;
	}

	return array->step;
}
