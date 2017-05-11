#ifndef _INCLUDE_NXS_CRYPT_GOST_H
#define _INCLUDE_NXS_CRYPT_GOST_H

// clang-format off

/** @addtogroup nxs-crypt-gost
 *
 * @brief Module 'nxs-crypt-gost' is a basic module providing functions to work with GOST 28147-89 encryption.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_CRYPT_GOST_E_OK		0
#define NXS_CRYPT_GOST_E_KEY_SRC_LEN	1
#define NXS_CRYPT_GOST_E_KEY_READ	2

#define NXS_CRYPT_GOST_INIT_NO		0
#define NXS_CRYPT_GOST_INIT_YES		1

/*
 * Количество 32-х битных регистров из которых состоит ключ
 */
#define	NXS_CRYPT_GOST_KEY_REGISTERS	8

/*
 * Количество строк в таблице(матрице) замен
 */
#define	NXS_CRYPT_GOST_S_LINES		8

/*
 * Количество столбцов в таблице(матрице) замен
 */
#define	NXS_CRYPT_GOST_S_VALUES		16

/*
 * Максимальное значение
 */
#define	NXS_CRYPT_GOST_S_MAX_VALUE	15

/*
 * Количество 4-байтных элементов в 64 битном векторе
 */
#define	NXS_CRYPT_GOST_64_REG32		2

/*
 * Количество 1-байтных элементов в 64 битном векторе
 */
#define	NXS_CRYPT_GOST_64_REG8		8

union nxs_crypt_gost_s_vector_u
{
	uint32_t			reg32[NXS_CRYPT_GOST_64_REG32];
	u_char				reg8[NXS_CRYPT_GOST_64_REG8];
};

/*
 * Structure that contains all secret elements needed for GOST 28147-89 encryption algorythm:
 * * CEM initialization vector
 * * Key Data Store
 * * Substitution Box
 */
struct	nxs_crypt_gost_el_s
{
	/*
	 * Переменная, которая содержит количество байт,
	 * шифрованных/дешифрованных текущим набором
	 * ключей.
	 * Значение изменяется в пределах [0,8)
	 */
	int				bytes_count;

	/*
	 * Ключ шифрования: восемь 32-битных регистров
	 */
	uint32_t			key[NXS_CRYPT_GOST_KEY_REGISTERS];

	/*
	 * Таблица замен (substitution table)
	 */
	u_char				subs_table[NXS_CRYPT_GOST_S_LINES][NXS_CRYPT_GOST_S_VALUES];

	/* The initialization vector for Counter Encryption Mode */
	union nxs_crypt_gost_s_vector_u	s_vector;
};

struct nxs_crypt_gost_ctx_s
{

	/*
	 * Флаг для обозначения инициализации текущего набора ключей.
	 * Может принимать значения:
	 * * NXS_CRYPT_GOST_INIT_NO
	 * * NXS_CRYPT_GOST_INIT_YES
	 */
	int				init;

	nxs_crypt_gost_el_t		main_el;
	nxs_list_t			els;
};

#define nxs_crypt_gost_string(str, nxs_crypt_gost_el) \
			nxs_crypt_gost(nxs_string_str(str), nxs_string_len(str), nxs_crypt_gost_el)

#define nxs_crypt_gost_buf(buf, nxs_crypt_gost_el) \
			nxs_crypt_gost(nxs_buf_get_subbuf(buf, 0), nxs_buf_get_len(buf), nxs_crypt_gost_el)

nxs_crypt_gost_ctx_t		*nxs_crypt_gost_malloc			(nxs_string_t *key);
nxs_crypt_gost_ctx_t		*nxs_crypt_gost_malloc2			(nxs_process_t *proc, nxs_string_t *path);
nxs_crypt_gost_ctx_t		*nxs_crypt_gost_destroy			(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx);
int				nxs_crypt_gost_init			(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx, nxs_string_t *key);
int				nxs_crypt_gost_init2			(nxs_process_t *proc, nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx, nxs_string_t *path);
void				nxs_crypt_gost_free			(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx);
nxs_crypt_gost_el_t		*nxs_crypt_gost_key_open		(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx);
nxs_crypt_gost_el_t		*nxs_crypt_gost_key_close		(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx, nxs_crypt_gost_el_t	*key_el);
int				nxs_crypt_gost_key_read			(nxs_process_t *proc, nxs_string_t *key, nxs_string_t *path);
void				nxs_crypt_gost_key_gen			(nxs_string_t *key);
void				nxs_crypt_gost_reset			(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx, nxs_crypt_gost_el_t *nxs_crypt_gost_el);
void				nxs_crypt_gost				(u_char *data, size_t len, nxs_crypt_gost_el_t *nxs_crypt_gost_el);

/** @} */ // end of nxs-crypt-gost
#endif /* _INCLUDE_NXS_CRYPT_GOST_H */
