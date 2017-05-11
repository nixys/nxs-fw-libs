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

static const u_char nxs_base64_encoding_table[] =
{
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};

static const u_char nxs_base64_decoding_table[] =
{
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

static size_t nxs_base64_mod_table[] = {0, 2, 1};

/* Module global functions */

// clang-format on

void nxs_base64_encode_string(nxs_string_t *dst, nxs_string_t *src)
{
	size_t   i, j, il, ol;
	uint32_t octet_a, octet_b, octet_c, triple;

	il = nxs_string_len(src);
	ol = 4 * ((il + 2) / 3);

	nxs_string_resize(dst, ol + 1);
	nxs_string_clear(dst);

	for(i = 0, j = 0; i < il;) {

		octet_a = (uint32_t)(i < il ? nxs_string_get_char(src, i++) : 0);
		octet_b = (uint32_t)(i < il ? nxs_string_get_char(src, i++) : 0);
		octet_c = (uint32_t)(i < il ? nxs_string_get_char(src, i++) : 0);

		triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		nxs_string_set_char(dst, j++, nxs_base64_encoding_table[(triple >> 3 * 6) & 0x3F]);
		nxs_string_set_char(dst, j++, nxs_base64_encoding_table[(triple >> 2 * 6) & 0x3F]);
		nxs_string_set_char(dst, j++, nxs_base64_encoding_table[(triple >> 1 * 6) & 0x3F]);
		nxs_string_set_char(dst, j++, nxs_base64_encoding_table[(triple >> 0 * 6) & 0x3F]);
	}

	for(i = 0; i < nxs_base64_mod_table[il % 3]; i++) {

		nxs_string_set_char(dst, ol - i - 1, (u_char)'=');
	}

	nxs_string_set_len(dst, ol);
}

void nxs_base64_decode_string(nxs_string_t *dst, nxs_string_t *src)
{
	size_t   i, j, il, ol;
	uint32_t sextet_a, sextet_b, sextet_c, sextet_d, triple;

	il = nxs_string_len(src);

	if(il % 4 != 0) {

		return;
	}

	ol = il / 4 * 3;

	if(nxs_string_get_char(src, il - 1) == (u_char)'=') {

		ol--;
	}

	if(nxs_string_get_char(src, il - 2) == (u_char)'=') {

		ol--;
	}

	nxs_string_resize(dst, ol + 1);
	nxs_string_set_len(dst, ol);

	for(i = 0, j = 0; i < il;) {

		sextet_a = (uint32_t)(
		        nxs_string_get_char(src, i) == (u_char)'=' ? 0 & i++ : nxs_base64_decoding_table[nxs_string_get_char(src, i++)]);
		sextet_b = (uint32_t)(
		        nxs_string_get_char(src, i) == (u_char)'=' ? 0 & i++ : nxs_base64_decoding_table[nxs_string_get_char(src, i++)]);
		sextet_c = (uint32_t)(
		        nxs_string_get_char(src, i) == (u_char)'=' ? 0 & i++ : nxs_base64_decoding_table[nxs_string_get_char(src, i++)]);
		sextet_d = (uint32_t)(
		        nxs_string_get_char(src, i) == (u_char)'=' ? 0 & i++ : nxs_base64_decoding_table[nxs_string_get_char(src, i++)]);

		triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

		if(j < ol)
			nxs_string_set_char(dst, j++, (u_char)((triple >> 2 * 8) & 0xFF));
		if(j < ol)
			nxs_string_set_char(dst, j++, (u_char)((triple >> 1 * 8) & 0xFF));
		if(j < ol)
			nxs_string_set_char(dst, j++, (u_char)((triple >> 0 * 8) & 0xFF));
	}
}

void nxs_base64_encode_void(nxs_string_t *dst, void *src_data, size_t src_len)
{
	size_t   i, j, ol;
	uint32_t octet_a, octet_b, octet_c, triple;
	u_char * src;

	src = (u_char *)src_data;

	ol = 4 * ((src_len + 2) / 3);

	nxs_string_resize(dst, ol + 1);
	nxs_string_set_len(dst, ol);

	for(i = 0, j = 0; i < src_len;) {

		octet_a = (uint32_t)(i < src_len ? src[i++] : 0);
		octet_b = (uint32_t)(i < src_len ? src[i++] : 0);
		octet_c = (uint32_t)(i < src_len ? src[i++] : 0);

		triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		nxs_string_set_char(dst, j++, nxs_base64_encoding_table[(triple >> 3 * 6) & 0x3F]);
		nxs_string_set_char(dst, j++, nxs_base64_encoding_table[(triple >> 2 * 6) & 0x3F]);
		nxs_string_set_char(dst, j++, nxs_base64_encoding_table[(triple >> 1 * 6) & 0x3F]);
		nxs_string_set_char(dst, j++, nxs_base64_encoding_table[(triple >> 0 * 6) & 0x3F]);
	}

	for(i = 0; i < nxs_base64_mod_table[src_len % 3]; i++) {

		nxs_string_set_char(dst, ol - i - 1, (u_char)'=');
	}
}

size_t nxs_base64_decode_void(void *dst_data, size_t dst_max_len, nxs_string_t *src)
{
	size_t   i, j, il, ol;
	uint32_t sextet_a, sextet_b, sextet_c, sextet_d, triple;
	u_char * dst;

	il = nxs_string_len(src);

	if(il % 4 != 0) {

		return 0;
	}

	ol = il / 4 * 3;

	if(nxs_string_get_char(src, il - 1) == (u_char)'=') {

		ol--;
	}

	if(nxs_string_get_char(src, il - 2) == (u_char)'=') {

		ol--;
	}

	if(ol > dst_max_len) {

		return 0;
	}

	dst = (u_char *)dst_data;

	for(i = 0, j = 0; i < il;) {

		sextet_a = (uint32_t)(
		        nxs_string_get_char(src, i) == (u_char)'=' ? 0 & i++ : nxs_base64_decoding_table[nxs_string_get_char(src, i++)]);
		sextet_b = (uint32_t)(
		        nxs_string_get_char(src, i) == (u_char)'=' ? 0 & i++ : nxs_base64_decoding_table[nxs_string_get_char(src, i++)]);
		sextet_c = (uint32_t)(
		        nxs_string_get_char(src, i) == (u_char)'=' ? 0 & i++ : nxs_base64_decoding_table[nxs_string_get_char(src, i++)]);
		sextet_d = (uint32_t)(
		        nxs_string_get_char(src, i) == (u_char)'=' ? 0 & i++ : nxs_base64_decoding_table[nxs_string_get_char(src, i++)]);

		triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

		if(j < ol)
			dst[j++] = (u_char)((triple >> 2 * 8) & 0xFF);
		if(j < ol)
			dst[j++] = (u_char)((triple >> 1 * 8) & 0xFF);
		if(j < ol)
			dst[j++] = (u_char)((triple >> 0 * 8) & 0xFF);
	}

	return ol;
}

/* Module internal (static) functions */
