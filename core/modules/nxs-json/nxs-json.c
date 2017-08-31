// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define _NXS_JSON_BUF_READ_SIZE			4096

#define _NXS_JSON_PRINT_TYPE_OBJECT		(u_char *)"object"
#define _NXS_JSON_PRINT_TYPE_ARRAY		(u_char *)"array"
#define _NXS_JSON_PRINT_TYPE_STRING		(u_char *)"string"
#define _NXS_JSON_PRINT_TYPE_INTEGER		(u_char *)"integer"
#define _NXS_JSON_PRINT_TYPE_REAL		(u_char *)"real"
#define _NXS_JSON_PRINT_TYPE_TRUE		(u_char *)"true"
#define _NXS_JSON_PRINT_TYPE_FALSE		(u_char *)"false"
#define _NXS_JSON_PRINT_TYPE_NULL		(u_char *)"null"
#define _NXS_JSON_PRINT_TYPE_UNKNOWN		(u_char *)"unknown"

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static nxs_json_t *nxs_json_parse(u_char *key, json_t *obj);
static nxs_json_type_t nxs_json_get_type_jansson(json_t *obj);

static nxs_json_t *nxs_json_free_recursive(nxs_json_t *obj);

static void nxs_json_print_recursive(nxs_process_t *proc, int log_dst, nxs_json_t *obj, nxs_string_t *shift);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

void nxs_json_init(nxs_json_t **json)
{

	if(json == NULL) {

		return;
	}

	*json = NULL;
}

void nxs_json_free(nxs_json_t **json)
{

	if(json == NULL) {

		return;
	}

	*json = nxs_json_free_recursive(*json);
}

int nxs_json_read_file(nxs_process_t *proc, nxs_json_t **json, nxs_string_t *filename, size_t flags)
{
	nxs_buf_t    buf;
	nxs_string_t json_text;
	int          fd;
	ssize_t      rc;
	size_t       tb;
	json_t *     j;
	json_error_t j_error;

	if(*json != NULL) {

		return NXS_JSON_E_NOT_NULL_PTR;
	}

	if((fd = nxs_fs_open(filename, O_RDONLY)) == -1) {

		nxs_log_write_error(proc, "error open json file: %s (filename: \"%s\")", strerror(errno), nxs_string_str(filename));

		return NXS_JSON_E_READ_JSON_FILE;
	}

	nxs_buf_init(&buf, _NXS_JSON_BUF_READ_SIZE);
	nxs_string_init(&json_text);

	tb = 0;

	while((rc = nxs_fs_read_buf(fd, &buf)) > 0) {

		nxs_string_char_ncpy(&json_text, tb, nxs_buf_get_subbuf(&buf, 0), nxs_buf_get_len(&buf));

		tb += rc;
	}

	nxs_buf_free(&buf);

	if(rc == -1) {

		nxs_log_write_error(
		        proc, "error while reading json data from file: %s (filename: \"%s\")", strerror(errno), nxs_string_str(filename));

		nxs_string_free(&json_text);

		nxs_fs_close(fd);

		return NXS_JSON_E_READ_JSON_FILE;
	}

	nxs_fs_close(fd);

	j = json_loads((char *)nxs_string_str(&json_text), flags, &j_error);

	nxs_string_free(&json_text);

	if(j == NULL) {
		nxs_log_write_error(proc,
		                    "error while parse json file: %s (file: \"%s\", line: %d)",
		                    j_error.text,
		                    nxs_string_str(filename),
		                    j_error.line);

		return NXS_JSON_E_PARSE;
	}

	*json = nxs_json_parse(NULL, j);

	json_decref(j);

	return NXS_JSON_E_OK;
}

