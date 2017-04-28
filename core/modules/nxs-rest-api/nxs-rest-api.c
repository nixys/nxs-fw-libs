#include <nxs-core/nxs-core.h>

typedef struct
{
	nxs_string_t						key;
	nxs_string_t						value;
} nxs_rest_api_keyval_t;

typedef struct
{
	nxs_string_t						handler_name;			/* Имя обработчика (начальная часть uri) */
	nxs_rest_api_common_cmd_type_t		cmd_type;				/* Тип запроса */
	nxs_bool_t							fixed_name;				/* Является ли имя обработчика жёстко заданным или только начальной части пути после которого может следовать продолжение */

	void								*user_ctx;

	int									(*handl_func)(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, void *user_ctx);
} nxs_rest_api_handler_t;

static void								nxs_rest_api_request_init								(nxs_rest_api_request_t *req);
static void								nxs_rest_api_request_free								(nxs_rest_api_request_t *req);

static void								nxs_rest_api_generic_handler							(struct evhttp_request *_req, void *arg);

static void								nxs_rest_api_parse_uri									(nxs_array_t *uri_pats, char *decoded_path);
static nxs_rest_api_err_t				nxs_rest_api_parse_args									(nxs_array_t *uri_args, char *args_str);
static void								nxs_rest_api_parse_headers								(nxs_array_t *uri_args, struct evhttp_request *req);

static nxs_rest_api_err_t				nxs_rest_api_handler_exec								(nxs_rest_api_ctx_t *ctx, nxs_rest_api_request_t *req, int *http_status);
static void								nxs_rest_api_handler_free								(nxs_rest_api_ctx_t *ctx);

static nxs_rest_api_err_t				nxs_rest_api_setup_certs								(nxs_rest_api_ctx_t *ctx, nxs_string_t *crt_chain, nxs_string_t *crt_pkey);
static struct							bufferevent* nxs_rest_api_bevcb							(struct event_base *base, void *arg);

nxs_rest_api_err_t nxs_rest_api_init(nxs_process_t *proc, nxs_rest_api_ctx_t *ctx, void *user_ctx, nxs_string_t *iface, uint16_t port, nxs_rest_api_format_err_t format_type, nxs_string_t *crt_chain, nxs_string_t *crt_pkey)
{
	nxs_rest_api_err_t	rc;
	EC_KEY				*ecdh;

	rc = NXS_REST_API_E_OK;

	nxs_proc_signal_set(proc, SIGPIPE, NXS_PROCESS_SA_FLAG_EMPTY, NXS_SIG_IGN, NULL, NXS_PROCESS_FORCE_NO);

	ctx->ev_base			= NULL;
	ctx->ev_httpd			= NULL;
	ctx->ev_handle			= NULL;

	ctx->ssl				= NULL;

	ctx->listen_port		= port;

	ctx->user_ctx			= user_ctx;

	ctx->format_type		= format_type;

	ecdh					= NULL;

	nxs_string_init2(&ctx->listen_ip, 0, nxs_string_str(iface));
	nxs_string_init2(&ctx->error_text, 0, NXS_STRING_EMPTY_STR);

	nxs_list_init(&ctx->handlers, sizeof(nxs_rest_api_handler_t));

	if((ctx->ev_base = event_base_new()) == NULL){

		rc = NXS_REST_API_E_CORE_BASE;
		goto error;
	}

	ctx->ev_httpd = evhttp_new(ctx->ev_base);

	/* Если требуется SSL */
	if(crt_chain != NULL && crt_pkey != NULL){

		SSL_library_init();

		nxs_log_write_debug(proc, "core: rest api init: using SSL");

		if((ctx->ssl = SSL_CTX_new(SSLv23_server_method())) == NULL){

			nxs_log_write_debug(proc, "core: rest api init error: SSL_CTX_new");

			rc =  NXS_REST_API_E_SSL;
			goto error;
		}

		SSL_CTX_set_options(ctx->ssl, SSL_OP_SINGLE_DH_USE | SSL_OP_SINGLE_ECDH_USE | SSL_OP_NO_SSLv2);

		if((ecdh = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1)) == NULL){

			nxs_log_write_debug(proc, "core: rest api init error: EC_KEY_new_by_curve_name");

			rc =  NXS_REST_API_E_SSL;
			goto error;
		}

		if(SSL_CTX_set_tmp_ecdh(ctx->ssl, ecdh) != 1){

			nxs_log_write_debug(proc, "core: rest api init error: SSL_CTX_set_tmp_ecdh");

			rc =  NXS_REST_API_E_SSL;
			goto error;
		}

		if((rc = nxs_rest_api_setup_certs(ctx, crt_chain, crt_pkey)) != NXS_REST_API_E_OK){

			nxs_log_write_debug(proc, "core: rest api init error: setup crt (errno: %d)", rc);

			goto error;
		}

		evhttp_set_bevcb(ctx->ev_httpd, nxs_rest_api_bevcb, ctx->ssl);
	}

	if((ctx->ev_handle = evhttp_bind_socket_with_handle(ctx->ev_httpd, (char *)nxs_string_str(&ctx->listen_ip), ctx->listen_port)) == NULL){

		rc = NXS_REST_API_E_CORE_BIND;
		goto error;
	}

	evhttp_set_gencb(ctx->ev_httpd, nxs_rest_api_generic_handler, ctx);

    evhttp_set_max_headers_size(ctx->ev_httpd, NXS_REST_API_MAX_HEADER_SIZE_DEFAULT);
    evhttp_set_max_body_size(ctx->ev_httpd, NXS_REST_API_MAX_BODY_SIZE_DEFAULT);

