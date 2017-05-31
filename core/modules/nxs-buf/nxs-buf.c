#include <nxs-core/nxs-core.h>

nxs_buf_t *nxs_buf_malloc(size_t size)
{
	nxs_buf_t *buf = NULL;

	buf = (nxs_buf_t *)nxs_calloc(buf, sizeof(nxs_buf_t));

	nxs_buf_init(buf, size);

	return buf;
}
nxs_buf_t *nxs_buf_destroy(nxs_buf_t *buf)
{

	if(buf == NULL){

		return NULL;
	}

	nxs_buf_free(buf);

	return nxs_free(buf);
}

int nxs_buf_init(nxs_buf_t *buf, size_t size)
{

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	buf->data = (u_char *)nxs_calloc(NULL, size);

	buf->size = size;

	buf->len = 0;

	return NXS_BUF_E_OK;
}

int nxs_buf_init_string(nxs_buf_t *buf, u_char *str)
{
	size_t len;

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	len = strlen((char *)str);

	buf->data = (u_char *)nxs_malloc(NULL, len + 1);

	buf->size = len + 1;
	buf->len = 0;

//	return nxs_buf_cpy(buf, 0, str, len + 1);
	return nxs_buf_fill_str(buf, str);
}

int nxs_buf_free(nxs_buf_t *buf)
{

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	if(buf->size == 0 || buf->data == NULL){

		return NXS_BUF_E_NOT_CREATED;
	}

	buf->data = (u_char *)nxs_free(buf->data);

	buf->size = 0;
	buf->len = 0;

	return NXS_BUF_E_OK;
}

int nxs_buf_clear(nxs_buf_t *buf)
{
	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	if(buf->size == 0 || buf->data == NULL){

		return NXS_BUF_E_NOT_CREATED;
	}

	nxs_memzero(buf->data, buf->size);

	buf->len = 0;

	return NXS_BUF_E_OK;
}

int nxs_buf_resize(nxs_buf_t *buf, size_t new_size)
{

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	if(buf->size == 0 || buf->data == NULL){

		nxs_buf_init(buf, new_size);
	}
	else{

		buf->data = nxs_realloc(buf->data, new_size);

		if(new_size < buf->len){

			buf->len = new_size;
		}

		buf->size = new_size;
	}

	return NXS_BUF_E_OK;
}

int nxs_buf_cpy(nxs_buf_t *buf, size_t offset, void *data, size_t size)
{

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	if(offset + size > buf->size){

		return NXS_BUF_E_SIZE_EXCEEDED;
	}

	if(size == 0){

		return NXS_BUF_E_OK;
	}

	if(data == NULL){

		return NXS_BUF_E_NULL_SRC_DATA;
	}

	nxs_memcpy(buf->data + offset, data, size);

	if(offset + size > buf->len){

		buf->len = offset + size;
	}

	return NXS_BUF_E_OK;
}

int nxs_buf_cpy_dyn(nxs_buf_t *buf, size_t offset, void *data, size_t size)
{
	int rc;

	if((rc = nxs_buf_cpy(buf, offset, data, size)) == NXS_BUF_E_SIZE_EXCEEDED){

		nxs_buf_resize(buf, offset + size);

		rc = nxs_buf_cpy(buf, offset, data, size);
	}

	return rc;
}

int nxs_buf_clone(nxs_buf_t *buf_dst, nxs_buf_t *buf_src)
{
	size_t	src_len;

	src_len = buf_src->len;

	nxs_buf_resize(buf_dst, src_len);

	nxs_memcpy(buf_dst->data, buf_src->data, src_len);

	buf_dst->len = src_len;

	return NXS_BUF_E_OK;
}

int nxs_buf_cat(nxs_buf_t *buf, void *data, size_t size)
{

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	if(buf->len + size > buf->size){

		return NXS_BUF_E_SIZE_EXCEEDED;
	}

	if(size == 0){

		return NXS_BUF_E_OK;
	}

	if(data == NULL){

		return NXS_BUF_E_NULL_SRC_DATA;
	}

	nxs_memcpy(buf->data + buf->len, data, size);

	buf->len += size;

	return NXS_BUF_E_OK;
}

int nxs_buf_cat_dyn(nxs_buf_t *buf, void *data, size_t size)
{
	int rc;

	if((rc = nxs_buf_cat(buf, data, size)) == NXS_BUF_E_SIZE_EXCEEDED){

		nxs_buf_resize(buf, buf->len + size);

		rc = nxs_buf_cat(buf, data, size);
	}

	return rc;
}

