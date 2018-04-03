// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */



/* Module typedefs */



/* Module declarations */

typedef struct
{
	nxs_curl_t		*curl_ctx;
	nxs_string_t		*file_save_path;
	int			fd;
	nxs_process_t		*proc;
} nxs_curl_download_t;

typedef struct
{
	nxs_curl_t		*curl_ctx;
	nxs_string_t		*file_read_path;
	int			fd;
	nxs_process_t		*proc;
} nxs_curl_upload_t;


/* Module internal (static) functions prototypes */

// clang-format on

static CURL * nxs_curl_q_init(void);
static size_t nxs_curl_q_get_data(void *buffer, size_t size, size_t nmemb, void *r);
static size_t nxs_curl_q_download_file(void *buffer, size_t size, size_t nmemb, void *r);
static size_t nxs_curl_q_upload_file(void *buffer, size_t size, size_t nmemb, void *r);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

void nxs_curl_init(nxs_curl_t *curl)
{

	if(curl == NULL) {

		return;
	}

	nxs_array_init(&curl->headers, 0, sizeof(nxs_string_t), 1, NULL, NULL);

	nxs_buf_init(&curl->out_buf, 1);
	nxs_buf_init(&curl->post_data, 1);

	curl->ssl_verifyhost = NXS_YES;
	curl->ret_code       = NXS_HTTP_CODE_UNKNOWN;
	curl->log_type       = NXS_CURL_LOG_ERROR;

	curl->proxy.use_proxy = NXS_NO;
	curl->proxy.type      = 0;
	curl->proxy.auth_type = CURLAUTH_NONE;

	nxs_string_init_empty(&curl->proxy.host_data);
	nxs_string_init_empty(&curl->proxy.auth_data);
}

void nxs_curl_free(nxs_curl_t *curl)
{
	size_t        i;
	nxs_string_t *h;

	if(curl == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(&curl->headers); i++) {

		h = nxs_array_get(&curl->headers, i);

		nxs_string_free(h);
	}

	nxs_array_free(&curl->headers);

	nxs_buf_free(&curl->out_buf);
	nxs_buf_free(&curl->post_data);

	curl->ssl_verifyhost = NXS_YES;
	curl->ret_code       = NXS_HTTP_CODE_UNKNOWN;
	curl->log_type       = NXS_CURL_LOG_ERROR;

	curl->proxy.use_proxy = NXS_NO;
	curl->proxy.type      = 0;
	curl->proxy.auth_type = CURLAUTH_NONE;

	nxs_string_free(&curl->proxy.host_data);
	nxs_string_free(&curl->proxy.auth_data);
}

void nxs_curl_add_header(nxs_curl_t *curl, nxs_string_t *header)
{
	nxs_string_t *h;

	if(curl == NULL) {

		return;
	}

	h = nxs_array_add(&curl->headers);

	nxs_string_init3(h, header);
}

void nxs_curl_add_header2(nxs_curl_t *curl, u_char *header)
{
	nxs_string_t *h;

	if(curl == NULL) {

		return;
	}

	h = nxs_array_add(&curl->headers);

	nxs_string_init2(h, 0, header);
}

void nxs_curl_set_post(nxs_curl_t *curl, nxs_buf_t *post_data)
{

	if(curl == NULL) {

		return;
	}

	nxs_buf_clone(&curl->post_data, post_data);
}

void nxs_curl_set_post2(nxs_curl_t *curl, nxs_string_t *post_data)
{

	if(curl == NULL) {

		return;
	}

	nxs_string_to_buf(post_data, 0, &curl->post_data);
}

void nxs_curl_set_ssl_verivyhost(nxs_curl_t *curl, nxs_bool_t ssl_verivyhost)
{

	if(curl == NULL) {

		return;
	}

	curl->ssl_verifyhost = ssl_verivyhost;
}