error:

	if(crt_chain != NULL && crt_pkey != NULL){

		/* Если использовался SSL (https://wiki.openssl.org/index.php/Library_Initialization) */

		FIPS_mode_set(0);
		ENGINE_cleanup();
		CONF_modules_unload(1);
		EVP_cleanup();
		CRYPTO_cleanup_all_ex_data();
		ERR_remove_state(0);
		ERR_free_strings();
	}

	if(ecdh != NULL){

		EC_KEY_free(ecdh);
	}

	if(rc != NXS_REST_API_E_OK){

		nxs_rest_api_free(ctx);
	}

	return rc;
}

void nxs_rest_api_free(nxs_rest_api_ctx_t *ctx)
{

	if(ctx->ev_handle != NULL){

		evhttp_del_accept_socket(ctx->ev_httpd, ctx->ev_handle);
		ctx->ev_handle = NULL;
	}

	if(ctx->ev_httpd != NULL){

		evhttp_free(ctx->ev_httpd);
		ctx->ev_httpd = NULL;
	}

	if(ctx->ev_base != NULL){

		event_base_free(ctx->ev_base);
		ctx->ev_base = NULL;
	}

	if(ctx->ssl != NULL){

		SSL_CTX_free(ctx->ssl);
		ctx->ssl = NULL;
	}

	nxs_string_free(&ctx->listen_ip);
	nxs_string_free(&ctx->error_text);

	ctx->user_ctx = NULL;

	nxs_rest_api_handler_free(ctx);

	ctx->listen_port = 0;
}

/*
 * Функция для обработки запросов.
 * Ожидает запроса в течение timeout_sec секунд и timeout_microsec микросекунд
 *
 * Возвращаемые значения:
 * * NXS_REST_API_E_OK			- функция завершилась успешно или в указанный тайм-аут событий не произошло
 * * NXS_REST_API_E_CORE_LOOP	- ошибка event_base_loop()
 * *
 */
nxs_rest_api_err_t nxs_rest_api_process(nxs_rest_api_ctx_t *ctx, __time_t timeout_sec, __suseconds_t timeout_microsec)
{
	struct timeval	tv;

	tv.tv_sec	= timeout_sec;
	tv.tv_usec	= timeout_microsec;

	nxs_string_clear(&ctx->error_text);

	event_base_loopexit(ctx->ev_base, &tv);

	if(event_base_loop(ctx->ev_base, 0) == -1){

		return NXS_REST_API_E_CORE_LOOP;
	}

	return NXS_REST_API_E_OK;
}


