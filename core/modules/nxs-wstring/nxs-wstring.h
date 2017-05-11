#ifndef _INCLUDE_NXS_WSTRING_H
#define _INCLUDE_NXS_WSTRING_H

// clang-format off

/** @addtogroup nxs-wstring
 *
 * @brief Module 'nxs-wstring' is a basic module providing functions to work with wide strings (not complete).
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_WSTRING_E_OK		0

#define NXS_WSTRING_E			-1
#define NXS_WSTRING_E_INIT		-2
#define NXS_WSTRING_E_DST_SIZE		-3
#define NXS_WSTRING_E_SRC_SIZE		-4
#define NXS_WSTRING_E_DST_LEN		-5
#define NXS_WSTRING_E_SRC_LEN		-6
#define NXS_WSTRING_E_OFFSET		-7
#define NXS_WSTRING_E_NOT_CREATED	-8
#define NXS_WSTRING_E_PTR		-9
#define NXS_WSTRING_E_SPEC_UNKNOWN	-10
#define NXS_WSTRING_E_SPEC_OVERFLOW	-11

#define NXS_WSTRING_CMP_EQ		0
#define NXS_WSTRING_CMP_NE		1

#define NXS_WSTRING_EMPTY_STR		L""

struct nxs_wstring_s
{
	wchar_t			*str;
	size_t			len;
	size_t			size;
};

#define nxs_wstring_str(str) \
			nxs_wstring_get_substr(str, 0)

void				nxs_wstring_ctx_init			(void);

nxs_wstring_t			*nxs_wstring_malloc			(size_t size, wchar_t *new_str);
nxs_wstring_t			*nxs_wstring_destroy			(nxs_wstring_t *str);
void				nxs_wstring_init			(nxs_wstring_t *str);
ssize_t				nxs_wstring_init2			(nxs_wstring_t *str, size_t size, wchar_t *new_str);
ssize_t				nxs_wstring_create			(nxs_wstring_t *str, size_t size, wchar_t *new_str);
int				nxs_wstring_free			(nxs_wstring_t *str);
ssize_t				nxs_wstring_resize			(nxs_wstring_t *str, size_t new_size);
int				nxs_wstring_clear			(nxs_wstring_t *str);

ssize_t				nxs_wstring_cpy				(nxs_wstring_t *dst, size_t offset_dst, nxs_wstring_t *src, size_t offset_src);
ssize_t				nxs_wstring_cpy_dyn			(nxs_wstring_t *dst, size_t offset_dst, nxs_wstring_t *src, size_t offset_src);

ssize_t				nxs_wstring_ncpy			(nxs_wstring_t *dst, size_t offset_dst, nxs_wstring_t *src, size_t offset_src, size_t n);
ssize_t				nxs_wstring_ncpy_dyn			(nxs_wstring_t *dst, size_t offset_dst, nxs_wstring_t *src, size_t offset_src, size_t n);

ssize_t				nxs_wstring_cat				(nxs_wstring_t *dst, nxs_wstring_t *src);
ssize_t				nxs_wstring_cat_dyn			(nxs_wstring_t *dst, nxs_wstring_t *src);

ssize_t				nxs_wstring_ncat			(nxs_wstring_t *dst, nxs_wstring_t *src, size_t n);
ssize_t				nxs_wstring_ncat_dyn			(nxs_wstring_t *dst, nxs_wstring_t *src, size_t n);

int				nxs_wstring_cmp				(nxs_wstring_t *str1, size_t offset1, nxs_wstring_t *str2, size_t offset2);
int				nxs_wstring_ncmp			(nxs_wstring_t *str1, size_t offset1, nxs_wstring_t *str2, size_t offset2, size_t n);

ssize_t				nxs_wstring_wchar_cpy			(nxs_wstring_t *str, size_t offset, wchar_t *ch_str);
ssize_t				nxs_wstring_wchar_cpy_dyn		(nxs_wstring_t *str, size_t offset, wchar_t *ch_str);

ssize_t				nxs_wstring_wchar_ncpy			(nxs_wstring_t *str, size_t offset, wchar_t *ch_str, size_t n);
ssize_t				nxs_wstring_wchar_ncpy_dyn		(nxs_wstring_t *str, size_t offset, wchar_t *ch_str, size_t n);

ssize_t				nxs_wstring_wchar_cat			(nxs_wstring_t *str, wchar_t *ch_str);
ssize_t				nxs_wstring_wchar_cat_dyn		(nxs_wstring_t *str, wchar_t *ch_str);

ssize_t				nxs_wstring_wchar_ncat			(nxs_wstring_t *str, wchar_t *ch_str, size_t n);
ssize_t				nxs_wstring_wchar_ncat_dyn		(nxs_wstring_t *str, wchar_t *ch_str, size_t n);

int				nxs_wstring_wchar_cmp			(nxs_wstring_t *str, size_t offset, wchar_t *ch_str);
int				nxs_wstring_wchar_ncmp			(nxs_wstring_t *str, size_t offset, wchar_t *ch_str, size_t n);

ssize_t				nxs_wstring_wchar_add_wchar		(nxs_wstring_t *str, wchar_t c);
ssize_t				nxs_wstring_wchar_add_wchar_dyn		(nxs_wstring_t *str, wchar_t c);

void				nxs_wstring_basename			(nxs_wstring_t *dst, nxs_wstring_t *path);
void				nxs_wstring_dirname			(nxs_wstring_t *dst, nxs_wstring_t *path);

ssize_t				nxs_wstring_set_wchar			(nxs_wstring_t *str, size_t pos, wchar_t c);
ssize_t				nxs_wstring_set_len			(nxs_wstring_t *str, size_t len);
ssize_t				nxs_wstring_len_fix			(nxs_wstring_t *str);
wchar_t				nxs_wstring_get_wchar			(nxs_wstring_t *str, size_t pos);
wchar_t				*nxs_wstring_find_substr_first		(nxs_wstring_t *str, size_t offset, wchar_t *f_str, size_t f_str_len);
wchar_t				*nxs_wstring_find_substr_last		(nxs_wstring_t *str, size_t offset, wchar_t *f_str, size_t f_str_len);


wchar_t				*nxs_wstring_get_substr			(nxs_wstring_t *str, size_t offset);

/** @} */ // end of nxs-wstring
#endif /* _INCLUDE_NXS_WSTRING_H */
