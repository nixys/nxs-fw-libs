#ifndef _INCLUDE_NXS_STRING_H
#define _INCLUDE_NXS_STRING_H

/** @addtogroup nxs-string
 *
 * @brief Module 'nxs-string' is a basic module providing functions to work with strings.
 *
 * The following terminology is used in the description of this module:
 *
 * \b String - a string declared as nxs_string_s.
 *
 * <b>Initialize a string</b> - zero-initialize a string by setting nxs_string_s#str to NULL and nxs_string_s#len and nxs_string_s#size to
 * 0.
 *
 * <b>Create a string</b> - allocate memory for a string and fill it with some data.
 *
 * Nxs-fw strings always end by character '\\0'.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_STRING_OK 0

#define NXS_STRING_ERROR -1
#define NXS_STRING_ERROR_NOT_INIT -2
#define NXS_STRING_ERROR_DST_SIZE -3
#define NXS_STRING_ERROR_SRC_SIZE -4
#define NXS_STRING_ERROR_DST_LEN -5
#define NXS_STRING_ERROR_SRC_LEN -6
#define NXS_STRING_ERROR_OFFSET -7
#define NXS_STRING_ERROR_NOT_CREATED -8
#define NXS_STRING_ERROR_NULL_PTR -9
#define NXS_STRING_ERROR_SPEC_UNKNOWN -10
#define NXS_STRING_ERROR_SPEC_OVERFLOW -11

#define NXS_STRING_CMP_EQ 0
#define NXS_STRING_CMP_NE 1

#define NXS_STRING_NOT_FULL 0
#define NXS_STRING_FULL 1

#define NXS_STRING_NO_OFFSET 0

#define NXS_STRING_EMPTY_STR (u_char *)""

#define nxs_string(str)                                     \
	{                                                   \
		(u_char *)str, sizeof(str) - 1, sizeof(str) \
	}

struct nxs_string_s
{
	u_char *str;  /**< u_char array pointer */
	size_t  len;  /**< The length of the string (max string length = #size - 1). Character '\\0' is always placed at #len position. */
	size_t  size; /**< The amount of allocated memory for #str */
};

enum nxs_string_escape_types_e
{
	NXS_STRING_ESCAPE_TYPE_GENERIC, /**<  GENERIC */
	NXS_STRING_ESCAPE_TYPE_JSON     /**<  JSON */
};

#define nxs_string_atoi(str) atoi((char *)nxs_string_get_substr(str, NXS_STRING_NO_OFFSET))

#define nxs_string_atof(str) atof((char *)nxs_string_get_substr(str, NXS_STRING_NO_OFFSET))

#define nxs_string_str(str) nxs_string_get_substr(str, NXS_STRING_NO_OFFSET)
/*
#define nxs_string_printf2_cat(str, msg...) \
                        nxs_string_printf2(str, nxs_string_len(str), msg)
*/
#define nxs_string_printf2_cat_dyn(str, msg...) nxs_string_printf2_dyn(str, nxs_string_len(str), msg)

/**
 * @brief Allocates memory for a new string and fill ut with data from an u_char array.
 *
 * * If the pointer to \b new_str is NULL and the \b size is 0 - a pointer to the allocated and zero-initialized memory for struct
 * nxs_string_t will be returned.
 * * If the pointer to \b new_str is NULL and the \b size is greater than 0 - a pointer to a zero-length string with determined size will be
 * returned
 *
 * * If pointer to \b new_str is not NULL  - a pointer to allocated, initialized and created string will be returned.
 * In this case nxs_string_create() will be called after the memory will be allocated for the new string.
 *
 * @param size Size of the new string.
 * @param new_str Pointer to the source sequence of characters.
 *
 * @return Pointer to the created string.
 */
nxs_string_t *nxs_string_malloc(size_t size, u_char *new_str);

/**
 * @brief Destroys a string.
 *
 * Sets the size and the length of the string to 0 and then frees the pointer to this string.
 * Use this function only if the string was created by nxs_string_malloc().
 *
 * @param str Pointer to the string to be destroyed.
 *
 * @return \b NULL.
 */
nxs_string_t *nxs_string_destroy(nxs_string_t *str);

/**
 * @brief Zero-initializes a string.
 *
 * By calling this function, nxs_string_s#str is set to NULL, nxs_string_s#size and nxs_string_s#len are set to 0.
 *
 * If the string is declared as "nxs_string_t *str", make sure to allocate memory for it before calling this function.
 *
 * This function must be called only before the first use of a \b str.
 *
 * Examples:
 * @code
 * ...
 *
 * nxs_string_t	str1;
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
 * nxs_string_init(&str2);
 *
 * ...
 *
 * @endcode
 *
 * @param str Pointer to the string to be initialized.
 *
 * @return none.
 */
void nxs_string_init(nxs_string_t *str);

