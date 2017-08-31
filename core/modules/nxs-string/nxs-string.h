#ifndef _INCLUDE_NXS_STRING_H
#define _INCLUDE_NXS_STRING_H

// clang-format off

/** @addtogroup nxs-string
 *
 * @brief Module 'nxs-string' is a basic module providing functions to work with strings.
 *
 * The following terminology is used in the description of this module:
 *
 * \b String - a string declared as nxs_string_s.
 *
 * <b>Initialize a string</b> - zero-initialize a string by setting nxs_string_s#str to NULL and nxs_string_s#len and nxs_string_s#size to 0.
 *
 * <b>Create a string</b> - allocate memory for a string and fill it (if nessessary) with some data.
 *
 * <b>Clear a string</b> - clear a string by setting nxs_string_s#len to 0, nxs_string_s#str[0] to '\\0' and nxs_string_s#size is unchanged.
 *
 * Nxs-fw strings always end by character '\\0'.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

enum nxs_string_err_e
{
	NXS_STRING_E_OK,
	NXS_STRING_E_ERR,
	NXS_STRING_E_NOT_INIT,
	NXS_STRING_E_DST_SIZE,
	NXS_STRING_E_SRC_SIZE,
	NXS_STRING_E_DST_LEN,
	NXS_STRING_E_SRC_LEN,
	NXS_STRING_E_OFFSET,
	NXS_STRING_E_NOT_CREATED,
	NXS_STRING_E_NULL_PTR,
	NXS_STRING_E_SPEC_UNKNOWN,
	NXS_STRING_E_SPEC_OVERFLOW,
	NXS_STRING_E_UNKNOWN,
};

enum nxs_string_escape_types_e
{
	NXS_STRING_ESCAPE_TYPE_GENERIC,		/**<  GENERIC */
	NXS_STRING_ESCAPE_TYPE_JSON		/**<  JSON */
};

#define NXS_STRING_EMPTY_STR			(u_char *)""

#define NXS_STRING_NULL_STR			{NULL, 0, 0}

#define	nxs_string(str)				{(u_char *)str, sizeof(str) - 1, sizeof(str)}

struct nxs_string_s
{
	u_char	*str;				/**< u_char array pointer */
	size_t	len;				/**< The length of the string (max string length = #size - 1). Character '\\0' is always placed at #len position. */
	size_t	size;				/**< The amount of allocated memory for #str */
};

#define nxs_string_atoi(str) \
			atoi((char *)nxs_string_get_substr(str, 0))

#define nxs_string_atof(str) \
			atof((char *)nxs_string_get_substr(str, 0))

#define nxs_string_str(str) \
			nxs_string_get_substr(str, 0)

#define nxs_string_printf2_cat(str, msg...) \
			nxs_string_printf2(str, nxs_string_len(str), msg)

/**
 * @brief Allocates memory for a new string and fills it with data from an u_char array.
 *
 * * If the pointer to \b new_str is NULL and the \b size is 0 - a pointer to the allocated and zero-initialized memory for struct nxs_string_t will be returned.
 * * If the pointer to \b new_str is NULL and the \b size is greater than 0 - a pointer to a zero-length string with determined size will be returned
 *
 * * If pointer to \b new_str is not NULL  - a pointer to allocated, initialized and created string will be returned.
 * In this case nxs_string_create() will be called after the memory will be allocated for the new string.
 *
 * @param size Size of the new string.
 * @param new_str Pointer to the source sequence of characters.
 *
 * @return Pointer to the created string.
 */
nxs_string_t 			*nxs_string_malloc			(size_t size, u_char *new_str);

/**
 * @brief Destroys a string.
 *
 * Sets the nxs_string_s#size and nxs_string_s#len to 0, frees nxs_string_s#str and then frees the string pointed by \b str.
 *
 * @warning Use this function only if the string was created by nxs_string_malloc().
 *
 * @param str Pointer to the string to be destroyed.
 *
 * @return \b NULL.
 */
nxs_string_t			*nxs_string_destroy			(nxs_string_t *str);

/**
 * @brief Zero-initializes a string.
 *
 * By calling this function, nxs_string_s#str is set to NULL, nxs_string_s#size and nxs_string_s#len are set to 0.
 *
 * If the string is declared as "nxs_string_t *", make sure to allocate memory for it before calling this function.
 *
 * @warning This function must be called only before the first use of a \b str.
 *
 * Examples:
 * @code
 * ...
 *
 * nxs_string_t str1;
 *
 * nxs_string_init(&str1);
 *
 * ...
 * @endcode
 *
 * @code
 * ...
 *
 * nxs_string_t *str2;
 *
 * str2 = nxs_string_malloc(0, NULL);
 *
 * nxs_string_init(str2);
 *
 * ...
 *
 * @endcode
 *
 * @param str Pointer to the string to be initialized.
 *
 * @return none.
 */
void				nxs_string_init				(nxs_string_t *str);