void nxs_rest_api_handler_add(nxs_rest_api_ctx_t *ctx, nxs_string_t *handler_name, nxs_rest_api_common_cmd_type_t type, nxs_bool_t fixed_name, void *user_ctx, int (*handl_func)(nxs_rest_api_ctx_t *rest_api_ctx, nxs_rest_api_request_t *req, void *user_ctx))
{
	nxs_rest_api_handler_t	*h;

	h = nxs_list_add_tail(&ctx->handlers);

	h->cmd_type		= type;
	h->fixed_name	= fixed_name;
	h->handl_func	= handl_func;
	h->user_ctx		= user_ctx;

	nxs_string_init2(&h->handler_name, 0, nxs_string_str(handler_name));
}

void nxs_rest_api_handler_del(nxs_rest_api_ctx_t *ctx, nxs_string_t *handler_name)
{
	nxs_rest_api_handler_t	*h;

	for(h = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &ctx->handlers); h != NULL; h = nxs_list_ptr_next(&ctx->handlers)){

		if(nxs_string_cmp(&h->handler_name, 0, handler_name, 0) == NXS_STRING_CMP_EQ){

			nxs_string_free(&h->handler_name);

			h = nxs_list_del(&ctx->handlers, NXS_LIST_MOVE_NEXT);
		}
	}
}

/*
 * Добавление заголовков к отправляемому сообщению
 * Если заголовок с таким ключом уже существует - значение будет заменено на новое
 */
void nxs_rest_api_header_add(nxs_rest_api_request_t *req, nxs_string_t *key, nxs_string_t *val)
{
	nxs_rest_api_keyval_t	*kv;
	size_t					i;

	if(req == NULL || key == NULL || val == NULL){

		return;
	}

	for(i = 0; i < nxs_array_count(&req->uri_out_headers); i++){

		kv = nxs_array_get(&req->uri_out_headers, i);

		if(nxs_string_cmp(key, 0, &kv->key, 0) == NXS_STRING_CMP_EQ){

			nxs_string_cpy_dyn(&kv->value, 0, val, 0);

			return;
		}
	}

	kv = nxs_array_add(&req->uri_out_headers);

	nxs_string_init2(&kv->key, 0, nxs_string_str(key));
	nxs_string_init2(&kv->value, 0, nxs_string_str(val));
}

nxs_rest_api_err_t nxs_rest_api_set_max_header_size(nxs_rest_api_ctx_t *ctx, size_t size)
{

	if(ctx == NULL){

		return NXS_REST_API_E_PTR;
	}

	if(ctx->ev_httpd == NULL){

		return NXS_REST_API_E_INIT;
	}

	evhttp_set_max_headers_size(ctx->ev_httpd, (ssize_t)size);

	return NXS_REST_API_E_OK;
}

nxs_rest_api_err_t nxs_rest_api_set_max_body_size(nxs_rest_api_ctx_t *ctx, size_t size)
{

	if(ctx == NULL){

		return NXS_REST_API_E_PTR;
	}

	if(ctx->ev_httpd == NULL){

		return NXS_REST_API_E_INIT;
	}

	evhttp_set_max_body_size(ctx->ev_httpd, (ssize_t)size);

	return NXS_REST_API_E_OK;
}

nxs_rest_api_common_cmd_type_t nxs_rest_api_get_req_type(nxs_rest_api_request_t *req)
{

	if(req == NULL){

		return NXS_REST_API_COMMON_CMD_UNKNOWN;
	}

	return req->cmd_type;
}

nxs_string_t *nxs_rest_api_get_req_part(nxs_rest_api_request_t *req, size_t _index)
{

	if(req == NULL){

		return NULL;
	}

	if(_index > nxs_array_count(&req->uri_in_parts) - 1){

		return NULL;
	}

	return nxs_array_get(&req->uri_in_parts, _index);
}

nxs_string_t *nxs_rest_api_get_req_args_key(nxs_rest_api_request_t *req, size_t _index)
{
	nxs_rest_api_keyval_t	*kv;

	if(req == NULL){

		return NULL;
	}

	if(_index > nxs_array_count(&req->uri_in_args) - 1){

		return NULL;
	}

	kv = nxs_array_get(&req->uri_in_args, _index);

	return &kv->key;
}