/**
 * @brief Zero-initializes a string and fills it with data from an u_char array
 *
 * If the string is declared as "nxs_string_t *str", make sure to allocate memory for it before calling this function.
 *
 * This function must be called only before the first use of a \b str.
 *
 * The resultant string will be a copy of data in \b new_str
 *
 * This function is a conjunction of functions nxs_string_init() and nxs_string_create().
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
 * nxs_string_t	*str2;
 *
 * str2 = nxs_string_malloc(0, NULL);
 *
 * nxs_string_init2(&str2);
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
 * * On succes, the new length of the \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR if  \b str is a null pointer.
 * * To see other possible error return values, view the returns of nxs_string_create().
 */
ssize_t nxs_string_init2(nxs_string_t *str, size_t size, u_char *new_str);

/**
 * @brief Zero-initializes a string and fills it with data from another string
 *
 * If the string is declared as "nxs_string_t *str", make sure to allocate memory for it before calling this function.
 *
 * This function must be called only before the first use of a \b str.
 *
 * The resultant string will be a copy of \b src.
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
 *  nxs_string_init3(&str1, &src);
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
 * nxs_string_t	*str2;
 *
 * str2 = nxs_string_malloc(0, NULL);
 *
 * nxs_string_init3(&str1, &str2);
 *
 * ...
 * }
 * @endcode
 *
 * @param str Pointer to the string to be initialized
 * @param src Pointer to the source string
 *
 * @return
 * * On succes, the new length of the \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR if  either \b str or \src is a null pointer.
 * * To see other possible error return values, view the returns of nxs_string_create().
 */
ssize_t nxs_string_init3(nxs_string_t *str, nxs_string_t *src);

/**
 * @brief Creates a string from an u_char array.
 *
 * To prevent memory leaks \b str must be zero-initialized before calling this function.
 *
 * If \b str is not zero-initialized, \b NXS_STRING_ERROR_NOT_INIT will be returned.
 *
 * If \b new_str != \b NULL and \b size == 0, the size of \b str will be counted automatically as the length of \b new_str + 1.
 *
 * If \b new_str == \b NULL and \b size != 0, a string with specified size and zero length will be created.
 *
 * If \b new_str == \b NULL and \b size == 0, the string will not be created.
 *
 * @param str Pointer to the string.
 * @param size New size of \b str.
 * @param new_str Pointer to source u_char array.
 *
 * @return
 * * \b NXS_STRING_ERROR_NOT_INIT - If \b str is not zero-initialized.
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 * * \b NXS_STRING_ERROR_DST_SIZE - If \b new_str is larger than \b size.
 * * NXS_STRING_ERROR_NOT_CREATED - If \b new_str == \b NULL and \b size == 0.
 */
ssize_t nxs_string_create(nxs_string_t *str, size_t size, u_char *new_str);

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
 * * \b NXS_STRING_OK - on success.
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 * * \b NXS_STRING_ERROR_NOT_CREATED - If string has already been freed or zero-initialized and not created yet.
 */
int nxs_string_free(nxs_string_t *str);

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
 * * On success, the new size value of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 * * \b NXS_STRING_ERROR_NOT_CREATED - If string already freed or initialized and not created yet
 */
ssize_t nxs_string_resize(nxs_string_t *str, size_t new_size);

/**
* @brief Clears the contents of a string.
*
* @warning Does not free the allocated memory!
*
* @param str Pointer to the string.
*
* @return
* * \b NXS_STRING_OK - On success.
* * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
* * \b NXS_STRING_ERROR_NOT_CREATED - If the \b str pointer is already clear, freed or created but not initialized yet
*/
int nxs_string_clear(nxs_string_t *str);

/**
 * @brief Writes the string represented by a char array to a string.
 *
 * The string to write is pointed by \b msg and may include format specifiers (begin with %).
 * If there are any format specifiers, the corresponding number of additional arguments must be included to replace their respective
 * specifiers.
 *
 * The size of \b str is increased dynamically by calling nxs_string_resize() function, if it's necessary.
 *
 * Unlike the standard printf() function, the result is written not to stdout but to a string.
 *
 * The supported format specifiers and their corresponding data types are listed in the table below:
 *
 * Specifier  | Output
 * ------------- | -------------
 * %[0][width]zu  | size_t
 * %[0][width]zd  | ssize_t
 * %[0][width]d   | int
 * %[0][width]u   | unsigned int
 * %[0][width]lu   | unsigned long int
 * %[0][width]ld   |  unsigned long int
 * %[0][width]llu   | unsigned long long int
 * %[0][width]lld   | long long int
 * %[0][width]o   | oct
 * %[0][width]x   | hex
 * %[0][width][.precision]f   | double
 * %c   | char
 * %s   | char * (null-terminated string)
 * %r   | nxs_string_t *
 * %R   | nxs_buf_t *
 * %%   | %
 *
 * @param str Pointer to the string.
 * @param msg String that contains the text to be written to \b str with optional format specifiers.
 * @param additional arguments Values to be used to replace format specifiers in the format string \b msg.
 *
 * @return
 * * On success, the new length of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 */
ssize_t nxs_string_printf_dyn(nxs_string_t *str, const char *msg, ...);

