#ifndef _INCLUDE_NXS_BUF_H
#define _INCLUDE_NXS_BUF_H

/** @addtogroup nxs-buf
 *
 * @brief Module 'nxs-buf' is a basic module providing functions to work with
 * u_char buffers.
 *
 * The difference between nxs_buf_s and nxs_string_s is that the '\0' symbol is not added to the end of the data string.
 *
 * The following terminology is used in the description of this module:
 *
 * \b Buffer - a buffer declared as nxs_buf_s.
 * <b>Initialize a buffer</b> - zero-initialize a buffer by setting
 * nxs_buf_s#data to NULL and nxs_buf_s#len and nxs_buf_s#size to 0.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_BUF_E_OK				0
#define NXS_BUF_E_NULL				-1
#define NXS_BUF_E_NOT_CREATED		-2
#define NXS_BUF_E_NULL_SRC_DATA		-3
#define NXS_BUF_E_SIZE_EXCEEDED		-4
#define NXS_BUF_E_OFFSET			-5
#define NXS_BUF_E_LEN_EXCEEDED		-6

#define NXS_BUF_CMP_EQ				0
#define NXS_BUF_CMP_NE				1

struct nxs_buf_s
{
		u_char *data; /**< u_char array pointer */
		size_t len;   /** < the length of the buffer (max buffer length = size) */
		size_t size;  /** < the amount of allocated memory for #data */
};

/**
 * @brief Allocates memory for a new buffer.
 *
 * Allocates \b size bytes of memory for a new buffer and zero-initializes it.
 *
 * @param size Amount of memory to be allocated for the buffer.
 *
 * @return
 * * Pointer to the new buffer.
 */
nxs_buf_t			*nxs_buf_malloc				(size_t size);

/**
 * @brief Destroys a buffer.
 *
 * Sets the size and the length of the buffer to 0 and then frees the pointer to
 * this buffer.
 *
 * @param buf Pointer to the buffer to be destroyed.
 *
 * @return
 * * \b NULL
 */
nxs_buf_t			*nxs_buf_destroy			(nxs_buf_t *buf);

/**
 * @brief Initializes a buffer.
 *
 * Initializes a buffer by allocating an amount of memory specified by \b size
 * for nxs_buf_s#data, setting its size to \b size and its length to 0.
 *
 * @param buf Pointer to the buffer.
 * @param size New size of the buffer.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success.
 * * \b NXS_BUF_E_NULL		- If \b buf is a null pointer.
 */
int					nxs_buf_init				(nxs_buf_t *buf, size_t size);


/**
 * @brief Initializes a buffer with a string represented by an u_char array.
 *
 * Allocates amount of memory sufficient for \b str and fills \buf with \b str.
 *
 * @param buf Pointer to the buffer.
 * @param str Pointer to the string.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success.
 * * \b NXS_BUF_E_NULL		- If \b buf is a null pointer.
 */
int					nxs_buf_init_string			(nxs_buf_t *buf, u_char *str);

/**
 * @brief Deallocates memory for a buffer and zero-initializes it.
 *
 * This function doesn't destroy \b buf.
 *
 * If \b buf was created by nxs_buf_malloc(), use nxs_buf_destroy() to free and
 * destroy string.
 *
 * @param buf Pointer to the buffer.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success.
 * * \b NXS_BUF_E_NULL		- If \buf is a null pointer.
 * * \b NXS_BUF_E_NOT_CREATED		- if either \b buf#size is 0 or \b buf#data is a
 * null pointer.
 *
 */
int					nxs_buf_free				(nxs_buf_t *buf);

/**
 * @brief Clears the contents of a buffer by filling it with zeroes.
 *
 * @warning Does not free the allocated memory!
 *
 * @param buf Pointer to the buffer.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success.
 * * \b NXS_BUF_E_NULL		- If \buf is a null pointer.
 * * \b NXS_BUF_E_NOT_CREATED		- If either nxs_buf_s#size of \b buf is 0 or nxs_buf_s#data is a null pointer.
 */
int					nxs_buf_clear				(nxs_buf_t *buf);

/**
 * @brief Resizes a buffer.
 *
 * Changes the allocated memory amount for nxs_buf_s#data
 *
 * @param buf Pointer to the buffer.
 * @param size New size of the buffer
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success
 * * \b NXS_BUF_E_NULL		- If \b buf is a null pointer
 */
int					nxs_buf_resize				(nxs_buf_t *buf, size_t new_size);

/**
 * @brief Copies \b size bytes from \b data to a buffer with a specified offset.
 *
 * @param buf Pointer to the destination buffer.
 * @param offset Offset from the start index of the buffer.
 * @param data Pointer to the source of data to copy from.
 * @param size Amount of bytes to copy.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success or if \b size is 0.
 * * \b NXS_BUF_E_NULL		- If \b buf is a null pointer.
 * * \b NXS_BUF_E_SIZE_EXCEEDED		- If the result size of \b buf exceeded its possible size.
 * * \b NXS_BUF_E_NULL_SRC_DATA		- If \b data is a null pointer.
 * allocated size.
 */