nxs_string_t *nxs_rest_api_get_req_args_val(nxs_rest_api_request_t *req, size_t _index)
{
	nxs_rest_api_keyval_t	*kv;

	if(req == NULL){

		return NULL;
	}

	if(_index > nxs_array_count(&req->uri_in_args) - 1){

		return NULL;
	}

	kv = nxs_array_get(&req->uri_in_args, _index);

	return &kv->key;
}

nxs_string_t *nxs_rest_api_get_req_args_find(nxs_rest_api_request_t *req, nxs_string_t *key)
{
	nxs_rest_api_keyval_t	*kv;
	size_t					i;

	if(req == NULL){

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&req->uri_in_args); i++){

		kv = nxs_array_get(&req->uri_in_args, i);

		if(nxs_string_cmp(key, 0, &kv->key, 0) == NXS_STRING_CMP_EQ){

			return &kv->value;
		}
	}

	return NULL;
}

nxs_string_t *nxs_rest_api_get_req_headers_key(nxs_rest_api_request_t *req, size_t _index)
{
	nxs_rest_api_keyval_t	*kv;

	if(req == NULL){

		return NULL;
	}

	if(_index > nxs_array_count(&req->uri_in_headers) - 1){

		return NULL;
	}

	kv = nxs_array_get(&req->uri_in_headers, _index);

	return &kv->key;
}

nxs_string_t *nxs_rest_api_get_req_headers_val(nxs_rest_api_request_t *req, size_t _index)
{
	nxs_rest_api_keyval_t	*kv;

	if(req == NULL){

		return NULL;
	}

	if(_index > nxs_array_count(&req->uri_in_headers) - 1){

		return NULL;
	}

	kv = nxs_array_get(&req->uri_in_headers, _index);

	return &kv->key;
}

nxs_string_t *nxs_rest_api_get_req_headers_find(nxs_rest_api_request_t *req, nxs_string_t *key)
{
	nxs_rest_api_keyval_t	*kv;
	size_t					i;

	if(req == NULL){

		return NULL;
	}

	for(i = 0; i < nxs_array_count(&req->uri_in_headers); i++){

		kv = nxs_array_get(&req->uri_in_headers, i);

		if(nxs_string_cmp(key, 0, &kv->key, 0) == NXS_STRING_CMP_EQ){

			return &kv->value;
		}
	}

	return NULL;
}

nxs_buf_t *nxs_rest_api_get_req_body(nxs_rest_api_request_t *req)
{

	if(req == NULL){

		return NULL;
	}

	return &req->body;
}

nxs_buf_t *nxs_rest_api_get_out_buf(nxs_rest_api_request_t *req)
{

	if(req == NULL){

		return NULL;
	}

	return &req->out_buf;
}

nxs_string_t *nxs_rest_api_get_err_string(nxs_rest_api_ctx_t *ctx)
{

	if(ctx == NULL){

		return NULL;
	}

	return &ctx->error_text;
}

void nxs_rest_api_page_std_error(nxs_rest_api_request_t *req, nxs_rest_api_format_err_t format, int http_status, u_char *middle_text)
{
	u_char			*err_text;
	nxs_string_t	_s_header_ct_key = nxs_string("Content-Type"),
					_s_header_ct_val_json = nxs_string("application/json; charset=utf-8"),
					_s_header_ct_val_html = nxs_string("text/html; charset=utf-8");

	err_text = nxs_rest_api_common_http_code_to_text(http_status);

	switch(format){

		case NXS_REST_API_FORMAT_ERR_HTML:

			nxs_rest_api_header_add(req, &_s_header_ct_key, &_s_header_ct_val_html);

			nxs_string_printf_dyn((nxs_string_t *)&req->out_buf, "<html>" \
															"<head><title>%d %s</title></head>" \
															"<body bgcolor=\"white\">" \
															"<center><h2>%d %s</h2></center>" \
															"<center><h3>%s</h3></center>"
															"<center><sub>nxs-fw</sub></center></body>" \
															"</html>", http_status, err_text, http_status, err_text, middle_text);

			break;

		case NXS_REST_API_FORMAT_ERR_JSON:

			nxs_rest_api_header_add(req, &_s_header_ct_key, &_s_header_ct_val_json);

			nxs_string_printf_dyn((nxs_string_t *)&req->out_buf, "{" \
															"\"code\": %d," \
															"\"error_text\": \"%s\","
															"\"advanced_text\": \"%s\"" \
															"}", http_status, err_text, middle_text);

			break;

		default:

			break;
	}
}

