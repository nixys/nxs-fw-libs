#ifndef _INCLUDE_NXS_REST_API_H
#define _INCLUDE_NXS_REST_API_H

// clang-format off

/** @addtogroup nxs-rest-api
 *
 * @brief Module 'nxs-rest-api' is a basic module providing functions to work with Rest API.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define NXS_REST_API_MAX_HEADER_SIZE_DEFAULT		4096		/* 4 KB */
#define NXS_REST_API_MAX_BODY_SIZE_DEFAULT		10485760	/* 10 MB */

enum nxs_rest_api_err_e
{
	NXS_REST_API_E_OK,
	NXS_REST_API_E_ERR,
	NXS_REST_API_E_PTR,
	NXS_REST_API_E_INIT,
	NXS_REST_API_E_EMPTY,
	NXS_REST_API_E_TIMEOUT,
	NXS_REST_API_E_CORE_BASE,
	NXS_REST_API_E_CORE_BIND,
	NXS_REST_API_E_CORE_LOOP,
	NXS_REST_API_E_SSL,
	NXS_REST_API_E_SSL_USE_CHAIN_FILE,
	NXS_REST_API_E_SSL_USE_PKEY_FILE,
	NXS_REST_API_E_SSL_CHECK_PKEY_FILE,
};

enum nxs_rest_api_format_err_e
{
	NXS_REST_API_FORMAT_ERR_NONE,
	NXS_REST_API_FORMAT_ERR_HTML,
	NXS_REST_API_FORMAT_ERR_JSON
};

enum nxs_rest_api_log_stage_e
{
	NXS_REST_API_LOG_STAGE_BEFORE,
	NXS_REST_API_LOG_STAGE_AFTER
};

struct nxs_rest_api_request_s
{
	nxs_rest_api_common_cmd_type_t	cmd_type;		/* Тип запроса */

	nxs_http_code_t			http_status;		/* Статус выполнения запроса */

	nxs_string_t			peer_ip;		/* IP адрес клиента */
	uint16_t			peer_port;		/* Порт клиента */

	nxs_array_t			uri_in_parts;		/* type: nxs_string_t. Части uri (не включая аргументы) входящего запроса */
	nxs_array_t			uri_in_args;		/* type: nxs_rest_api_keyval_t. Пары ключ-значения аргументов uri входящего запроса */
	nxs_array_t			uri_in_headers;		/* type: nxs_rest_api_keyval_t. Пары ключ-значения заголовков входящего запроса */

	nxs_array_t			uri_out_headers;	/* type: nxs_rest_api_keyval_t. Пары ключ-значения заголовков исходящего ответа */

	nxs_buf_t			body;			/* Тело запроса */

	nxs_buf_t			out_buf;		/* Исходящий буфер, т.е. то, что будет отправлено клиенту */
};

struct nxs_rest_api_ctx_s
{
	struct evhttp			*ev_httpd;
	struct event_base		*ev_base;
	struct evhttp_bound_socket	*ev_handle;

	SSL_CTX				*ssl;

	nxs_string_t			listen_ip;		/* Сетевой интерфейс на котором будет слушать сервер */
	uint16_t			listen_port;		/* Порт на котором будет слушать сервер */

	nxs_list_t			handlers;		/* Список описателей handler'ов */

	/* Logging handler */
	void				*handl_log_user_ctx;	/* User context for logging handler */
	nxs_rest_api_err_t		(*handl_log_before_func)(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, nxs_rest_api_log_stage_t stage, void *user_ctx);
	nxs_rest_api_err_t		(*handl_log_after_func)(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, nxs_rest_api_log_stage_t stage, void *user_ctx);

	/* Default handler */
	void				*handl_def_user_ctx;	/* User context for default handler */
	void				(*handl_def_func)(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, void *user_ctx);

	nxs_rest_api_format_err_t	format_type;		/* В каком формате выдавать пользователю сообщение об ошибке (JSON, HTML или только код возврата) */
};