/**
 * @brief Writes the string represented by a char array to a string, beginning from a specified position.
 *
 * The data will be written starting from the position specified by \b offset.
 *
 * The string to write is pointed by \b msg and may include format specifiers (begin with %).
 * If there are any format specifiers, the corresponding number of additional arguments must be included to replace their respective
 * specifiers.
 *
 * The size of \b str is increased dynamically by calling nxs_string_resize() function, if it's necessary.
 *
 * Unlike the standard printf() function, the result is written not to stdout but to a string.
 *
 * The supported format specifiers and their corresponding data types are listed in the table below:
 *
 * Specifier  | Output
 * ------------- | -------------
 * %[0][width]zu  | size_t
 * %[0][width]zd  | ssize_t
 * %[0][width]d   | int
 * %[0][width]u   | unsigned int
 * %[0][width]lu   | unsigned long int
 * %[0][width]ld   |  unsigned long int
 * %[0][width]llu   | unsigned long long int
 * %[0][width]lld   | long long int
 * %[0][width]o   | oct
 * %[0][width]x   | hex
 * %[0][width][.precision]f   | double
 * %c   | char
 * %s   | char * (null-terminated string)
 * %r   | nxs_string_t *
 * %R   | nxs_buf_t *
 * %%   | %
 *
 * @param str Pointer to the string.
 * @param offset Offset from the beginning of the string.
 * @param msg String that contains the text to be written to \b str with optional format specifiers.
 * @param additional arguments Values to be used to replace format specifiers in the format string \b msg.
 *
 * @return
 * * On success, the new length of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 */
ssize_t nxs_string_printf2_dyn(nxs_string_t *str, size_t offset, const char *msg, ...);

/**
 * @brief Writes the string represented by a char array to a string, beginning from a specified position.
 *
 * The data will be written starting from the position specified by \b offset.
 *
 * The string to write is pointed by \b msg and may include format specifiers (begin with %).
 * If there are any format specifiers, the corresponding number of additional arguments must be included to replace their respective
 * specifiers.
 *
 * This function accepts additional arguments as va_list only.
 *
 * The size of \b str is increased dynamically by calling nxs_string_resize() function, if it's necessary.
 *
 * Unlike the standard printf() function, the result is written not to stdout but to a string.
 *
 * The supported format specifiers and their corresponding data types are listed in the table below:
 *
 * Specifier  | Output
 * ------------- | -------------
 * %[0][width]zu  | size_t
 * %[0][width]zd  | ssize_t
 * %[0][width]d   | int
 * %[0][width]u   | unsigned int
 * %[0][width]lu   | unsigned long int
 * %[0][width]ld   |  unsigned long int
 * %[0][width]llu   | unsigned long long int
 * %[0][width]lld   | long long int
 * %[0][width]o   | oct
 * %[0][width]x   | hex
 * %[0][width][.precision]f   | double
 * %c   | char
 * %s   | char * (null-terminated string)
 * %r   | nxs_string_t *
 * %R   | nxs_buf_t *
 * %%   | %
 *
 * @param str Pointer to the string.
 * @param offset Offset from the beginning of the string.
 * @param msg String that contains the text to be written to \b str with optional format specifiers.
 * @param additional arguments Values to be used to replace format specifiers in the format string \b msg.
 *
 * @return
 * * On success, the new length of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 */
ssize_t nxs_string_vprintf_dyn(nxs_string_t *str, const char *fmt, va_list ap);

/*
ssize_t			nxs_string_printf					(nxs_string_t *str, const char *msg, ...);
ssize_t			nxs_string_printf2					(nxs_string_t *str, size_t offset, const char *msg, ...);
ssize_t			nxs_string_vprintf					(nxs_string_t *str, const char *fmt, va_list ap);
*/

/**
 * @brief Copies the data from a source string beginning from a specified index to destination string beginning from a specified index,
 * dynamically increasing the size of the destination string if it's necessary.
 *
 * Copies string \b src beginning from  \b offset_src index to string \b dst beginning from \b offset_dst index.
 *
 * @param dst Pointer to the destination string.
 * @param offset_dst Offset from the beginning of \b dst.
 * @param src Pointer to the source string.
 * @param offset_src Offset from the beginning of \b src.
 *
 * @return
 * * On success, the new length value of \b dst.
 * * \b NXS_STRING_ERROR_NULL_PTR - If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_ERROR_OFFSET - If either \b offset_dst or \b offset_src is greater than the length of the correspondig string.
 * * \b NXS_STRING_ERROR_DST_SIZE - If the result size of \b dst is greater than its possible size.
 */
ssize_t nxs_string_cpy(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src);

/**
 * @brief Copies the data from a source string beginning from a specified index to destination string beginning from a specified index,
 * dynamically increasing the size of the destination string if it's necessary.
 *
 * Copies string \b src beginning from  \b offset_src index to string \b dst beginning from \b offset_dst index.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize().
 *
 * @param dst Pointer to the destination string.
 * @param offset_dst Offset from the beginning of \b dst.
 * @param src Pointer to the source string.
 * @param offset_src Offset from the beginning of \b src.
 *
 * @return
 * * On success, the new length value of \b dst.
 * * \b NXS_STRING_ERROR_NULL_PTR - If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_ERROR_OFFSET - If either \b offset_dst or \b offset_src is greater than the length of the correspondig string.
 */