static void nxs_rest_api_request_init(nxs_rest_api_request_t *req)
{

	req->cmd_type = NXS_REST_API_COMMON_CMD_UNKNOWN;

	nxs_buf_init(&req->body, 1);
	nxs_buf_init_string(&req->out_buf, NXS_STRING_EMPTY_STR);

	nxs_array_init(&req->uri_in_parts, 0, sizeof(nxs_string_t), 1);
	nxs_array_init(&req->uri_in_args, 0, sizeof(nxs_rest_api_keyval_t), 1);
	nxs_array_init(&req->uri_in_headers, 0, sizeof(nxs_rest_api_keyval_t), 1);
	nxs_array_init(&req->uri_out_headers, 0, sizeof(nxs_rest_api_keyval_t), 1);
}

static void nxs_rest_api_request_free(nxs_rest_api_request_t *req)
{
	size_t							i;
	nxs_string_t					*s;
	nxs_rest_api_keyval_t			*kv;

	req->cmd_type = NXS_REST_API_COMMON_CMD_UNKNOWN;

	nxs_buf_free(&req->body);
	nxs_buf_free(&req->out_buf);

	for(i = 0; i < nxs_array_count(&req->uri_in_parts); i++){

		s = nxs_array_get(&req->uri_in_parts, i);

		nxs_string_free(s);
	}

	for(i = 0; i < nxs_array_count(&req->uri_in_args); i++){

		kv = nxs_array_get(&req->uri_in_args, i);

		nxs_string_free(&kv->key);
		nxs_string_free(&kv->value);
	}

	for(i = 0; i < nxs_array_count(&req->uri_in_headers); i++){

		kv = nxs_array_get(&req->uri_in_headers, i);

		nxs_string_free(&kv->key);
		nxs_string_free(&kv->value);
	}

	for(i = 0; i < nxs_array_count(&req->uri_out_headers); i++){

		kv = nxs_array_get(&req->uri_out_headers, i);

		nxs_string_free(&kv->key);
		nxs_string_free(&kv->value);
	}

	nxs_array_free(&req->uri_in_parts);
	nxs_array_free(&req->uri_in_args);
	nxs_array_free(&req->uri_in_headers);
	nxs_array_free(&req->uri_out_headers);
}

