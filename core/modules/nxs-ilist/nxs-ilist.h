#ifndef _INCLUDE_NXS_ILIST_H
#define _INCLUDE_NXS_ILIST_H

// clang-format off

/** @addtogroup nxs-ilist
 *
 * @brief Module 'nxs-ilist' is a basic module providing functions to work with indexing lists (experimental, not complete).
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_ILIST_INDEX_UNDEF	-1

#define NXS_ILIST_EL_MISMATCH	0
#define NXS_ILIST_EL_MATCH	1

struct nxs_ilist_s
{
	/*
	 * Список хранимых элементов
	 */
	nxs_list_t		*els;

	/*
	 * Массив индексов
	 */
	nxs_array_t		*iarray;

	/*
	 * Функция вычисления индекса для элемента по ключу, который передаётся в качестве аргумента.
	 * Прототип функции должен иметь вид: "int el_index_f(TYPE *p)", где TYPE - это тип ключа.
	 * Возвращаемое значение функции: значение индекса или "NXS_ILIST_INDEX_UNDEF", если индекс по каким-то причинам вычислить невозможно
	 */
	int			(*el_index_f)();

	/*
	 * Функция для проверки соответствия элемента искомому ключу.
	 * Прототип функции должен иметь вид: "u_int el_check_f(EL_TYPE *el, TYPE *p)",
	 * где EL_TYPE - эти тип элемента, размещаемого в списке "els", TYPE - это тип ключа.
	 * озвращаемое значение функции: NXS_ILIST_EL_MATCH - элемент el соответствует индексу p, NXS_ILIST_EL_MISMATCH - не соответствует
	 */
	u_int			(*el_check_f)();
};

void				nxs_ilist_init				(nxs_ilist_t *ilist, u_int el_size, void *el_index_f, void *el_check_f);
void				*nxs_ilist_add				(nxs_ilist_t *ilist, void *el, void *key);
void				*nxs_ilist_find				(nxs_ilist_t *ilist, void *key);

void				nxs_ilist_string_init			(nxs_ilist_t *ilist);

/** @} */ // end of nxs-ilist
#endif /* _INCLUDE_NXS_ILIST_H */