int nxs_json_read_mem(nxs_process_t *proc, nxs_json_t **json, nxs_buf_t *buf, size_t flags)
{
	json_t *     j;
	json_error_t j_error;
	size_t       buf_len;

	if(*json != NULL) {

		return NXS_JSON_E_NOT_NULL_PTR;
	}

	/* Данная проверка требуется, т.к.  json_loadb ожидает не null-терминированную строку */
	if(nxs_buf_get_char(buf, nxs_buf_get_len(buf) - 1) == (u_char)'\0') {

		buf_len = nxs_buf_get_len(buf) - 1;
	}
	else {

		buf_len = nxs_buf_get_len(buf);
	}

	j = json_loadb((char *)nxs_buf_get_subbuf(buf, 0), buf_len, flags, &j_error);

	if(j == NULL) {
		nxs_log_write_error(proc, "error while parse json buf: %s (line: %d)", j_error.text, j_error.line);

		return NXS_JSON_E_PARSE;
	}

	*json = nxs_json_parse(NULL, j);

	json_decref(j);

	return NXS_JSON_E_OK;
}

nxs_json_t *nxs_json_child_get_by_index(nxs_json_t *obj, size_t index)
{
	nxs_json_t **json;

	if(obj == NULL) {

		return NULL;
	}

	switch(obj->type) {

		case NXS_JSON_TYPE_OBJECT:
		case NXS_JSON_TYPE_ARRAY:

			if((json = nxs_array_get(obj->value, index)) == NULL) {

				return NULL;
			}

			return *json;

		default:

			return NULL;
	}
}

nxs_json_t *nxs_json_child_get_by_key(nxs_json_t *obj, u_char *key)
{
	size_t      i;
	nxs_json_t *json, **j;

	if(obj == NULL || key == NULL) {

		return NULL;
	}

	switch(obj->type) {

		case NXS_JSON_TYPE_OBJECT:

			for(i = 0; i < nxs_array_count(obj->value); i++) {

				if((j = nxs_array_get(obj->value, i)) != NULL && *j != NULL) {

					json = *j;

					if(nxs_string_char_cmp(&json->key, 0, key) == NXS_YES) {

						return json;
					}
				}
			}

			break;

		default:

			return NULL;
	}

	return NULL;
}

size_t nxs_json_child_get_count(nxs_json_t *obj)
{

	if(obj == NULL) {

		return 0;
	}

	switch(obj->type) {

		case NXS_JSON_TYPE_OBJECT:
		case NXS_JSON_TYPE_ARRAY:

			return nxs_array_count(obj->value);

		default:

			return 0;
	}
}

nxs_string_t *nxs_json_get_key(nxs_json_t *obj)
{

	if(obj == NULL) {

		return NULL;
	}

	return &obj->key;
}

nxs_string_t *nxs_json_string_val(nxs_json_t *obj)
{

	if(obj == NULL) {

		return NULL;
	}

	if(obj->type == NXS_JSON_TYPE_STRING) {

		return (nxs_string_t *)(obj->value);
	}

	return NULL;
}

nxs_json_int_t nxs_json_integer_val(nxs_json_t *obj)
{
	nxs_json_int_t *r;

	if(obj == NULL) {

		return 0;
	}

	if(obj->type == NXS_JSON_TYPE_INTEGER) {

		r = obj->value;

		return *r;
	}

	return 0;
}

double nxs_json_real_val(nxs_json_t *obj)
{
	double *r;

	if(obj == NULL) {

		return 0;
	}

	if(obj->type == NXS_JSON_TYPE_REAL) {

		r = obj->value;

		return *r;
	}

	return 0;
}

nxs_bool_t nxs_json_bool_val(nxs_json_t *obj)
{
	nxs_bool_t *r;

	if(obj == NULL) {

		return 0;
	}

	if(obj->type == NXS_JSON_TYPE_TRUE || obj->type == NXS_JSON_TYPE_FALSE) {

		r = obj->value;

		return *r;
	}

	return 0;
}

nxs_json_type_t nxs_json_type_get(nxs_json_t *obj)
{

	if(obj == NULL) {

		return NXS_JSON_TYPE_NULL;
	}

	return obj->type;
}

