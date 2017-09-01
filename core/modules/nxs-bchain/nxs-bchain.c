// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */



/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_bchain_t *nxs_bchain_malloc(void)
{
	nxs_bchain_t *bchain = NULL;

	bchain = (nxs_bchain_t *)nxs_calloc(bchain, sizeof(nxs_bchain_t));

	nxs_bchain_init(bchain);

	return bchain;
}

nxs_bchain_t *nxs_bchain_destroy(nxs_bchain_t *bchain)
{

	if(bchain == NULL) {

		return NULL;
	}

	nxs_bchain_free(bchain);

	return (nxs_bchain_t *)nxs_free(bchain);
}

nxs_bchain_err_t nxs_bchain_init(nxs_bchain_t *bchain)
{

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	bchain->first = NULL;
	bchain->last  = NULL;
	bchain->cur   = NULL;
	bchain->bcur  = NULL;

	bchain->count = 0;

	return NXS_BCHAIN_E_OK;
}

nxs_bchain_err_t nxs_bchain_free(nxs_bchain_t *bchain)
{

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	bchain->cur  = NULL;
	bchain->bcur = NULL;

	nxs_bchain_drop(bchain);

	return NXS_BCHAIN_E_OK;
}

nxs_bchain_err_t nxs_bchain_add_buf(nxs_bchain_t *bchain, nxs_buf_t *buf)
{
	nxs_bchain_el_t *el = NULL;

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	el       = (nxs_bchain_el_t *)nxs_malloc(el, sizeof(nxs_bchain_el_t));
	el->buf  = buf;
	el->next = NULL;
	el->prev = NULL;

	if(bchain->first == NULL) {

		bchain->first = el;
		bchain->last  = el;
		bchain->cur   = el;

		bchain->bcur = nxs_buf_get_subbuf(el->buf, 0);
	}
	else {

		el->prev = bchain->last;

		bchain->last->next = el;

		bchain->last = el;

		if(bchain->cur == NULL) {

			bchain->cur  = el;
			bchain->bcur = nxs_buf_get_subbuf(el->buf, 0);
		}
	}

	bchain->count++;

	return NXS_BCHAIN_E_OK;
}

nxs_bchain_err_t nxs_bchain_add_buf_cpy(nxs_bchain_t *bchain, nxs_buf_t *buf)
{
	nxs_buf_t *b = NULL;

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	if(buf == NULL) {

		return NXS_BCHAIN_E_BUF_NULL;
	}

	b = nxs_buf_malloc(buf->len);

	nxs_buf_cpy_static(b, 0, buf, buf->len);

	nxs_bchain_add_buf(bchain, b);

	return NXS_BCHAIN_E_OK;
}

nxs_bchain_err_t nxs_bchain_drop(nxs_bchain_t *bchain)
{
	nxs_bchain_el_t *el;

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	if(bchain->first == NULL) {

		return NXS_BCHAIN_E_EMPTY;
	}

	for(el = bchain->first; el != bchain->cur; el = bchain->first) {

		el->buf = nxs_buf_destroy(el->buf);

		bchain->first = el->next;

		if(bchain->first != NULL) {

			bchain->first->prev = NULL;
		}

		el = (nxs_bchain_el_t *)nxs_free(el);

		bchain->count--;
	}

	if(bchain->first == NULL) {

		bchain->last = NULL;
	}

	return NXS_BCHAIN_E_OK;
}

nxs_bchain_err_t nxs_bchain_get_buf(nxs_bchain_t *bchain, nxs_buf_t **buf)
{
	int rc;

	*buf = NULL;

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	if(bchain->cur == NULL) {

		return NXS_BCHAIN_E_EOB;
	}

	*buf = bchain->cur->buf;

	if((rc = nxs_bchain_seek_buf(bchain, NXS_BCHAIN_SEEK_CUR, 1)) == NXS_BCHAIN_E_OFFSET) {

		bchain->cur  = NULL;
		bchain->bcur = NULL;
	}

	return NXS_BCHAIN_E_OK;
}

nxs_bchain_err_t nxs_bchain_get_char(nxs_bchain_t *bchain, u_char *c)
{

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	if(bchain->bcur == NULL) {

		return NXS_BCHAIN_E_EOC;
	}

	*c = *bchain->bcur;

	if(nxs_bchain_seek_char(bchain, NXS_BCHAIN_SEEK_CUR, 1) == NXS_BCHAIN_E_OFFSET) {

		bchain->cur  = NULL;
		bchain->bcur = NULL;
	}

	return NXS_BCHAIN_E_OK;
}

nxs_bchain_err_t nxs_bchain_read_char(nxs_bchain_t *bchain, u_char *c)
{
	int rc;

	rc = nxs_bchain_get_char(bchain, c);

	nxs_bchain_drop(bchain);

	return rc;
}

ssize_t nxs_bchain_get_block(nxs_bchain_t *bchain, nxs_buf_t *buf, size_t size)
{
	u_char c;
	size_t i;

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	if(bchain->bcur == NULL) {

		return NXS_BCHAIN_E_EOC;
	}

	for(i = 0; i < size; i++) {

		if(nxs_bchain_get_char(bchain, &c) != NXS_BCHAIN_E_OK) {

			break;
		}

		nxs_buf_add_char(buf, c);
	}

	return (ssize_t)i;
}

