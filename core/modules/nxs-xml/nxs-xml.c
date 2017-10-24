// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */



/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_xml_el_t *nxs_xml_parse(xmlNode *r_el);
static nxs_xml_el_t *nxs_xml_el_create(u_char *name, u_char *content, xmlAttr *attrs);
static void nxs_xml_attrs_add(nxs_xml_el_t *xml_el, xmlAttr *attrs);

static nxs_xml_el_t *nxs_xml_free_el_recursive(nxs_xml_el_t *xml_el);
static void nxs_xml_free_attrs(nxs_xml_el_t *xml_el);

static void nxs_xml_element_get_recursive(nxs_array_t *res, nxs_xml_el_t *xml_el, nxs_array_t *a_names, size_t a_name_ind);
static nxs_xml_el_t *nxs_xml_element_get_recursive_first(nxs_xml_el_t *xml_el, nxs_array_t *a_names, size_t a_name_ind);
static void nxs_xml_print_recursion(nxs_process_t *proc, nxs_xml_el_t *xml_el, size_t level);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_xml_t *nxs_xml_malloc(void)
{
	nxs_xml_t *xml = NULL;

	xml = (nxs_xml_t *)nxs_malloc(xml, sizeof(nxs_xml_t));

	nxs_xml_init(xml);

	return xml;
}

nxs_xml_t *nxs_xml_destroy(nxs_xml_t *xml)
{

	if(xml == NULL) {

		return NULL;
	}

	nxs_xml_free(xml);

	return nxs_free(xml);
}

void nxs_xml_init(nxs_xml_t *xml)
{

	if(xml == NULL) {

		return;
	}

	nxs_string_init2(&xml->version, 0, NXS_STRING_EMPTY_STR);

	xml->root = NULL;
}

void nxs_xml_free(nxs_xml_t *xml)
{

	if(xml == NULL) {

		return;
	}

	xml->root = nxs_xml_free_el_recursive(xml->root);

	nxs_string_free(&xml->version);
}

void nxs_xml_clear(nxs_xml_t *xml)
{

	if(xml == NULL) {

		return;
	}

	nxs_xml_free(xml);

	nxs_xml_init(xml);
}

/*
 * Создание xml по документу, считанному из файла filename
 */
int nxs_xml_read_file(nxs_xml_t *xml, nxs_string_t *filename, nxs_string_t *encoding, int options)
{
	xmlDoc * d    = NULL;
	xmlNode *r_el = NULL;

	if(xml == NULL || filename == NULL) {

		return NXS_XML_E_NULL_PTR;
	}

	if((d = xmlReadFile((char *)nxs_string_str(filename), (char *)nxs_string_str(encoding), options)) == NULL) {

		return NXS_XML_E_READ_XML_FILE;
	}

	if((r_el = xmlDocGetRootElement(d)) == NULL) {

		return NXS_XML_E_GET_ROOT;
	}

	xml->root = nxs_xml_parse(r_el);

	xmlFreeDoc(d);
	xmlCleanupParser();

	return NXS_XML_E_OK;
}

/*
 * Создание xml по документу, считанному из файла filename
 */
int nxs_xml_read_mem(nxs_xml_t *xml, nxs_buf_t *buf, nxs_string_t *url, nxs_string_t *encoding, int options)
{
	xmlDoc * d    = NULL;
	xmlNode *r_el = NULL;

	if(xml == NULL || buf == NULL) {

		return NXS_XML_E_NULL_PTR;
	}

	if((d = xmlReadMemory((char *)nxs_buf_get_subbuf(buf, 0),
	                      nxs_buf_get_len(buf),
	                      (char *)nxs_string_str(url),
	                      (char *)nxs_string_str(encoding),
	                      options)) == NULL) {

		return NXS_XML_E_READ_XML_MEM;
	}

	if((r_el = xmlDocGetRootElement(d)) == NULL) {

		return NXS_XML_E_GET_ROOT;
	}

	xml->root = nxs_xml_parse(r_el);

	xmlFreeDoc(d);
	xmlCleanupParser();

	return NXS_XML_E_OK;
}

