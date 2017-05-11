// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */



/* Module typedefs */

typedef struct			nxs_cfg_json_par_def_s			nxs_cfg_json_par_def_t;

/* Module declarations */

struct nxs_cfg_json_par_def_s
{
	nxs_cfg_json_par_t	*par;
	size_t			count;
} ;

/* Module internal (static) functions prototypes */

// clang-format on

static nxs_cfg_json_par_t *nxs_cfg_json_find_par(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par);
static nxs_bool_t nxs_cfg_json_eop(nxs_cfg_json_par_t cfg);
static nxs_cfg_json_state_t nxs_cfg_json_read_val(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_void(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_object(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_cfg_json_type_handler_array_object(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t
        nxs_cfg_json_type_handler_array_int(nxs_process_t *proc, nxs_json_t *json, nxs_bool_t is_list, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_array_int_16(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_bool_t          is_list,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_array_int_32(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_bool_t          is_list,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_array_real(nxs_process_t *     proc,
                                                                 nxs_json_t *        json,
                                                                 nxs_bool_t          is_list,
                                                                 nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_array_string(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_bool_t          is_list,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_array_bool(nxs_process_t *     proc,
                                                                 nxs_json_t *        json,
                                                                 nxs_bool_t          is_list,
                                                                 nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_int(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_int_16(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_int_32(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_real(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_string(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_type_handler_bool(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

static void nxs_cfg_json_conf_array_add_eol(nxs_array_t *cfg_arr, nxs_bool_t skip_undef);

static void nxs_cfg_json_par_def_init(nxs_array_t *arr, nxs_cfg_json_par_t *cfg_json_par);
static void nxs_cfg_json_par_def_free(nxs_array_t *arr);
static nxs_cfg_json_state_t nxs_cfg_json_par_def_add(nxs_process_t *proc, nxs_array_t *arr, nxs_cfg_json_par_t *cfg_json_par_el);
static nxs_cfg_json_state_t nxs_cfg_json_par_def_check(nxs_process_t *proc, nxs_array_t *arr);

static nxs_bool_t nxs_cfg_json_par_check_skip_undef(nxs_cfg_json_par_t *cfg_json_par);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

/*
 * Функция инициализации cfg
 */
nxs_cfg_json_state_t nxs_cfg_json_init(nxs_process_t * proc,
                                       nxs_cfg_json_t *cfg,
                                       nxs_string_t *  cfg_path,
                                       nxs_cfg_json_state_t (*prep_function)(nxs_cfg_json_t cfg),
                                       nxs_cfg_json_state_t (*post_function)(nxs_cfg_json_t cfg),
                                       nxs_array_t *cfg_arr)
{

	if(nxs_cfg_json_conf_array_merge(proc, cfg, cfg_arr) != NXS_CFG_JSON_CONF_OK) {

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(cfg_path == NULL) {

		nxs_string_init2(&cfg->cfg_path, 0, NXS_STRING_EMPTY_STR);
	}
	else {

		nxs_string_init2(&cfg->cfg_path, 0, nxs_string_str(cfg_path));
	}

	cfg->prep_function = prep_function;
	cfg->post_function = post_function;

	return NXS_CFG_JSON_CONF_OK;
}

/*
 * Функция очистки cfg
 *
 * Массив, на который указывает cfg->cfg_par необходимо очищать отдельно!
 */
void nxs_cfg_json_free(nxs_cfg_json_t *cfg)
{

	nxs_string_free(&cfg->cfg_path);

	cfg->prep_function = NULL;
	cfg->post_function = NULL;
	cfg->cfg_par       = NULL;
}

/*
 * Чтение конфигурационного файла "cfg.cfg_path"
 */
nxs_cfg_json_state_t nxs_cfg_json_read_file(nxs_process_t *proc, nxs_cfg_json_t cfg)
{
	nxs_cfg_json_state_t rc;
	nxs_json_t *         json;

	nxs_json_init(&json);

	rc = NXS_CFG_JSON_CONF_OK;

	if(nxs_string_len(&cfg.cfg_path) == 0) {

		nxs_log_write_error(proc, "json cfg process error: config file not specified");

		rc = NXS_CFG_JSON_CONF_ERROR;
		goto error;
	}

	if(nxs_json_read_file(proc, &json, &cfg.cfg_path, JSON_REJECT_DUPLICATES) != NXS_JSON_E_OK) {

		rc = NXS_CFG_JSON_CONF_ERROR;
		goto error;
	}

	if(nxs_cfg_json_read_json(proc, cfg, json) != NXS_CFG_JSON_CONF_OK) {

		rc = NXS_CFG_JSON_CONF_ERROR;
		goto error;
	}

error:

	nxs_json_free(&json);

	return rc;
}

nxs_cfg_json_state_t nxs_cfg_json_read_buf(nxs_process_t *proc, nxs_cfg_json_t cfg, nxs_buf_t *json_buf)
{
	nxs_cfg_json_state_t rc;
	nxs_json_t *         json;

	nxs_json_init(&json);

	rc = NXS_CFG_JSON_CONF_OK;

	if(nxs_json_read_mem(proc, &json, json_buf, JSON_REJECT_DUPLICATES) != NXS_JSON_E_OK) {

		rc = NXS_CFG_JSON_CONF_ERROR;
		goto error;
	}

	if(nxs_cfg_json_read_json(proc, cfg, json) != NXS_CFG_JSON_CONF_OK) {

		rc = NXS_CFG_JSON_CONF_ERROR;
		goto error;
	}

error:

	nxs_json_free(&json);

	return rc;
}

nxs_cfg_json_state_t nxs_cfg_json_read_json(nxs_process_t *proc, nxs_cfg_json_t cfg, nxs_json_t *json)
{

	/*
	 * Если задано - производим вызов начальной функции, которая произведёт подготовительные действия
	 */
	if(cfg.prep_function != NULL) {

		if(cfg.prep_function(cfg) != NXS_CFG_JSON_CONF_OK) {

			nxs_log_write_error(proc, "json cfg process error: error in prep-function");

			return NXS_CFG_JSON_CONF_ERROR;
		}
	}

	if(nxs_cfg_json_element_process(proc, json, cfg.cfg_par) != NXS_CFG_JSON_CONF_OK) {

		return NXS_CFG_JSON_CONF_ERROR;
	}

	/*
	 * Если задано - производим вызов постконфигурационной функции, которая произведёт завершающие (проверочные) действия
	 */
	if(cfg.post_function != NULL) {

		if(cfg.post_function(cfg) != NXS_CFG_JSON_CONF_OK) {

			nxs_log_write_error(proc, "json cfg process error: error in post-function");

			return NXS_CFG_JSON_CONF_ERROR;
		}
	}

	return NXS_CFG_JSON_CONF_OK;
}

nxs_cfg_json_state_t nxs_cfg_json_element_process(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par)
{
	size_t               i;
	nxs_json_t *         j;
	nxs_cfg_json_par_t * p;
	nxs_array_t          def_arr;
	nxs_cfg_json_state_t rc;

	rc = NXS_CFG_JSON_CONF_OK;

	/* Инициализация массива определения опций (сколько встретилось каждой опции) */
	nxs_cfg_json_par_def_init(&def_arr, cfg_json_par);

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_ARRAY && nxs_json_type_get(json) != NXS_JSON_TYPE_OBJECT) {

		nxs_log_write_error(proc,
		                    "json cfg process error: json object has incorrect type (key: \"%s\")",
		                    nxs_string_str(nxs_json_get_key(json)));

		rc = NXS_CFG_JSON_CONF_ERROR;
		goto error;
	}

	for(i = 0; i < nxs_json_child_get_count(json); i++) {

		j = nxs_json_child_get_by_index(json, i);

		if((p = nxs_cfg_json_find_par(proc, j, cfg_json_par)) == NULL) {

			if(nxs_cfg_json_par_check_skip_undef(cfg_json_par) == NXS_YES) {

				continue;
			}

			nxs_log_write_error(
			        proc, "json cfg process error: unknown option (option: \"%s\")", nxs_string_str(nxs_json_get_key(j)));

			rc = NXS_CFG_JSON_CONF_ERROR;
			goto error;
		}

		/* Добавление найденной опции в массив и проверка на двойное определение (на самом деле двойное определение отслживается на
		 * этапе парсинга json) */
		if(nxs_cfg_json_par_def_add(proc, &def_arr, p) != NXS_CFG_JSON_CONF_OK) {

			rc = NXS_CFG_JSON_CONF_ERROR;
			goto error;
		}

		if(nxs_cfg_json_read_val(proc, j, p) != NXS_CFG_JSON_CONF_OK) {

			rc = NXS_CFG_JSON_CONF_ERROR;
			goto error;
		}
	}

	/* Проверка определения всех обязательных опций */
	if(nxs_cfg_json_par_def_check(proc, &def_arr) != NXS_CFG_JSON_CONF_OK) {

		rc = NXS_CFG_JSON_CONF_ERROR;
		goto error;
	}

error:

	nxs_cfg_json_par_def_free(&def_arr);

	return rc;
}

/*
 * Массив описания конфигурационных опций.
 * Создаёт массив, аналогичный тому, который определяется в core-модулях для считывания конфигурацинных файлов.
 *
 * Используется для разбора блоков с массивами (дочерними элементами которых являются объекты) в конфигурационных файлах.
 */

void nxs_cfg_json_conf_array_init(nxs_array_t *cfg_arr)
{

	nxs_array_init(cfg_arr, 0, sizeof(nxs_cfg_json_par_t), 1);

	nxs_cfg_json_conf_array_add_eol(cfg_arr, NXS_NO);
}

void nxs_cfg_json_conf_array_free(nxs_array_t *cfg_arr)
{
	size_t              i;
	nxs_cfg_json_par_t *cfg;

	for(i = 0; i < nxs_array_count(cfg_arr); i++) {

		cfg = nxs_array_get(cfg_arr, i);

		nxs_string_free(&cfg->name);

		cfg->value            = NULL;
		cfg->proc_function    = NULL;
		cfg->el_prep_function = NULL;
		cfg->type             = 0;
		cfg->min_val          = 0;
		cfg->max_val          = 0;
		cfg->required         = 0;
		cfg->sub_els          = NULL;
	}

	nxs_array_free(cfg_arr);
}

void nxs_cfg_json_conf_array_add(
        nxs_array_t * cfg_arr,
        nxs_string_t *par_name,
        void *        value,
        nxs_cfg_json_state_t (*proc_function)(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par),
        nxs_cfg_json_state_t (
                *el_prep_function)(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par, nxs_array_t *cfg_arr),
        nxs_cfg_json_type_t type,
        double              min_val,
        double              max_val,
        nxs_bool_t          required,
        nxs_cfg_json_par_t *sub_els)
{
	nxs_cfg_json_par_t *cfg;
	nxs_bool_t          skip_undef;

	cfg = nxs_array_get(cfg_arr, nxs_array_count(cfg_arr) - 1);

	nxs_string_cpy_dyn(&cfg->name, 0, par_name, 0);

	if(cfg->proc_function == NULL) {

		skip_undef = NXS_NO;
	}
	else {

		skip_undef = NXS_YES;
	}

	cfg->value            = value;
	cfg->proc_function    = proc_function;
	cfg->el_prep_function = el_prep_function;
	cfg->type             = type;
	cfg->min_val          = min_val;
	cfg->max_val          = max_val;
	cfg->required         = required;
	cfg->sub_els          = sub_els;

	nxs_cfg_json_conf_array_add_eol(cfg_arr, skip_undef);
}

/*
 * Установка флага "пропускать без ошибок неопределённые опции"
 */
void nxs_cfg_json_conf_array_skip_undef(nxs_array_t *cfg_arr)
{
	nxs_cfg_json_par_t *cfg;

	if(cfg_arr == NULL) {

		return;
	}

	cfg = nxs_array_get(cfg_arr, nxs_array_count(cfg_arr) - 1);

	cfg->proc_function = ((void *)1);
}

/*
 * Добавление к структуре параметров указателя на описатель параметров, который сождержится в массиве cfg_arr,
 * сформированный функциями nxs_cfg_json_conf_array_*()
 */
nxs_cfg_json_state_t nxs_cfg_json_conf_array_merge(nxs_process_t *proc, nxs_cfg_json_t *cfg, nxs_array_t *cfg_arr)
{
	nxs_cfg_json_par_t *c;

	/* Проверка размера массива (должен быть равен sizeof(nxs_cfg_json_par_t), т.к. содержит элементы данного типа) */
	if(nxs_array_size(cfg_arr) != sizeof(nxs_cfg_json_par_t)) {

		nxs_log_write_error(proc, "json cfg process error: wrong size of cfg array");

		return NXS_CFG_JSON_CONF_ERROR;
	}

	/* Проверка последнего элемента массива. Он долежен быть нулевым, чтобы верно обработался. */

	c = nxs_array_get(cfg_arr, nxs_array_count(cfg_arr) - 1);

	if(nxs_cfg_json_eop(*c) == NXS_NO) {

		nxs_log_write_error(proc, "json cfg process error: cfg array have incorrect last element");

		return NXS_CFG_JSON_CONF_ERROR;
	}

	c = cfg_arr->el;

	cfg->cfg_par = c;

	return NXS_CFG_JSON_CONF_OK;
}

/* Module internal (static) functions */

/*
 * Поиск параметра, которому соответствует данная опция конфигурации
 *
 * Возвращаемые значения:
 * * Указатель на конкретный параметр, если соответствующий параметр был найден
 * * NULL - если параметр найден не был
 */
static nxs_cfg_json_par_t *nxs_cfg_json_find_par(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par)
{
	size_t i;

	for(i = 0; nxs_cfg_json_eop(cfg_json_par[i]) == NXS_NO; i++) {

		if(nxs_string_cmp(nxs_json_get_key(json), 0, &cfg_json_par[i].name, 0) == NXS_STRING_CMP_EQ) {

			return &(cfg_json_par[i]);
		}
	}

	return NULL;
}

/*
 * Проверка достижения конца списка параметров
 *
 * Возвращаемые значения:
 * 0 - конец списка параметров ещё не достигнут
 * 1 - конец списка параметров достигнут
 */
static nxs_bool_t nxs_cfg_json_eop(nxs_cfg_json_par_t cfg)
{

	if(nxs_string_str(&cfg.name) == NULL) {

		return NXS_YES;
	}

	return NXS_NO;
}

static nxs_cfg_json_state_t nxs_cfg_json_read_val(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{

	if(cfg_json_par_el->type != NXS_CFG_JSON_TYPE_VOID && cfg_json_par_el->proc_function != NULL) {

		nxs_log_write_error(
		        proc,
		        "json cfg process error: option have none void type and pointer to process function (option: \"%s\", type: %s)",
		        nxs_string_str(nxs_json_get_key(json)),
		        nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	switch(cfg_json_par_el->type) {

		case NXS_CFG_JSON_TYPE_VOID:

			return nxs_cfg_json_type_handler_void(proc, json, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_OBJECT:

			return nxs_cfg_json_type_handler_object(proc, json, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_ARRAY_OBJECT:

			return nxs_cfg_json_type_handler_array_object(proc, json, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_ARRAY_INT:

			return nxs_cfg_json_type_handler_array_int(proc, json, NXS_NO, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_ARRAY_INT_16:

			return nxs_cfg_json_type_handler_array_int_16(proc, json, NXS_NO, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_ARRAY_INT_32:

			return nxs_cfg_json_type_handler_array_int_32(proc, json, NXS_NO, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_ARRAY_REAL:

			return nxs_cfg_json_type_handler_array_real(proc, json, NXS_NO, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_ARRAY_STRING:

			return nxs_cfg_json_type_handler_array_string(proc, json, NXS_NO, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_ARRAY_BOOL:

			return nxs_cfg_json_type_handler_array_bool(proc, json, NXS_NO, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_LIST_INT:

			return nxs_cfg_json_type_handler_array_int(proc, json, NXS_YES, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_LIST_INT_16:

			return nxs_cfg_json_type_handler_array_int_16(proc, json, NXS_YES, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_LIST_INT_32:

			return nxs_cfg_json_type_handler_array_int_32(proc, json, NXS_YES, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_LIST_REAL:

			return nxs_cfg_json_type_handler_array_real(proc, json, NXS_YES, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_LIST_STRING:

			return nxs_cfg_json_type_handler_array_string(proc, json, NXS_YES, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_LIST_BOOL:

			return nxs_cfg_json_type_handler_array_bool(proc, json, NXS_YES, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_INT:

			return nxs_cfg_json_type_handler_int(proc, json, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_INT_16:

			return nxs_cfg_json_type_handler_int_16(proc, json, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_INT_32:

			return nxs_cfg_json_type_handler_int_32(proc, json, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_REAL:

			return nxs_cfg_json_type_handler_real(proc, json, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_STRING:

			return nxs_cfg_json_type_handler_string(proc, json, cfg_json_par_el);

		case NXS_CFG_JSON_TYPE_BOOL:

			return nxs_cfg_json_type_handler_bool(proc, json, cfg_json_par_el);
	}

	nxs_log_write_error(proc,
	                    "json cfg process error: undefined behavior while processing option (option: \"%s\")",
	                    nxs_string_str(nxs_json_get_key(json)));

	return NXS_CFG_JSON_CONF_ERROR;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_void(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{

	if(cfg_json_par_el->proc_function == NULL) {

		nxs_log_write_error(proc,
		                    "json cfg process error: option have void type and no pointer to process function (option: \"%s\")",
		                    nxs_string_str(nxs_json_get_key(json)));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	return cfg_json_par_el->proc_function(proc, json, cfg_json_par_el);
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_object(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_OBJECT) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: object)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(cfg_json_par_el->sub_els == NULL) {

		nxs_log_write_error(proc,
		                    "json cfg process error: missing pointer to parameters definition structure for option (option: "
		                    "\"%s\", option type: object)",
		                    nxs_string_str(nxs_json_get_key(json)));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	return nxs_cfg_json_element_process(proc, json, cfg_json_par_el->sub_els);
}

static nxs_cfg_json_state_t
        nxs_cfg_json_type_handler_array_object(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_array_t         cfg_arr;
	nxs_json_t *        j;
	nxs_cfg_json_par_t *c;
	size_t              i;

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_ARRAY) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: array)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(cfg_json_par_el->el_prep_function == NULL) {

		nxs_log_write_error(
		        proc,
		        "json cfg process error: missing element's prepare function for array object option type (option: \"%s\")",
		        nxs_string_str(nxs_json_get_key(json)));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	/*
	 * Проход по всем элементам массива json, определение для каждого структуры nxs_cfg_json_par_t (с указанием области памяти для
	 * заполнения нужными значениями) и разбором считанного объекта
	 */
	for(i = 0; i < nxs_json_child_get_count(json); i++) {

		j = nxs_json_child_get_by_index(json, i);

		if(nxs_json_type_get(j) != NXS_JSON_TYPE_OBJECT) {

			nxs_log_write_error(
			        proc,
			        "json cfg process error: wrong type for sub-value (option: \"%s\", type: %s, expected type: object)",
			        nxs_string_str(nxs_json_get_key(json)),
			        nxs_json_type_string(j));

			return NXS_CFG_JSON_CONF_ERROR;
		}

		nxs_cfg_json_conf_array_init(&cfg_arr);

		if(cfg_json_par_el->el_prep_function(proc, j, cfg_json_par_el, &cfg_arr) != NXS_CFG_JSON_CONF_OK) {

			nxs_log_write_error(
			        proc,
			        "json cfg process error: error in element's prep function while processin array sub-value (option: \"%s\")",
			        nxs_string_str(nxs_json_get_key(json)));

			nxs_cfg_json_conf_array_free(&cfg_arr);

			return NXS_CFG_JSON_CONF_ERROR;
		}

		/* Пропускаем обработку данного элемента массива если для него не определено ни одной опции */
		if(nxs_array_count(&cfg_arr) > 1) {

			c = cfg_arr.el;

			if(nxs_cfg_json_element_process(proc, j, c) != NXS_CFG_JSON_CONF_OK) {

				nxs_cfg_json_conf_array_free(&cfg_arr);

				return NXS_CFG_JSON_CONF_ERROR;
			}
		}

		nxs_cfg_json_conf_array_free(&cfg_arr);
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_cfg_json_type_handler_array_int(nxs_process_t *proc, nxs_json_t *json, nxs_bool_t is_list, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_array_t *var_array;
	nxs_list_t * var_list;
	nxs_json_t * j;
	ssize_t *    p;
	size_t       i;

	var_array = NULL;
	var_list  = NULL;

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_ARRAY) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: array)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(is_list == NXS_YES) {

		var_list = nxs_cfg_json_get_val(cfg_json_par_el);
	}
	else {

		var_array = nxs_cfg_json_get_val(cfg_json_par_el);
	}

	for(i = 0; i < nxs_json_child_get_count(json); i++) {

		j = nxs_json_child_get_by_index(json, i);

		if(nxs_json_type_get(j) != NXS_JSON_TYPE_INTEGER) {

			nxs_log_write_error(
			        proc,
			        "json cfg process error: wrong type for sub-value (option: \"%s\", type: %s, expected type: integer)",
			        nxs_string_str(nxs_json_get_key(json)),
			        nxs_json_type_string(j));

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(is_list == NXS_YES) {

			p = nxs_list_add_tail(var_list);
		}
		else {

			p = nxs_array_add(var_array);
		}

		*p = nxs_json_integer_val(j);

		/* Проверка нахождения считываемых значений в заданных границах */

		if((ssize_t)cfg_json_par_el->min_val == 0 && (ssize_t)cfg_json_par_el->max_val == 0) {

			continue;
		}

		if(*p < (ssize_t)cfg_json_par_el->min_val) {

			nxs_log_write_error(proc,
			                    "json cfg process error: value of integer option less then allowed minimum value (option: "
			                    "\"%s\", value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
			                    "' till '%" NXS_JSON_INT_FORMAT "')",
			                    nxs_string_str(nxs_json_get_key(json)),
			                    nxs_json_integer_val(j),
			                    (ssize_t)cfg_json_par_el->min_val,
			                    (ssize_t)cfg_json_par_el->max_val);

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(*p > (ssize_t)cfg_json_par_el->max_val) {

			nxs_log_write_error(proc,
			                    "json cfg process error: value of integer option more then allowed maximum value (option: "
			                    "\"%s\", value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
			                    "' till '%" NXS_JSON_INT_FORMAT "')",
			                    nxs_string_str(nxs_json_get_key(json)),
			                    nxs_json_integer_val(j),
			                    (ssize_t)cfg_json_par_el->min_val,
			                    (ssize_t)cfg_json_par_el->max_val);

			return NXS_CFG_JSON_CONF_ERROR;
		}
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_array_int_16(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_bool_t          is_list,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_array_t *var_array;
	nxs_list_t * var_list;
	nxs_json_t * j;
	int16_t *    p;
	size_t       i;

	var_array = NULL;
	var_list  = NULL;

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_ARRAY) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: array)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(is_list == NXS_YES) {

		var_list = nxs_cfg_json_get_val(cfg_json_par_el);
	}
	else {

		var_array = nxs_cfg_json_get_val(cfg_json_par_el);
	}

	for(i = 0; i < nxs_json_child_get_count(json); i++) {

		j = nxs_json_child_get_by_index(json, i);

		if(nxs_json_type_get(j) != NXS_JSON_TYPE_INTEGER) {

			nxs_log_write_error(
			        proc,
			        "json cfg process error: wrong type for sub-value (option: \"%s\", type: %s, expected type: integer)",
			        nxs_string_str(nxs_json_get_key(json)),
			        nxs_json_type_string(j));

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(is_list == NXS_YES) {

			p = nxs_list_add_tail(var_list);
		}
		else {

			p = nxs_array_add(var_array);
		}

		*p = (int16_t)nxs_json_integer_val(j);

		/* Проверка нахождения считываемых значений в заданных границах */

		if((ssize_t)cfg_json_par_el->min_val == 0 && (ssize_t)cfg_json_par_el->max_val == 0) {

			continue;
		}

		if(*p < (int16_t)cfg_json_par_el->min_val) {

			nxs_log_write_error(proc,
			                    "json cfg process error: value of integer option less then allowed minimum value (option: "
			                    "\"%s\", value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
			                    "' till '%" NXS_JSON_INT_FORMAT "')",
			                    nxs_string_str(nxs_json_get_key(json)),
			                    nxs_json_integer_val(j),
			                    (ssize_t)cfg_json_par_el->min_val,
			                    (ssize_t)cfg_json_par_el->max_val);

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(*p > (int16_t)cfg_json_par_el->max_val) {

			nxs_log_write_error(proc,
			                    "json cfg process error: value of integer option more then allowed maximum value (option: "
			                    "\"%s\", value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
			                    "' till '%" NXS_JSON_INT_FORMAT "')",
			                    nxs_string_str(nxs_json_get_key(json)),
			                    nxs_json_integer_val(j),
			                    (ssize_t)cfg_json_par_el->min_val,
			                    (ssize_t)cfg_json_par_el->max_val);

			return NXS_CFG_JSON_CONF_ERROR;
		}
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_array_int_32(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_bool_t          is_list,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_array_t *var_array;
	nxs_list_t * var_list;
	nxs_json_t * j;
	int *        p;
	size_t       i;

	var_array = NULL;
	var_list  = NULL;

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_ARRAY) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: array)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(is_list == NXS_YES) {

		var_list = nxs_cfg_json_get_val(cfg_json_par_el);
	}
	else {

		var_array = nxs_cfg_json_get_val(cfg_json_par_el);
	}

	for(i = 0; i < nxs_json_child_get_count(json); i++) {

		j = nxs_json_child_get_by_index(json, i);

		if(nxs_json_type_get(j) != NXS_JSON_TYPE_INTEGER) {

			nxs_log_write_error(
			        proc,
			        "json cfg process error: wrong type for sub-value (option: \"%s\", type: %s, expected type: integer)",
			        nxs_string_str(nxs_json_get_key(json)),
			        nxs_json_type_string(j));

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(is_list == NXS_YES) {

			p = nxs_list_add_tail(var_list);
		}
		else {

			p = nxs_array_add(var_array);
		}

		*p = (int)nxs_json_integer_val(j);

		/* Проверка нахождения считываемых значений в заданных границах */

		if((ssize_t)cfg_json_par_el->min_val == 0 && (ssize_t)cfg_json_par_el->max_val == 0) {

			continue;
		}

		if(*p < (int)cfg_json_par_el->min_val) {

			nxs_log_write_error(proc,
			                    "json cfg process error: value of integer option less then allowed minimum value (option: "
			                    "\"%s\", value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
			                    "' till '%" NXS_JSON_INT_FORMAT "')",
			                    nxs_string_str(nxs_json_get_key(json)),
			                    nxs_json_integer_val(j),
			                    (ssize_t)cfg_json_par_el->min_val,
			                    (ssize_t)cfg_json_par_el->max_val);

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(*p > (int)cfg_json_par_el->max_val) {

			nxs_log_write_error(proc,
			                    "json cfg process error: value of integer option more then allowed maximum value (option: "
			                    "\"%s\", value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
			                    "' till '%" NXS_JSON_INT_FORMAT "')",
			                    nxs_string_str(nxs_json_get_key(json)),
			                    nxs_json_integer_val(j),
			                    (ssize_t)cfg_json_par_el->min_val,
			                    (ssize_t)cfg_json_par_el->max_val);

			return NXS_CFG_JSON_CONF_ERROR;
		}
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_cfg_json_type_handler_array_real(nxs_process_t *proc, nxs_json_t *json, nxs_bool_t is_list, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_array_t *var_array;
	nxs_list_t * var_list;
	nxs_json_t * j;
	double *     p;
	size_t       i;

	var_array = NULL;
	var_list  = NULL;

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_ARRAY) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: array)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(is_list == NXS_YES) {

		var_list = nxs_cfg_json_get_val(cfg_json_par_el);
	}
	else {

		var_array = nxs_cfg_json_get_val(cfg_json_par_el);
	}

	for(i = 0; i < nxs_json_child_get_count(json); i++) {

		j = nxs_json_child_get_by_index(json, i);

		if(nxs_json_type_get(j) != NXS_JSON_TYPE_REAL) {

			nxs_log_write_error(
			        proc,
			        "json cfg process error: wrong type for sub-value (option: \"%s\", type: %s, expected type: real)",
			        nxs_string_str(nxs_json_get_key(json)),
			        nxs_json_type_string(j));

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(is_list == NXS_YES) {

			p = nxs_list_add_tail(var_list);
		}
		else {

			p = nxs_array_add(var_array);
		}

		*p = (double)nxs_json_real_val(j);

		/* Проверка нахождения считываемых значений в заданных границах */

		if(cfg_json_par_el->min_val == 0 && cfg_json_par_el->max_val == 0) {

			continue;
		}

		if(*p < cfg_json_par_el->min_val) {

			nxs_log_write_error(proc,
			                    "json cfg process error: value of integer option less then allowed minimum value (option: "
			                    "\"%s\", value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
			                    "' till '%" NXS_JSON_INT_FORMAT "')",
			                    nxs_string_str(nxs_json_get_key(json)),
			                    nxs_json_integer_val(j),
			                    (ssize_t)cfg_json_par_el->min_val,
			                    (ssize_t)cfg_json_par_el->max_val);

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(*p > cfg_json_par_el->max_val) {

			nxs_log_write_error(proc,
			                    "json cfg process error: value of integer option more then allowed maximum value (option: "
			                    "\"%s\", value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
			                    "' till '%" NXS_JSON_INT_FORMAT "')",
			                    nxs_string_str(nxs_json_get_key(json)),
			                    nxs_json_integer_val(j),
			                    (ssize_t)cfg_json_par_el->min_val,
			                    (ssize_t)cfg_json_par_el->max_val);

			return NXS_CFG_JSON_CONF_ERROR;
		}
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_array_string(nxs_process_t *     proc,
                                                                   nxs_json_t *        json,
                                                                   nxs_bool_t          is_list,
                                                                   nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_array_t * var_array;
	nxs_list_t *  var_list;
	nxs_json_t *  j;
	nxs_string_t *p;
	size_t        i;

	var_array = NULL;
	var_list  = NULL;

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_ARRAY) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: array)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(is_list == NXS_YES) {

		var_list = nxs_cfg_json_get_val(cfg_json_par_el);
	}
	else {

		var_array = nxs_cfg_json_get_val(cfg_json_par_el);
	}

	for(i = 0; i < nxs_json_child_get_count(json); i++) {

		j = nxs_json_child_get_by_index(json, i);

		if(nxs_json_type_get(j) != NXS_JSON_TYPE_STRING) {

			nxs_log_write_error(
			        proc,
			        "json cfg process error: wrong type for sub-value (option: \"%s\", type: %s, expected type: string)",
			        nxs_string_str(nxs_json_get_key(json)),
			        nxs_json_type_string(j));

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(is_list == NXS_YES) {

			p = nxs_list_add_tail(var_list);
		}
		else {

			p = nxs_array_add(var_array);
		}

		nxs_string_init2(p, 0, nxs_string_str(nxs_json_string_val(j)));
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t
        nxs_cfg_json_type_handler_array_bool(nxs_process_t *proc, nxs_json_t *json, nxs_bool_t is_list, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_array_t *var_array;
	nxs_list_t * var_list;
	nxs_json_t * j;
	nxs_bool_t * p;
	size_t       i;

	var_array = NULL;
	var_list  = NULL;

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_ARRAY) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: array)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(is_list == NXS_YES) {

		var_list = nxs_cfg_json_get_val(cfg_json_par_el);
	}
	else {

		var_array = nxs_cfg_json_get_val(cfg_json_par_el);
	}

	for(i = 0; i < nxs_json_child_get_count(json); i++) {

		j = nxs_json_child_get_by_index(json, i);

		if(nxs_json_type_get(j) != NXS_JSON_TYPE_TRUE && nxs_json_type_get(j) != NXS_JSON_TYPE_FALSE) {

			nxs_log_write_error(
			        proc,
			        "json cfg process error: wrong type for sub-value (option: \"%s\", type: %s, expected type: true or false)",
			        nxs_string_str(nxs_json_get_key(json)),
			        nxs_json_type_string(j));

			return NXS_CFG_JSON_CONF_ERROR;
		}

		if(is_list == NXS_YES) {

			p = nxs_list_add_tail(var_list);
		}
		else {

			p = nxs_array_add(var_array);
		}

		*p = nxs_json_bool_val(j);
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_int(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	ssize_t *var = nxs_cfg_json_get_val(cfg_json_par_el);

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_INTEGER) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: integer)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	*var = (ssize_t)nxs_json_integer_val(json);

	if((ssize_t)cfg_json_par_el->min_val == 0 && (ssize_t)cfg_json_par_el->max_val == 0) {

		return NXS_CFG_JSON_CONF_OK;
	}

	if(*var < (ssize_t)cfg_json_par_el->min_val) {

		nxs_log_write_error(proc,
		                    "json cfg process error: value of integer option less then allowed minimum value (option: \"%s\", "
		                    "value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
		                    "' till '%" NXS_JSON_INT_FORMAT "')",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_integer_val(json),
		                    (ssize_t)cfg_json_par_el->min_val,
		                    (ssize_t)cfg_json_par_el->max_val);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(*var > (ssize_t)cfg_json_par_el->max_val) {

		nxs_log_write_error(proc,
		                    "json cfg process error: value of integer option more then allowed maximum value (option: \"%s\", "
		                    "value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
		                    "' till '%" NXS_JSON_INT_FORMAT "')",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_integer_val(json),
		                    (ssize_t)cfg_json_par_el->min_val,
		                    (ssize_t)cfg_json_par_el->max_val);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_int_16(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	int16_t *var = nxs_cfg_json_get_val(cfg_json_par_el);

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_INTEGER) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: integer)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	*var = (int16_t)nxs_json_integer_val(json);

	if((ssize_t)cfg_json_par_el->min_val == 0 && (ssize_t)cfg_json_par_el->max_val == 0) {

		return NXS_CFG_JSON_CONF_OK;
	}

	if(*var < (int16_t)cfg_json_par_el->min_val) {

		nxs_log_write_error(proc,
		                    "json cfg process error: value of integer option less then allowed minimum value (option: \"%s\", "
		                    "value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
		                    "' till '%" NXS_JSON_INT_FORMAT "')",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_integer_val(json),
		                    (ssize_t)cfg_json_par_el->min_val,
		                    (ssize_t)cfg_json_par_el->max_val);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(*var > (int16_t)cfg_json_par_el->max_val) {

		nxs_log_write_error(proc,
		                    "json cfg process error: value of integer option more then allowed maximum value (option: \"%s\", "
		                    "value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
		                    "' till '%" NXS_JSON_INT_FORMAT "')",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_integer_val(json),
		                    (ssize_t)cfg_json_par_el->min_val,
		                    (ssize_t)cfg_json_par_el->max_val);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_int_32(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	int *var = nxs_cfg_json_get_val(cfg_json_par_el);

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_INTEGER) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: integer)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	*var = (int)nxs_json_integer_val(json);

	if((ssize_t)cfg_json_par_el->min_val == 0 && (ssize_t)cfg_json_par_el->max_val == 0) {

		return NXS_CFG_JSON_CONF_OK;
	}

	if(*var < (int)cfg_json_par_el->min_val) {

		nxs_log_write_error(proc,
		                    "json cfg process error: value of integer option less then allowed minimum value (option: \"%s\", "
		                    "value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
		                    "' till '%" NXS_JSON_INT_FORMAT "')",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_integer_val(json),
		                    (ssize_t)cfg_json_par_el->min_val,
		                    (ssize_t)cfg_json_par_el->max_val);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(*var > (int)cfg_json_par_el->max_val) {

		nxs_log_write_error(proc,
		                    "json cfg process error: value of integer option more then allowed maximum value (option: \"%s\", "
		                    "value: \"%" NXS_JSON_INT_FORMAT "\", allowed values: from '%" NXS_JSON_INT_FORMAT
		                    "' till '%" NXS_JSON_INT_FORMAT "')",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_integer_val(json),
		                    (ssize_t)cfg_json_par_el->min_val,
		                    (ssize_t)cfg_json_par_el->max_val);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_real(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	double *var = nxs_cfg_json_get_val(cfg_json_par_el);

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_REAL) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: real)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	*var = (double)nxs_json_real_val(json);

	if(cfg_json_par_el->min_val == 0 && cfg_json_par_el->max_val == 0) {

		return NXS_CFG_JSON_CONF_OK;
	}

	if(*var < cfg_json_par_el->min_val) {

		nxs_log_write_error(proc,
		                    "json cfg process error: value of real option less then allowed minimum value (option: \"%s\", value: "
		                    "\"%lf\", allowed values: from '%lf' till '%lf')",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_real_val(json),
		                    cfg_json_par_el->min_val,
		                    cfg_json_par_el->max_val);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	if(*var > cfg_json_par_el->max_val) {

		nxs_log_write_error(proc,
		                    "json cfg process error: value of real option more then allowed maximum value (option: \"%s\", value: "
		                    "\"%lf\", allowed values: from '%lf' till '%lf')",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_real_val(json),
		                    cfg_json_par_el->min_val,
		                    cfg_json_par_el->max_val);

		return NXS_CFG_JSON_CONF_ERROR;
	}

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_string(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_string_t *var = nxs_cfg_json_get_val(cfg_json_par_el);

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_STRING) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: string)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	nxs_string_cpy_dyn(var, 0, nxs_json_string_val(json), 0);

	return NXS_CFG_JSON_CONF_OK;
}

static nxs_cfg_json_state_t nxs_cfg_json_type_handler_bool(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_bool_t *var = nxs_cfg_json_get_val(cfg_json_par_el);

	if(nxs_json_type_get(json) != NXS_JSON_TYPE_TRUE && nxs_json_type_get(json) != NXS_JSON_TYPE_FALSE) {

		nxs_log_write_error(proc,
		                    "json cfg process error: wrong type for value (option: \"%s\", type: %s, expected type: true or false)",
		                    nxs_string_str(nxs_json_get_key(json)),
		                    nxs_json_type_string(json));

		return NXS_CFG_JSON_CONF_ERROR;
	}

	*var = nxs_json_bool_val(json);

	return NXS_CFG_JSON_CONF_OK;
}

static void nxs_cfg_json_conf_array_add_eol(nxs_array_t *cfg_arr, nxs_bool_t skip_undef)
{
	nxs_cfg_json_par_t *cfg;

	cfg = nxs_array_add(cfg_arr);

	nxs_string_init(&cfg->name);

	if(skip_undef == NXS_YES) {

		cfg->proc_function = ((void *)1);
	}
	else {

		cfg->proc_function = NULL;
	}

	cfg->value            = NULL;
	cfg->el_prep_function = NULL;
	cfg->type             = 0;
	cfg->min_val          = 0;
	cfg->max_val          = 0;
	cfg->required         = 0;
	cfg->sub_els          = NULL;
}

static void nxs_cfg_json_par_def_init(nxs_array_t *arr, nxs_cfg_json_par_t *cfg_json_par)
{
	nxs_cfg_json_par_def_t *p;
	size_t                  i;

	nxs_array_init(arr, 0, sizeof(nxs_cfg_json_par_def_t), 1);

	for(i = 0; nxs_cfg_json_eop(cfg_json_par[i]) == NXS_NO; i++) {

		p = nxs_array_add(arr);

		p->par   = &cfg_json_par[i];
		p->count = 0;
	}
}

static void nxs_cfg_json_par_def_free(nxs_array_t *arr)
{

	nxs_array_free(arr);
}

static nxs_cfg_json_state_t nxs_cfg_json_par_def_add(nxs_process_t *proc, nxs_array_t *arr, nxs_cfg_json_par_t *cfg_json_par_el)
{
	nxs_cfg_json_par_def_t *p;
	size_t                  i;

	for(i = 0; i < nxs_array_count(arr); i++) {

		p = nxs_array_get(arr, i);

		if(p->par == cfg_json_par_el) {

			if(p->count == 1) {

				nxs_log_write_error(proc,
				                    "json cfg process error: duplicate entry for option (option: \"%s\")",
				                    nxs_string_str(&cfg_json_par_el->name));

				return NXS_CFG_JSON_CONF_ERROR;
			}

			p->count = 1;

			return NXS_CFG_JSON_CONF_OK;
		}
	}

	nxs_log_write_error(
	        proc, "json cfg process error: can't find option in def-list (option: \"%s\")", nxs_string_str(&cfg_json_par_el->name));

	return NXS_CFG_JSON_CONF_ERROR;
}

static nxs_cfg_json_state_t nxs_cfg_json_par_def_check(nxs_process_t *proc, nxs_array_t *arr)
{
	nxs_cfg_json_par_def_t *p;
	size_t                  i;

	for(i = 0; i < nxs_array_count(arr); i++) {

		p = nxs_array_get(arr, i);

		if(p->count == 0 && p->par->required == NXS_YES) {

			nxs_log_write_error(
			        proc, "json cfg process error: missing required option (option: \"%s\")", nxs_string_str(&p->par->name));

			return NXS_CFG_JSON_CONF_ERROR;
		}
	}

	return NXS_CFG_JSON_CONF_OK;
}

/*
 * Проверка пропуска ошибок в случае, если встретился неопределённый параметр.
 * Флагом для этого служит не нулевой указаль на функцию proc_function().
 *
 * Возвращаемые значения:
 * NXS_YES	- пропустить ошибку в случае встречи с неопределённого параметра
 * NXS_NO	- вызвать ошибку в случае встречи с неопределённого параметра
 */
static nxs_bool_t nxs_cfg_json_par_check_skip_undef(nxs_cfg_json_par_t *cfg_json_par)
{
	size_t i;

	for(i = 0; nxs_string_str(&cfg_json_par[i].name) != NULL; i++)
		;

	if(cfg_json_par[i].proc_function != NULL) {

		return NXS_YES;
	}

	return NXS_NO;
}