nxs_rest_api_err_t			nxs_rest_api_init			(nxs_process_t *proc, nxs_rest_api_ctx_t *ctx, nxs_string_t *iface, uint16_t port, nxs_rest_api_format_err_t format_type, nxs_string_t *crt_chain, nxs_string_t *crt_pkey);
void					nxs_rest_api_free			(nxs_rest_api_ctx_t *ctx);
nxs_rest_api_err_t			nxs_rest_api_process			(nxs_rest_api_ctx_t *ctx, __time_t timeout_sec, __suseconds_t timeout_microsec);

void					nxs_rest_api_handler_add_log		(nxs_rest_api_ctx_t *ctx, void *user_ctx, nxs_rest_api_err_t (*handl_log_before_func)(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, nxs_rest_api_log_stage_t stage, void *user_ctx), nxs_rest_api_err_t (*handl_log_after_func)(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, nxs_rest_api_log_stage_t stage, void *user_ctx));
void					nxs_rest_api_handler_del_log		(nxs_rest_api_ctx_t *ctx);
void					nxs_rest_api_handler_add_default	(nxs_rest_api_ctx_t *ctx, void *user_ctx, void (*handl_def_func)(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, void *user_ctx));
void					nxs_rest_api_handler_del_default	(nxs_rest_api_ctx_t *ctx);
void					nxs_rest_api_handler_add		(nxs_rest_api_ctx_t *ctx, nxs_string_t *handler_name, nxs_rest_api_common_cmd_type_t type, nxs_bool_t fixed_name, void *user_ctx, void (*handl_func)(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, void *user_ctx));
void					nxs_rest_api_handler_del		(nxs_rest_api_ctx_t *ctx, nxs_string_t *handler_name);

void					nxs_rest_api_header_add			(nxs_rest_api_request_t *req, nxs_string_t *key, nxs_string_t *val);

nxs_rest_api_err_t			nxs_rest_api_set_max_header_size	(nxs_rest_api_ctx_t *ctx, size_t size);
nxs_rest_api_err_t			nxs_rest_api_set_max_body_size		(nxs_rest_api_ctx_t *ctx, size_t size);
void					nxs_rest_api_set_req_http_status	(nxs_rest_api_request_t *req, nxs_http_code_t http_code);

nxs_http_code_t				nxs_rest_api_get_req_http_status	(nxs_rest_api_request_t *req);
nxs_rest_api_common_cmd_type_t		nxs_rest_api_get_req_type		(nxs_rest_api_request_t *req);
nxs_string_t				*nxs_rest_api_get_req_peer_ip		(nxs_rest_api_request_t *req);
uint16_t				nxs_rest_api_get_req_peer_port		(nxs_rest_api_request_t *req);
nxs_string_t				*nxs_rest_api_get_req_part		(nxs_rest_api_request_t *req, size_t index);
nxs_string_t				*nxs_rest_api_get_req_args_key		(nxs_rest_api_request_t *req, size_t _index);
nxs_string_t				*nxs_rest_api_get_req_args_val		(nxs_rest_api_request_t *req, size_t _index);
nxs_string_t				*nxs_rest_api_get_req_args_find		(nxs_rest_api_request_t *req, nxs_string_t *key);
nxs_string_t				*nxs_rest_api_get_req_headers_key	(nxs_rest_api_request_t *req, size_t _index);
nxs_string_t				*nxs_rest_api_get_req_headers_val	(nxs_rest_api_request_t *req, size_t _index);
nxs_string_t				*nxs_rest_api_get_req_headers_find	(nxs_rest_api_request_t *req, nxs_string_t *key);
nxs_buf_t				*nxs_rest_api_get_req_body		(nxs_rest_api_request_t *req);
nxs_buf_t				*nxs_rest_api_get_out_buf		(nxs_rest_api_request_t *req);

void					nxs_rest_api_page_std			(nxs_rest_api_request_t *req, nxs_rest_api_format_err_t format, nxs_http_code_t http_status, u_char *middle_text);

/** @} */ // end of nxs-rest-api
#endif /* _INCLUDE_NXS_REST_API_H */
