#ifndef _INCLUDE_NXS_STRING_H
#define _INCLUDE_NXS_STRING_H

/** @addtogroup nxs-string
 *
 * @brief Module 'nxs-string' it's a basic module providing works with strings.
 *
 * Nxs-fw strings always ends by character '\\n'.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_STRING_OK					0

#define NXS_STRING_ERROR				-1
#define NXS_STRING_ERROR_NOT_INIT		-2
#define NXS_STRING_ERROR_DST_SIZE		-3
#define NXS_STRING_ERROR_SRC_SIZE		-4
#define NXS_STRING_ERROR_DST_LEN		-5
#define NXS_STRING_ERROR_SRC_LEN		-6
#define NXS_STRING_ERROR_OFFSET			-7
#define NXS_STRING_ERROR_NOT_CREATED	-8
#define NXS_STRING_ERROR_NULL_PTR		-9
#define NXS_STRING_ERROR_SPEC_UNKNOWN	-10
#define NXS_STRING_ERROR_SPEC_OVERFLOW	-11

#define NXS_STRING_CMP_EQ			0
#define NXS_STRING_CMP_NE			1

#define NXS_STRING_NOT_FULL			0
#define NXS_STRING_FULL				1

#define NXS_STRING_NO_OFFSET		0

#define NXS_STRING_EMPTY_STR		(u_char *)""

#define	nxs_string(str)				{(u_char *)str, sizeof(str) - 1, sizeof(str)}

struct nxs_string_s
{
	u_char	*str;	/**< u_char array pointer */
	size_t	len;	/**< String length (max string length: #size - 1). Character '\\n' is always placed in #len position. */
	size_t	size;	/**< Amount of allocated memory for #str */
};

enum nxs_string_escape_types_e
{
	NXS_STRING_ESCAPE_TYPE_GENERIC,		/**<  GENERIC */
	NXS_STRING_ESCAPE_TYPE_JSON			/**<  JSON */
};

#define nxs_string_atoi(str) \
			atoi((char *)nxs_string_get_substr(str, NXS_STRING_NO_OFFSET))

#define nxs_string_atof(str) \
			atof((char *)nxs_string_get_substr(str, NXS_STRING_NO_OFFSET))

#define nxs_string_str(str) \
			nxs_string_get_substr(str, NXS_STRING_NO_OFFSET)
/*
#define nxs_string_printf2_cat(str, msg...) \
			nxs_string_printf2(str, nxs_string_len(str), msg)
*/
#define nxs_string_printf2_cat_dyn(str, msg...) \
			nxs_string_printf2_dyn(str, nxs_string_len(str), msg)

/**
 * @brief Allocate memory for new string and copy in data from \b new_str (if not NULL).
 *
 * * If pointer to \b new_str is NULL and \b size is 0 - pointer to allocated and initialized (but not created) string will be retuned.
 * * If pointer to \b new_str is NULL and \b size greater than 0 - pointer to allocated, initialized and created (with size \b size) string will be retuned.
 *
 * * If pointer to \b new_str is not NULL - pointer to allocated, initialized and created string will be retuned.
 * In this case nxs_string_create() will be called after memory allocated for new string.
 *
 * This function uses only if string was created by nxs_string_malloc()
 *
 * @param size Size of new string
 * @param new_str Pointer to source string (u_char array)
 *
 * @return NULL
 */
nxs_string_t 	*nxs_string_malloc					(size_t size, u_char *new_str);

/**
 * @brief Destroy string
 *
 * This function uses only if string was created by nxs_string_malloc()
 *
 * @param str Pointer to string to be destroyed
 *
 * @return NULL
 */
nxs_string_t	*nxs_string_destroy					(nxs_string_t *str);

/**
 * @brief Initialize string (empty)
 *
 * Memory for \b str must be allocated before call this function.
 * Initializing string will not be created.
 * This function must be called before first use of \b str.
 *
 * Example:
 * @code
 * ...
 *
 * nxs_string_t	str;
 *
 * nxs_string_init(&str);
 *
 * ...
 * @endcode
 *
 * @param str Pointer to string to be initialized
 *
 * @return void
 */
void			nxs_string_init						(nxs_string_t *str);

