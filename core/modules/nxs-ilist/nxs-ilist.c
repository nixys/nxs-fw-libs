// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define	NXS_ILIST_STEP_DEFAULT	10

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static int nxs_ilist_string_index_calc(nxs_array_t *iarray, u_char *str);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

/*
 * Инициализация индексного списка
 */
void nxs_ilist_init(nxs_ilist_t *ilist, u_int el_size, void *el_index_f, void *el_check_f)
{

	ilist->els = nxs_list_malloc(el_size);

	ilist->iarray = nxs_array_malloc(0, sizeof(nxs_list_t), NXS_ILIST_STEP_DEFAULT);

	ilist->el_index_f = el_index_f;
	ilist->el_check_f = el_check_f;
}

/*
 * Добавление нового элемента в индексный список
 *
 * TODO: Проверка наличия элемента с таким же ключом в списке
 * TODO: Изменение типа возвращаемого данной функцией значения и определение констант с кодами ошибок
 */
void *nxs_ilist_add(nxs_ilist_t *ilist, void *el, void *key)
{
	void *          el_data;
	nxs_list_t *    el_iarray;
	nxs_list_el_t **el_iarray_data;
	int             index;

	/*
	 * Получение индексов добавленного элемента
	 */
	if((index = ilist->el_index_f(key)) == NXS_ILIST_INDEX_UNDEF) {

		return NULL;
	}

	printf("ilist index: %d\n", index);

	/*
	 * Добавление элемента в конец списка
	 */
	el_data = nxs_list_add_tail(ilist->els);
	nxs_memcpy(el_data, el, nxs_list_size(ilist->els));

	/*
	 * Создание индексов для добавленного элемента
	 */

	/*
	 * Получение списка с указателями на элементы из массива индексов
	 */
	el_iarray = nxs_array_add_i(ilist->iarray, index);

	/*
	 * Если список не инициализирован
	 */
	if(nxs_list_size(el_iarray) == 0) {

		/*
		 * В качестве данных этот список содержит указатели на элементы списка "els"
		 */
		nxs_list_init(el_iarray, sizeof(nxs_list_el_t *));
	}

	/*
	 * Добавляем новый элемент в список "ilist->iarray[index]".
	 * Новый элемент этого списка указывает на последний созданный элемент списка "els".
	 */
	el_iarray_data    = nxs_list_add_tail(el_iarray);
	(*el_iarray_data) = ilist->els->tail;

	return el_data;
}

nxs_list_el_t *nxs_ilist_add_no_index(nxs_ilist_t *ilist, void *el)
{
	void *el_data;

	/*
	 * Добавление элемента в конец списка
	 */
	el_data = nxs_list_add_tail(ilist->els);
	nxs_memcpy(el_data, el, nxs_list_size(ilist->els));

	return ilist->els->tail;
}

int nxs_ilist_add_index(nxs_ilist_t *ilist, nxs_list_el_t *list_el, void *key)
{
	int index;

	/*
	 * Получение индексов для ключа key
	 */
	if((index = ilist->el_index_f(key)) == NXS_ILIST_INDEX_UNDEF) {

		return NULL;
	}

	/*
	 * Создание индексов для добавленного элемента
	 */

	/*
	 * Получение списка с указателями на элементы из массива индексов
	 */
	el_iarray = nxs_array_add_i(ilist->iarray, index);

	/*
	 * Если список не инициализирован
	 */
	if(nxs_list_size(el_iarray) == 0) {

		/*
		 * В качестве данных этот список содержит указатели на элементы списка "els"
		 */
		nxs_list_init(el_iarray, sizeof(nxs_list_el_t *));
	}

	/*
	 * Добавляем новый элемент в список "ilist->iarray[index]".
	 * Новый элемент этого списка указывает на последний созданный элемент списка "els".
	 */
	el_iarray_data    = nxs_list_add_tail(el_iarray);
	(*el_iarray_data) = list_el;

	return 0;
}

/*
 * Поиск элемента в списке "ilist" по ключу "key"
 */
void *nxs_ilist_find(nxs_ilist_t *ilist, void *key)
{
	nxs_list_t *    el_iarray;
	nxs_list_el_t **el_iarray_data;
	int             index;

	/*
	 * Получение индекса элемента по ключу "key"
	 */
	if((index = ilist->el_index_f(key)) == NXS_ILIST_INDEX_UNDEF) {

		return NULL;
	}

	printf("nxs_ilist_find: index: %d\n", index);

	/*
	 * Получение списка с указателями на элементы из массива индексов для индекса "index"
	 */
	if((el_iarray = nxs_array_get(ilist->iarray, index)) == NULL) {

		return NULL;
	}

	/*
	 * Проход по всем элементам списка "el_iarray" и поиск нужного элемента
	 */
	for(el_iarray_data = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, el_iarray); el_iarray_data != NULL;
	    el_iarray_data = nxs_list_ptr_next(el_iarray)) {

		printf("index: %d, ilist->index: %p\n", index, (*el_iarray_data));

		if(ilist->el_check_f((*el_iarray_data)->data, key) == NXS_ILIST_EL_MATCH) {

			return (*el_iarray_data)->data;
		}
	}

	return NULL;
}

void nxs_ilist_string_init(nxs_ilist_t *ilist)
{

	ilist->els = nxs_list_malloc(sizeof(nxs_string_t));

	ilist->iarray = nxs_array_malloc(0, sizeof(nxs_list_el_t), NXS_ILIST_STEP_DEFAULT);

	ilist->el_index_f = nxs_ilist_string_index_calc;
}

/* Module internal (static) functions */

static int nxs_ilist_string_index_calc(nxs_array_t *iarray, u_char *str)
{
	u_int *d;

	if(str == NULL) {

		return -1;
	}

	d = nxs_array_add(iarray);

	(*d) = str[0];

	return nxs_array_count(iarray);
}