/**
 * @brief Zero-initializes a string and fills it with data from an u_char array.
 *
 * If the string is declared as "nxs_string_t *", make sure to allocate memory for it before calling this function.
 *
 * The resultant string will be a copy of data in \b new_str
 *
 * This function is a conjunction of functions nxs_string_init() and nxs_string_create().
 *
 * @warning his function must be called only before the first use of a \b str.
 *
 * Examples:
 *
 * @code
 * void foo(u_char *src)
 * {
 * ...
 *
 *  nxs_string_t str1;
 *
 *  nxs_string_init2(&str1, 0, src);
 *
 * ...
 * }
 * @endcode
 *
 *@code
 * void foo(u_char *src)
 * {
 * ...
 *
 * nxs_string_t *str2;
 *
 * str2 = nxs_string_malloc(0, NULL);
 *
 * nxs_string_init2(str2, 0, src);
 *
 * ...
 * }
 * @endcode
 *
 * @param str Pointer to the string to be initialized.
 * @param size New size of \b str.
 * @param new_str Pointer to source u_char array.
 *
 * @return
 * * \b NXS_STRING_E_OK		- On succes.
 * * \b NXS_STRING_E_NULL_PTR	- If \b str is a null pointer.
 * * To see other possible error return values, view the returns of nxs_string_create().
 */
nxs_string_err_t		nxs_string_init2			(nxs_string_t *str, size_t size, u_char *new_str);

/**
 * @brief Zero-initializes a string and fills it with data from another string.
 *
 * If the string is declared as "nxs_string_t *", make sure to allocate memory for it before calling this function.
 *
 * The resultant string will be a copy of \b src.
 *
 * @warning This function must be called only before the first use of a \b str.
 *
 * Examples:
 *
 * @code
 * void foo(nxs_string_t *src)
 * {
 * ...
 *
 *  nxs_string_t str1;
 *
 *  nxs_string_init3(&str1, src);
 *
 * ...
 * }
 * @endcode
 *
 * @code
 * void foo(nxs_string_t *src)
 * {
 * ...
 *
 * nxs_string_t *str2;
 *
 * str2 = nxs_string_malloc(0, NULL);
 *
 * nxs_string_init3(str2, src);
 *
 * ...
 * }
 * @endcode
 *
 * @param str Pointer to the string to be initialized
 * @param src Pointer to the source string
 *
 * @return
 * * \b NXS_STRING_E_OK		- On succes.
 * * \b NXS_STRING_R_NULL_PTR 	- If either \b str or \src is a null pointer.
 */
nxs_string_err_t		nxs_string_init3			(nxs_string_t *str, nxs_string_t *src);

/**
 * @brief Zero-initializes a string and fills it with empty string.
 *
 * By calling this function, nxs_string_s#str is allocated memory only for '\0', nxs_string_s#size is set to 1
 * and nxs_string_s#len are set to 0.
 *
 * If the string is declared as "nxs_string_t *", make sure to allocate memory for it before calling this function.
 *
 * @warning This function must be called only before the first use of a \b str.
 *
 * @param str Pointer to the string to be initialized.
 *
 * @return
 * * \b NXS_STRING_E_OK		- On succes.
 * * \b NXS_STRING_E_NULL_PTR 	- If either \b str is a null pointer.
 */
nxs_string_err_t		nxs_string_init_empty			(nxs_string_t *str);

/**
 * @brief Creates a string from an u_char array.
 *
 * To prevent memory leaks \b str must be zero-initialized before calling this function.
 *
 * If \b str is not zero-initialized, \b NXS_STRING_E_NOT_INIT will be returned.
 *
 * If \b new_str != \b NULL and \b size == 0, the size of \b str will be counted automatically as the length of \b new_str + 1.
 *
 * If \b new_str == \b NULL and \b size != 0, a string with specified size and zero length will be created.
 *
 * If \b new_str == \b NULL and \b size == 0, the string will not be created.
 *
 * @warning This function must be called only before the first use of a \b str.
 *
 * @param str Pointer to the string.
 * @param size New size of \b str.
 * @param new_str Pointer to source u_char array.
 *
 * @return
 * * \b NXS_STRING_E_OK			- On succes.
 * * \b NXS_STRING_E_NOT_INIT 		- If \b str is not zero-initialized.
 * * \b NXS_STRING_E_NULL_PTR 		- If \b str is a null pointer.
 * * \b NXS_STRING_E_DST_SIZE 		- If \b new_str is larger than \b size.
 * * \b NXS_STRING_E_NOT_CREATED 	- If \b new_str == \b NULL and \b size == 0.
 */
nxs_string_err_t		nxs_string_create			(nxs_string_t *str, size_t size, u_char *new_str);

/**
 * @brief Deallocates memory for a string and zero-initializes it
 *
 * The memory for nxs_string_s#str will be deallocated, nxs_string_s#size and nxs_string_s#len will be set to 0.
 *
 * This function doesn't destroy \b str.
 *
 * If \b str was created by nxs_string_malloc(), use nxs_string_destroy() to free and destroy string.
 *
 * @param str Pointer to the string.
 *
 * @return
 * * \b NXS_STRING_E_OK			- On success.
 * * \b NXS_STRING_E_NULL_PTR 		- If \b str is a null pointer.
 * * \b NXS_STRING_E_NOT_CREATED 	- If string has already been freed or zero-initialized and not created yet.
 */
nxs_string_err_t		nxs_string_free				(nxs_string_t *str);