nxs_curl_err_t nxs_curl_set_proxy(nxs_process_t *            proc,
                                  nxs_curl_t *               curl,
                                  nxs_curl_proxy_type_t      proxy_type,
                                  nxs_curl_proxy_auth_type_t proxy_auth_type,
                                  nxs_string_t *             proxy_host,
                                  uint16_t                   proxy_port,
                                  nxs_string_t *             username,
                                  nxs_string_t *             password)
{
	nxs_curl_err_t rc;

	if(curl == NULL) {

		return NXS_CURL_E_PTR;
	}

	rc = NXS_CURL_E_OK;

	curl->proxy.use_proxy = NXS_NO;
	curl->proxy.type      = 0;
	curl->proxy.auth_type = CURLAUTH_NONE;

	nxs_string_clear(&curl->proxy.host_data);
	nxs_string_clear(&curl->proxy.auth_data);

	switch(proxy_type) {

		case NXS_CURL_PROXY_TYPE_NONE:

			return NXS_CURL_E_OK;

		case NXS_CURL_PROXY_TYPE_HTTP:

			curl->proxy.type = CURLPROXY_HTTP;

			break;

		case NXS_CURL_PROXY_TYPE_HTTP_1_0:

			curl->proxy.type = CURLPROXY_HTTP_1_0;

			break;

		case NXS_CURL_PROXY_TYPE_SOCKS4:

			curl->proxy.type = CURLPROXY_SOCKS4;

			break;

		case NXS_CURL_PROXY_TYPE_SOCKS5:

			curl->proxy.type = CURLPROXY_SOCKS5;

			break;

		case NXS_CURL_PROXY_TYPE_SOCKS4A:

			curl->proxy.type = CURLPROXY_SOCKS4A;

			break;

		case NXS_CURL_PROXY_TYPE_SOCKS5_HOSTNAME:

			curl->proxy.type = CURLPROXY_SOCKS5_HOSTNAME;

			break;

		default:

			/* For unknown proxytypes */

			nxs_log_write_debug(proc, "curl proxy error: uknown proxy type (proxy type: %d)", proxy_type);

			nxs_error(rc, NXS_CURL_E_ERR, error);
	}

	/* Proxy host data */

	if(nxs_string_len(proxy_host) == 0) {

		nxs_log_write_debug(proc, "curl proxy error: empty host");

		nxs_error(rc, NXS_CURL_E_ERR, error);
	}

	if(proxy_port == 0) {

		nxs_log_write_debug(proc, "curl proxy error: empty port");

		nxs_error(rc, NXS_CURL_E_ERR, error);
	}

	nxs_string_printf(&curl->proxy.host_data, "%r:%d", proxy_host, proxy_port);

	/* Proxy auth data */

	if((proxy_auth_type & NXS_CURL_PROXY_AUTH_TYPE_BASIC) == NXS_CURL_PROXY_AUTH_TYPE_BASIC) {

		curl->proxy.auth_type |= CURLAUTH_BASIC;
	}

	if((proxy_auth_type & NXS_CURL_PROXY_AUTH_TYPE_DIGEST) == NXS_CURL_PROXY_AUTH_TYPE_DIGEST) {

		curl->proxy.auth_type |= CURLAUTH_DIGEST;
	}

	/* Enable this after end of Debian 7 support */
	/*
	if((proxy_auth_type & NXS_CURL_PROXY_AUTH_TYPE_NEGOTIATE) == NXS_CURL_PROXY_AUTH_TYPE_NEGOTIATE) {

	        curl->proxy.auth_type |= CURLAUTH_NEGOTIATE;
	}
	*/

	if((proxy_auth_type & NXS_CURL_PROXY_AUTH_TYPE_NTLM) == NXS_CURL_PROXY_AUTH_TYPE_NTLM) {

		curl->proxy.auth_type |= CURLAUTH_NTLM;
	}

	if(curl->proxy.auth_type > 0) {

		/* Proxy username and password */

		if(nxs_string_len(username) == 0) {

			nxs_log_write_debug(proc, "curl proxy error: empty user name for auth");

			nxs_error(rc, NXS_CURL_E_ERR, error);
		}
		else {

			nxs_string_printf(&curl->proxy.auth_data, "%r", username);
		}

		if(nxs_string_len(password) > 0) {

			nxs_string_printf2_cat(&curl->proxy.auth_data, ":%r", password);
		}
	}

error:

	if(rc == NXS_CURL_E_OK) {

		curl->proxy.use_proxy = NXS_YES;
	}
	else {

		curl->proxy.use_proxy = NXS_NO;
		curl->proxy.type      = 0;
		curl->proxy.auth_type = CURLAUTH_NONE;

		nxs_string_clear(&curl->proxy.host_data);
		nxs_string_clear(&curl->proxy.auth_data);
	}

	return rc;
}