static void nxs_rest_api_generic_handler(struct evhttp_request *_req, void *arg)
{
	nxs_rest_api_request_t	req;
	nxs_rest_api_ctx_t		*ctx = arg;
	nxs_rest_api_keyval_t	*kv;

	size_t					body_len, i;
	struct evbuffer 		*in_evbuf, *out_evbuf;

	struct evhttp_uri		*decoded = NULL;
	const char				*uri, *path, *args;
	char					*decoded_path = NULL, *decoded_args = NULL;

	int						http_status;

	nxs_rest_api_request_init(&req);

	nxs_string_clear(&ctx->error_text);

	/*
	 * Query type
	 */

	http_status = NXS_HTTP_400_BAD_REQUEST;

	switch(evhttp_request_get_command(_req)){

		case EVHTTP_REQ_GET:		req.cmd_type = NXS_REST_API_COMMON_CMD_GET;				break;
		case EVHTTP_REQ_POST:		req.cmd_type = NXS_REST_API_COMMON_CMD_POST;			break;
		case EVHTTP_REQ_HEAD:		req.cmd_type = NXS_REST_API_COMMON_CMD_HEAD;			break;
		case EVHTTP_REQ_PUT:		req.cmd_type = NXS_REST_API_COMMON_CMD_PUT;				break;
		case EVHTTP_REQ_DELETE:		req.cmd_type = NXS_REST_API_COMMON_CMD_DELETE;			break;
		case EVHTTP_REQ_OPTIONS:	req.cmd_type = NXS_REST_API_COMMON_CMD_OPTIONS; 		break;
		case EVHTTP_REQ_TRACE:		req.cmd_type = NXS_REST_API_COMMON_CMD_TRACE;			break;
		case EVHTTP_REQ_CONNECT:	req.cmd_type = NXS_REST_API_COMMON_CMD_CONNECT;			break;
		case EVHTTP_REQ_PATCH:		req.cmd_type = NXS_REST_API_COMMON_CMD_PATCH;			break;

		default:

			nxs_string_printf_dyn(&ctx->error_text, "wrong request type");

			nxs_rest_api_page_std_error(&req, ctx->format_type, NXS_HTTP_405_METHOD_NOT_ALLOWED, nxs_string_str(&ctx->error_text));

			http_status = NXS_HTTP_405_METHOD_NOT_ALLOWED;

			break;
	}

	/*
	 * Body
	 */

	in_evbuf = evhttp_request_get_input_buffer(_req);
	evbuffer_add(in_evbuf, "", 1);
	body_len = evbuffer_get_length(in_evbuf);

	nxs_buf_resize(&req.body, body_len);

	evbuffer_copyout(in_evbuf, nxs_buf_get_subbuf(&req.body, 0), body_len);

	nxs_buf_set_len(&req.body, body_len);

	/*
	 * Uri parts
	 */

	uri = evhttp_request_get_uri(_req);

	decoded = evhttp_uri_parse_with_flags(uri, EVHTTP_URI_NONCONFORMANT);

	if(decoded == NULL){

		nxs_string_printf_dyn(&ctx->error_text, "can't parse uri, internal server error");

		nxs_rest_api_page_std_error(&req, ctx->format_type, NXS_HTTP_500_INTERNAL_SERVER_ERROR, nxs_string_str(&ctx->error_text));
		http_status = NXS_HTTP_500_INTERNAL_SERVER_ERROR;
		goto error;
	}

	if((path = evhttp_uri_get_path(decoded)) == NULL){

		path = "/";
	}

	if((decoded_path = evhttp_uridecode(path, 0, NULL)) == NULL){

		nxs_string_printf_dyn(&ctx->error_text, "can't decode uri, internal server error");

		nxs_rest_api_page_std_error(&req, ctx->format_type, NXS_HTTP_500_INTERNAL_SERVER_ERROR, nxs_string_str(&ctx->error_text));
		http_status = NXS_HTTP_500_INTERNAL_SERVER_ERROR;
		goto error;
	}

	if(strstr(decoded_path, "..") != NULL){

		nxs_string_printf_dyn(&ctx->error_text, "not allowed use '..' characters sequence in uri, bad request");

		nxs_rest_api_page_std_error(&req, ctx->format_type, NXS_HTTP_400_BAD_REQUEST, nxs_string_str(&ctx->error_text));
		http_status = NXS_HTTP_400_BAD_REQUEST;
		goto error;
	}

	nxs_rest_api_parse_uri(&req.uri_in_parts, decoded_path);

	/*
	 * Uri args
	 */

	if((args = evhttp_uri_get_query(decoded)) != NULL){

		decoded_args = evhttp_uridecode(args, 0, NULL);

		if(nxs_rest_api_parse_args(&req.uri_in_args, decoded_args) != NXS_REST_API_E_OK){

			nxs_string_printf_dyn(&ctx->error_text, "can't parse args, bad request");

			nxs_rest_api_page_std_error(&req, ctx->format_type, NXS_HTTP_400_BAD_REQUEST, nxs_string_str(&ctx->error_text));
			http_status = NXS_HTTP_400_BAD_REQUEST;
			goto error;
		}
	}

	/*
	 * Uri headers
	 */

	nxs_rest_api_parse_headers(&req.uri_in_headers, _req);

	/*
	 * Handler exec
	 */

	switch(nxs_rest_api_handler_exec(ctx, &req, &http_status)){

		case NXS_REST_API_E_INIT:

			nxs_string_printf_dyn(&ctx->error_text, "no handlers was specified, internal server error");

			nxs_rest_api_page_std_error(&req, ctx->format_type, NXS_HTTP_500_INTERNAL_SERVER_ERROR, nxs_string_str(&ctx->error_text));
			http_status = NXS_HTTP_500_INTERNAL_SERVER_ERROR;
			goto error;

		case NXS_REST_API_E_EMPTY:

			nxs_string_printf_dyn(&ctx->error_text, "request string is too short, bad request");

			nxs_rest_api_page_std_error(&req, ctx->format_type, NXS_HTTP_400_BAD_REQUEST, nxs_string_str(&ctx->error_text));
			http_status = NXS_HTTP_400_BAD_REQUEST;
			goto error;

		case NXS_REST_API_E_ERR:

			nxs_string_printf_dyn(&ctx->error_text, "handler not found for query");

			nxs_rest_api_page_std_error(&req, ctx->format_type, NXS_HTTP_404_NOT_FOUND, nxs_string_str(&ctx->error_text));
			http_status = NXS_HTTP_404_NOT_FOUND;
			goto error;

		default:

			break;
	}

error:

	/* Подготавливаем заголовки к отправке */

	for(i = 0; i < nxs_array_count(&req.uri_out_headers); i++){

		kv = nxs_array_get(&req.uri_out_headers, i);

		evhttp_add_header(evhttp_request_get_output_headers(_req), (char *)nxs_string_str(&kv->key), (char *)nxs_string_str(&kv->value));
	}

	/* Подготавливаем тело ответа к отправке */

	out_evbuf = evbuffer_new();
	evbuffer_add_printf(out_evbuf, "%s", (char *)nxs_buf_get_subbuf(&req.out_buf, 0));
	evhttp_send_reply(_req, http_status, NULL, out_evbuf);
	evbuffer_free(out_evbuf);

	/* Освобождаем память */

	nxs_rest_api_request_free(&req);

	if(decoded != NULL){

		evhttp_uri_free(decoded);
	}

	if(decoded_path != NULL){

		decoded_path = nxs_free(decoded_path);
	}

	if(decoded_args != NULL){

		decoded_args = nxs_free(decoded_args);
	}
}