ssize_t nxs_string_cpy_dyn(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src);

/**
 * @brief Copies the values of a specified number of characters from the source string, beginning from a specified index, to the destination
 * string, beginning from a specified index.
 *
 * Copies the values of \b n bytes of \b src, beginning from \b offset_src index to \b dst beginning from \b offset_dst index.
 *
 * @param str Pointer to the destination string.
 * @param offset_dst Offset from the beginning of \b dst.
 * @param src Pointer to the source string.
 * @param offset_src Offset from the beginning of \b src.
 * @param n Number of characters to copy.
 *
 * @return
 * * On success, the new length value of \b dst.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset_dst is greater than the length of \b dst.
 * * \b NXS_STRING_ERROR_NULL_PTR - If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_ERROR_DST_SIZE - If the result size of \b dst is greater than its possible size.
 * * \b NXS_STRING_ERROR_SRC_SIZE - If \b n starting from \b offset_src is more than its size.
 */
ssize_t nxs_string_ncpy(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src, size_t n);

/**
 * @brief Copies the values of a specified number of characters from the source string with specified offset to the destination string with
 * specified offset, dynamically increasing the size of the destination string if it's necessary..
 *
 * Copies the values of \b n bytes of \b src, beginning from \b offset_src index to \b dst beginning from \b offset_dst index.
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
 * * On success, the new length value of \b dst.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset_dst is greater than the length of \b dst.
 * * \b NXS_STRING_ERROR_NULL_PTR - If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_ERROR_DST_SIZE - If the result size of \b dst is greater than its possible size.
 * * \b NXS_STRING_ERROR_SRC_SIZE - If \b n starting from \b offset_src is more than its size.
 * @version This function is available since v0.2-0 r13
 */
ssize_t nxs_string_ncpy_dyn(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src, size_t n);

/**
 * @brief Concatenates two strings.
 *
 * Concatenates \b dst and \b src.
 *
 * The result is saved to \b dst.
 *
 * @param dst Pointer to the first string.
 * @param src Pointer to the second string.
 *
 * @return
 * * On success, the new length value of \b dst.
 * * \b NXS_STRING_ERROR_DST_SIZE	- If the result size of \b dst is greater than its possible size.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b dst or \b src is a null pointer.
 */
ssize_t nxs_string_cat(nxs_string_t *dst, nxs_string_t *src);

/**
 * @brief Concatenates two strings, dynamically increasing the size of the result string if it's necessary.
 *
 * Concatenates \b dst and \b src.
 *
 * The result is saved to \b dst.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize().
 *
 * @param dst Pointer to the first string.
 * @param src Pointer to the second string.
 *
 * @return
 * * On success, the new length value of \b dst.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b dst or \b src is a null pointer.
 * @version This function is available since v0.2-0 r13
 */
ssize_t nxs_string_cat_dyn(nxs_string_t *dst, nxs_string_t *src);

/**
 * @brief Adds a specified number of bytes from the beginning of a source string to the end of a destination string.
 *
 * Concatenates \b dst and first \b n bytes of \b src.
 *
 * @param dst Pointer to the first string.
 * @param src Pointer to the second string.
 * @param n Number of bytes from \b src to add to \b dst
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b dst or \b src is a null pointer.
 * * \b NXS_STRING_ERROR_DST_SIZE	- If the result size of \b dst is greater than its possible size.
 */
ssize_t nxs_string_ncat(nxs_string_t *dst, nxs_string_t *src, size_t n);

/**
 * @brief Adds a specified number of bytes from the beginning of one string to the end of another, dynamically increasing the size of the
 * result string if it's necessary.
 *
 * Concatenates \b dst and first \b n bytes of \b src, dynamically increasing the size of \b dst if it's necessary.
 *
 * If the result size of \b dst is greater than its possible size, \b dst will be resized by calling nxs_string_resize();
 *
 * @param dst Pointer to the first string.
 * @param src Pointer to the second string.
 * @param n Number of bytes from \b src to add to \b dst
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b dst or \b src is a null pointer.
 * @version This function is available since v0.2-0 r13
 */
ssize_t nxs_string_ncat_dyn(nxs_string_t *dst, nxs_string_t *src, size_t n);

/**
 * @brief Compares one string with a specified offset to another string with specified offset.
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
 * * \b NXS_STRING_CMP_EQ - If the compared strings are equal.
 * * \b NXS_STRING_CMP_NE - If the compared strings are not equal.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset1 or \b offset2 is greater than the length of the correspondig string.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b str1 or \b str2 is a null pointer.
 */
int nxs_string_cmp(nxs_string_t *str1, size_t offset1, nxs_string_t *str2, size_t offset2);