void nxs_curl_set_debug(nxs_curl_t *curl, nxs_curl_log_t log_type)
{

	if(curl == NULL) {

		return;
	}

	curl->log_type = log_type;
}

nxs_http_code_t nxs_curl_get_ret_code(nxs_curl_t *curl)
{

	if(curl == NULL) {

		return NXS_HTTP_CODE_UNKNOWN;
	}

	return curl->ret_code;
}

nxs_buf_t *nxs_curl_get_out_buf(nxs_curl_t *curl)
{

	if(curl == NULL) {

		return NULL;
	}

	return &curl->out_buf;
}

nxs_curl_err_t nxs_curl_query(nxs_process_t *proc, nxs_curl_t *curl, nxs_rest_api_common_cmd_type_t method, u_char *query_str, ...)
{
	va_list            va_query_str;
	nxs_string_t       query;
	CURL *             c;
	CURLcode           res;
	struct curl_slist *h_list    = NULL;
	long int           http_code = 0;
	nxs_string_t *     h;
	size_t             i;
	nxs_curl_err_t     rc;

	if(curl == NULL || query_str == NULL) {

		return NXS_CURL_E_PTR;
	}

	if((c = nxs_curl_q_init()) == NULL) {

		return NXS_CURL_E_INIT;
	}

	rc = NXS_CURL_E_OK;

	nxs_string_init(&query);

	va_start(va_query_str, query_str);
	nxs_string_vprintf(&query, (char *)query_str, va_query_str);
	va_end(va_query_str);

	if(nxs_array_count(&curl->headers) > 0) {

		for(i = 0; i < nxs_array_count(&curl->headers); i++) {

			h = nxs_array_get(&curl->headers, i);

			h_list = curl_slist_append(h_list, (char *)nxs_string_str(h));
		}

		curl_easy_setopt(c, CURLOPT_HTTPHEADER, h_list);
	}

	if(nxs_buf_get_len(&curl->post_data) > 0) {

		curl_easy_setopt(c, CURLOPT_POSTFIELDSIZE, nxs_buf_get_len(&curl->post_data));
		curl_easy_setopt(c, CURLOPT_POSTFIELDS, (char *)nxs_buf_get_subbuf(&curl->post_data, 0));
	}

	curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, nxs_rest_api_common_type_to_text(method));
	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, nxs_curl_q_get_data);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, &curl->out_buf);
	curl_easy_setopt(c, CURLOPT_URL, nxs_string_str(&query));

	/* Set up proxy */
	nxs_curl_opt_set_proxy(curl, c);

	if(curl->ssl_verifyhost == NXS_NO) {

		curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 0);
	}

	res = curl_easy_perform(c);

	nxs_buf_add_char(&curl->out_buf, (u_char)'\0');

	if(res != CURLE_OK) {

		if(curl->log_type & NXS_CURL_LOG_ERROR) {

			nxs_log_write_debug(proc,
			                    "failed to process curl query get: %s (uri: \"%s\")",
			                    curl_easy_strerror(res),
			                    nxs_string_str(&query));
		}

		nxs_error(rc, NXS_CURL_E_ERR, error);
	}

	curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code);

	curl->ret_code = (nxs_http_code_t)http_code;

	if(curl->log_type & NXS_CURL_LOG_DEBUG) {

		nxs_log_write_debug(proc,
		                    "got data from server (response code: %d, request url: \"%s\", response body: \"%s\")",
		                    curl->ret_code,
		                    nxs_string_str(&query),
		                    nxs_buf_get_subbuf(&curl->out_buf, 0));
	}

