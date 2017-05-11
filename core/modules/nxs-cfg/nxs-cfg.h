#ifndef _INCLUDE_NXS_CFG_H
#define _INCLUDE_NXS_CFG_H

// clang-format off

/** @addtogroup nxs-cfg
 *
 * @brief Module 'nxs-cfg' is a basic module providing functions to work with ini-like configuration files.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

extern u_char	*nxs_fs_errtext;

#define NXS_CFG_CONF_OK			0
#define NXS_CFG_CONF_ERROR		1

#define NXS_CFG_REQUIRED_NO		0
#define NXS_CFG_REQUIRED_YES		1

#define NXS_CFG_TWICE_DEFINITION_NO	0
#define NXS_CFG_TWICE_DEFINITION_YES	1

#define NXS_CFG_TYPE_VOID		0 /* Без типа: используется собственный обработчик */
#define NXS_CFG_TYPE_INT		1 /* Тип int: значения между 'min_val' и 'max_val' */
#define NXS_CFG_TYPE_STRING		2 /* Тип строка */
#define NXS_CFG_TYPE_NUM		3 /* Тип число: каждый символ - это цифра, но сохраняется как строка */
#define NXS_CFG_TYPE_LIST		4 /* Тип список с повторениями: каждый элемент списка - это строка */
#define NXS_CFG_TYPE_LIST_DISTINCT	5 /* Тип список без повторений: каждый элемент списка - это уникальная для данного списка строка */
#define NXS_CFG_TYPE_LIST_NUM		6 /* Тип список чисел с повторениями: каждый элемент списка - это число (каждый символ - это цифра, но сохраняется как строка) */
#define NXS_CFG_TYPE_LIST_NUM_DISTINCT	7 /* Тип список чисел без повторений: каждый элемент списка - это число уникальное для данного списка (каждый символ - это цифра, но сохраняется как строка) */

#define	NXS_CFG_NULL	{{NULL, 0, 0}, NULL, NULL, 0, 0, 0, 0, 0}

struct nxs_cfg_par_s
{
	/*
	 * Имя параметра
	 */
	nxs_string_t		name;

	/*
	 * Значение параметра
	 */
	void			*value;

	/*
	 * Функция обработчик параметра (если необходимо)
	 */
	int			(*init_function)(nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par);

	/*
	 * Тип параметра
	 */
	int			type;

	/*
	 * Минимальное значение параметра (используется только для типа int)
	 */
	int			min_val;

	/*
	 * Максимальное значение параметра (используется только для типа int)
	 */
	int			max_val;

	/*
	 * Обязательность указания параметра.
	 */
	int			required;

	/*
	 * Разрешение на повторное определение опции
	 */
	int			twice_def;
};

struct nxs_cfg_s
{
	nxs_string_t		cfg_path;
	int			(*prep_function)(nxs_cfg_t cfg);
	int			(*post_function)(nxs_cfg_t cfg);
	nxs_cfg_par_t		*cfg_par;
};

#define	nxs_cfg_get_val(cfg_par) \
			cfg_par->value

int				nxs_cfg_read				(nxs_process_t *proc, nxs_cfg_t cfg);

/** @} */ // end of nxs-cfg
#endif /* _INCLUDE_NXS_CFG_H */