/**
 * @brief Resizes a string
 *
 * Changes the allocated memory amount for nxs_string_s#str.
 *
 * If \b new_size is 0 - nxs_string_free() will be called.
 *
 * If \b str is zero-initialized, but not created, nxs_string_create(str, new_size, NULL) will be called.
 *
 * @param str Pointer to the string.
 * @param new_size New size of the string.
 *
 * @return
 * * \b NXS_STRING_E_OK			- On success.
 * * \b NXS_STRING_E_NULL_PTR 		- If \b str is a null pointer.
 * * \b NXS_STRING_E_NOT_CREATED 	- If string already freed or initialized and not created yet
 */
nxs_string_err_t		nxs_string_resize			(nxs_string_t *str, size_t new_size);

/**
* @brief Clears the contents of a string.
*
* Clear a string by setting nxs_string_s#len to 0, nxs_string_s#str[0] to '\\0' and nxs_string_s#size is unchanged.
*
* @warning Does not free the allocated memory!
*
* @param str Pointer to the string.
*
* @return
* * \b NXS_STRING_E_OK 			- On success.
* * \b NXS_STRING_E_NULL_PTR 		- If \b str is a null pointer.
* * \b NXS_STRING_E_NOT_CREATED 	- If the \b str pointer is already clear, freed or created but not initialized yet
*/
nxs_string_err_t		nxs_string_clear			(nxs_string_t *str);

/**
 * @brief Prints formatted data to string \b str according to a format specified in \b msg.
 *
 * The string to write is pointed by \b msg and may include format specifiers (like printf() family functions).
 * If there are any format specifiers, the corresponding number of additional arguments must be included to replace
 * their respective specifiers.
 *
 * The size of \b str is increased dynamically by calling nxs_string_resize() function, if it's necessary.
 *
 * The supported format specifiers and their corresponding data types are listed in the table below:
 *
 * Specifier                  | Output
 * -------------------------- | ---------------
 * %[0][width]zu              | size_t
 * %[0][width]zd              | ssize_t
 * %[0][width]d               | int
 * %[0][width]u               | unsigned int
 * %[0][width]lu              | unsigned long int
 * %[0][width]ld              | unsigned long int
 * %[0][width]llu             | unsigned long long int
 * %[0][width]lld             | long long int
 * %[0][width]o               | oct
 * %[0][width]x               | hex
 * %[0][width][.precision]f   | double
 * %c                         | char
 * %s                         | char * (null-terminated string)
 * %r                         | nxs_string_t *
 * %R                         | nxs_buf_t *
 * %%                         | %
 *
 * @param str Pointer to the string.
 * @param msg String that contains the text to be written to \b str with optional format specifiers.
 * @param ... Additional arguments values to be used to replace format specifiers in the format string \b msg.
 *
 * @return
 * The new length of \b str.
 */
size_t				nxs_string_printf			(nxs_string_t *str, const char *msg, ...);

/**
 * @brief Prints formatted data to string \b str (with specified \b offset) according to a format specified in \b msg.
 *
 * The string to write is pointed by \b msg and may include format specifiers (like printf() family functions).
 * If there are any format specifiers, the corresponding number of additional arguments must be included to replace
 * their respective specifiers.
 *
 * The size of \b str is increased dynamically by calling nxs_string_resize() function, if it's necessary.
 *
 * The supported format specifiers and their corresponding data types are listed in the table below:
 *
 * Specifier                  | Output
 * -------------------------- | -------------
 * %[0][width]zu              | size_t
 * %[0][width]zd              | ssize_t
 * %[0][width]d               | int
 * %[0][width]u               | unsigned int
 * %[0][width]lu              | unsigned long int
 * %[0][width]ld              | unsigned long int
 * %[0][width]llu             | unsigned long long int
 * %[0][width]lld             | long long int
 * %[0][width]o               | oct
 * %[0][width]x               | hex
 * %[0][width][.precision]f   | double
 * %c                         | char
 * %s                         | char * (null-terminated string)
 * %r                         | nxs_string_t *
 * %R                         | nxs_buf_t *
 * %%                         | %
 *
 * @param str Pointer to the string.
 * @param offset Offset from the beginning of the string.
 * @param msg String that contains the text to be written to \b str with optional format specifiers.
 * @param ... Additional arguments values to be used to replace format specifiers in the format string \b msg.
 *
 * @return
 * The new length of \b str.
 */
size_t				nxs_string_printf2			(nxs_string_t *str, size_t offset, const char *msg, ...);

/**
 * @brief Prints formatted data to string \b str according to a format specified in \b msg.
 *
 * This function is equivalent to the function nxs_string_printf(), except that they are called with a va_list instead of a
 * variable number of arguments. These function do not call the va_end macro.
 *
 * The size of \b str is increased dynamically by calling nxs_string_resize() function, if it's necessary.
 *
 * @param str Pointer to the string.
 * @param msg String that contains the text to be written to \b str with optional format specifiers.
 * @param ap va_list to be used to replace format specifiers in the format string \b msg.
 *
 * @return
 * The new length of \b str.
 */
size_t				nxs_string_vprintf			(nxs_string_t *str, const char *fmt, va_list ap);

/**
 * @brief Clone the source string \b src to destination string \b dst,
 * dynamically increasing the size of the destination string \b dst if it's necessary.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize().
 *
 * @param dst Pointer to the destination string.
 * @param src Pointer to the source string.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR 	- If either \b dst or \b src is a null pointer.
 */