/**
 * @brief Initialize string (from u_char *)
 *
 * Memory for \b str must be allocated before call this function.
 * Initializing string will be created (as copy of \b new_str).
 * This function must be called before first use of \b str.
 *
 * This function it's a conjunction of functions nxs_string_init() and nxs_string_create().
 *
 * Example:
 * @code
 * void foo(u_char *src)
 * {
 *  nxs_string_t	str;
 *
 *  nxs_string_init2(&str, 0, src);
 *
 *  ...
 * }
 * @endcode
 *
 * @param str Pointer to string to be initialized
 * @param size New size of string \b str
 * @param new_str Pointer to source string (u_char array)
 *
 * @return
 * * (>= 0) size of new string on success
 * * (< 0) on error:
 *  * \b NXS_STRING_ERROR_NULL_PTR if pointer to \b str is NULL
 *  * to get other errors see returns of nxs_string_create()
 */
ssize_t			nxs_string_init2					(nxs_string_t *str, size_t size, u_char *new_str);

/**
 * @brief Initialize string (from nxs_string_t)
 *
 * Memory for \b str must be allocated before call this function.
 * Initializing string will be created (as clone of \b src).
 * This function must be called before first use of \b str.
 *
 * Example:
 * @code
 * void foo(nxs_string_t *src)
 * {
 *  nxs_string_t	str;
 *
 *  nxs_string_init3(&str, src);
 *
 *  ...
 * }
 * @endcode
 *
 * @param str Pointer to string to be initialized
 * @param src Pointer to source string
 *
 * @return
 * * (>= 0) size of new string on success
 * * (< 0) on error:
 *  * \b NXS_STRING_ERROR_NULL_PTR if pointer to \b str or \b src is NULL
 *  * to get other errors see returns of nxs_string_create()
 */
ssize_t			nxs_string_init3					(nxs_string_t *str, nxs_string_t *src);

/**
 * Create string 'str'.
 * Initialize string 'str' before call this function.
 *
 *
 */
ssize_t			nxs_string_create					(nxs_string_t *str, size_t size, u_char *new_str);

/**
 * @brief Free string
 *
 * Free memory for nxs_string_s#str, nxs_string_s#size and nxs_string_s#len will be set to 0.
 *
 * This function doesn't destroy the \b str.
 *
 * If \b str was created by nxs_string_malloc(), use nxs_string_destroy() to free and destroy string.
 *
 * @param str Pointer to string
 *
 * @return
 * * NXS_STRING_OK - on success
 * * NXS_STRING_ERROR_NULL_PTR - if pointer \b str is NULL
 * * NXS_STRING_ERROR_NOT_CREATED - if string already freed or initialized and not created yet
 */
int				nxs_string_free						(nxs_string_t *str);

/**
 * @brief Resize string
 *
 *  !!! NOT FINISHED !!!
 *
 * Change allocated memory size for nxs_string_s#str.
 *
 * * If new size \b new_size is 0 - nxs_string_free() will be called.
 * * If string is only initialized -
 *
 * This function doesn't destroy the \b str.
 *
 * If \b str was created by nxs_string_malloc(), use nxs_string_destroy() to free and destroy string.
 *
 * @param str Pointer to string
 *
 * @return
 * * NXS_STRING_OK - on success
 * * NXS_STRING_ERROR_NULL_PTR - if pointer \b str is NULL
 * * NXS_STRING_ERROR_NOT_CREATED - if string already freed or initialized and not created yet
 */
ssize_t			nxs_string_resize					(nxs_string_t *str, size_t new_size);

int				nxs_string_clear					(nxs_string_t *str);

ssize_t			nxs_string_printf_dyn				(nxs_string_t *str, const char *msg, ...);
ssize_t			nxs_string_printf2_dyn				(nxs_string_t *str, size_t offset, const char *msg, ...);
ssize_t			nxs_string_vprintf_dyn				(nxs_string_t *str, const char *fmt, va_list ap);

/*
ssize_t			nxs_string_printf					(nxs_string_t *str, const char *msg, ...);
ssize_t			nxs_string_printf2					(nxs_string_t *str, size_t offset, const char *msg, ...);
ssize_t			nxs_string_vprintf					(nxs_string_t *str, const char *fmt, va_list ap);
*/

ssize_t			nxs_string_cpy						(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src);
ssize_t			nxs_string_cpy_dyn					(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src);