static void nxs_rest_api_parse_uri(nxs_array_t *uri_pats, char *decoded_path)
{
	size_t			i, offset;
	nxs_string_t	*p;

	for(offset = 0, i = 0; decoded_path[i] != '\0'; i++){

		if(decoded_path[i] == '/' && i > 0){

			p = nxs_array_add(uri_pats);

			nxs_string_init(p);

			nxs_string_char_ncpy_dyn(p, 0, (u_char *)decoded_path + offset, i - offset);

			offset = i;
		}
	}

	if(i - offset > 1){

		p = nxs_array_add(uri_pats);

		nxs_string_init(p);

		nxs_string_char_ncpy_dyn(p, 0, (u_char *)decoded_path + offset, i - offset);
	}
}

static nxs_rest_api_err_t nxs_rest_api_parse_args(nxs_array_t *uri_args, char *args_str)
{
	struct evkeyvalq				args;
	struct evkeyval					*arg;
	nxs_rest_api_keyval_t			*kv;

	if(evhttp_parse_query_str(args_str, &args) != 0){

		return NXS_REST_API_E_ERR;
	}

	for(arg = args.tqh_first; arg != NULL; arg = arg->next.tqe_next){

		kv = nxs_array_add(uri_args);

		nxs_string_init2(&kv->key, 0, (u_char *)arg->key);
		nxs_string_init2(&kv->value, 0, (u_char *)arg->value);
	}

	evhttp_clear_headers(&args);

	return NXS_REST_API_E_OK;
}

/*
 * Подсказка на случай, если будет необходимо парсить аргументы, содержащиеся в body (например, для POST)
static nxs_rest_api_err_t nxs_rest_api_args_parse(nxs_array_t *uri_in_args, struct evbuffer *in_evbuf)
{
	char							*args_str;
	struct evkeyvalq				args;
	struct evkeyval					*arg;
	nxs_rest_api_keyval_t			*kv;

	args_str = (char *)evbuffer_pullup(in_evbuf, -1);

	if(evhttp_parse_query_str(args_str, &args) != 0){

		printf ("nxs_rest_api: can't parse args, bad request\n");

		return NXS_REST_API_E_OK;
	}

	for(arg = args.tqh_first; arg != NULL; arg = arg->next.tqe_next){

		kv = nxs_array_add(uri_in_args);

		nxs_string_init2(&kv->key, 0, (u_char *)arg->key);
		nxs_string_init2(&kv->value, 0, (u_char *)arg->value);
	}

	evhttp_clear_headers(&args);

	return NXS_REST_API_E_OK;
}*/