nxs_string_err_t		nxs_string_clone			(nxs_string_t *dst, nxs_string_t *src);

/**
 * @brief Copies the data from a source string \b src with offset \b offset_src to destination string \b dst with offset \b offset_dst.
 *
 * @param dst Pointer to the destination string.
 * @param offset_dst Offset from the beginning of \b dst.
 * @param src Pointer to the source string.
 * @param offset_src Offset from the beginning of \b src.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR 	- If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_E_OFFSET 	- If either \b offset_dst or \b offset_src is greater than the length of the correspondig string.
 * * \b NXS_STRING_E_DST_SIZE 	- If the result size of \b dst is greater than its possible size.
 */
nxs_string_err_t		nxs_string_cpy_static			(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src);

/**
 * @brief Copies the data from a source string \b src with offset \b offset_src to destination string \b dst with offset \b offset_dst,
 * dynamically increasing the size of the destination string \b dst if it's necessary.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize().
 *
 * @param dst Pointer to the destination string.
 * @param offset_dst Offset from the beginning of \b dst.
 * @param src Pointer to the source string.
 * @param offset_src Offset from the beginning of \b src.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR 	- If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_E_OFFSET 	- If either \b offset_dst or \b offset_src is greater than the length of the correspondig string.
 */
nxs_string_err_t		nxs_string_cpy				(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src);


/**
 * @brief Copies at most \b n of characters from a source string \b src with offset \b offset_src to destination
 * string \b dst with offset \b offset_dst.
 *
 * @param str Pointer to the destination string.
 * @param offset_dst Offset from the beginning of \b dst.
 * @param src Pointer to the source string.
 * @param offset_src Offset from the beginning of \b src.
 * @param n Number of characters to copy.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_OFFSET 	- If \b offset_dst is greater than the length of \b dst.
 * * \b NXS_STRING_E_NULL_PTR 	- If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_E_DST_SIZE 	- If the result size of \b dst is greater than its possible size.
 */
nxs_string_err_t		nxs_string_ncpy_static			(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src, size_t n);

/**
 * @brief Copies at most \b n of characters from a source string \b src with offset \b offset_src to destination
 * string \b dst with offset \b offset_dst, dynamically increasing the size of the destination string \b dst if it's necessary.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the destination string.
 * @param offset_dst Offset from the beginning of \b dst.
 * @param src Pointer to the source string.
 * @param offset_src Offset from the beginning of \b src.
 * @param n Number of characters to copy.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_OFFSET 	- If \b offset_dst is greater than the length of \b dst.
 * * \b NXS_STRING_E_NULL_PTR 	- If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_E_DST_SIZE 	- If the result size of \b dst is greater than its possible size.
 */
nxs_string_err_t		nxs_string_ncpy				(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src, size_t n);

/**
 * @brief Concatenates two strings.
 *
 * Appends the \b src string to the \b dst string.
 *
 * @param dst Pointer to the destination string.
 * @param src Pointer to the source string.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_DST_SIZE	- If the result size of \b dst is greater than its possible size.
 * * \b NXS_STRING_E_NULL_PTR	- If either \b dst or \b src is a null pointer.
 */
nxs_string_err_t		nxs_string_cat_static			(nxs_string_t *dst, nxs_string_t *src);

/**
 * @brief Concatenates two strings, dynamically increasing the size of the \b dst string if it's necessary.
 *
 * Appends the \b src string to the \b dst string.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize().
 *
 * @param dst Pointer to the destination string.
 * @param src Pointer to the source string.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR	- If either \b dst or \b src is a null pointer.
 */
nxs_string_err_t		nxs_string_cat				(nxs_string_t *dst, nxs_string_t *src);

/**
 * @brief Appends at most \b n characters from the \b src string to the \b dst string.
 *
 * @param dst Pointer to the first string.
 * @param src Pointer to the second string.
 * @param n Number of bytes from \b src to add to \b dst
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR	- If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_E_DST_SIZE	- If the result size of \b dst is greater than its possible size.
 */
nxs_string_err_t		nxs_string_ncat_static			(nxs_string_t *dst, nxs_string_t *src, size_t n);

/**
 * @brief Appends at most \b n characters from the \b src string to the \b dst string,
 * dynamically increasing the size of the string \b dst if it's necessary.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize();
 *
 * @param dst Pointer to the first string.
 * @param src Pointer to the second string.
 * @param n Number of bytes from \b src to add to \b dst
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR	- If either \b dst or \b src is a null pointer.
 */
nxs_string_err_t		nxs_string_ncat				(nxs_string_t *dst, nxs_string_t *src, size_t n);

/**
 * @brief Compares two strings with specified offsets.
 *
 * Compares \b str1 with \b offset1 to \b str2 with \b offset2
 *
 * The comparision is case-sensitive.
 *
 * @param str1 Pointer to the first string.
 * @param offset1 Offset from the beginning of \b str1
 * @param str2 Pointer to the second string.
 * @param offset2 Offset from the beginning of \b str2
 *
 * @return
 * * \b NXS_YES or NXS_TRUE 		- If the compared strings are equal.
 * * \b NXS_NO or NXS_FALSE 		- If the compared strings are not equal.
 * 						Or if \b offset1 or \b offset2 is greater than the length of the correspondig string.
 * 						Or if either \b str1 or \b str2 is a null pointer.
 */