u_char *nxs_json_type_string(nxs_json_t *obj)
{

	switch(obj->type) {

		case NXS_JSON_TYPE_OBJECT:

			return _NXS_JSON_PRINT_TYPE_OBJECT;

		case NXS_JSON_TYPE_ARRAY:

			return _NXS_JSON_PRINT_TYPE_ARRAY;

		case NXS_JSON_TYPE_STRING:

			return _NXS_JSON_PRINT_TYPE_STRING;

		case NXS_JSON_TYPE_INTEGER:

			return _NXS_JSON_PRINT_TYPE_INTEGER;

		case NXS_JSON_TYPE_REAL:

			return _NXS_JSON_PRINT_TYPE_REAL;

		case NXS_JSON_TYPE_TRUE:

			return _NXS_JSON_PRINT_TYPE_TRUE;

		case NXS_JSON_TYPE_FALSE:

			return _NXS_JSON_PRINT_TYPE_FALSE;

		case NXS_JSON_TYPE_NULL:

			return _NXS_JSON_PRINT_TYPE_NULL;

		default:

			return _NXS_JSON_PRINT_TYPE_UNKNOWN;
	}
}

/*
 * Создаёт клон json-объекта obj и возвращает указатель на него
 */
nxs_json_t *nxs_json_clone(nxs_json_t *obj)
{
	nxs_array_t *   array;
	nxs_json_t *    json = NULL, *json_ret, **j, **j_obj;
	nxs_string_t *  str;
	size_t          i;
	double *        val_real;
	nxs_bool_t *    val_bool;
	nxs_json_int_t *val_int;

	if(obj == NULL) {

		return NULL;
	}

	json = (nxs_json_t *)nxs_malloc(NULL, sizeof(nxs_json_t));

	nxs_string_init2(&json->key, 0, nxs_string_str(&obj->key));

	switch((json->type = obj->type)) {

		case NXS_JSON_TYPE_OBJECT:
		case NXS_JSON_TYPE_ARRAY:

			array = nxs_array_malloc(1, sizeof(nxs_json_t *), 1);

			json->value = array;

			for(i = 0; i < nxs_array_count(obj->value); i++) {

				j_obj = nxs_array_get(obj->value, i);

				json_ret = nxs_json_clone(*j_obj);

				j  = nxs_array_add(array);
				*j = json_ret;
			}

			break;

		case NXS_JSON_TYPE_STRING:

			str = nxs_string_malloc(0, nxs_string_str(nxs_json_string_val(obj)));

			json->value = str;

			break;

		case NXS_JSON_TYPE_INTEGER:

			val_int  = (nxs_json_int_t *)nxs_malloc(NULL, sizeof(nxs_json_int_t));
			*val_int = nxs_json_integer_val(obj);

			json->value = val_int;

			break;

		case NXS_JSON_TYPE_REAL:

			val_real  = (double *)nxs_malloc(NULL, sizeof(double));
			*val_real = nxs_json_real_val(obj);

			json->value = val_real;

			break;

		case NXS_JSON_TYPE_TRUE:

			val_bool  = (nxs_bool_t *)nxs_malloc(NULL, sizeof(nxs_bool_t));
			*val_bool = NXS_TRUE;

			json->value = val_bool;

			break;

		case NXS_JSON_TYPE_FALSE:

			val_bool  = (nxs_bool_t *)nxs_malloc(NULL, sizeof(nxs_bool_t));
			*val_bool = NXS_FALSE;

			json->value = val_bool;

			break;

		case NXS_JSON_TYPE_NULL:

			json->value = NULL;

			break;
	}

	return json;
}

void nxs_json_print(nxs_process_t *proc, int log_dst, nxs_json_t *obj)
{
	nxs_string_t shift;

	if(obj == NULL) {

		return;
	}

	nxs_string_init2(&shift, 0, NXS_STRING_EMPTY_STR);

	nxs_json_print_recursive(proc, log_dst, obj, &shift);

	nxs_string_free(&shift);
}

/* Module internal (static) functions */