nxs_xml_el_t *nxs_xml_root_get(nxs_xml_t *xml)
{

	if(xml == NULL) {

		return NULL;
	}

	return xml->root;
}

/*
 * Получение элемента xml по заданному набору подэлементов. Список el_name должен быть задан от элемента xml_el (это не обязательно должен
 * быть корень документа).
 * По итогам выполнения данной функции формируется массив элементов, попадающих под заданные критерии.
 * Например, с помощью данной функции можно выбрать все имена или ID пользователей, имеющихся в xml-документе
 */
nxs_array_t *nxs_xml_element_get(nxs_xml_el_t *xml_el, u_char *el_name, ...)
{
	va_list       va_names;
	u_char *      _el_name;
	size_t        i;
	nxs_string_t *s_name;
	nxs_array_t   a_names, *res;

	if(xml_el == NULL) {

		return NULL;
	}

	if(el_name == NULL) {

		return NULL;
	}

	res = nxs_array_malloc(0, sizeof(nxs_xml_el_t *), 1, NULL, NULL);

	nxs_array_init(&a_names, 1, sizeof(nxs_string_t), 1, NULL, NULL);

	va_start(va_names, el_name);

	for(_el_name = el_name; _el_name != NULL; _el_name = va_arg(va_names, u_char *)) {

		s_name = nxs_array_add(&a_names);

		nxs_string_init2(s_name, 0, _el_name);
	}

	va_end(va_names);

	nxs_xml_element_get_recursive(res, xml_el, &a_names, 0);

	for(i = 0; i < nxs_array_count(&a_names); i++) {

		s_name = nxs_array_get(&a_names, i);

		nxs_string_free(s_name);
	}

	nxs_array_free(&a_names);

	return res;
}

/*
 * Функция аналогична предыдущей, но поиск элементов осуществляется до первого совпавшего. По этому возвращается не массив элементов,
 * а указатель на найденный (один) элемент.
 */
nxs_xml_el_t *nxs_xml_element_get_first(nxs_xml_el_t *xml_el, u_char *el_name, ...)
{
	va_list       va_names;
	u_char *      _el_name;
	size_t        i;
	nxs_string_t *s_name;
	nxs_array_t   a_names;
	nxs_xml_el_t *res;

	if(xml_el == NULL) {

		return NULL;
	}

	if(el_name == NULL) {

		return NULL;
	}

	nxs_array_init(&a_names, 1, sizeof(nxs_string_t), 1, NULL, NULL);

	va_start(va_names, el_name);

	for(_el_name = el_name; _el_name != NULL; _el_name = va_arg(va_names, u_char *)) {

		s_name = nxs_array_add(&a_names);

		nxs_string_init2(s_name, 0, _el_name);
	}

	va_end(va_names);

	res = nxs_xml_element_get_recursive_first(xml_el, &a_names, 0);

	for(i = 0; i < nxs_array_count(&a_names); i++) {

		s_name = nxs_array_get(&a_names, i);

		nxs_string_free(s_name);
	}

	nxs_array_free(&a_names);

	return res;
}

nxs_string_t *nxs_xml_element_get_name(nxs_xml_el_t *xml_el)
{

	if(xml_el == NULL) {

		return NULL;
	}

	return &xml_el->name;
}

nxs_string_t *nxs_xml_element_get_content(nxs_xml_el_t *xml_el)
{

	if(xml_el == NULL) {

		return NULL;
	}

	return &xml_el->content;
}

nxs_string_t *nxs_xml_attr_get_name(nxs_xml_attr_t *attr)
{

	if(attr == NULL) {

		return NULL;
	}

	return &attr->attr_name;
}

nxs_string_t *nxs_xml_attr_get_value(nxs_xml_attr_t *attr)
{

	if(attr == NULL) {

		return NULL;
	}

	return &attr->attr_value;
}

/*
 * Получение указателя на элемент из массива элементов (попавших при выборке под заданные критерии) по заданному индексу index
 */
nxs_xml_el_t *nxs_xml_element_res_get(nxs_array_t *res, size_t index)
{
	nxs_xml_el_t **r;

	if(res == NULL) {

		return NULL;
	}

	if(index >= nxs_array_count(res)) {

		return NULL;
	}

	r = nxs_array_get(res, index);

	return *r;
}