nxs_bool_t			nxs_string_cmp				(nxs_string_t *str1, size_t offset1, nxs_string_t *str2, size_t offset2);

/**
 * @brief Compares \b n (at most) characters of two strings with specified offsets.
 *
 * Compares \b n characters of \b str1 with \b offset1 to \b n characters of \b str2 with \b offset2.
 *
 * The comparision is case-sensitive.
 *
 * @param str1 Pointer to the first string.
 * @param offset1 Offset from the beginning of \b str1
 * @param str2 Pointer to the second string.
 * @param offset2 Offset from the beginning of \b str2
 * @param n Number of bytes to compare
 *
 * @return
 * * \b NXS_YES or NXS_TRUE 		- If the compared strings are equal.
 * * \b NXS_NO or NXS_FALSE 		- If the compared strings are not equal.
 * 						Or if \b offset1 or \b offset2 is greater than the length of the correspondig string.
 * 						Or if either \b str1 or \b str2 is a null pointer.
 */
nxs_bool_t			nxs_string_ncmp				(nxs_string_t *str1, size_t offset, nxs_string_t *str2, size_t offset2, size_t n);

/**
 * @brief Copies the u_char array \b ch_str, excluding the terminating null byte ('\0'), to a string \b str with specified \b offset.
 *
 * @param str Pointer to the destination string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to source u_char array.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_OFFSET 	- If \b offset is greater than the length of the string.
 * * \b NXS_STRING_E_NULL_PTR 	- If either \b str or \b ch_str is a null pointer.
 * * \b NXS_STRING_E_DST_SIZE 	- If the result size of \b str is greater than its possible size.
 */
nxs_string_err_t		nxs_string_char_cpy_static		(nxs_string_t *str, size_t offset, u_char *ch_str);

/**
 * @brief Copies the u_char array \b ch_str, excluding the terminating null byte ('\0'), to a string \b str with specified \b offset,
 * dynamically increasing its size if it's necessary.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the destination string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to source u_char array.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_OFFSET 	- If \b offset is greater than the length of the string.
 * * \b NXS_STRING_E_NULL_PTR 	- If either \b str or \b ch_str is a null pointer.
 */
nxs_string_err_t		nxs_string_char_cpy			(nxs_string_t *str, size_t offset, u_char *ch_str);

/**
 * @brief Copies \b n first bytes of u_char array \b ch_str to a string \b str with specified \b offset.
 *
 * @param str Pointer to the destination string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to source u_char array.
 * @param n Number of bytes to copy.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_OFFSET 	- If \b offset is greater than the length of the string.
 * * \b NXS_STRING_E_NULL_PTR 	- If either \b str or \b ch_str is a null pointer.
 * * \b NXS_STRING_E_DST_SIZE	- If the result size of \b str is greater than its possible size.
 */
nxs_string_err_t		nxs_string_char_ncpy_static		(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n);

/**
 * @brief Copies \b n first bytes of u_char array \b ch_str to a string \b str with specified \b offset,
 * dynamically increasing its size if it's necessary.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the destination string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to source u_char array.
 * @param n Number of bytes to copy.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_OFFSET 	- If \b offset is greater than the length of \b str.
 * * \b NXS_STRING_E_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 */
nxs_string_err_t		nxs_string_char_ncpy			(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n);

/**
 * @brief Concatenates a string \b str and an u_char array \b ch_str terminated by null byte ('\0').
 *
 * @param str Pointer to the string.
 * @param ch_str Pointer to u_char array.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_DST_SIZE	- If the result size of \b str is greater than its possible size.
 * * \b NXS_STRING_E_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 */
nxs_string_err_t		nxs_string_char_cat_static		(nxs_string_t *str, u_char *ch_str);

/**
 * @brief Concatenates a string \b str and an u_char array \b ch_str terminated by null byte ('\0'),
 * dynamically increasing the size of the string \b str if it's necessary.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize();
 *
 * @param str Pointer to the string.
 * @param ch_str Pointer to u_char array.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 */
nxs_string_err_t		nxs_string_char_cat			(nxs_string_t *str, u_char *ch_str);

/**
 * @brief Concatenates a string \b str and first \b n bytes of u_char array \b ch_str.
 *
 * @param str Pointer to the string.
 * @param ch_str Pointer to u_char array.
 * @param n Number of bytes from \b ch_str to add to \b str
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_DST_SIZE	- If the result size of \b str is greater than its possible size.
 * * \b NXS_STRING_E_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 */
nxs_string_err_t		nxs_string_char_ncat_static		(nxs_string_t *str, u_char *ch_str, size_t n);

/**
 * @brief Concatenates a string \b str and first \b n bytes of u_char array \b ch_str,
 * dynamically increasing the size of the string \b str if it's necessary.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize();
 *
 * @param str Pointer to the string.
 * @param ch_str Pointer to u_char array.
 * @param n Number of bytes from \b ch_str to add to \b str
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 */
nxs_string_err_t		nxs_string_char_ncat			(nxs_string_t *str, u_char *ch_str, size_t n);