/**
 * @brief Compares a specified number of bytes of one string beginning from specified index to the equal number of bytes of another string,
 * beginning from a specified index.
 *
 * Compares \b n first bytes of \b str1 with \b offset1 to \b n first bytes of \b str2 with \b offset2.
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
 * * \b NXS_STRING_CMP_EQ - If the compared strings are equal.
 * * \b NXS_STRING_CMP_NE - If the compared strings are not equal.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset1 or \b offset2 is greater than the length of the correspondig string.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b str1 or \b str2 is a null pointer.
 */
int nxs_string_ncmp(nxs_string_t *str1, size_t offset, nxs_string_t *str2, size_t offset2, size_t n);

/**
 * @brief Copies the values of all bytes from an u_char array to a string.
 *
 * Copies the values of all bytes from \b ch_str array to \b str with an \b offset in \b str.
 *
 * @param str Pointer to the destination string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to source u_char array.
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset is greater than the length of the string.
 * * \b NXS_STRING_ERROR_NULL_PTR - If either \b str or \b ch_str is a null pointer.
 * * \b NXS_STRING_ERROR_DST_SIZE - If the result size of \b str is greater than its possible size.
 */
ssize_t nxs_string_char_cpy(nxs_string_t *str, size_t offset, u_char *ch_str);

/**
 * @brief Copies the values of all bytes from an u_char array to a string, dynamically increasing its size if it's necessary..
 *
 * Copies the values of all bytes from \b ch_str array to \b str with an \b offset in \b str.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the destination string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to source u_char array.
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset is greater than the length of the string.
 * * \b NXS_STRING_ERROR_NULL_PTR - If either \b str or \b ch_str is a null pointer.
 */
ssize_t nxs_string_char_cpy_dyn(nxs_string_t *str, size_t offset, u_char *ch_str);

/**
 * @brief Copies the values of a specified number of bytes from an u_char array to a string.
 *
 * Copies the values of first \b n bytes of \b ch_str array to \b str beginning from the index specified by \b offset.
 *
 * @param str Pointer to the destination string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to source u_char array.
 * @param n Number of bytes to copy.
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset is greater than the length of the string.
 * * \b NXS_STRING_ERROR_NULL_PTR - If either \b str or \b ch_str is a null pointer.
 * * \b NXS_STRING_ERROR_DST_SIZE - If the result size of \b str is greater than its possible size.
 */
ssize_t nxs_string_char_ncpy(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n);

/**
 * @brief Copies the values of a specified number of bytes from an u_char array to a string, dynamically increasing its size if it's
 * necessary.
 *
 * Copies the values of first \b n bytes of \b ch_str array to \b str beginning from the index specified by \b offset.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the destination string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to source u_char array.
 * @param n Number of bytes to copy.
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset is greater than the length of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR - If either \b str or \b ch_str is a null pointer.
 *
 * @version This function is available since v0.2-0 r13
 */
ssize_t nxs_string_char_ncpy_dyn(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n);

/**
 * @brief Concatenates a string and an u_char array.
 *
 * Concatenates \b str and u_char array \b ch_str.
 *
 * @param str Pointer to the string.
 * @param ch_str Pointer to u_char array.
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_DST_SIZE	- If the result size of \b str is greater than its possible size.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 */
ssize_t nxs_string_char_cat(nxs_string_t *str, u_char *ch_str);

/**
 * @brief Concatenates a string and an u_char array, dynamically increasing the size of the result string if it's necessary.
 *
 * Concatenates \b str and u_char array \b ch_str, dynamically increasing the size of \b str if it's necessary.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize();
 *
 * @param str Pointer to the string.
 * @param ch_str Pointer to u_char array.
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 *
 * @version This function is available since v0.2-0 r13
 */
ssize_t nxs_string_char_cat_dyn(nxs_string_t *str, u_char *ch_str);

/**
 * @brief Concatenates a string and first n bytes of an u_char array.
 *
 * Concatenates \b str and first \b n bytes of \b ch_str
 *
 * @param str Pointer to the string.
 * @param ch_str Pointer to u_char array.
 * @param n Number of bytes from \b ch_str to add to \b str
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_DST_SIZE	- If the result size of \b str is greater than its possible size.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 */
ssize_t nxs_string_char_ncat(nxs_string_t *str, u_char *ch_str, size_t n);

/**
 * @brief Concatenates a string and first n bytes of u_char array, dynamically increasing the size of the result string if it's necessary.
 *
 * Concatenates \b str and first \b n bytes of \b ch_str, dynamically increasing the size of \b str if it's necessary.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize();
 *
 * @param str Pointer to the string.
 * @param ch_str Pointer to u_char array.
 * @param n Number of bytes from \b ch_str to add to \b str
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 *
 * @version This function is available since v0.2-0 r13
 */
ssize_t nxs_string_char_ncat_dyn(nxs_string_t *str, u_char *ch_str, size_t n);

/**
 * @brief Compares a string with a specified offset to an u_char array.
 *
 * Compares \b str beginning from the index specified by \b offset to \b ch_str
 *
 * The comparision is case-sensitive.
 *
 * @param str Pointer to the string.
 * @param offset Offset from the beginning of \b str
 * @param ch_str Pointer to u_char array.
 *
 * @return
 * * \b NXS_STRING_CMP_EQ - If the compared strings are equal.
 * * \b NXS_STRING_CMP_NE - If the compared strings are not equal.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset is greater than the length of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 */
