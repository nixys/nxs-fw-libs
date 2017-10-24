// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */



/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_list_init_callback_string(void *element);
static void nxs_list_free_callback_string(void *element);

static void nxs_list_init_callback_buf(void *element);
static void nxs_list_free_callback_buf(void *element);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_list_t *nxs_list_malloc(size_t size, void (*init_callback)(void *element), void (*free_callback)(void *element))
{
	nxs_list_t *list = NULL;

	list = (nxs_list_t *)nxs_malloc(list, sizeof(nxs_list_t));

	nxs_list_init(list, size, init_callback, free_callback);

	return list;
}

nxs_list_t *nxs_list_destroy(nxs_list_t *list)
{

	if(list == NULL) {

		return NULL;
	}

	nxs_list_free(list);

	return nxs_free(list);
}

/*
 * Инициализация списка.
 * Обнуляются все указатели, размер списка и задаётся размер данных
 */
void nxs_list_init(nxs_list_t *list, size_t size, void (*init_callback)(void *element), void (*free_callback)(void *element))
{

	if(list == NULL) {

		return;
	}

	list->ptr = list->head = list->tail = NULL;
	list->size                          = size;
	list->count                         = 0;

	list->init_callback = init_callback;
	list->free_callback = free_callback;
}

void nxs_list_init_string(nxs_list_t *list)
{

	if(list == NULL) {

		return;
	}

	nxs_list_init(list, sizeof(nxs_string_t), &nxs_list_init_callback_string, &nxs_list_free_callback_string);
}

void nxs_list_init_buf(nxs_list_t *list)
{

	if(list == NULL) {

		return;
	}

	nxs_list_init(list, sizeof(nxs_buf_t), &nxs_list_init_callback_buf, &nxs_list_free_callback_buf);
}

void nxs_list_free(nxs_list_t *list)
{
	nxs_list_el_t *p;

	if(list == NULL) {

		return;
	}

	for(list->ptr = list->head; list->ptr != NULL; list->ptr = p) {

		p = list->ptr->next;

		if(list->free_callback != NULL) {

			list->free_callback(list->ptr->data);
		}

		list->ptr->data = nxs_free(list->ptr->data);

		list->ptr = nxs_free(list->ptr);
	}

	list->ptr = list->head = list->tail = NULL;
	list->size                          = 0;
	list->count                         = 0;

	list->init_callback = NULL;
	list->free_callback = NULL;
}

void nxs_list_clear(nxs_list_t *list)
{
	nxs_list_el_t *p;

	if(list == NULL) {

		return;
	}

	for(list->ptr = list->head; list->ptr != NULL; list->ptr = p) {

		p = list->ptr->next;

		if(list->free_callback != NULL) {

			list->free_callback(list->ptr->data);
		}

		list->ptr->data = nxs_free(list->ptr->data);

		list->ptr = nxs_free(list->ptr);
	}

	list->ptr = list->head = list->tail = NULL;
	list->count                         = 0;
}

/*
 * Добавление элемента в начало списка.
 * Если указатель на текущий элемент не установлен - он определяется так, что указывает на первый элемент
 *
 * Возвращаемое значение:
 * * Указатель на данные нового элемента
 */
void *nxs_list_add_head(nxs_list_t *list)
{
	nxs_list_el_t *p = NULL;

	if(list == NULL) {

		return NULL;
	}

	p = (nxs_list_el_t *)nxs_calloc(p, sizeof(nxs_list_el_t));

	if(list->head == NULL) {

		list->head = p;
	}
	else {

		list->head->prev = p;
		p->next          = list->head;
		list->head       = p;
	}

	if(list->tail == NULL) {

		list->tail = p;
	}

	list->count++;

	if(list->ptr == NULL) {

		list->ptr = list->head;
	}

	p->data = nxs_calloc(p->data, list->size);

	if(list->init_callback != NULL) {

		list->init_callback(p->data);
	}

	return p->data;
}

/*
 * Добавление элемента в конец списка.
 * Если указатель на текущий элемент не установлен - он определяется так, что указывает на последний элемент
 *
 * Возвращаемое значение:
 * * Указатель на данные нового элемента
 */
void *nxs_list_add_tail(nxs_list_t *list)
{
	nxs_list_el_t *p = NULL;

	if(list == NULL) {

		return NULL;
	}

	p = (nxs_list_el_t *)nxs_calloc(p, sizeof(nxs_list_el_t));

	if(list->tail == NULL) {

		list->tail = p;
	}
	else {

		list->tail->next = p;
		p->prev          = list->tail;
		list->tail       = p;
	}

	if(list->head == NULL) {

		list->head = p;
	}

	list->count++;

	if(list->ptr == NULL) {

		list->ptr = list->tail;
	}

	p->data = nxs_calloc(p->data, list->size);

	if(list->init_callback != NULL) {

		list->init_callback(p->data);
	}

	return p->data;
}

