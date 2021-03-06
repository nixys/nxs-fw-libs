#ifndef _INCLUDE_NXS_BASE64_H
#define _INCLUDE_NXS_BASE64_H

// clang-format off

/** @addtogroup nxs-base64
 *
 * @brief Module 'nxs-base64' is a basic module providing functions to work with base64 encoding.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

/*
 * Кодирование строки 'src' в base64. Результат будет записан в строку 'dst'
 * Необходимо, чтобы обе строки были инициализированы до вызова данной фунции.
 * При необходимости, дополнительная память под строку 'dst' будет выделена.
 */
void				nxs_base64_encode_string		(nxs_string_t *dst, nxs_string_t *src);

/*
 * Декодирование строки 'src', содежащей данные в base64. Результат будет записан в строку 'dst'
 * Необходимо, чтобы обе строки были инициализированы до вызова данной фунции.
 * При необходимости, дополнительная память под строку 'dst' будет выделена.
 */
void				nxs_base64_decode_string		(nxs_string_t *dst, nxs_string_t *src);

/*
 * Кодирование блока данных, на которые указывает 'src_data' длиной 'src_len' в base64.
 * Результат будет записан в строку 'dst'
 * Необходимо, чтобы строка 'dst' была инициализирована до вызова данной фунции.
 * При необходимости, дополнительная память под строку 'dst' будет выделена.
 */
void				nxs_base64_encode_void			(nxs_string_t *dst, void *src_data, size_t src_len);

/*
 * Декодирование строки 'src', содежащей данные в base64. Результат будет записан в область памяти,
 * на которую указывает 'dst_data', причём будет записано не более 'dst_max_len' байт,
 * в случае превышения данного обьёма будет возвращён '0'.
 * Память под 'dst_data' должна быть выделена до вызова данной функции.
 *
 * Возвращает количество декодированных байт.
 */
size_t				nxs_base64_decode_void			(void *dst_data, size_t dst_max_len, nxs_string_t *src);

/** @} */ // end of nxs-base64
#endif /* _INCLUDE_NXS_BASE64_H */