/*
 * Получение указателя на элемент с атрибутами, имя которого соответствует строке attr.
 * Поиск производится в элементе el
 */
nxs_xml_attr_t *nxs_xml_element_res_get_attr(nxs_xml_el_t *xml_el, nxs_string_t attr)
{
	size_t          i;
	nxs_xml_attr_t *a;

	if(xml_el == NULL) {

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&xml_el->attrs); i++) {

		a = nxs_array_get(&xml_el->attrs, i);

		if(nxs_string_cmp(&a->attr_name, 0, &attr, 0) == NXS_YES) {

			return a;
		}
	}

	return NULL;
}

nxs_array_t *nxs_xml_element_res_free(nxs_array_t *res)
{

	if(res == NULL) {

		return NULL;
	}

	return nxs_array_destroy(res);
}

/*
 * Распечатка дерева xml.
 * Может использоваться для отладочных целей.
 */
void nxs_xml_print(nxs_process_t *proc, nxs_xml_el_t *xml_el)
{

	if(xml_el == NULL) {

		return;
	}

	nxs_xml_print_recursion(proc, xml_el, 0);
}

/* Module internal (static) functions */

/*
 * Обработка считанного (из файла или памяти) документа и формирвание дерева xml
 */
static nxs_xml_el_t *nxs_xml_parse(xmlNode *r_el)
{
	xmlNode *     n_el = NULL;
	xmlChar *     c_el;
	nxs_xml_el_t *xml_el, *el = NULL, **x;

	if(r_el == NULL) {

		return NULL;
	}

	c_el = xmlNodeGetContent(r_el);

	xml_el = nxs_xml_el_create((u_char *)r_el->name, (u_char *)c_el, r_el->properties);

	xmlFree(c_el);

	for(n_el = r_el->children; n_el != NULL; n_el = n_el->next) {

		if(n_el->type == XML_ELEMENT_NODE) {

			el = nxs_xml_parse(n_el);

			x = nxs_array_add(&xml_el->childs);

			*x = el;
		}
	}

	return xml_el;
}

/*
 * Добавление нового элемента в массив дочерних элементов a_el
 */
static nxs_xml_el_t *nxs_xml_el_create(u_char *name, u_char *content, xmlAttr *attrs)
{
	nxs_xml_el_t *el;

	el = (nxs_xml_el_t *)nxs_malloc(NULL, sizeof(nxs_xml_el_t));

	nxs_string_init2(&el->name, 0, name);
	nxs_string_init2(&el->content, 0, content);

	nxs_array_init(&el->childs, 0, sizeof(nxs_xml_el_t *), 1, NULL, NULL);

	nxs_array_init(&el->attrs, 0, sizeof(nxs_xml_attr_t), 1, NULL, NULL);

	nxs_xml_attrs_add(el, attrs);

	return el;
}

/*
 * Добавление в массив атрибутов элемента el дополнительного атрибута
 */
static void nxs_xml_attrs_add(nxs_xml_el_t *xml_el, xmlAttr *attrs)
{
	xmlAttr *       _attrs;
	nxs_xml_attr_t *a;

	_attrs = attrs;

	while(_attrs) {

		a = nxs_array_add(&xml_el->attrs);

		nxs_string_init2(&a->attr_name, 0, (u_char *)_attrs->name);
		nxs_string_init2(&a->attr_value, 0, (u_char *)_attrs->children->content);

		_attrs = _attrs->next;
	}
}

/*
 * Рекурсивная очистка всего содержимого xml
 */
static nxs_xml_el_t *nxs_xml_free_el_recursive(nxs_xml_el_t *xml_el)
{
	size_t         i;
	nxs_xml_el_t **el;

	if(xml_el == NULL) {

		return NULL;
	}

	nxs_string_free(&xml_el->content);
	nxs_string_free(&xml_el->name);

	nxs_xml_free_attrs(xml_el);

	for(i = 0; i < nxs_array_count(&xml_el->childs); i++) {

		el = nxs_array_get(&xml_el->childs, i);

		*el = nxs_xml_free_el_recursive(*el);
	}

	nxs_array_free(&xml_el->childs);

	return nxs_free(xml_el);
}

