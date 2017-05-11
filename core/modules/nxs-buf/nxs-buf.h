#ifndef _INCLUDE_NXS_BUF_H
#define _INCLUDE_NXS_BUF_H

// clang-format off

/** @addtogroup nxs-buf
 *
 * @brief Module 'nxs-buf' is a basic module providing functions to work with buffers.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_BUF_E_OK			0
#define NXS_BUF_E_NULL			-1
#define NXS_BUF_E_NOT_CREATED		-2
#define NXS_BUF_E_NULL_SRC_DATA		-3
#define NXS_BUF_E_SIZE_EXCEEDED		-4
#define NXS_BUF_E_OFFSET		-5
#define NXS_BUF_E_LEN_EXCEEDED		-6

#define NXS_BUF_CMP_EQ			0
#define NXS_BUF_CMP_NE			1

struct nxs_buf_s
{
	u_char			*data;
	size_t			len;
	size_t			size;
};

nxs_buf_t			*nxs_buf_malloc				(size_t size);
nxs_buf_t			*nxs_buf_destroy			(nxs_buf_t *buf);
int				nxs_buf_init				(nxs_buf_t *buf, size_t size);
int				nxs_buf_init_string			(nxs_buf_t *buf, u_char *str);
int				nxs_buf_free				(nxs_buf_t *buf);
int				nxs_buf_clear				(nxs_buf_t *buf);
int				nxs_buf_resize				(nxs_buf_t *buf, size_t new_size);
int				nxs_buf_cpy				(nxs_buf_t *buf, size_t offset, void *data, size_t size);
int				nxs_buf_cpy_dyn				(nxs_buf_t *buf, size_t offset, void *data, size_t size);
int				nxs_buf_clone				(nxs_buf_t *buf_dst, nxs_buf_t *buf_src);
int				nxs_buf_cat				(nxs_buf_t *buf, void *data, size_t size);
int				nxs_buf_cat_dyn				(nxs_buf_t *buf, void *data, size_t size);
int				nxs_buf_add_char			(nxs_buf_t *buf, u_char c);
int				nxs_buf_cmp				(nxs_buf_t *buf1, size_t offset1, nxs_buf_t *buf2, size_t offset2);
int				nxs_buf_ncmp				(nxs_buf_t *buf1, size_t offset1, nxs_buf_t *buf2, size_t offset2, size_t n);
int				nxs_buf_add_char_dyn			(nxs_buf_t *buf, u_char c);
int				nxs_buf_set_char			(nxs_buf_t *buf, size_t pos, u_char c);
int				nxs_buf_set_len				(nxs_buf_t *buf, size_t new_len);
u_char				*nxs_buf_get_subbuf			(nxs_buf_t *buf, size_t offset);
int				nxs_buf_get_mem				(nxs_buf_t *buf, size_t offset, void *mem, size_t size);
u_char				nxs_buf_get_char			(nxs_buf_t *buf, size_t pos);
size_t				nxs_buf_get_size			(nxs_buf_t *buf);
size_t				nxs_buf_get_len				(nxs_buf_t *buf);
u_char				*nxs_buf_to_string			(nxs_buf_t *buf, size_t offset,  nxs_string_t *str);
int				nxs_buf_fill_str			(nxs_buf_t *buf, u_char *str);

/** @} */ // end of nxs-buf
#endif /* _INCLUDE_NXS_BUF_H */