int					nxs_buf_cpy					(nxs_buf_t *buf, size_t offset, void *data, size_t size);

/**
 * @brief Copies \b size bytes from \b data to a buffer with a specified offset, increasing its size if it's necessary.
 *
 * If the result size of \b buf is greater than its possible size, \b buf will be resized by calling nxs_buf_resize().
 *
 * @param buf Pointer to the destination buffer.
 * @param offset Offset from the start index of the buffer.
 * @param data Pointer to the source of data to copy from.
 * @param size Amount of bytes to copy.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success or if \b size is 0.
 * * \b NXS_BUF_E_NULL		- If \b buf is a null pointer.
 * * \b NXS_BUF_E_SIZE_EXCEEDED		- If the result size of \b buf exceeds its possible size.
 * * \b NXS_BUF_E_NULL_SRC_DATA		- If \b data is a null pointer.
 * allocated size.
 */
int					nxs_buf_cpy_dyn				(nxs_buf_t *buf, size_t offset, void *data, size_t size);

/**
 * @brief Clones a \b src buffer to a \b dst buffer.
 *
 * The contents of \b src are copied to \b dst.
 * The size of \b dst is adjusted by calling nxs_buf_resize(), so both buffers are identical after calling this function.
 *
 * @param src Pointer to the source buffer.
 * @param dst Pointer to the destination buffer.
 *
 * @return
 * * \b NXS_BUF_E_OK
 */
int					nxs_buf_clone				(nxs_buf_t *buf_dst, nxs_buf_t *buf_src);

/**
 * @brief Concatenates the contents of a buffer with a specified amount of bytes from a \b data source.
 *
 * @param buf Pointer to the buffer.
 * @param data Pointer to the source of data.
 * @param size Amount of bytes to append.
 *
 * @return
 * * \b NXS_BUF_E_OK 		- On success.
 * * \b NXS_BUF_E_NULL 		- If \b buf is a null pointer.
 * * \b NXS_BUF_E_SIZE_EXCEEDED 		- If the result size of \b buf exceeds its possible size.
 * * \b NXS_BUF_E_NULL_SRC_DATA 		- If \b data is a null pointer.
 */
int					nxs_buf_cat					(nxs_buf_t *buf, void *data, size_t size);

/**
 * @brief Concatenates the contents of a buffer with a specified amount of bytes from a \b data source,
 * dynamically increasing the size of \b buf if it's necessary.
 *
 * If the result size of \b buf is greater than its possible size, \b buf will be resized by calling nxs_buf_resize().
 *
 * @param buf Pointer to the buffer.
 * @param data Pointer to the source of data.
 * @param size Amount of bytes to append.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success.
 * * \b NXS_BUF_E_NULL		- If \b buf is a null pointer.
 * * \b NXS_BUF_E_SIZE_EXCEEDED		- If the result size of \b buf exceeds its possible size.
 * * \b NXS_BUF_E_NULL_SRC_DATA		- If \b data is a null pointer.
 */
int					nxs_buf_cat_dyn				(nxs_buf_t *buf, void *data, size_t size);

/**
 * @brief Adds a character \b c to the end of the buffer \b buf.
 *
 * @param buf Pointer to the buffer.
 * @param c Character to be added.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success.
 * * \b NXS_BUF_E_SIZE_EXCEEDED		- If the result length of \b buf exceeds its possible size.
 * * \b NXS_BUF_E_NULL		- If \b buf is a null pointer.
 */
int					nxs_buf_add_char			(nxs_buf_t *buf, u_char c);

/**
 * @brief Compares two buffers with specified offsets.
 *
 * Compares \b buf11 with \b offset1 to \b buf2 with \b offset2
 *
 * @param buf1 Pointer to the first buffer.
 * @param offset1 Offset from the beginning of \b buf1
 * @param buf2 Pointer to the second string.
 * @param offset2 Offset from the beginning of \b buf2
 *
 * @return
 * * \b NXS_BUF_E_NULL		- If either \b buf1 or \b buf2 is a null pointer.
 * * \b NXS_BUF_CMP_EQ		- If the compared strings are equal.
 * * \b NXS_BUF_CMP_NE 		- If the compared buffers are not equal.
 * * \b NXS_BUF_E_OFFSET		- If \b offset1 or \b offset2 is greater than the length of the data string in the corresponding buffer.
 */
int					nxs_buf_cmp					(nxs_buf_t *buf1, size_t offset1, nxs_buf_t *buf2, size_t offset2);

