#ifndef _INCLUDE_NXS_CURL_H
#define _INCLUDE_NXS_CURL_H

// clang-format off

/** @addtogroup nxs-curl
 *
 * @brief Module 'nxs-curl' is a basic module providing functions to work with cURL.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

enum nxs_curl_err_e
{
	NXS_CURL_E_OK,
	NXS_CURL_E_ERR,
	NXS_CURL_E_PTR,
	NXS_CURL_E_INIT
};

enum nxs_curl_log_e
{
	NXS_CURL_LOG_NONE,
	NXS_CURL_LOG_ERROR,
	NXS_CURL_LOG_DEBUG
};

struct nxs_curl_s
{
	nxs_buf_t		out_buf;
	nxs_buf_t		post_data;
	nxs_array_t		headers;		/* type: nxs_string_t */
	nxs_http_code_t		ret_code;
	nxs_bool_t		ssl_verifyhost;
	nxs_curl_log_t		log_type;
};

void				nxs_curl_init				(nxs_curl_t *curl);
void				nxs_curl_free				(nxs_curl_t *curl);

void				nxs_curl_add_header			(nxs_curl_t *curl, nxs_string_t *header);
void				nxs_curl_add_header2			(nxs_curl_t *curl, u_char *header);

void				nxs_curl_set_post			(nxs_curl_t *curl, nxs_buf_t *post_data);
void				nxs_curl_set_post2			(nxs_curl_t *curl, nxs_string_t *post_data);
void				nxs_curl_set_ssl_verivyhost		(nxs_curl_t *curl, nxs_bool_t ssl_verivyhost);
void				nxs_curl_set_debug			(nxs_curl_t *curl, nxs_curl_log_t log_type);

nxs_http_code_t			nxs_curl_get_ret_code			(nxs_curl_t *curl);
nxs_buf_t			*nxs_curl_get_out_buf			(nxs_curl_t *curl);

nxs_curl_err_t			nxs_curl_query				(nxs_process_t *proc, nxs_curl_t *curl, nxs_rest_api_common_cmd_type_t method, u_char *query_str, ...);

/** @} */ // end of nxs-curl
#endif /* _INCLUDE_NXS_CURL_H */