static void nxs_rest_api_parse_headers(nxs_array_t *uri_headers, struct evhttp_request *req)
{
	struct evkeyvalq				*headers;
	struct evkeyval					*header;
	nxs_rest_api_keyval_t			*kv;

	if((headers = evhttp_request_get_input_headers(req)) == NULL){

		return;
	}

	for(header = headers->tqh_first; header != NULL; header = header->next.tqe_next){

		kv = nxs_array_add(uri_headers);

		nxs_string_init2(&kv->key, 0, (u_char *)header->key);
		nxs_string_init2(&kv->value, 0, (u_char *)header->value);
	}
}

static nxs_rest_api_err_t nxs_rest_api_handler_exec(nxs_rest_api_ctx_t *ctx, nxs_rest_api_request_t *req, int *http_status)
{
	nxs_rest_api_handler_t			*h;
	nxs_string_t				*r_h, uri;
	nxs_rest_api_common_cmd_type_t		r_type;
	size_t					part_count, i;
	nxs_rest_api_err_t			rc;

	if(nxs_list_count(&ctx->handlers) == 0){

		return NXS_REST_API_E_INIT;
	}

	if((part_count = nxs_array_count(&req->uri_in_parts)) == 0){

		return NXS_REST_API_E_EMPTY;
	}

	rc = NXS_REST_API_E_ERR;

	nxs_string_init(&uri);

	r_type = req->cmd_type;

	for(i = 0; i < part_count; i++){

		r_h = nxs_array_get(&req->uri_in_parts, i);

		nxs_string_cat_dyn(&uri, r_h);

		for(h = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &ctx->handlers); h != NULL; h = nxs_list_ptr_next(&ctx->handlers)){

			if(nxs_string_cmp(&h->handler_name, 0, &uri, 0) == NXS_STRING_CMP_EQ && r_type == h->cmd_type){

				if(h->fixed_name == NXS_YES && (part_count - i) > 1){

					continue;
				}

				*http_status = h->handl_func(ctx, req, h->user_ctx);

				rc = NXS_REST_API_E_OK;
				goto error;
			}
		}
	}

error:

	nxs_string_free(&uri);

	return rc;
}

static void nxs_rest_api_handler_free(nxs_rest_api_ctx_t *ctx)
{
	nxs_rest_api_handler_t	*h;

	for(h = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &ctx->handlers); h != NULL; h = nxs_list_del(&ctx->handlers, NXS_LIST_MOVE_NEXT)){

		nxs_string_free(&h->handler_name);
	}

	nxs_list_free(&ctx->handlers);
}

static nxs_rest_api_err_t nxs_rest_api_setup_certs(nxs_rest_api_ctx_t *ctx, nxs_string_t *crt_chain, nxs_string_t *crt_pkey)
{

	if(SSL_CTX_use_certificate_chain_file(ctx->ssl, (char *)nxs_string_str(crt_chain)) != 1){

		return NXS_REST_API_E_SSL_USE_CHAIN_FILE;
	}

	if(SSL_CTX_use_PrivateKey_file(ctx->ssl, (char *)nxs_string_str(crt_pkey), SSL_FILETYPE_PEM) != 1){

		return NXS_REST_API_E_SSL_USE_PKEY_FILE;
	}

	if(SSL_CTX_check_private_key(ctx->ssl) != 1){

		return NXS_REST_API_E_SSL_CHECK_PKEY_FILE;
	}

	return NXS_REST_API_E_OK;
}

static struct bufferevent *nxs_rest_api_bevcb(struct event_base *base, void *arg)
{
	struct bufferevent		*r;
	SSL_CTX					*ctx = (SSL_CTX *)arg;

	r = bufferevent_openssl_socket_new(base, -1, SSL_new(ctx), BUFFEREVENT_SSL_ACCEPTING, BEV_OPT_CLOSE_ON_FREE);

	return r;
}