/**
 * @brief Compares a string \b str with a specified \b offset to an u_char array \b ch_str terminated by null byte ('\0').
 *
 * The comparision is case-sensitive.
 *
 * @param str Pointer to the string.
 * @param offset Offset from the beginning of \b str
 * @param ch_str Pointer to u_char array.
 *
 * @return
 * * \b NXS_YES or NXS_TRUE 		- If the compared strings are equal.
 * * \b NXS_NO or NXS_FALSE 		- If the compared strings are not equal.
 *						Or if \b offset is greater than the length of \b str.
 *						Or if either \b str or \b ch_str is a null pointer.
 */
nxs_bool_t			nxs_string_char_cmp			(nxs_string_t *str, size_t offset, u_char *ch_str);

/**
 * @brief Compares \b n bytes of a string \b str with a specified \b offset to an u_char array \b ch_str.
 *
 * The comparision is case-sensitive.
 *
 * @param str Pointer to the string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to u_char array.
 * @param n Number of bytes to compare to.
 *
 * @return
 * * \b NXS_YES or NXS_TRUE 		- If the compared strings are equal.
 * * \b NXS_NO or NXS_FALSE 		- If the compared strings are not equal.
 *						Or if \b offset is greater than the length of \b str.
 *						Or if either \b str or \b ch_str is a null pointer.
 */
nxs_bool_t			nxs_string_char_ncmp			(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n);

/**
 * @brief Adds a character \b c to a string \b str.
 *
 * @param str Pointer to the string.
 * @param c Character to be added.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_DST_SIZE	- If the result size of \b str is greater than its possible size.
 * * \b NXS_STRING_E_NULL_PTR	- If \b str is a null pointer.
 */
nxs_string_err_t		nxs_string_char_add_char_static		(nxs_string_t *str, u_char c);

/**
 * @brief Adds a character \b c to a string \b str, dynamically increasing the size of the string if it's necessary.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the string
 * @param c Character to be added.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_DST_SIZE	- If the result size of \b str is greater than its possible size.
 * * \b NXS_STRING_E_NULL_PTR	- If \b str is a null pointer.
 */
nxs_string_err_t		nxs_string_char_add_char		(nxs_string_t *str, u_char c);

/**
 * @brief Extracts the component of \b path following the final '/' to string \b dst.
 * Trailing '/' characters are not counted as part of the pathname.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize().
 *
 * The following list of examples shows the strings returned by nxs_string_basename() for different paths:
 *
 * path       | dst
 * -----------|-----
 * (Zero len) | .
 * /usr/lib   | lib
 * /usr/      | usr
 * usr        | usr
 * /          | /
 * .          | .
 * ..         | ..
 *
 * @param dst String to save the result.
 * @param path Path string.
 *
 * @return
 * * none.
 */
void				nxs_string_basename			(nxs_string_t *dst, nxs_string_t *path);

/**
 * @brief Extracts the string up to, but not including, the final '/'.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize().
 *
 * The following list of examples shows the strings returned by nxs_string_dirname() for different paths:
 *
 * path       | dst
 * -----------|-----
 * (Zero len) | .
 * /usr/lib   | /usr
 * /usr/      | /
 * usr        | .
 * /          | /
 * .          | .
 * ..         | .
 *
 * @param dst String to save the result.
 * @param path Path string.
 *
 * @return
 * * none.
 */
void				nxs_string_dirname			(nxs_string_t *dst, nxs_string_t *path);

/**
 * @brief Replaces a character \b c at the specified position \b pos of a string \b str.
 *
 * If the character \b c is being placed at the end of the string \b str, it's length will be increased by 1 within the available size.
 *
 * @param str Pointer to the string.
 * @param pos Position to set the character at.
 * @param c Character to be set.
 *
 * @return
 * * \b NXS_STRING_E_OK 		- On success.
 * * \b NXS_STRING_E_NULL_PTR		- If \b str is a null pointer.
 * * \b NXS_STRING_E_NOT_CREATED	- If \b str is zero-initialized.
 * * \b NXS_STRING_E_DST_LEN		- If \b pos is beyond the length of \b str.
 * * \b NXS_STRING_E_DST_SIZE		- If the result size of \b str is greater than its possible size.
 */
nxs_string_err_t		nxs_string_set_char			(nxs_string_t *str, size_t pos, u_char c);

/**
 * @brief Inserts the character \b c at the specified position \b pos of a string \b str.
 *
 * Shifts all subsequent characters (if there are any) to the right.
 *
 * The length of \b str will be increased by 1 within its available size.
 *
 * @param str Pointer to the string.
 * @param pos Position to insert the character at.
 *
 * @return
 * * \b NXS_STRING_E_OK 		- On success.
 * * \b NXS_STRING_E_NULL_PTR		- If \b str is a null pointer.
 * * \b NXS_STRING_E_NOT_CREATED	- If \b str is zero-initialized.
 * * \b NXS_STRING_E_DST_LEN		- If \b pos is beyond the length of \b str.
 * * \b NXS_STRING_E_DST_SIZE		- If the result size of \b str is greater than its possible size.
 */
nxs_string_err_t		nxs_string_ins_char_static		(nxs_string_t *str, size_t pos, u_char c);

