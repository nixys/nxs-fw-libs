#ifndef _INCLUDE_NXS_BCHAIN_H
#define _INCLUDE_NXS_BCHAIN_H

#include <nxs-core/nxs-core.h>

#define NXS_BCHAIN_E_OK				0
#define NXS_BCHAIN_E_NULL			-1
#define NXS_BCHAIN_E_EMPTY			-2
#define NXS_BCHAIN_E_BUF_NULL		-3
#define NXS_BCHAIN_E_PTR_INIT		-4
#define NXS_BCHAIN_E_OFFSET			-5
#define NXS_BCHAIN_E_PARAMETER		-6
#define NXS_BCHAIN_E_EOB			-7 // End Of Buffers
#define NXS_BCHAIN_E_EOC			-8 // End Of Chars

#define	NXS_BCHAIN_SEEK_FIRST		0
#define	NXS_BCHAIN_SEEK_LAST		1
#define	NXS_BCHAIN_SEEK_CUR			2

struct nxs_bchain_el_s
{
	nxs_bchain_el_t		*next;
	nxs_bchain_el_t		*prev;

	nxs_buf_t			*buf;
};

struct nxs_bchain_s
{
	nxs_bchain_el_t		*first;
	nxs_bchain_el_t		*last;
	nxs_bchain_el_t		*cur;
	u_char				*bcur;

	size_t				count;
};

nxs_bchain_t			*nxs_bchain_malloc					();
nxs_bchain_t			*nxs_bchain_destroy					(nxs_bchain_t *bchain);
int						nxs_bchain_init						(nxs_bchain_t *bchain);
int						nxs_bchain_free						(nxs_bchain_t *bchain);
int						nxs_bchain_add_buf					(nxs_bchain_t *bchain, nxs_buf_t *buf);
int						nxs_bchain_add_buf_cpy				(nxs_bchain_t *bchain, nxs_buf_t *buf);
int						nxs_bchain_drop						(nxs_bchain_t *bchain);
int						nxs_bchain_get_buf					(nxs_bchain_t *bchain, nxs_buf_t **buf);
int						nxs_bchain_get_char					(nxs_bchain_t *bchain, u_char *c);
int						nxs_bchain_read_char				(nxs_bchain_t *bchain, u_char *c);
ssize_t					nxs_bchain_get_block				(nxs_bchain_t *bchain, nxs_buf_t *buf, size_t size);
ssize_t					nxs_bchain_read_block				(nxs_bchain_t *bchain, nxs_buf_t *buf, size_t size);
int						nxs_bchain_seek_buf					(nxs_bchain_t *bchain, u_char whence, ssize_t offset);
int						nxs_bchain_seek_char				(nxs_bchain_t *bchain, u_char whence, ssize_t offset);
size_t					nxs_bchain_get_len					(nxs_bchain_t *bchain);
size_t					nxs_bchain_get_size					(nxs_bchain_t *bchain);
size_t					nxs_bchain_get_count				(nxs_bchain_t *bchain);

#endif /* _INCLUDE_NXS_BCHAIN_H */