/*
 * Добавить новый элемент после того элемента, на который указывает указатель списка.
 * Если указатель списка не определён - новый элемент добавляется в конец списка.
 * После добавления нового элемента указатель списка смещается вправо (т.е. указывает на новый элемент)
 *
 * Возвращаемое значение:
 * * Указатель на данные нового элемента
 */
void *nxs_list_add_after(nxs_list_t *list)
{
	nxs_list_el_t *p = NULL;

	if(list == NULL) {

		return NULL;
	}

	if(list->ptr == NULL) {

		return nxs_list_add_tail(list);
	}

	p = (nxs_list_el_t *)nxs_calloc(p, sizeof(nxs_list_el_t));

	p->next = list->ptr->next;

	if(p->next != NULL) {

		p->next->prev = p;
	}

	list->ptr->next = p;
	p->prev         = list->ptr;

	if(list->tail == list->ptr) {

		list->tail = p;
	}

	list->count++;

	list->ptr = p;

	p->data = nxs_calloc(p->data, list->size);

	if(list->init_callback != NULL) {

		list->init_callback(p->data);
	}

	return p->data;
}

/*
 * Добавить новый элемент перед тем элементом, на который указывает указатель списка.
 * Если указатель списка не определён - новый элемент добавляется в начало списка.
 * После добавления нового элемента указатель списка смещается влево (т.е. указывает на новый элемент)
 *
 * Возвращаемое значение:
 * * Указатель на данные нового элемента
 */
void *nxs_list_add_before(nxs_list_t *list)
{
	nxs_list_el_t *p = NULL;

	if(list == NULL) {

		return NULL;
	}

	if(list->ptr == NULL) {

		return nxs_list_add_head(list);
	}

	p = (nxs_list_el_t *)nxs_calloc(p, sizeof(nxs_list_el_t));

	p->prev = list->ptr->prev;

	if(p->prev != NULL) {

		p->prev->next = p;
	}

	list->ptr->prev = p;
	p->next         = list->ptr;

	if(list->head == list->ptr) {

		list->head = p;
	}

	list->count++;

	list->ptr = p;

	p->data = nxs_calloc(p->data, list->size);

	if(list->init_callback != NULL) {

		list->init_callback(p->data);
	}

	return p->data;
}

/*
 * Удалить первый элемент списка.
 * Если указатель списка указывает на первый элемент - он смещается на один элемент вправо (т.е. остаётся указывать на первый элемент)
 *
 * Возвращаемое значение:
 * * Указатель на данные текущего первого элемента списка
 * * NULL - если в списке не осталось элементов
 */
void *nxs_list_del_head(nxs_list_t *list)
{
	nxs_list_el_t *p = NULL;

	if(list == NULL) {

		return NULL;
	}

	if(list->head == NULL) {

		return NULL;
	}

	p = list->head;

	list->head = list->head->next;
	list->count--;

	if(list->ptr == p) {

		list->ptr = list->head;
	}

	if(list->free_callback != NULL) {

		list->free_callback(p->data);
	}

	p->data = nxs_free(p->data);
	p       = nxs_free(p);

	if(list->head != NULL) {

		list->head->prev = NULL;

		return list->head->data;
	}

	list->tail = NULL;

	return NULL;
}

/*
 * Удалить последний элемент списка.
 * Если указатель списка указывает на последний элемент - он смещается на один элемент влево (т.е. остаётся указывать на последний элемент)
 *
 * Возвращаемое значение:
 * * Указатель на данные текущего последнего элемента списка
 * * NULL - если в списке не осталось элементов
 */
void *nxs_list_del_tail(nxs_list_t *list)
{
	nxs_list_el_t *p = NULL;

	if(list == NULL) {

		return NULL;
	}

	if(list->tail == NULL) {

		return NULL;
	}

	p = list->tail;

	list->tail = list->tail->prev;
	list->count--;

	if(list->ptr == p) {

		list->ptr = list->tail;
	}

	if(list->free_callback != NULL) {

		list->free_callback(p->data);
	}

	p->data = nxs_free(p->data);
	p       = nxs_free(p);

	if(list->tail != NULL) {

		list->tail->next = NULL;

		return list->tail->data;
	}

	list->head = NULL;

	return NULL;
}

/*
 * Удалить тот элемент, на который указывает указатель списка.
 * После удаления элемента указатель списка смещается либо на элемент влево, либо влево относительно удалённого (всё зависит от значения
 * аргумента "ptr_move")
 *
 * Возвращаемое значение:
 * * Указатель на данные того элемента, который либо следует за, либо перед ним (в зависимости от "ptr_move").
 * * NULL - если следующий, либо предшествующий удаляемому элементу отсутствует (т.е. достигнут конец списка в какую-то из сторон)
 * 		(в этом случае указатель списка необходимо инициировать заново)
 */