error:

	nxs_string_free(&query);

	curl_easy_cleanup(c);
	curl_slist_free_all(h_list);

	return rc;
}

nxs_curl_err_t nxs_curl_download(nxs_process_t *                proc,
                                 nxs_curl_t *                   curl,
                                 nxs_rest_api_common_cmd_type_t method,
                                 nxs_string_t *                 file_path,
                                 mode_t                         file_mode,
                                 u_char *                       query_str,
                                 ...)
{
	va_list             va_query_str;
	nxs_string_t        query;
	CURL *              c;
	CURLcode            res;
	struct curl_slist * h_list    = NULL;
	long int            http_code = 0;
	nxs_string_t *      h;
	size_t              i;
	nxs_curl_err_t      rc;
	nxs_curl_download_t curl_download;
	int                 fd;

	if(curl == NULL || query_str == NULL || file_path == NULL) {

		return NXS_CURL_E_PTR;
	}

	if((c = nxs_curl_q_init()) == NULL) {

		return NXS_CURL_E_INIT;
	}

	if((fd = nxs_fs_open(file_path, O_WRONLY | O_CREAT | O_TRUNC, file_mode)) < 0) {

		if(curl->log_type & NXS_CURL_LOG_ERROR) {

			nxs_log_write_debug(
			        proc, "curl download file fail, open file error: %s (file path: \"%r\")", strerror(errno), file_path);
		}

		return NXS_CURL_E_ERR;
	}

	rc = NXS_CURL_E_OK;

	curl_download.curl_ctx       = curl;
	curl_download.file_save_path = file_path;
	curl_download.fd             = fd;
	curl_download.proc           = proc;

	nxs_buf_clear(&curl->out_buf);
	nxs_buf_add_char(&curl->out_buf, (u_char)'\0');

	nxs_string_init(&query);

	va_start(va_query_str, query_str);
	nxs_string_vprintf(&query, (char *)query_str, va_query_str);
	va_end(va_query_str);

	if(nxs_array_count(&curl->headers) > 0) {

		for(i = 0; i < nxs_array_count(&curl->headers); i++) {

			h = nxs_array_get(&curl->headers, i);

			h_list = curl_slist_append(h_list, (char *)nxs_string_str(h));
		}

		curl_easy_setopt(c, CURLOPT_HTTPHEADER, h_list);
	}

	if(nxs_buf_get_len(&curl->post_data) > 0) {

		curl_easy_setopt(c, CURLOPT_POSTFIELDSIZE, nxs_buf_get_len(&curl->post_data));
		curl_easy_setopt(c, CURLOPT_POSTFIELDS, (char *)nxs_buf_get_subbuf(&curl->post_data, 0));
	}

	curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, nxs_rest_api_common_type_to_text(method));
	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, nxs_curl_q_download_file);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, &curl_download);
	curl_easy_setopt(c, CURLOPT_URL, nxs_string_str(&query));

	/* Set up proxy */
	nxs_curl_opt_set_proxy(curl, c);

	if(curl->ssl_verifyhost == NXS_NO) {

		curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 0);
	}

	res = curl_easy_perform(c);

	if(res != CURLE_OK) {

		if(curl->log_type & NXS_CURL_LOG_ERROR) {

			nxs_log_write_debug(
			        proc, "curl download file fail, query get error: %s (uri: \"%r\")", curl_easy_strerror(res), &query);
		}

		nxs_error(rc, NXS_CURL_E_ERR, error);
	}

	curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code);

	curl->ret_code = (nxs_http_code_t)http_code;

	if(curl->log_type & NXS_CURL_LOG_DEBUG) {

		nxs_log_write_debug(proc,
		                    "curl file downloaded (response code: %d, request url: \"%r\", file path: \"%r\")",
		                    curl->ret_code,
		                    &query,
		                    file_path);
	}