ssize_t			nxs_string_ncpy						(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src, size_t n);
ssize_t			nxs_string_ncpy_dyn					(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src, size_t n);

ssize_t			nxs_string_cat						(nxs_string_t *dst, nxs_string_t *src);
ssize_t			nxs_string_cat_dyn					(nxs_string_t *dst, nxs_string_t *src);

ssize_t			nxs_string_ncat						(nxs_string_t *dst, nxs_string_t *src, size_t n);
ssize_t			nxs_string_ncat_dyn					(nxs_string_t *dst, nxs_string_t *src, size_t n);

int				nxs_string_cmp						(nxs_string_t *str1, size_t offset1, nxs_string_t *str2, size_t offset2);
int				nxs_string_ncmp						(nxs_string_t *str1, size_t offset, nxs_string_t *str2, size_t offset2, size_t n);

ssize_t			nxs_string_char_cpy					(nxs_string_t *str, size_t offset, u_char *ch_str);
ssize_t			nxs_string_char_cpy_dyn				(nxs_string_t *str, size_t offset, u_char *ch_str);

ssize_t			nxs_string_char_ncpy				(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n);
ssize_t			nxs_string_char_ncpy_dyn			(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n);

ssize_t			nxs_string_char_cat					(nxs_string_t *str, u_char *ch_str);
ssize_t			nxs_string_char_cat_dyn				(nxs_string_t *str, u_char *ch_str);

ssize_t			nxs_string_char_ncat				(nxs_string_t *str, u_char *ch_str, size_t n);
ssize_t			nxs_string_char_ncat_dyn			(nxs_string_t *str, u_char *ch_str, size_t n);

int				nxs_string_char_cmp					(nxs_string_t *str, size_t offset, u_char *ch_str);
int				nxs_string_char_ncmp				(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n);

ssize_t			nxs_string_char_add_char			(nxs_string_t *str, u_char c);
ssize_t			nxs_string_char_add_char_dyn		(nxs_string_t *str, u_char c);

void			nxs_string_basename					(nxs_string_t *dst, nxs_string_t *path);
void			nxs_string_dirname					(nxs_string_t *dst, nxs_string_t *path);

ssize_t			nxs_string_set_char					(nxs_string_t *str, size_t pos, u_char c);

ssize_t			nxs_string_ins_char					(nxs_string_t *str, size_t pos, u_char c);
ssize_t			nxs_string_ins_char_dyn				(nxs_string_t *str, size_t pos, u_char c);

ssize_t			nxs_string_set_len					(nxs_string_t *str, size_t len);
ssize_t			nxs_string_len_fix					(nxs_string_t *str);
u_char			nxs_string_get_char					(nxs_string_t *str, size_t pos);
u_char			*nxs_string_get_substr				(nxs_string_t *str, size_t offset);
u_char			*nxs_string_find_substr_first		(nxs_string_t *str, size_t offset, u_char *f_str, size_t f_str_len);
u_char			*nxs_string_find_substr_last		(nxs_string_t *str, size_t offset, u_char *f_str, size_t f_str_len);
u_char			*nxs_string_find_char_first				(nxs_string_t *str, size_t offset, u_char c);
u_char			*nxs_string_find_char_last				(nxs_string_t *str, size_t offset, u_char c);
size_t			nxs_string_subs						(nxs_string_t *src, nxs_string_t *dst, nxs_string_t *f_str, nxs_string_t *d_str, size_t max_count);
u_char			*nxs_string_to_buf					(nxs_string_t *str, size_t offset, nxs_buf_t *buf);
u_char			*nxs_string_strerror					(int nxs_str_errno);
size_t			nxs_string_len						(nxs_string_t *str);
size_t			nxs_string_size						(nxs_string_t *str);
int				nxs_string_check_space				(nxs_string_t *str);
ssize_t			nxs_string_strftime					(nxs_string_t *str, u_char *fmt, time_t t);
ssize_t			nxs_string_fmt_time_interval				(nxs_string_t *str, u_char *fmt, time_t t);

/*
 * Экранирование символов.
 *
 * Если указатель на строку str_from == NULL - будет произведено экранирование символов в строке str_to
 */
void			nxs_string_escape					(nxs_string_t *str_to, nxs_string_t *str_from, nxs_string_escape_types_t type);

/** @} */ // end of nxs-string
#endif /* _INCLUDE_NXS_STRING_H */