static nxs_json_t *nxs_json_parse(u_char *key, json_t *obj)
{
	nxs_array_t *   array;
	nxs_json_t *    json = NULL, *json_ret, **j;
	nxs_string_t *  str;
	const char *    _key;
	json_t *        _val;
	size_t          i;
	double *        val_real;
	nxs_bool_t *    val_bool;
	nxs_json_int_t *val_int;

	json = (nxs_json_t *)nxs_malloc(NULL, sizeof(nxs_json_t));

	if(key == NULL) {

		nxs_string_init2(&json->key, 0, NXS_STRING_EMPTY_STR);
	}
	else {

		nxs_string_init2(&json->key, 0, key);
	}

	switch((json->type = nxs_json_get_type_jansson(obj))) {

		case NXS_JSON_TYPE_OBJECT:

			array = nxs_array_malloc(1, sizeof(nxs_json_t *), 1);

			json->value = array;

			json_object_foreach(obj, _key, _val)
			{

				json_ret = nxs_json_parse((u_char *)_key, _val);

				j  = nxs_array_add(array);
				*j = json_ret;
			}

			break;

		case NXS_JSON_TYPE_ARRAY:

			array = nxs_array_malloc(1, sizeof(nxs_json_t *), 1);

			json->value = array;

			for(i = 0; i < json_array_size(obj); i++) {

				_val = json_array_get(obj, i);

				json_ret = nxs_json_parse(NULL, _val);

				j  = nxs_array_add(array);
				*j = json_ret;
			}

			break;

		case NXS_JSON_TYPE_STRING:

			str = nxs_string_malloc(0, (u_char *)json_string_value(obj));

			json->value = str;

			break;

		case NXS_JSON_TYPE_INTEGER:

			val_int  = (nxs_json_int_t *)nxs_malloc(NULL, sizeof(nxs_json_int_t));
			*val_int = json_integer_value(obj);

			json->value = val_int;

			break;

		case NXS_JSON_TYPE_REAL:

			val_real  = (double *)nxs_malloc(NULL, sizeof(double));
			*val_real = json_real_value(obj);

			json->value = val_real;

			break;

		case NXS_JSON_TYPE_TRUE:

			val_bool  = (nxs_bool_t *)nxs_malloc(NULL, sizeof(nxs_bool_t));
			*val_bool = NXS_TRUE;

			json->value = val_bool;

			break;

		case NXS_JSON_TYPE_FALSE:

			val_bool  = (nxs_bool_t *)nxs_malloc(NULL, sizeof(nxs_bool_t));
			*val_bool = NXS_FALSE;

			json->value = val_bool;

			break;

		case NXS_JSON_TYPE_NULL:

			json->value = NULL;

			break;
	}

	return json;
}

static nxs_json_type_t nxs_json_get_type_jansson(json_t *obj)
{

	switch(json_typeof(obj)) {

		case JSON_OBJECT:

			return NXS_JSON_TYPE_OBJECT;

		case JSON_ARRAY:

			return NXS_JSON_TYPE_ARRAY;

		case JSON_STRING:

			return NXS_JSON_TYPE_STRING;

		case JSON_INTEGER:

			return NXS_JSON_TYPE_INTEGER;

		case JSON_REAL:

			return NXS_JSON_TYPE_REAL;

		case JSON_TRUE:

			return NXS_JSON_TYPE_TRUE;

		case JSON_FALSE:

			return NXS_JSON_TYPE_FALSE;

		case JSON_NULL:

			return NXS_JSON_TYPE_NULL;
	}

	return NXS_JSON_TYPE_NULL;
}

