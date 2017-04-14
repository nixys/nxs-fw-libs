#ifndef _INCLUDE_NXS_CFG_JSON_H
#define _INCLUDE_NXS_CFG_JSON_H

#include <nxs-core/nxs-core.h>

enum nxs_cfg_json_state_e
{
	NXS_CFG_JSON_CONF_OK,
	NXS_CFG_JSON_CONF_ERROR
};

enum nxs_cfg_json_type_e
{
	NXS_CFG_JSON_TYPE_VOID,
	NXS_CFG_JSON_TYPE_OBJECT,
	NXS_CFG_JSON_TYPE_ARRAY_OBJECT,
	NXS_CFG_JSON_TYPE_ARRAY_INT,
	NXS_CFG_JSON_TYPE_ARRAY_REAL,
	NXS_CFG_JSON_TYPE_ARRAY_STRING,
	NXS_CFG_JSON_TYPE_ARRAY_BOOL,
	NXS_CFG_JSON_TYPE_LIST_INT,
	NXS_CFG_JSON_TYPE_LIST_REAL,
	NXS_CFG_JSON_TYPE_LIST_STRING,
	NXS_CFG_JSON_TYPE_LIST_BOOL,
	NXS_CFG_JSON_TYPE_INT,
	NXS_CFG_JSON_TYPE_REAL,
	NXS_CFG_JSON_TYPE_STRING,
	NXS_CFG_JSON_TYPE_BOOL,
	NXS_CFG_JSON_TYPE_ARRAY_INT_16,
	NXS_CFG_JSON_TYPE_ARRAY_INT_32,
	NXS_CFG_JSON_TYPE_LIST_INT_16,
	NXS_CFG_JSON_TYPE_LIST_INT_32,
	NXS_CFG_JSON_TYPE_INT_16,
	NXS_CFG_JSON_TYPE_INT_32,
};

struct nxs_cfg_json_par_s
{
	/*
	 * Имя параметра
	 */
	nxs_string_t					name;

	/*
	 * Значение параметра
	 */
	void							*value;

	/*
	 * Функция обработчик параметра (если необходимо)
	 */
	nxs_cfg_json_state_t			(*proc_function)(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el);

	/*
	 * Функция для подготовки элемента для обработки
	 * Используется при обработке элементов-массивов, дочерними элементами которых являются объекты.
	 */
	nxs_cfg_json_state_t			(*el_prep_function)(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par_el, nxs_array_t *cfg_arr);

	/*
	 * Тип параметра
	 */
	nxs_cfg_json_type_t				type;

	/*
	 * Минимальное значение параметра (используется только для типа int и real)
	 */
	double							min_val;

	/*
	 * Максимальное значение параметра (используется только для типа int и real)
	 */
	double							max_val;

	/*
	 * Обязательность указания параметра.
	 */
	nxs_bool_t						required;

	/*
	 * Указатель на массив описаний параметров.
	 * Используется для элементов типа объект, чьим дочерним элементом так же является объект
	 */
	nxs_cfg_json_par_t				*sub_els;
};

struct nxs_cfg_json_s
{
	nxs_string_t					cfg_path;
	nxs_cfg_json_state_t			(*prep_function)(nxs_cfg_json_t cfg);
	nxs_cfg_json_state_t			(*post_function)(nxs_cfg_json_t cfg);
	nxs_cfg_json_par_t				*cfg_par;
};

#define	NXS_CFG_JSON_NULL			{{NULL, 0, 0},	NULL,	NULL, 			NULL,	0,	0,	0,	0,	NULL}
#define	NXS_CFG_JSON_NULL_UNDEF		{{NULL, 0, 0},	NULL,	((void *)1),	NULL,	0,	0,	0,	0,	NULL}

#define	nxs_cfg_json_get_val(cfg_json_par) \
		cfg_json_par->value

nxs_cfg_json_state_t								nxs_cfg_json_init										(	nxs_process_t *proc,
																												nxs_cfg_json_t *cfg,
																												nxs_string_t *cfg_path,
																												nxs_cfg_json_state_t (*prep_function)(nxs_cfg_json_t cfg),
																												nxs_cfg_json_state_t (*post_function)(nxs_cfg_json_t cfg),
																												nxs_array_t	*cfg_arr);
void nxs_cfg_json_free(nxs_cfg_json_t *cfg);

nxs_cfg_json_state_t								nxs_cfg_json_read_file									(nxs_process_t *proc, nxs_cfg_json_t cfg);
nxs_cfg_json_state_t								nxs_cfg_json_read_buf									(nxs_process_t *proc, nxs_cfg_json_t cfg, nxs_buf_t *json_buf);
nxs_cfg_json_state_t								nxs_cfg_json_read_json									(nxs_process_t *proc, nxs_cfg_json_t cfg, nxs_json_t *json);
nxs_cfg_json_state_t								nxs_cfg_json_element_process							(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par);

void												nxs_cfg_json_conf_array_init							(nxs_array_t *cfg_arr);
void												nxs_cfg_json_conf_array_free							(nxs_array_t *cfg_arr);
void												nxs_cfg_json_conf_array_add								(	nxs_array_t				*cfg_arr,
																												nxs_string_t			*par_name,
																												void					*value,
																												nxs_cfg_json_state_t	(*proc_function)(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par),
																												nxs_cfg_json_state_t	(*el_prep_function)(nxs_process_t *proc, nxs_json_t *json, nxs_cfg_json_par_t *cfg_json_par, nxs_array_t *cfg_arr),
																												nxs_cfg_json_type_t		type,
																												double					min_val,
																												double					max_val,
																												nxs_bool_t				required,
																												nxs_cfg_json_par_t		*sub_els);
void												nxs_cfg_json_conf_array_skip_undef						(nxs_array_t *cfg_arr);
nxs_cfg_json_state_t								nxs_cfg_json_conf_array_merge							(nxs_process_t *proc, nxs_cfg_json_t *cfg, nxs_array_t *cfg_arr);

#endif /* _INCLUDE_NXS_CFG_JSON_H */