/**
 * @brief Compares \b n (at most) characters of two buffers with specified offsets.
 *
 * Compares \b n characters of \b buf1 with \b offset1 to \b n characters of \b buf2 with \b offset2.
 *
 * The comparision is case-sensitive.
 *
 * @param buf1 Pointer to the first buffer.
 * @param offset1 Offset from the beginning of \b buf1
 * @param buf2 Pointer to the second buffer.
 * @param offset2 Offset from the beginning of \b buf2
 * @param n Number of bytes to compare
 *
 * @return
 * * \b NXS_BUF_E_NULL		- If either \b buf1 or \b buf2 is a null pointer.
 * * \b NXS_BUF_CMP_EQ		- If the compared strings are equal.
 * * \b NXS_BUF_CMP_NE 		- If the compared buffers are not equal.
 * * \b NXS_BUF_E_OFFSET		- If \b offset1 or \b offset2 is greater than the length of the data string in the corresponding buffer.
 */
int					nxs_buf_ncmp				(nxs_buf_t *buf1, size_t offset1, nxs_buf_t *buf2, size_t offset2, size_t n);

/**
 * @brief Adds a character \b c to the end of the buffer \b buf, dynamically increasing the size of \b buf if it's necessary.
 *
 * If the result size of \b buf is greater than its possible size, \b buf will be resized by calling nxs_buf_resize().
 *
 * @param buf Pointer to the buffer.
 * @param c Character to be added.
 *
 * @return
 * * \b NXS_BUF_E_OK 		- On success.
 * * \b NXS_BUF_E_SIZE_EXCEEDED 		- If the result length of \b buf exceeds its possible size.
 * * \b NXS_BUF_E_NULL 		- If \b buf is a null pointer.
 */
int					nxs_buf_add_char_dyn		(nxs_buf_t *buf, u_char c);

/**
 * @brief Replaces a character at the specified index of a buffer with a new character.
 *
 * If a character is being placed beyond the nxs_string_s#len of \b buf, it's length will be increased to (\b pos + 1).
 *
 * @param buf Pointer to the buffer.
 * @param pos Position to set the character at.
 * @param c Character to be set.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success
 * * \b NXS_BUF_E_NULL 		- If \b buf is a null pointer.
 * * \b NXS_BUF_E_SIZE_EXCEEDED		- If \b pos is beyond possible size of \b buf.
 */
int					nxs_buf_set_char			(nxs_buf_t *buf, size_t pos, u_char c);

/**
 * @brief Sets new length for a buffer.
 *
 * @param buf Pointer to the buffer.
 * @param new_len New length of the buffer.
 *
 * @return
 * * \b NXS_BUF_E_OK		- On success.
 * * \b NXS_BUF_E_NULL		- If \b buf is a null pointer.
 * * \b NXS_BUF_E_SIZE_EXCEEDED		- If \new_len is greater than the possible size of \b buf.
 */
int					nxs_buf_set_len				(nxs_buf_t *buf, size_t new_len);

/**
 * @brief Returns a subbuffer as an u_char array from buffer \b buf with \b offset.
 *
 * @param buf Pointer to the buffer.
 * @param offset Beginning index for subbuffer.
 *
 * @return
 * * On success, a pointer to the subbuffer.
 * * NULL - If \b buf is a NULL pointer of \b offset is greater than its size.
 */
u_char				*nxs_buf_get_subbuf			(nxs_buf_t *buf, size_t offset);

/*
 * @param buf Pointer to the buffer
 * @param offset Offset from the beginning of \b buf
 *
 * @return
 * * \b NXS_BUF_E_NULL		- If \b buf is a null pointer.
 * * \b
 */
int					nxs_buf_get_mem				(nxs_buf_t *buf, size_t offset, void *mem, size_t size);
u_char				nxs_buf_get_char			(nxs_buf_t *buf, size_t pos);
/**
 * @brief Returns the size of a buffer.
 *
 * @param buf Pointer to the buffer.
 *
 * @return
 * * On success, the size of \b buf.
 * * 0 - If \b buf is a null pointer.
 */
size_t nxs_buf_get_size(nxs_buf_t *buf);

/**
 * @brief Returns the length of a buffer.
 *
 * @param buf Pointer to the buffer.
 *
 * @return
 * * On success, the length of \b buf
 * * 0 - If \b buf is a null pointer.
 */
size_t nxs_buf_get_len(nxs_buf_t *buf);
u_char *nxs_buf_to_string(nxs_buf_t *buf, size_t offset, nxs_string_t *str);

/** Puts a string represented by an u_char array into a buffer.
 *
 * The nxs_buf_s#data of \b buf is filled with \b str.
 * The '\0' symbol is also added.
 *
 * @param buf Pointer to the buffer.
 * @param str Pointer to the string (u_char array).
 *
 * @return
 * * \b NXS_BUF_E_OK - On success.
 * * \b NXS_BUF_E_SIZE_EXCEEDED - If the result length of \b buf exceeds its possible size.
 * * \b NXS_BUF_E_NULL - If \b buf is a null pointer.
 */
int nxs_buf_fill_str(nxs_buf_t *buf, u_char *str);

#endif /* _INCLUDE_NXS_BUF_H */