int nxs_string_char_cmp(nxs_string_t *str, size_t offset, u_char *ch_str);

/**
 * @brief Compares a string with a specified offset to a specified number of bytes of an u_char array.
 *
 * Compares \b str beginning from the index specified by \b offset to first \b n bytes of \b ch_str.
 *
 * The comparision is case-sensitive.
 *
 * @param str Pointer to the string.
 * @param offset Offset from the beginning of \b str.
 * @param ch_str Pointer to u_char array.
 * @param n Number of bytes from \b ch_str to compare to.
 *
 * @return
 * * \b NXS_STRING_CMP_EQ - If the compared strings are equal.
 * * \b NXS_STRING_CMP_NE - If the compared strings are not equal.
 * * \b NXS_STRING_ERROR_OFFSET - If \b offset is greater than the length of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If either \b str or \b ch_str is a null pointer.
 *
 * @version This function was changed in v0.2-0 r13
 */
int nxs_string_char_ncmp(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n);

/**
 * @brief Adds a character to a string.
 *
 * @param str Pointer to the string.
 * @param c Character to be added.
 *
 * @return
 * * \b NXS_STRING_ERROR_DST_SIZE	- If the result size of \b str is greater than its possible size.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If \b str is a null pointer.
 */
ssize_t nxs_string_char_add_char(nxs_string_t *str, u_char c);

/**
 * @brief Adds a character to a string, dynamically increasing the size of the string if it's necessary.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the string
 * @param c Character to be added.
 *
 * @return
 * * \b NXS_STRING_ERROR_DST_SIZE	- If the result size of \b str is greater than its possible size.
 * * \b NXS_STRING_ERROR_NULL_PTR	- If \b str is a null pointer.
 *
 * @version This function is available since v0.2-0 r13
 */
ssize_t nxs_string_char_add_char_dyn(nxs_string_t *str, u_char c);

/**
 * @brief Ejects the filename from a path.
 *
 * Strips directory and suffix from \b path and puts the filename to \b dst.
 *
 * @param dst String to save the result filename.
 * @param path Path to file.
 *
 * @return
 * * none.
 */
void nxs_string_basename(nxs_string_t *dst, nxs_string_t *path);

/**
 * @brief Ejects the parent directory name from a path.
 *
 * Strips non-directory suffix from \b path and puts the parent directory name to \b dst.
 *
 * @param dst String to save the result directory name.
 * @param path Path to file.
 *
 * @return
 * * none.
 */
void nxs_string_dirname(nxs_string_t *dst, nxs_string_t *path);

/**
 * @brief Replaces a character at specified position of a string with a new character.
 *
 * If a character is being placed at the end of the string, it's length will be increased by 1  within the available size.
 *
 * @param str Pointer to the string.
 * @param pos Position to set the character at.
 * @param c Character to be set.
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 * * \b NXS_STRING_ERROR_NOT_CREATED - If \b str is zero-initialized.
 * * \b NXS_STRING_ERROR_DST_LEN - If \b pos is beyond the length of \b str.
 * * \b NXS_STRING_ERROR_DST_SIZE	- If the result size of \b str is greater than its possible size.
 */
ssize_t nxs_string_set_char(nxs_string_t *str, size_t pos, u_char c);

/**
 * @brief Inserts a character at the specified position of a string.
 *
 * Shifts all subsequent characters (if there are any) to the right.
 *
 * The length of \b str will be increased by 1 within its available size.
 *
 * @param str Pointer to the string.
 * @param pos Position to insert the character at.
 *
 * @return
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 * * \b NXS_STRING_ERROR_NOT_CREATED - If \b str is zero-initialized.
 * * \b NXS_STRING_ERROR_DST_LEN - If \b pos is beyond the length of \b str.
 * * \b NXS_STRING_ERROR_DST_SIZE	- If the result size of \b str is greater than its possible size.
 */
ssize_t nxs_string_ins_char(nxs_string_t *str, size_t pos, u_char c);

/**
 * @brief Inserts a character at the specified position of a string, dynamically increasing its size if it's necessary.
 *
 * Shifts all subsequent characters (if there are any) to the right.
 *
 * The length of \b str will be increased by 1 within its available size.
 *
 * If the result size of \b str is greater than its possible size, \b str will be resized by calling nxs_string_resize().
 *
 * @param str Pointer to the string.
 * @param pos Position to insert the character at.
 *
 * @return
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 * * \b NXS_STRING_ERROR_NOT_CREATED - If \b str is zero-initialized.
 * * \b NXS_STRING_ERROR_DST_LEN - If \b pos is beyond the length of \b str.
 */
ssize_t nxs_string_ins_char_dyn(nxs_string_t *str, size_t pos, u_char c);

