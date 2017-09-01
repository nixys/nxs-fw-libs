#ifndef _INCLUDE_NXS_BCHAIN_H
#define _INCLUDE_NXS_BCHAIN_H

// clang-format off

/** @addtogroup nxs-bchain
 *
 * @brief Module 'nxs-bchain' is a basic module providing functions to work with buffers chains.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

enum nxs_bchain_err_e
{
	NXS_BCHAIN_E_OK,
	NXS_BCHAIN_E_NULL,
	NXS_BCHAIN_E_EMPTY,
	NXS_BCHAIN_E_BUF_NULL,
	NXS_BCHAIN_E_PTR_INIT,
	NXS_BCHAIN_E_OFFSET,
	NXS_BCHAIN_E_PARAMETER,
	NXS_BCHAIN_E_EOB,		/* End Of Buffers */
	NXS_BCHAIN_E_EOC		/* End Of Chars */
};

enum nxs_bchain_seek_e
{
	NXS_BCHAIN_SEEK_FIRST,
	NXS_BCHAIN_SEEK_LAST,
	NXS_BCHAIN_SEEK_CUR
};

struct nxs_bchain_el_s
{
	nxs_bchain_el_t		*next;
	nxs_bchain_el_t		*prev;

	nxs_buf_t		*buf;
};

struct nxs_bchain_s
{
	nxs_bchain_el_t		*first;
	nxs_bchain_el_t		*last;
	nxs_bchain_el_t		*cur;
	u_char			*bcur;

	size_t			count;
};

nxs_bchain_t			*nxs_bchain_malloc			(void);
nxs_bchain_t			*nxs_bchain_destroy			(nxs_bchain_t *bchain);
nxs_bchain_err_t		nxs_bchain_init				(nxs_bchain_t *bchain);
nxs_bchain_err_t		nxs_bchain_free				(nxs_bchain_t *bchain);
nxs_bchain_err_t		nxs_bchain_add_buf			(nxs_bchain_t *bchain, nxs_buf_t *buf);
nxs_bchain_err_t		nxs_bchain_add_buf_cpy			(nxs_bchain_t *bchain, nxs_buf_t *buf);
nxs_bchain_err_t		nxs_bchain_drop				(nxs_bchain_t *bchain);
nxs_bchain_err_t		nxs_bchain_get_buf			(nxs_bchain_t *bchain, nxs_buf_t **buf);
nxs_bchain_err_t		nxs_bchain_get_char			(nxs_bchain_t *bchain, u_char *c);
nxs_bchain_err_t		nxs_bchain_read_char			(nxs_bchain_t *bchain, u_char *c);
ssize_t				nxs_bchain_get_block			(nxs_bchain_t *bchain, nxs_buf_t *buf, size_t size);
ssize_t				nxs_bchain_read_block			(nxs_bchain_t *bchain, nxs_buf_t *buf, size_t size);
nxs_bchain_err_t		nxs_bchain_seek_buf			(nxs_bchain_t *bchain, nxs_bchain_seek_t whence, ssize_t offset);
nxs_bchain_err_t		nxs_bchain_seek_char			(nxs_bchain_t *bchain, nxs_bchain_seek_t whence, ssize_t offset);
size_t				nxs_bchain_get_len			(nxs_bchain_t *bchain);
size_t				nxs_bchain_get_size			(nxs_bchain_t *bchain);
size_t				nxs_bchain_get_count			(nxs_bchain_t *bchain);

/** @} */ // end of nxs-bchain
#endif /* _INCLUDE_NXS_BCHAIN_H */