/*
 * Очистка массива атрибутов элемента el
 */
static void nxs_xml_free_attrs(nxs_xml_el_t *xml_el)
{
	size_t          i;
	nxs_xml_attr_t *attr;

	for(i = 0; i < nxs_array_count(&xml_el->attrs); i++) {

		attr = nxs_array_get(&xml_el->attrs, i);

		nxs_string_free(&attr->attr_name);
		nxs_string_free(&attr->attr_value);
	}

	nxs_array_free(&xml_el->attrs);
}

/*
 * Рекурсивная выборка элементов из дерева xml, попадающих под заданные условия выборки.
 */
static void nxs_xml_element_get_recursive(nxs_array_t *res, nxs_xml_el_t *xml_el, nxs_array_t *a_names, size_t a_name_ind)
{
	size_t         i;
	nxs_xml_el_t **el, **el_res;
	nxs_string_t * a_name;

	if(nxs_array_count(a_names) <= a_name_ind) {

		return;
	}

	a_name = nxs_array_get(a_names, a_name_ind);

	if(nxs_string_cmp(&xml_el->name, 0, a_name, 0) == NXS_YES) {

		if(nxs_array_count(a_names) == a_name_ind + 1) {

			el_res = nxs_array_add(res);

			*el_res = xml_el;
		}
		else {

			for(i = 0; i < nxs_array_count(&xml_el->childs); i++) {

				el = nxs_array_get(&xml_el->childs, i);

				nxs_xml_element_get_recursive(res, *el, a_names, a_name_ind + 1);
			}
		}
	}
}

static nxs_xml_el_t *nxs_xml_element_get_recursive_first(nxs_xml_el_t *xml_el, nxs_array_t *a_names, size_t a_name_ind)
{
	size_t         i;
	nxs_xml_el_t **el, *el_res;
	nxs_string_t * a_name;

	if(nxs_array_count(a_names) <= a_name_ind) {

		return NULL;
	}

	a_name = nxs_array_get(a_names, a_name_ind);

	if(nxs_string_cmp(&xml_el->name, 0, a_name, 0) == NXS_YES) {

		if(nxs_array_count(a_names) == a_name_ind + 1) {

			return xml_el;
		}
		else {

			for(i = 0; i < nxs_array_count(&xml_el->childs); i++) {

				el = nxs_array_get(&xml_el->childs, i);

				if((el_res = nxs_xml_element_get_recursive_first(*el, a_names, a_name_ind + 1)) != NULL) {

					return el_res;
				}
			}
		}
	}

	return NULL;
}

static void nxs_xml_print_recursion(nxs_process_t *proc, nxs_xml_el_t *xml_el, size_t level)
{
	size_t          i, j;
	nxs_xml_el_t ** el;
	nxs_xml_attr_t *attr_el;
	nxs_string_t    spaces;

	nxs_string_init2(&spaces, 0, NXS_STRING_EMPTY_STR);

	for(i = 0; i < level; i++) {

		nxs_string_char_add_char(&spaces, (u_char)' ');
	}

	nxs_log_write_debug(proc,
	                    "%sname: \"%s\", content: \"%s\"",
	                    nxs_string_str(&spaces),
	                    nxs_string_str(&xml_el->name),
	                    nxs_string_str(&xml_el->content));

	for(j = 0; j < nxs_array_count(&xml_el->attrs); j++) {

		attr_el = nxs_array_get(&xml_el->attrs, j);

		nxs_log_write_debug(proc,
		                    "%s\tattr: \"%s\", value: \"%s\"",
		                    nxs_string_str(&spaces),
		                    nxs_string_str(&attr_el->attr_name),
		                    nxs_string_str(&attr_el->attr_value));
	}

	nxs_string_free(&spaces);

	for(i = 0; i < nxs_array_count(&xml_el->childs); i++) {

		el = nxs_array_get(&xml_el->childs, i);

		nxs_xml_print_recursion(proc, *el, level + 1);
	}
}