/**
 * @brief Sets a new length for a string.
 *
 * The '\0' character will be placed at \b len position.
 *
 * This function does not check if there are any other '\0' characters in \b str.
 *
 * @param str Pointer to the string.
 * @param len New length of the string.
 *
 * @return
 * * On success, the new length value of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 * * \b NXS_STRING_ERROR_DST_SIZE	- If \len is greater than the possible size of \b str.
 *
 * @version This function is available since v0.2-0 r13
 */
ssize_t nxs_string_set_len(nxs_string_t *str, size_t len);

/**
 * @brief Places the '\0' character at the position corresponding to the length value of the string.
 *
 * As data is sent as a buffer via network communication, the '\0' character is ignored.
 * This function is intended to provide availability to correctly use strings for network communication.
 *
 * @param str Pointer to the string.
 *
 * @return
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 * * \b NXS_STRING_ERROR_DST_SIZE - If the size of \b str equals 0.
 */
ssize_t nxs_string_len_fix(nxs_string_t *str);

/**
 * @brief Returns the character at the specified position of a string.
 *
 * @param str Pointer to the string.
 * @pos Position of the character to get.
 *
 * @return
 * * On success, the character at \b pos.
 * * 0 - If \b str is a NULL pointer or \b pos is greater that its length.
 */
u_char nxs_string_get_char(nxs_string_t *str, size_t pos);

/**
 * @brief Returns a new u_char array which is a substring of specified string.
 *
 * The substring begins with the character at \offset and extends to the end of \b str.
 *
 * @param str Pointer to the string.
 * @param offset Beginning index.
 *
 * @return
 * * On success, a pointer to the substring.
 * * 0 - If \b str is a NULL pointer of \b offset is greater than its size.
 *
 * @version This function was changed in v0.2-0 r13
 */
u_char *nxs_string_get_substr(nxs_string_t *str, size_t offset);

/**
 * @brief Searches through a string for the first occurence of a specified substring.
 *
 * Searches through \b str, beginning from the index specified by \b offset, for the first occurence of the substring pointed by \b f_str
 * with a length, specified by \b f_str_len.
 *
 * @param str Pointer to the string to search in.
 * @param offset Offset for the source string.
 * @param f_str String to search for.
 * @param f_str_len Length of the string to search for.
 *
 * @return
 * * On success, a pointer to the found substring.
 * * \b NULL
 *  * If either \b str or \b f_str is a null pointer.
 *  * If \b f_str_len is greater than the length of \b str.
 *  * If \b offset is greater than the length of \b str.
 *  * If \b f_str was not found in \b str.
 */
u_char *nxs_string_find_substr_first(nxs_string_t *str, size_t offset, u_char *f_str, size_t f_str_len);

/**
 * @brief Searches through a string for the last occurence of a specified substring.
 *
 * Searches through \b str, beginning from the index specified by \b offset, for the last occurence of the substring pointed  by \b f_str
 * with a length, specified by \b f_str_len.
 *
 * @param str Pointer to the string to search in.
 * @param offset Offset for the source string.
 * @param f_str String to search for.
 * @param f_str_len Length of the string to search for.
 *
 * @return
 * * On success, a pointer to the found substring.
 * * \b NULL
 *  * If either \b str or \b f_str is a null pointer.
 *  * If \b f_str_len is greater than the length of \b str.
 *  * If \b offset is greater than the length of \b str.
 *  * If \b f_str was not found in \b str.
 */
u_char *nxs_string_find_substr_last(nxs_string_t *str, size_t offset, u_char *f_str, size_t f_str_len);

/**
 * @brief Searches through a string for the first occurence of a specified character.
 *
 * Searches through \b str, beginning from the index specified by \b offset, for the first occurence of the character specified by \b c.
 *
 * @param str Pointer to the string to search in.
 * @param offset Offset for the source string.
 * @param c Character to search for.
 *
 * @return
 * * On success, a pointer to the substring beginning with \b c.
 * * \b NULL
 *  * If \b str is a null pointer.
 *  * If \b offset is greater than the length of \b str.
 *  * If \b c was not found in \b str.
 */
u_char *nxs_string_find_char_first(nxs_string_t *str, size_t offset, u_char c);

/**
 * @brief Searches through a string for the last occurence of a specified character.
 *
 * Searches through \b str, beginning from the index specified by \b offset, for the last occurence of the character specified by \b c.
 *
 * @param str Pointer to the string to search in.
 * @param offset Offset for the source string.
 * @param c Character to search for.
 *
 * @return
 * * On success, a pointer to the substring beginning with \b c.
 * * \bNULL
 *  * If \b str is a null pointer.
 *  * If \b offset is greater than the length of \b str.
 *  * If \b c was not found in \b str.
 */
u_char *nxs_string_find_char_last(nxs_string_t *str, size_t offset, u_char c);

/**
 * @brief Replaces each substring of the specified string that matches the given substring with the given replacement.
 *
 *  Replaces each occurence of \b f_str by \b d_str in \b src for \b max_count times and saves the result to \b dst.
 *
 *  If \b dst is a null pointer, the result will be stored in \b src.
 *
 *  The maximum number of replacements is defined by \b max_count. To remove the restriction, set \b max_count to 0.
 *
 * @param src Pointer to the source string.
 * @param dst Pointer to the string to store the result.
 * @param f_str Pointer to the string to be replaced.
 * @param d_str Pointer to the replacement string.
 * @max_count Maximum number of replacements
 *
 * @return
 * * On success, the number of made replacements
 * * 0 - If either \b src or \b f_str or \b d_str is a null pointer.
 */