int nxs_buf_cmp(nxs_buf_t *buf1, size_t offset1, nxs_buf_t *buf2, size_t offset2)
{
	size_t i;

	if(buf1 == NULL || buf2 == NULL) {

		return NXS_BUF_E_NULL;
	}

	if(offset1 > buf1->len){

		return NXS_BUF_E_OFFSET;
	}

	if(offset2 > buf2->len){

		return NXS_BUF_E_OFFSET;
	}

	if(buf1->len - offset1 != buf2->len - offset2){

		return NXS_BUF_CMP_NE;
	}

	for(i = 0; i < buf1->len - offset1; i++){

		if(buf1->data[offset1 + i] != buf2->data[offset2 + i]){

			return NXS_BUF_CMP_NE;
		}
	}

	return NXS_BUF_CMP_EQ;
}

int nxs_buf_ncmp(nxs_buf_t *buf1, size_t offset1, nxs_buf_t *buf2, size_t offset2, size_t n)
{
	size_t i;

	if(buf1 == NULL || buf2 == NULL) {

			return NXS_BUF_E_NULL;
	}

	if(offset1 > buf1->len){

		return NXS_BUF_E_OFFSET;
	}

	if(offset2 > buf2->len){

		return NXS_BUF_E_OFFSET;
	}

	if(n > buf1->len - offset1 || n > buf2->len - offset2){

		return NXS_BUF_CMP_NE;
	}

	for(i = 0; i < n; i++){

		if(buf1->data[offset1 + i] != buf2->data[offset2 + i]){

			return NXS_BUF_CMP_NE;
		}
	}

	return NXS_BUF_CMP_EQ;
}

int nxs_buf_add_char(nxs_buf_t *buf, u_char c)
{

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	if(buf->len >= buf->size){

		return NXS_BUF_E_SIZE_EXCEEDED;
	}

	buf->data[buf->len] = c;

	buf->len++;

	return NXS_BUF_E_OK;
}

int nxs_buf_add_char_dyn(nxs_buf_t *buf, u_char c)
{
	int rc;

	if((rc = nxs_buf_add_char(buf, c)) == NXS_BUF_E_SIZE_EXCEEDED){

		nxs_buf_resize(buf, buf->len + 1);

		rc = nxs_buf_add_char(buf, c);
	}

	return rc;
}

int nxs_buf_set_char(nxs_buf_t *buf, size_t pos, u_char c)
{

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	if(pos >= buf->size){

		return NXS_BUF_E_SIZE_EXCEEDED;
	}

	buf->data[pos] = c;

	if(pos >= buf->len){

		buf->len = pos + 1;
	}

	return NXS_BUF_E_OK;
}

int nxs_buf_set_len(nxs_buf_t *buf, size_t new_len)
{

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	if(new_len > buf->size){

		return NXS_BUF_E_SIZE_EXCEEDED;
	}

	buf->len = new_len;

	return NXS_BUF_E_OK;
}

u_char *nxs_buf_get_subbuf(nxs_buf_t *buf, size_t offset)
{

	if(buf == NULL){

		return NULL;
	}

	if(offset > buf->size){

		return NULL;
	}

	return buf->data + offset;
}

int nxs_buf_get_mem(nxs_buf_t *buf, size_t offset, void *mem, size_t size)
{

	if(buf == NULL){

		return NXS_BUF_E_NULL;
	}

	if(offset >= buf->len){

		return NXS_BUF_E_OFFSET;
	}

	if(offset + size > buf->len){

		return NXS_BUF_E_LEN_EXCEEDED;
	}

	nxs_memcpy(mem, buf->data + offset, size);

	return NXS_BUF_E_OK;
}

u_char nxs_buf_get_char(nxs_buf_t *buf, size_t pos)
{

	if(buf == NULL){

		return 0;
	}

	if(pos >= buf->size){

		return 0;
	}

	return buf->data[pos];
}

size_t nxs_buf_get_size(nxs_buf_t *buf)
{

	if(buf == NULL){

		return 0;
	}

	return buf->size;
}

size_t nxs_buf_get_len(nxs_buf_t *buf)
{

	if(buf == NULL){

		return 0;
	}

	return buf->len;
}

u_char *nxs_buf_to_string(nxs_buf_t *buf, size_t offset,  nxs_string_t *str)
{
	size_t len;

	if(str == NULL){

		return NULL;
	}

	if(buf == NULL){

		return NULL;
	}

	if(offset >= buf->size){

		return NULL;
	}

	if(buf->data[buf->len -1] == '\0'){

		len = buf->len - 1;
	}
	else{

		len = buf->len;
	}

	nxs_string_char_ncpy_dyn(str, 0, buf->data + offset, len - offset);

	return nxs_string_str(str);
}

int nxs_buf_fill_str(nxs_buf_t *buf, u_char *str)
{
	size_t len;

	len = strlen((char *)str);

	nxs_buf_cpy_dyn(buf, 0, str, len);

	return nxs_buf_add_char_dyn(buf, (u_char)'\0');
}