static nxs_json_t *nxs_json_free_recursive(nxs_json_t *obj)
{
	size_t       i;
	nxs_json_t **json;

	if(obj == NULL) {

		return NULL;
	}

	nxs_string_free(&obj->key);

	switch(obj->type) {

		case NXS_JSON_TYPE_OBJECT:
		case NXS_JSON_TYPE_ARRAY:

			for(i = 0; i < nxs_array_count(obj->value); i++) {

				if((json = nxs_array_get(obj->value, i)) != NULL) {

					*json = nxs_json_free_recursive(*json);
				}
			}

			obj->value = nxs_array_destroy(obj->value);

			break;

		case NXS_JSON_TYPE_STRING:

			obj->value = nxs_string_destroy(obj->value);

			break;

		case NXS_JSON_TYPE_INTEGER:
		case NXS_JSON_TYPE_REAL:
		case NXS_JSON_TYPE_TRUE:
		case NXS_JSON_TYPE_FALSE:
		case NXS_JSON_TYPE_NULL:

			obj->value = nxs_free(obj->value);

			break;
	}

	return nxs_free(obj);
}

static void nxs_json_print_recursive(nxs_process_t *proc, int log_dst, nxs_json_t *obj, nxs_string_t *shift)
{
	size_t          count, i, len;
	u_char *        key, *val, *type;
	nxs_json_t *    json;
	nxs_string_t    val_str;
	double *        val_real;
	nxs_bool_t *    val_bool;
	nxs_json_int_t *val_int;

	key = nxs_string_str(&obj->key);

	nxs_string_init(&val_str);

	switch(obj->type) {

		case NXS_JSON_TYPE_OBJECT:

			type = _NXS_JSON_PRINT_TYPE_OBJECT;
			val  = NXS_STRING_EMPTY_STR;

			break;

		case NXS_JSON_TYPE_ARRAY:

			type = _NXS_JSON_PRINT_TYPE_ARRAY;
			val  = NXS_STRING_EMPTY_STR;

			break;

		case NXS_JSON_TYPE_STRING:

			type = _NXS_JSON_PRINT_TYPE_STRING;
			val  = nxs_string_str(obj->value);

			break;

		case NXS_JSON_TYPE_INTEGER:

			type = _NXS_JSON_PRINT_TYPE_INTEGER;

			val_int = obj->value;

			nxs_string_printf(&val_str, "%" NXS_JSON_INT_FORMAT, *val_int);
			val = nxs_string_str(&val_str);

			break;

		case NXS_JSON_TYPE_REAL:

			type = _NXS_JSON_PRINT_TYPE_REAL;

			val_real = obj->value;

			nxs_string_printf(&val_str, "%f", *val_real);
			val = nxs_string_str(&val_str);

			break;

		case NXS_JSON_TYPE_TRUE:

			type = _NXS_JSON_PRINT_TYPE_TRUE;

			val_bool = obj->value;

			nxs_string_printf(&val_str, "%d", *val_bool);
			val = nxs_string_str(&val_str);

			break;

		case NXS_JSON_TYPE_FALSE:

			type = _NXS_JSON_PRINT_TYPE_FALSE;

			val_bool = obj->value;

			nxs_string_printf(&val_str, "%d", *val_bool);
			val = nxs_string_str(&val_str);

			break;

		case NXS_JSON_TYPE_NULL:

			type = _NXS_JSON_PRINT_TYPE_NULL;

			val = NULL;

			break;

		default:

			type = _NXS_JSON_PRINT_TYPE_UNKNOWN;

			val = NULL;

			break;
	}

	if(log_dst == NXS_JSON_PRINT_CONSOLE) {

		nxs_log_write_console(proc, "%stype: %s, key: \"%s\", value: \"%s\"", nxs_string_str(shift), type, key, val);
	}
	else {

		nxs_log_write_debug(proc, "%stype: %s, key: \"%s\", value: \"%s\"", nxs_string_str(shift), type, key, val);
	}

	if((count = nxs_json_child_get_count(obj)) > 0) {

		len = nxs_string_len(shift);

		nxs_string_char_add_char(shift, (u_char)'\t');

		for(i = 0; i < count; i++) {

			json = nxs_json_child_get_by_index(obj, i);

			nxs_json_print_recursive(proc, log_dst, json, shift);
		}

		nxs_string_set_len(shift, len);
	}

	nxs_string_free(&val_str);
}