size_t nxs_string_subs(nxs_string_t *src, nxs_string_t *dst, nxs_string_t *f_str, nxs_string_t *d_str, size_t max_count);

/**
 * @brief Converts a string into a buffer.
 *
 * Converts a string \b str, beginning from the index specified by \b offset, to a buffer \b buff.
 *
 * If the size of \b buf is less than the size of \b str, the size of \b buf will be dynamically increased by calling nxs_buf_resize()
 * function.
 *
 * @param str Pointer to the string.
 * @param offset Offset from the beginning of the string.
 * @param buf Pointer to the destination nxs_buf_s.
 *
 * @return
 * * On success, a pointer to the u_char array containing the string.
 * * \b NULL
 *  * If either \b str or \b buf is a null pointer.
 *  * If \b offset is greater than the length of \b str.
 *
 * * @version This function is available since v0.2-0 r13
 */
u_char *nxs_string_to_buf(nxs_string_t *str, size_t offset, nxs_buf_t *buf);

/** @brief Get error message by its code
 *
* Returns the detailed message for the error constant or its numeric code
*
* @param nxs_str_errno The constant which defines the error or an integer corresponding to an error constant
*
* @return A string message for the specified error
*/
u_char *nxs_string_strerror(int nxs_str_errno);

/** @brief The length of a string
*
* Returns the length of a string.
*
* The length is equal to the number of characters in the string.
*
* @param str A pointer to the string
*
* @return The length of the string.
*/
size_t nxs_string_len(nxs_string_t *str);

/**
* @brief The size of a string
*
* Returns the amount of the allocated memory for a string.
*
* The return value is 1 byte more it is than available for usage as there is the '\\n' character in each string.
*
* @param str A pointer to the string
*
* @return The size of the string.
*/
size_t nxs_string_size(nxs_string_t *str);

/**
* @brief Checks for available space in a string
*
* Checks if there is still some available space in memory allocated for a string.
*
* @param str Pointer to the string
*
* @return
* * \b NXS_STRING_NOT_FULL - if there is available space
* * \b NXS_STRING_FULL - if the string is full and there's no more space available
*/
int nxs_string_check_space(nxs_string_t *str);

/**
 * @brief Formats time as char array and writes it to a string.
 *
 * Writes \b fmt into \b str, replacing its format specifiers with the corresponding values that represent the time described by \b t.
 *
 * @param str Pointer to the string.
 * @param fmt String containing any combination of regular characters and special format specifiers.
 * @param t Object that contains a time value.
 *
 * @return
 * * On success, the new length of \b str.
 * * \b NXS_STRING_ERROR_NULL_PTR - If \b str is a null pointer.
 */
ssize_t nxs_string_strftime(nxs_string_t *str, u_char *fmt, time_t t);

/**
 * @brief Formats time given in seconds as char array and writes it to a string.
 *
 * Writes \b fmt into \b str, replacing its format specifiers with the corresponding values that represent the time described by \b t.
 *
 * The supported format specifiers and their corresponding time units are listed in the table below:
 *
 * Specifier  | Output
 * ------------- | -------------
 * %d  | days
 * %h  | hours
 * %m  | minutes
 * %s  | seconds
 *
 * @param str Pointer to the string.
 * @param fmt String containing any combination of regular characters and special format specifiers.
 * @param t Object that contains a time value represented in seconds.
 *
 * @return
 * * On success, the new length of \b str
 * * \b NXS_STRING_ERROR_NULL_PTR - if \b str is a null pointer.
 * * \b NXS_STRING_ERROR_SPEC_OVERFLOW
 * * \b NXS_STRING_ERROR_SPEC_UNKNOWN - if there is an unknown format specifier in \b fmt.
 */
ssize_t nxs_string_fmt_time_interval(nxs_string_t *str, u_char *fmt, time_t sec)

        /**
         * @brief Adds escape symbols to all metacharacters in a string.
         *
         * 	Adds escape symbols to all metacharaterc in \b str_from and saves the result to \b str_to.
         *
         * 	If \b str_from is a null pointer, the function will work with \b str_to.
         *
         * 	The string can be either a regular string or a JSON string.
         *
         * 	The type of the string is defined by \b type.
         *
         * 	@param str_to Pointer to the result string.
         * 	@param str_from Pointer to the source string.
         * 	@param type Constant defining the type of the string. Can be either \b NXS_STRING_ESCAPE_TYPE_GENERIC or \b
         * NXS_STRING_ESCAPE_TYPE_JSON.
         *
         * @return
         * * none
         */
        void nxs_string_escape(nxs_string_t *str_to, nxs_string_t *str_from, nxs_string_escape_types_t type);

/** @} */ // end of nxs-string
#endif    /* _INCLUDE_NXS_STRING_H */