/**
 * @brief Inserts a character \b c at the specified position \b pos of a string \b str,
 * dynamically increasing its size if it's necessary.
 *
 * Shifts all subsequent characters (if there are any) to the right.
 *
 * The length of \b str will be increased by 1 and if the result size of \b str is greater
 * than its possible size, \b str will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the string.
 * @param pos Position to insert the character at.
 *
 * @return
 * * \b NXS_STRING_E_OK 		- On success.
 * * \b NXS_STRING_E_NULL_PTR		- If \b str is a null pointer.
 * * \b NXS_STRING_E_NOT_CREATED	- If \b str is zero-initialized.
 * * \b NXS_STRING_E_DST_LEN		- If \b pos is beyond the length of \b str.
 */
nxs_string_err_t		nxs_string_ins_char			(nxs_string_t *str, size_t pos, u_char c);

/**
 * @brief Sets length for a string \b str as \b len.
 *
 * The '\0' character will be placed at \b len position.
 *
 * This function does not check if there are any other '\0' characters in \b str.
 *
 * @param str Pointer to the string.
 * @param len New length of the string.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR 	- If \b str is a null pointer.
 * * \b NXS_STRING_E_DST_SIZE	- If \len is greater than the possible size of \b str.
 */
nxs_string_err_t		nxs_string_set_len			(nxs_string_t *str, size_t len);

/**
 * @brief Places the '\0' character at the position corresponding to the length value of the string \b str.
 *
 * As data is sent as a buffer via network communication, the '\0' character is ignored.
 * This function is intended to provide availability to correctly use strings for network communication.
 *
 * @param str Pointer to the string.
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR 	- If \b str is a null pointer.
 * * \b NXS_STRING_E_DST_SIZE 	- If the size of \b str equals 0.
 */
nxs_string_err_t		nxs_string_len_fix			(nxs_string_t *str);

/**
 * @brief Returns the character at the specified position \b pos of a string \b str.
 *
 * @param str Pointer to the string.
 * @param pos Position of the character to get.
 *
 * @return
 * * On success, the character at \b pos.
 * * 0 - If \b str is a NULL pointer or \b pos is greater that its length.
 */
u_char				nxs_string_get_char			(nxs_string_t *str, size_t pos);

/**
 * @brief Returns a substring as u_char array from string \b str with \b offset.
 *
 * @param str Pointer to the string.
 * @param offset Beginning index.
 *
 * @return
 * * On success, a pointer to the substring.
 * * 0 - If \b str is a NULL pointer of \b offset is greater than its size.
 */
u_char				*nxs_string_get_substr			(nxs_string_t *str, size_t offset);

/**
 * @brief Finds the first occurence of a specified u_char array \b f_str with length \b f_str_len in a string \b str whith \b offset.
 *
 * @param str Pointer to the string to search in.
 * @param offset Offset the string \b str.
 * @param f_str Substring to search for.
 * @param f_str_len Length of the string to search for.
 *
 * @return
 * * On success, a pointer to the found substring.
 * * \b NULL:
 *  * If either \b str or \b f_str is a null pointer.
 *  * If \b f_str_len is greater than the length of \b str.
 *  * If \b offset is greater than the length of \b str.
 *  * If \b f_str was not found in \b str.
 */
u_char				*nxs_string_find_substr_first		(nxs_string_t *str, size_t offset, u_char *f_str, size_t f_str_len);

/**
 * @brief Finds the last occurence of a specified u_char array \b f_str with length \b f_str_len in a string \b str whith \b offset.
 *
 * @param str Pointer to the string to search in.
 * @param offset Offset the string \b str.
 * @param f_str String to search for.
 * @param f_str_len Length of the string to search for.
 *
 * @return
 * * On success, a pointer to the found substring.
 * * \b NULL:
 *  * If either \b str or \b f_str is a null pointer.
 *  * If \b f_str_len is greater than the length of \b str.
 *  * If \b offset is greater than the length of \b str.
 *  * If \b f_str was not found in \b str.
 */
u_char				*nxs_string_find_substr_last		(nxs_string_t *str, size_t offset, u_char *f_str, size_t f_str_len);

/**
 * @brief Finds the first occurence of a specified character \b c in a string \b str whith \b offset.
 *
 * @param str Pointer to the string to search in.
 * @param offset Offset the string \b str.
 * @param c Character to search for.
 *
 * @return
 * * On success, a pointer to the substring beginning with \b c.
 * * \b NULL:
 *  * If \b str is a null pointer.
 *  * If \b offset is greater than the length of \b str.
 *  * If \b c was not found in \b str.
 */
u_char				*nxs_string_find_char_first		(nxs_string_t *str, size_t offset, u_char c);

/**
 * @brief Finds the last occurence of a specified character \b c in a string \b str whith \b offset.
 *
 * @param str Pointer to the string to search in.
 * @param offset Offset the string \b str.
 * @param c Character to search for.
 *
 * @return
 * * On success, a pointer to the substring beginning with \b c.
 * * \b NULL:
 *  * If \b str is a null pointer.
 *  * If \b offset is greater than the length of \b str.
 *  * If \b c was not found in \b str.
 */
u_char				*nxs_string_find_char_last		(nxs_string_t *str, size_t offset, u_char c);