void *nxs_list_del(nxs_list_t *list, nxs_list_move_t ptr_move)
{
	nxs_list_el_t *p = NULL;

	if(list == NULL) {

		return NULL;
	}

	if(list->ptr == NULL) {

		return NULL;
	}

	if(ptr_move == NXS_LIST_MOVE_NEXT) {

		p = list->ptr->next;
	}
	else {

		p = list->ptr->prev;
	}

	if(list->ptr->next != NULL) {

		list->ptr->next->prev = list->ptr->prev;
	}
	else {

		list->tail = list->ptr->prev;
	}

	if(list->ptr->prev != NULL) {

		list->ptr->prev->next = list->ptr->next;
	}
	else {

		list->head = list->ptr->next;
	}

	list->count--;

	if(list->free_callback != NULL) {

		list->free_callback(list->ptr->data);
	}

	list->ptr->data = nxs_free(list->ptr->data);
	list->ptr       = nxs_free(list->ptr);

	list->ptr = p;

	if(list->ptr != NULL) {

		return list->ptr->data;
	}

	return NULL;
}

/*
 * Получение указателя на данные того элемента списка, на который указывает указатель списка
 */
void *nxs_list_get(nxs_list_t *list)
{

	if(list == NULL) {

		return NULL;
	}

	if(list->ptr == NULL) {

		return NULL;
	}

	return list->ptr->data;
}

void *nxs_list_get_head(nxs_list_t *list)
{

	if(list == NULL) {

		return NULL;
	}

	if(list->head == NULL) {

		return NULL;
	}

	return list->head->data;
}

void *nxs_list_get_tail(nxs_list_t *list)
{

	if(list == NULL) {

		return NULL;
	}

	if(list->tail == NULL) {

		return NULL;
	}

	return list->tail->data;
}

/*
 * Задать указатель списка.
 * Указатель списка будет задан либо на первый элемент, либо на последний, в зависимости от значения аргумента "type"
 *
 * Возвращаемое значение:
 * * Указатель на данные первого или последнего элемента (в зависимости от значения "type")
 * * NULL - если список пустой
 */
void *nxs_list_ptr_init(nxs_list_t *list, nxs_list_init_t type)
{

	if(list == NULL) {

		return NULL;
	}

	if(type == NXS_LIST_PTR_INIT_HEAD) {

		list->ptr = list->head;
	}

	if(type == NXS_LIST_PTR_INIT_TAIL) {

		list->ptr = list->tail;
	}

	if(list->ptr == NULL) {

		return NULL;
	}

	return list->ptr->data;
}

/*
 * Установить указатель списка на элемент, на который указывает аргумент "ptr"
 *
 * Возвращаемое значение:
 * * Указатель на данные того эелемента, на который указывает "ptr"
 * * NULL - если "ptr" не определён
 */
void *nxs_list_ptr_set(nxs_list_t *list, nxs_list_el_t *ptr)
{

	if(list == NULL) {

		return NULL;
	}

	if(ptr == NULL) {

		return NULL;
	}

	list->ptr = ptr;

	return list->ptr->data;
}

/*
 * Сдвинуть указатель списка на один элемент вправо
 *
 * Возвращаемое значение:
 * * Указатель на данные следующего элемента списка
 * * NULL - если достигнут конец списка (в этом случае указатель списка необходимо инициировать заново)
 */
void *nxs_list_ptr_next(nxs_list_t *list)
{

	if(list == NULL) {

		return NULL;
	}

	if(list->ptr == NULL) {

		return NULL;
	}

	list->ptr = list->ptr->next;

	if(list->ptr == NULL) {

		return NULL;
	}

	return list->ptr->data;
}

/*
 * Сдвинуть указатель списка на один элемент влево
 *
 * Возвращаемое значение:
 * * Указатель на данные предыдущего элемента списка
 * * NULL - если достигнуто начало списка (в этом случае указатель списка необходимо инициировать заново)
 */
void *nxs_list_ptr_prev(nxs_list_t *list)
{

	if(list == NULL) {

		return NULL;
	}

	if(list->ptr == NULL) {

		return NULL;
	}

	list->ptr = list->ptr->prev;

	if(list->ptr == NULL) {

		return NULL;
	}

	return list->ptr->data;
}

/*
 * Получение текущего значения указателя списка
 */
nxs_list_el_t *nxs_list_ptr_get(nxs_list_t *list)
{

	return list->ptr;
}

/*
 * Получение количества элементов списка
 */
size_t nxs_list_count(nxs_list_t *list)
{

	if(list == NULL) {

		return 0;
	}

	return list->count;
}

/*
 * Получение значения размера элемента данных
 */
size_t nxs_list_size(nxs_list_t *list)
{

	if(list == NULL) {

		return 0;
	}

	return list->size;
}

/* Module internal (static) functions */

static void nxs_list_init_callback_string(void *element)
{
	nxs_string_t *s = element;

	nxs_string_init(s);
}

static void nxs_list_free_callback_string(void *element)
{
	nxs_string_t *s = element;

	nxs_string_free(s);
}

static void nxs_list_init_callback_buf(void *element)
{
	nxs_buf_t *b = element;

	nxs_buf_init2(b);
}

static void nxs_list_free_callback_buf(void *element)
{
	nxs_buf_t *b = element;

	nxs_buf_free(b);
}
