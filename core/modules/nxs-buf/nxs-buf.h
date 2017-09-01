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

enum nxs_buf_err_e
{
	NXS_BUF_E_OK,
	NXS_BUF_E_NULL,
	NXS_BUF_E_NOT_CREATED,
	NXS_BUF_E_NULL_SRC_DATA,
	NXS_BUF_E_SIZE_EXCEEDED,
	NXS_BUF_E_OFFSET,
	NXS_BUF_E_LEN_EXCEEDED,
	NXS_BUF_E_UNKNOWN
};

struct nxs_buf_s
{
	u_char			*data;
	size_t			len;
	size_t			size;
};

nxs_buf_t			*nxs_buf_malloc				(size_t size);
nxs_buf_t			*nxs_buf_destroy			(nxs_buf_t *buf);
nxs_buf_err_t			nxs_buf_init				(nxs_buf_t *buf, size_t size);
nxs_buf_err_t			nxs_buf_init2				(nxs_buf_t *buf);
nxs_buf_err_t			nxs_buf_init_string			(nxs_buf_t *buf, u_char *str);
nxs_buf_err_t			nxs_buf_free				(nxs_buf_t *buf);
nxs_buf_err_t			nxs_buf_clear				(nxs_buf_t *buf);
nxs_buf_err_t			nxs_buf_resize				(nxs_buf_t *buf, size_t new_size);
nxs_buf_err_t			nxs_buf_cpy_static			(nxs_buf_t *buf, size_t offset, void *data, size_t size);
nxs_buf_err_t			nxs_buf_cpy				(nxs_buf_t *buf, size_t offset, void *data, size_t size);
nxs_buf_err_t			nxs_buf_clone				(nxs_buf_t *buf_dst, nxs_buf_t *buf_src);
nxs_buf_err_t			nxs_buf_cat_static			(nxs_buf_t *buf, void *data, size_t size);
nxs_buf_err_t			nxs_buf_cat				(nxs_buf_t *buf, void *data, size_t size);
nxs_bool_t			nxs_buf_cmp				(nxs_buf_t *buf1, size_t offset1, nxs_buf_t *buf2, size_t offset2);
nxs_bool_t			nxs_buf_ncmp				(nxs_buf_t *buf1, size_t offset1, nxs_buf_t *buf2, size_t offset2, size_t n);
nxs_buf_err_t			nxs_buf_add_char_static			(nxs_buf_t *buf, u_char c);
nxs_buf_err_t			nxs_buf_add_char			(nxs_buf_t *buf, u_char c);
nxs_buf_err_t			nxs_buf_set_char			(nxs_buf_t *buf, size_t pos, u_char c);
nxs_buf_err_t			nxs_buf_set_len				(nxs_buf_t *buf, size_t new_len);
u_char				*nxs_buf_get_subbuf			(nxs_buf_t *buf, size_t offset);
nxs_buf_err_t			nxs_buf_get_mem				(nxs_buf_t *buf, size_t offset, void *mem, size_t size);
u_char				nxs_buf_get_char			(nxs_buf_t *buf, size_t pos);
size_t				nxs_buf_get_size			(nxs_buf_t *buf);
size_t				nxs_buf_get_len				(nxs_buf_t *buf);
u_char				*nxs_buf_to_string			(nxs_buf_t *buf, size_t offset,  nxs_string_t *str);
nxs_buf_err_t			nxs_buf_fill_str			(nxs_buf_t *buf, u_char *str);

/** @} */ // end of nxs-buf
#endif /* _INCLUDE_NXS_BUF_H */
