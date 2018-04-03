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
	NXS_CURL_LOG_NONE			= 0,
	NXS_CURL_LOG_ERROR			= 1,
	NXS_CURL_LOG_DEBUG			= 2
};

enum nxs_curl_proxy_type_e
{
	NXS_CURL_PROXY_TYPE_NONE,
	NXS_CURL_PROXY_TYPE_HTTP,
	NXS_CURL_PROXY_TYPE_HTTP_1_0,
	NXS_CURL_PROXY_TYPE_SOCKS4,
	NXS_CURL_PROXY_TYPE_SOCKS5,
	NXS_CURL_PROXY_TYPE_SOCKS4A,
	NXS_CURL_PROXY_TYPE_SOCKS5_HOSTNAME
};

enum nxs_curl_proxy_auth_type_e
{
	NXS_CURL_PROXY_AUTH_TYPE_NONE		= 0,
	NXS_CURL_PROXY_AUTH_TYPE_BASIC		= 1,
	NXS_CURL_PROXY_AUTH_TYPE_DIGEST		= 2,
	/* Enable this after end of Debian 7 support */
	/*NXS_CURL_PROXY_AUTH_TYPE_NEGOTIATE	= 4,*/
	NXS_CURL_PROXY_AUTH_TYPE_NTLM		= 8
};

struct nxs_curl_proxy_s
{
	nxs_bool_t			use_proxy;
	curl_proxytype			type;
	unsigned long			auth_type;
	nxs_string_t			host_data;		/* proxy host and port (format: host:port) */
	nxs_string_t			auth_data;
};

struct nxs_curl_s
{
	nxs_buf_t			out_buf;
	nxs_buf_t			post_data;
	nxs_array_t			headers;		/* type: nxs_string_t */
	nxs_http_code_t			ret_code;
	nxs_bool_t			ssl_verifyhost;
	nxs_curl_log_t			log_type;
	nxs_curl_proxy_t		proxy;
};

void					nxs_curl_init				(nxs_curl_t *curl);
void					nxs_curl_free				(nxs_curl_t *curl);

void					nxs_curl_add_header			(nxs_curl_t *curl, nxs_string_t *header);
void					nxs_curl_add_header2			(nxs_curl_t *curl, u_char *header);

void					nxs_curl_set_post			(nxs_curl_t *curl, nxs_buf_t *post_data);
void					nxs_curl_set_post2			(nxs_curl_t *curl, nxs_string_t *post_data);
void					nxs_curl_set_ssl_verivyhost		(nxs_curl_t *curl, nxs_bool_t ssl_verivyhost);
nxs_curl_err_t				nxs_curl_set_proxy			(nxs_process_t *proc, nxs_curl_t *curl, nxs_curl_proxy_type_t proxy_type, nxs_curl_proxy_auth_type_t proxy_auth_type, nxs_string_t *proxy_host, uint16_t proxy_port, nxs_string_t *username, nxs_string_t *password);
void					nxs_curl_set_debug			(nxs_curl_t *curl, nxs_curl_log_t log_type);

nxs_http_code_t				nxs_curl_get_ret_code			(nxs_curl_t *curl);
nxs_buf_t				*nxs_curl_get_out_buf			(nxs_curl_t *curl);

nxs_curl_err_t				nxs_curl_query				(nxs_process_t *proc, nxs_curl_t *curl, nxs_rest_api_common_cmd_type_t method, u_char *query_str, ...);

nxs_curl_err_t				nxs_curl_download			(nxs_process_t *proc, nxs_curl_t *curl, nxs_rest_api_common_cmd_type_t method, nxs_string_t *file_path, mode_t file_mode, u_char *query_str, ...);
nxs_curl_err_t				nxs_curl_upload				(nxs_process_t *proc, nxs_curl_t *curl, nxs_rest_api_common_cmd_type_t method, nxs_string_t *file_path, u_char *query_str, ...);

void					nxs_curl_opt_set_proxy			(nxs_curl_t *curl, CURL *c);

/** @} */ // end of nxs-curl
#endif /* _INCLUDE_NXS_CURL_H */