error:

	nxs_fs_close(fd);

	nxs_string_free(&query);

	curl_easy_cleanup(c);
	curl_slist_free_all(h_list);

	return rc;
}

nxs_curl_err_t nxs_curl_upload(nxs_process_t *                proc,
                               nxs_curl_t *                   curl,
                               nxs_rest_api_common_cmd_type_t method,
                               nxs_string_t *                 file_path,
                               u_char *                       query_str,
                               ...)
{
	va_list            va_query_str;
	nxs_string_t       query;
	CURL *             c;
	CURLcode           res;
	struct curl_slist *h_list    = NULL;
	long int           http_code = 0;
	nxs_string_t *     h;
	size_t             i;
	nxs_curl_err_t     rc;
	nxs_fs_stat_t      file_stat;
	nxs_curl_upload_t  curl_upload;
	int                fd;

	if(curl == NULL || query_str == NULL || file_path == NULL) {

		return NXS_CURL_E_PTR;
	}

	if((c = nxs_curl_q_init()) == NULL) {

		return NXS_CURL_E_INIT;
	}

	if(nxs_fs_stat(file_path, &file_stat) < 0) {

		nxs_log_write_warn(proc,
		                   "curl upload file fail, can't execute stat for object: %s (file path: \"%s\")",
		                   strerror(errno),
		                   nxs_string_str(file_path));

		return NXS_CURL_E_ERR;
	}

	if((fd = nxs_fs_open(file_path, O_RDONLY)) < 0) {

		if(curl->log_type & NXS_CURL_LOG_ERROR) {

			nxs_log_write_debug(
			        proc, "curl upload file fail, open file error: %s (file path: \"%r\")", strerror(errno), file_path);
		}

		return NXS_CURL_E_ERR;
	}

	rc = NXS_CURL_E_OK;

	curl_upload.curl_ctx       = curl;
	curl_upload.file_read_path = file_path;
	curl_upload.fd             = fd;
	curl_upload.proc           = proc;

	nxs_string_init(&query);

	va_start(va_query_str, query_str);
	nxs_string_vprintf(&query, (char *)query_str, va_query_str);
	va_end(va_query_str);

	if(nxs_array_count(&curl->headers) > 0) {

		for(i = 0; i < nxs_array_count(&curl->headers); i++) {

			h = nxs_array_get(&curl->headers, i);

			h_list = curl_slist_append(h_list, (char *)nxs_string_str(h));
		}

		curl_easy_setopt(c, CURLOPT_HTTPHEADER, h_list);
	}

	curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, nxs_rest_api_common_type_to_text(method));
	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, nxs_curl_q_get_data);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, &curl->out_buf);
	curl_easy_setopt(c, CURLOPT_READFUNCTION, nxs_curl_q_upload_file);
	curl_easy_setopt(c, CURLOPT_READDATA, &curl_upload);
	curl_easy_setopt(c, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt(c, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_stat.st_size);
	curl_easy_setopt(c, CURLOPT_URL, nxs_string_str(&query));

	/* Set up proxy */
	nxs_curl_opt_set_proxy(curl, c);

	if(curl->ssl_verifyhost == NXS_NO) {

		curl_easy_setopt(c, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(c, CURLOPT_SSL_VERIFYPEER, 0);
	}

	res = curl_easy_perform(c);

	if(res != CURLE_OK) {

		if(curl->log_type & NXS_CURL_LOG_ERROR) {

			nxs_log_write_debug(
			        proc, "curl upload file fail, query get error: %s (uri: \"%r\")", curl_easy_strerror(res), &query);
		}

		nxs_error(rc, NXS_CURL_E_ERR, error);
	}

	curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code);

	curl->ret_code = (nxs_http_code_t)http_code;

	if(curl->log_type & NXS_CURL_LOG_DEBUG) {

		nxs_log_write_debug(proc,
		                    "curl file uploaded (response code: %d, request url: \"%r\", file path: \"%r\")",
		                    curl->ret_code,
		                    &query,
		                    file_path);
	}

error:

	nxs_fs_close(fd);

	nxs_string_free(&query);

	curl_easy_cleanup(c);
	curl_slist_free_all(h_list);

	return rc;
}