/**
 * @brief Replaces in string \b str substrings \b f_str to \b d_str \b max_count times and store result in \b dst.
 * If \b dst is a null pointer, the result will be stored in \b src.
 *
 * Set \b max_count to 0 to avoid maximum number of replacements restriction.
 *
 * @param src Pointer to the source string.
 * @param dst Pointer to the string to store the result.
 * @param f_str Pointer to the string to be replaced.
 * @param d_str Pointer to the replacement string.
 * @param max_count Maximum number of replacements
 *
 * @return
 * * On success, the number of made replacements
 * * 0 - If either \b src or \b f_str or \b d_str is a null pointer.
 */
size_t				nxs_string_subs				(nxs_string_t *src, nxs_string_t *dst, nxs_string_t *f_str, nxs_string_t *d_str, size_t max_count);

/**
 * @brief Converts a string \b str with \b offset into a buffer \b buf.
 *
 * If the result size of \b buf is greater than its possible size, \b buf will be resized by calling nxs_buf_resize()  function.
 *
 * @param str Pointer to the string.
 * @param offset Offset from the beginning of the string.
 * @param buf Pointer to the destination nxs_buf_s.
 *
 * @return
 * * On success, a pointer to the u_char array containing the string.
 * * \b NULL:
 *  * If either \b str or \b buf is a null pointer.
 *  * If \b offset is greater than the length of \b str.
 */
u_char				*nxs_string_to_buf			(nxs_string_t *str, size_t offset, nxs_buf_t *buf);

/** @brief Returns a pointer to a u_char string that describes the nxs-string module error code passed in the argument \b nxs_string_errno.
 *
* @param nxs_str_errno The nxs-string module error code
*
* @return A string message for the specified error code
*/
u_char				*nxs_string_strerror			(nxs_string_err_t nxs_string_errno);

/** @brief Returns the length of a string \b str.
*
* @param str A pointer to the string
*
* @return The length of the string.
*/
size_t				nxs_string_len				(nxs_string_t *str);

/**
* @brief Returns the size (amount of the allocated memory) of a string \b str.
*
* For an nxs-strings the available for usage space is always one byte less than its size, because last byte is the '\0'.
*
* @param str A pointer to the string
*
* @return The size of the string \b str.
*/
size_t				nxs_string_size				(nxs_string_t *str);

/**
* @brief Checks for available space in the string \b str.
*
* @param str Pointer to the string
*
* @return
* * \b NXS_YES or NXS_TRUE		- if there is available space (one or more bytes)
* * \b NXS_NO or NXS_FALSE		- if the string is full and there's no more space available
*/
nxs_bool_t			nxs_string_check_space			(nxs_string_t *str);

/**
 * @brief Formats unix time stamp \b t according to the strftime() format specification
 * (specified in \b fmt) and places the result in the string \b str.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the string.
 * @param fmt The strftime() format specification.
 * @param t Time value (unix time stamp).
 *
 * @return
 * * \b NXS_STRING_E_OK 	- On success.
 * * \b NXS_STRING_E_NULL_PTR	- If \b str is a null pointer.
 */
nxs_string_err_t		nxs_string_strftime			(nxs_string_t *str, u_char *fmt, time_t t);

/**
 * @brief Formats time \b t given in seconds according to the format specification \b fmt
 * (see description below) and writes it to the string \b str.
 *
 * The supported format specifiers:
 *
 * Specifier  | Output
 * ---------- | -------------
 * %d         | days
 * %h         | hours
 * %m         | minutes
 * %s         | seconds
 *
 * @param str Pointer to the string.
 * @param fmt String containing any combination of regular characters and special format specifiers.
 * @param t Time value represented in seconds.
 *
 * @return
 * * \b NXS_STRING_E_OK 		- On success.
 * * \b NXS_STRING_E_NULL_PTR 		- If \b str is a null pointer.
 * * \b NXS_STRING_E_SPEC_OVERFLOW	- If one or more specifiers in \b fmt used more than one times.
 * * \b NXS_STRING_E_SPEC_UNKNOWN	- if there is an unknown format specifier in \b fmt.
 */
nxs_string_err_t		nxs_string_fmt_time_interval		(nxs_string_t *str, u_char *fmt, time_t t);

/**
 * @brief Adds escape symbols to an metacharacters according to the \b type in the string \b str_from and
 * store result to string \b str_to.
 *
 * If \b str_from is a null pointer, the function will work with \b str_to.
 *
 * Available types:
 * * NXS_STRING_ESCAPE_TYPE_GENERIC
 * * NXS_STRING_ESCAPE_TYPE_JSON
 *
 * Character | Escapes in GENERIC type | Escapes in  JSON type
 * ----------|---------|------
 * \         | yes     | yes
 * \"        | yes     | yes
 * '         | yes     | no
 * /         | yes     | yes
 * \\b       | yes     | yes
 * \\f       | yes     | yes
 * \\n       | yes     | yes
 * \\r       | yes     | yes
 * \\t       | yes     | yes
 *
 * @param str_to Pointer to the result string.
 * @param str_from Pointer to the source string.
 * @param type Constant defining the type of the string. Can be either \b NXS_STRING_ESCAPE_TYPE_GENERIC or \b NXS_STRING_ESCAPE_TYPE_JSON.
 *
 * @return
 * * none
 */
void				nxs_string_escape			(nxs_string_t *str_to, nxs_string_t *str_from, nxs_string_escape_types_t type);

/** @} */ // end of nxs-string
#endif /* _INCLUDE_NXS_STRING_H */