ssize_t nxs_bchain_read_block(nxs_bchain_t *bchain, nxs_buf_t *buf, size_t size)
{
	u_char c;
	size_t i;

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	if(bchain->bcur == NULL) {

		return NXS_BCHAIN_E_EOC;
	}

	for(i = 0; i < size; i++) {

		if(nxs_bchain_read_char(bchain, &c) != NXS_BCHAIN_E_OK) {

			break;
		}

		nxs_buf_add_char(buf, c);
	}

	return (ssize_t)i;
}

/*
 *
 * Возвращаемые значения:
 * * NXS_BCHAIN_E_NULL		- bchain равно NULL
 * * NXS_BCHAIN_E_EMPTY		- цепочка пуста
 * * NXS_BCHAIN_E_PTR_INIT	- курсор цепочки не задан (не определён)
 * * NXS_BCHAIN_E_OFFSET	- требуемое смещение больше доступного числа элементов (конец цепочки) или значение offset отрицательное для
 * случаев NXS_BCHAIN_SEEK_FIRST или NXS_BCHAIN_SEEK_LAST
 */
nxs_bchain_err_t nxs_bchain_seek_buf(nxs_bchain_t *bchain, nxs_bchain_seek_t whence, ssize_t offset)
{
	nxs_bchain_el_t *el = NULL;
	ssize_t          i;

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	switch(whence) {

		case NXS_BCHAIN_SEEK_FIRST:

			if(bchain->first == NULL) {

				return NXS_BCHAIN_E_EMPTY;
			}

			if(offset < 0) {

				return NXS_BCHAIN_E_OFFSET;
			}

			for(i = offset, el = bchain->first; el != NULL && i > 0; el = el->next, i--)
				;

			break;

		case NXS_BCHAIN_SEEK_LAST:

			if(bchain->last == NULL) {

				return NXS_BCHAIN_E_EMPTY;
			}

			if(offset < 0) {

				return NXS_BCHAIN_E_OFFSET;
			}

			for(i = offset, el = bchain->last; el != NULL && i > 0; el = el->prev, i--)
				;

			break;

		case NXS_BCHAIN_SEEK_CUR:
		default:

			if(bchain->cur == NULL) {

				return NXS_BCHAIN_E_PTR_INIT;
			}

			for(offset<0 ? (i = -offset) : (i = offset), el = bchain->cur; el != NULL && i> 0;
			    offset < 0 ? (el = el->prev) : (el = el->next), i--)
				;

			break;
	}

	if(el == NULL) {

		return NXS_BCHAIN_E_OFFSET;
	}

	bchain->cur  = el;
	bchain->bcur = el->buf->data;

	return NXS_BCHAIN_E_OK;
}

nxs_bchain_err_t nxs_bchain_seek_char(nxs_bchain_t *bchain, nxs_bchain_seek_t whence, ssize_t offset)
{
	nxs_bchain_el_t *el = NULL;
	ssize_t          i, l;
	u_char *         c = NULL;

	if(bchain == NULL) {

		return NXS_BCHAIN_E_NULL;
	}

	switch(whence) {

		case NXS_BCHAIN_SEEK_FIRST:

			if(bchain->cur == NULL) {

				return NXS_BCHAIN_E_PTR_INIT;
			}

			if(offset < 0) {

				return NXS_BCHAIN_E_OFFSET;
			}

			for(i = offset, el = bchain->cur, c = el->buf->data; el != NULL && i >= (ssize_t)el->buf->len;
			    i -= el->buf->len, el = el->next)
				;

			if(el != NULL) {

				c = el->buf->data;
			}

			break;

		case NXS_BCHAIN_SEEK_CUR:

			if(bchain->cur == NULL) {

				return NXS_BCHAIN_E_PTR_INIT;
			}

			if(bchain->bcur == NULL) {

				bchain->bcur = bchain->cur->buf->data;
			}

			el = bchain->cur;
			c  = bchain->bcur;

			if(offset < 0) {

				for(i = -offset, l = bchain->bcur - el->buf->data; i > l && el != NULL;) {

					i -= l;

					if((el = el->prev) != NULL) {

						l = el->buf->len;
						c = el->buf->data;
					}
					else {

						l = 0;
					}
				}

				i = l - i;
			}
			else {

				for(i = offset, l = el->buf->len - (bchain->bcur - el->buf->data); i >= l && el != NULL;) {

					i -= l;

					if((el = el->next) != NULL) {

						l = el->buf->len;
						c = el->buf->data;
					}
					else {

						l = 0;
					}
				}
			}

			break;

		default:

			return NXS_BCHAIN_E_PARAMETER;

			break;
	}

	if(el == NULL) {

		return NXS_BCHAIN_E_OFFSET;
	}

	bchain->cur  = el;
	bchain->bcur = c + i;

	return NXS_BCHAIN_E_OK;
}

size_t nxs_bchain_get_len(nxs_bchain_t *bchain)
{
	nxs_bchain_el_t *el = NULL;
	size_t           len;

	if(bchain == NULL) {

		return 0;
	}

	for(len = 0, el = bchain->first; el != NULL; el = el->next) {

		len += el->buf->len;
	}

	return len;
}

size_t nxs_bchain_get_size(nxs_bchain_t *bchain)
{
	nxs_bchain_el_t *el = NULL;
	size_t           size;

	if(bchain == NULL) {

		return 0;
	}

	for(size = 0, el = bchain->first; el != NULL; el = el->next) {

		size += el->buf->size;
	}

	return size;
}

size_t nxs_bchain_get_count(nxs_bchain_t *bchain)
{

	if(bchain == NULL) {

		return 0;
	}

	return bchain->count;
}

/* Module internal (static) functions */