/*
 * Set raw curl proxy options
 *
 * Come in handy if some reasons 'nxs_curl' and raw 'curl_easy' used together (e.g. for
 * 'multipart/form-data' which not supported by nxs-fw at the moment)
 */
void nxs_curl_opt_set_proxy(nxs_curl_t *curl, CURL *c)
{

	if(curl == NULL || c == NULL) {

		return;
	}

	if(curl->proxy.use_proxy == NXS_NO) {

		return;
	}

	/* Proxy type */
	curl_easy_setopt(c, CURLOPT_PROXYTYPE, curl->proxy.type);
	curl_easy_setopt(c, CURLOPT_PROXY, (char *)nxs_string_str(&curl->proxy.host_data));

	/* Proxy auth type */
	if(curl->proxy.auth_type > 0) {

		curl_easy_setopt(c, CURLOPT_PROXYAUTH, curl->proxy.auth_type);
		curl_easy_setopt(c, CURLOPT_PROXYUSERPWD, (char *)nxs_string_str(&curl->proxy.auth_data));
	}
	else {

		curl_easy_setopt(c, CURLOPT_PROXYAUTH, CURLAUTH_NONE);
	}
}

/* Module internal (static) functions */

static CURL *nxs_curl_q_init(void)
{
	CURL *curl;

	curl = curl_easy_init();

	if(!curl) {

		return NULL;
	}

	return curl;
}

static size_t nxs_curl_q_get_data(void *buffer, size_t size, size_t nmemb, void *r)
{
	size_t     chunk_size;
	nxs_buf_t *response;

	chunk_size = size * nmemb;
	response   = r;

	nxs_buf_cpy(response, nxs_buf_get_len(response), buffer, chunk_size);

	return chunk_size;
}

static size_t nxs_curl_q_download_file(void *buffer, size_t size, size_t nmemb, void *r)
{
	ssize_t              bc;
	size_t               chunk_size;
	nxs_curl_download_t *curl_download = r;

	chunk_size = size * nmemb;

	if((bc = nxs_fs_write(curl_download->fd, buffer, chunk_size)) < 0) {

		if(curl_download->curl_ctx->log_type & NXS_CURL_LOG_ERROR) {

			nxs_log_write_debug(curl_download->proc,
			                    "curl download file fail, file write error: %s (file path: \"%r\")",
			                    strerror(errno),
			                    curl_download->file_save_path);
		}

		return 0;
	}

	if((size_t)bc != chunk_size) {

		if(curl_download->curl_ctx->log_type & NXS_CURL_LOG_ERROR) {

			nxs_log_write_debug(curl_download->proc,
			                    "curl download file fail, file write error: write less bytes than expected (file path: \"%r\", "
			                    "write bytes: %zu, expected bytes: %zu)",
			                    curl_download->file_save_path,
			                    (size_t)bc,
			                    chunk_size);
		}
	}

	return (size_t)bc;
}

static size_t nxs_curl_q_upload_file(void *buffer, size_t size, size_t nmemb, void *r)
{
	ssize_t            bc;
	size_t             chunk_size;
	nxs_curl_upload_t *curl_upload = r;

	chunk_size = size * nmemb;

	if((bc = nxs_fs_read(curl_upload->fd, buffer, chunk_size)) < 0) {

		if(curl_upload->curl_ctx->log_type & NXS_CURL_LOG_ERROR) {

			nxs_log_write_debug(curl_upload->proc,
			                    "curl upload file fail, file read error: %s (file path: \"%r\")",
			                    strerror(errno),
			                    curl_upload->file_read_path);
		}

		return 0;
	}

	return (size_t)bc;
}
