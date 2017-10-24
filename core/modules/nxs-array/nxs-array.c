// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */



/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_array_init_callback_string(void *element);
static void nxs_array_free_callback_string(void *element);

static void nxs_array_init_callback_buf(void *element);
static void nxs_array_free_callback_buf(void *element);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_array_t *nxs_array_malloc(size_t nalloc,
                              size_t size,
                              size_t step,
                              void (*init_callback)(void *element),
                              void (*free_callback)(void *element))
{
	nxs_array_t *array = NULL;

	array = nxs_calloc(array, sizeof(nxs_array_t));

	nxs_array_init(array, nalloc, size, step, init_callback, free_callback);

	return array;
}

void *nxs_array_destroy(nxs_array_t *array)
{

	if(array == NULL) {

		return NULL;
	}

	nxs_array_free(array);

	return nxs_free(array);
}

void nxs_array_init(nxs_array_t *array,
                    size_t       nalloc,
                    size_t       size,
                    size_t       step,
                    void (*init_callback)(void *element),
                    void (*free_callback)(void *element))
{

	if(array == NULL) {

		return;
	}

	if(size == 0) {

		return;
	}

	array->size  = size;
	array->count = 0;
	array->step  = step == 0 ? NXS_ARRAY_STEP_DEFAULT : step;

	array->el = NULL;

	array->init_callback = init_callback;
	array->free_callback = free_callback;

	if(nalloc == 0) {

		array->nalloc = 0;

		return;
	}

	array->nalloc = nalloc;
	array->el     = nxs_calloc(array->el, array->size * array->nalloc);
}

void nxs_array_init_string(nxs_array_t *array)
{

	if(array == NULL) {

		return;
	}

	nxs_array_init(array, 0, sizeof(nxs_string_t), 1, &nxs_array_init_callback_string, &nxs_array_free_callback_string);
}

void nxs_array_init_buf(nxs_array_t *array)
{

	if(array == NULL) {

		return;
	}

	nxs_array_init(array, 0, sizeof(nxs_buf_t), 1, &nxs_array_init_callback_buf, &nxs_array_free_callback_buf);
}

void nxs_array_free(nxs_array_t *array)
{
	size_t i;
	void * e;

	if(array == NULL) {

		return;
	}

	if(array->free_callback != NULL) {

		for(i = 0; i < array->nalloc; i++) {

			e = (u_char *)array->el + array->size * i;

			if(e != NULL) {

				array->free_callback(e);
			}
		}
	}

	array->el = nxs_free(array->el);

	array->count = array->nalloc = 0;

	array->init_callback = NULL;
	array->free_callback = NULL;
}

void nxs_array_clear(nxs_array_t *array)
{
	size_t i;
	void * e;

	if(array == NULL) {

		return;
	}

	if(array->free_callback != NULL) {

		for(i = 0; i < array->nalloc; i++) {

			e = (u_char *)array->el + array->size * i;

			if(e != NULL) {

				array->free_callback(e);
			}
		}
	}

	array->count = 0;
}

void *nxs_array_add(nxs_array_t *array)
{
	void *e;

	if(array == NULL) {

		return NULL;
	}

	array->count++;

	if(array->count > array->nalloc) {

		array->el = nxs_realloc(array->el, array->size * (array->nalloc + array->step));

		nxs_memzero((u_char *)array->el + array->size * array->nalloc, array->size * array->step);

		array->nalloc += array->step;
	}

	e = (u_char *)array->el + array->size * (array->count - 1);

	if(array->init_callback != NULL) {

		array->init_callback(e);
	}

	return e;
}

/*
 * Добавление в массив i-го элемента.
 * Если размер массива меньше номера добавляемого элемента - он будет расширен до требуемых значений. Память, выделенная под новые элементы
 * будет обнулена
 */
void *nxs_array_add_i(nxs_array_t *array, size_t i)
{
	void *e;

	if(array == NULL) {

		return NULL;
	}

	if(i >= array->count) {

		array->count = i + 1;
	}

	if(array->count > array->nalloc) {

		array->el = nxs_realloc(array->el, array->size * array->count);

		nxs_memzero((u_char *)array->el + array->size * array->nalloc, array->size * (array->count - array->nalloc));

		array->nalloc = array->count;
	}

	e = (u_char *)array->el + array->size * i;

	if(array->init_callback != NULL) {

		array->init_callback(e);
	}

	return e;
}

/*
 * Получение элемента массива
 */
void *nxs_array_get(nxs_array_t *array, size_t i)
{

	if(array == NULL) {

		return NULL;
	}

	if(i >= array->count) {

		return NULL;
	}

	return (u_char *)array->el + array->size * i;
}

size_t nxs_array_nalloc(nxs_array_t *array)
{

	if(array == NULL) {

		return 0;
	}

	return array->nalloc;
}

size_t nxs_array_count(nxs_array_t *array)
{

	if(array == NULL) {

		return 0;
	}

	return array->count;
}

size_t nxs_array_size(nxs_array_t *array)
{

	if(array == NULL) {

		return 0;
	}

	return array->size;
}

size_t nxs_array_step(nxs_array_t *array)
{

	if(array == NULL) {

		return 0;
	}

	return array->step;
}

/* Module internal (static) functions */

static void nxs_array_init_callback_string(void *element)
{
	nxs_string_t *s = element;

	nxs_string_init(s);
}

static void nxs_array_free_callback_string(void *element)
{
	nxs_string_t *s = element;

	nxs_string_free(s);
}

static void nxs_array_init_callback_buf(void *element)
{
	nxs_buf_t *b = element;

	nxs_buf_init2(b);
}

static void nxs_array_free_callback_buf(void *element)
{
	nxs_buf_t *b = element;

	nxs_buf_free(b);
}
