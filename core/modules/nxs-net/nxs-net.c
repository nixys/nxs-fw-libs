// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define _NXS_NET_TMP_BUF_INIT_SIZE	1024
#define	_NXS_NET_SENDFILE_BUF_SIZE	65536

#define _NXS_NET_TMP_BUF_MAX_SIZE	10485760

#define _NXS_NET_DEFAULT_PERM		0644
#define _NXS_NET_DEFAULT_SOCK_PERM	0755

#define _NXS_NET_SENDFILE_C_CHOWN	"chown error"
#define _NXS_NET_SENDFILE_C_CHMOD	"chmod error"
#define _NXS_NET_SENDFILE_C_UTIME	"utime error"

#define macro_nxs_net_send_file_hdr()																											\
																																				\
			nxs_net_header_set_hdr(con);																										\
			nxs_net_header_set_flag(con, hdr_f);																								\
			nxs_net_header_set_offset(con, offset);																								\
																																				\
			nxs_crypt_gost_buf(&con->tmp_buf, con->gost_el);																					\
																																				\
			if((rc = nxs_net_send_core(proc, con, opt->timeout, nxs_buf_get_subbuf(&con->tmp_buf, 0), nxs_buf_get_len(&con->tmp_buf))) < 0){	\
				ec = rc;																														\
				goto error;																														\
			}

#define macro_nxs_net_send_file_hdr_emrg(__ec__)																								\
																																				\
			ec = __ec__;																														\
																																				\
			nxs_buf_set_len(&con->tmp_buf, 0);																									\
																																				\
			hdr_f |= NXS_NET_FLAG_FILE_CANCEL;																									\
																																				\
			nxs_net_header_set_hdr(con);																										\
			nxs_net_header_set_flag(con, hdr_f);																								\
			nxs_net_header_set_size(con, 0);																									\
																																				\
			nxs_crypt_gost_buf(&con->tmp_buf, con->gost_el);																					\
																																				\
			if((rc = nxs_net_send_core(proc, con, opt->timeout, nxs_buf_get_subbuf(&con->tmp_buf, 0), nxs_buf_get_len(&con->tmp_buf))) < 0){	\
				ec = rc;																														\
				goto error;																														\
			}																																	\
																																				\
			goto error;

#define macro_nxs_net_send_file_cancel(__ec__)																									\
																																				\
			ec = __ec__;																														\
																																				\
			bdy_f = NXS_NET_TRX_FLAG_CANCEL;																									\
																																				\
			nxs_log_write_debug_net(proc, "file send canceled (file: \"%s\")", nxs_string_str(path));											\
																																				\
			nxs_crypt_gost(&bdy_f, sizeof(u_char), con->gost_el);																				\
																																				\
			if((rc = nxs_net_send_core(proc, con, NXS_NET_TIMEOUT_DEFAULT, &bdy_f, sizeof(u_char))) < 0){										\
																																				\
				ec = rc;																														\
			}																																	\
																																				\
			goto error;

/* Module typedefs */

typedef struct		addrinfo		addrinfo_t;
typedef struct		sockaddr_in		sockaddr_in_t;
typedef struct		sockaddr_un		sockaddr_un_t;

/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static size_t nxs_net_header_len_calc(nxs_string_t *header);
static void nxs_net_header_prep(nxs_buf_t *buf, nxs_string_t *header, nxs_net_hdr_f_t flag, off_t size);
static void nxs_net_header_set_hdr(nxs_net_connect_t *con);
static void nxs_net_header_set_flag(nxs_net_connect_t *con, nxs_net_hdr_f_t flag);
static void nxs_net_header_set_size(nxs_net_connect_t *con, off_t size);
static void nxs_net_header_set_offset(nxs_net_connect_t *con, off_t offset);
static size_t nxs_net_header_set_metadata(nxs_net_connect_t *con, nxs_metadata_t *md);
static int nxs_net_header_get_flag(nxs_net_connect_t *con, nxs_net_hdr_f_t *flag);
static int nxs_net_header_get_size(nxs_net_connect_t *con, off_t *size);

/* static int					nxs_net_header_get_offset					(nxs_net_connect_t *con,
 * size_t
 * *offset);
 */
/* static int					nxs_net_header_get_metadata					(nxs_net_connect_t *con,
 * nxs_metadata_t
 * *md, size_t *msize); */
/* static int					nxs_net_header_get_metadata_ver				(nxs_net_connect_t *con,
 * nxs_metadata_v_t
 * *version); */

static int nxs_net_header_parse(nxs_net_connect_t *con, nxs_buf_t *buf, nxs_net_hdr_f_t *flag, off_t *size);
static int nxs_net_header_recv(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_net_hdr_f_t *flag, off_t *size);
static ssize_t nxs_net_recv_buf(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_buf_t *data, nxs_net_hdr_f_t *flag);
static ssize_t nxs_net_send_buf(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_buf_t *data, nxs_net_hdr_f_t flag);
static int nxs_net_check_conn_ready(nxs_process_t *proc, nxs_net_connect_t *con, int timeout);
static int nxs_net_sock_set_opt(nxs_process_t *proc, nxs_net_connect_t *con, int opt);
static int nxs_net_sock_remove_opt(nxs_process_t *proc, nxs_net_connect_t *con, int opt);
static nxs_bool_t nxs_net_send_file_check_path(nxs_metadata_t *md);
static int nxs_net_send_file_metadata_prep(nxs_process_t *    proc,
                                           nxs_net_connect_t *con,
                                           nxs_string_t *     path,
                                           nxs_metadata_v_t   fill_meta,
                                           nxs_metadata_t *   metadata,
                                           off_t *            f_size);
/* static void					nxs_net_send_file_metadata_prep_empty		(nxs_net_connect_t *con); */

static int nxs_net_metadata_recv(nxs_process_t *    proc,
                                 nxs_net_connect_t *con,
                                 time_t             timeout,
                                 size_t             offset,
                                 nxs_metadata_t *   md,
                                 size_t *           msize);

static void nxs_net_error_con_init(nxs_net_connect_t *con);
static void nxs_net_error_con_set(nxs_net_connect_t *con, int _errno);
static void nxs_net_error_srv_init(nxs_net_server_t *srv);
static void nxs_net_unix_error_srv_init(nxs_net_unix_server_t *srv);
/* static void			nxs_net_error_srv_set			(nxs_net_server_t *srv, int _errno); */

// clang-format off

/* Module initializations */

nxs_errors_t nxs_net_errors[] =
{
	{NXS_NET_ERRNO_SUCCESS,		(u_char *)"success"},
	{NXS_NET_ERRNO_NO_DETAIL,	(u_char *)"no details"},
	{NXS_NET_ERRNO_PTR_NULL,	(u_char *)"pointer is null"},
	{NXS_NET_ERRNO_BAD_SOCK_VAL,	(u_char *)"bad socket value"},
	{NXS_NET_ERRNO_FCNTL_GET,	(u_char *)"fcntl get error"},
	{NXS_NET_ERRNO_FCNTL_SET,	(u_char *)"fcntl set error"},

	{0, NULL}
};

/* Module global functions */

// clang-format on

/*
 * Инициализация серверной части.
 * Производится на стороне, принимающей подключение.
 */
int nxs_net_server_init(nxs_net_server_t *srv, u_char *listen_ip, u_char *listen_port, int con_queue_size)
{

	if(srv == NULL || listen_ip == NULL || listen_port == NULL) {

		return NXS_NET_E_INIT;
	}

	nxs_string_init2(&srv->l_ip, 0, listen_ip);
	nxs_string_init2(&srv->l_port, 0, listen_port);

	srv->l_sock      = NXS_NET_INIT_SOCK;
	srv->l_con_queue = con_queue_size;

	nxs_poll_init(&srv->l_poll);

	nxs_net_error_srv_init(srv);

	return NXS_NET_E_OK;
}

/*
 * Инициализация структуры, с которой будет производиться вся работа по передаче данных.
 * Данная функция вызывается на стороне, ожидающей подключение.
 */
int nxs_net_connect_init(nxs_net_connect_t *con, u_char *header)
{

	if(con == NULL || header == NULL) {

		return NXS_NET_E_INIT;
	}

	con->type = NXS_NET_SOCK_TYPE_INET;

	nxs_string_init2(&con->peer_ip, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&con->peer_port, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&con->peer_hostname, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&con->header, 0, header);
	nxs_string_init2(&con->sock_path, 0, NXS_STRING_EMPTY_STR);

	con->con_retr  = 0;
	con->sock      = NXS_NET_INIT_SOCK;
	con->poll_recv = NULL;
	con->poll_send = NULL;

	con->h_len = nxs_net_header_len_calc(&con->header);

	nxs_buf_init(&con->tmp_buf, _NXS_NET_TMP_BUF_INIT_SIZE);

	con->gost_ctx = NULL;
	con->gost_el  = NULL;

	nxs_net_error_con_init(con);

	return NXS_NET_E_OK;
}

/*
 * Инициализация структуры, с которой будет производиться вся работа по передаче данных.
 * Данная функция вызывается на стороне, инициирующей подключения.
 */
int nxs_net_connect_out_init(nxs_net_connect_t *con, u_char *hostname, u_char *port, int con_retr, u_char *header)
{

	if(con == NULL || hostname == NULL || port == NULL || header == NULL) {

		return NXS_NET_E_INIT;
	}

	con->type = NXS_NET_SOCK_TYPE_INET;

	nxs_string_init2(&con->peer_hostname, 0, hostname);
	nxs_string_init2(&con->peer_port, 0, port);
	nxs_string_init2(&con->header, 0, header);
	nxs_string_init2(&con->sock_path, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&con->peer_ip, 0, NXS_STRING_EMPTY_STR);

	nxs_buf_init(&con->tmp_buf, _NXS_NET_TMP_BUF_INIT_SIZE);

	con->h_len = nxs_net_header_len_calc(&con->header);

	con->con_retr = con_retr;

	con->sock      = NXS_NET_INIT_SOCK;
	con->poll_recv = NULL;
	con->poll_send = NULL;

	con->gost_ctx = NULL;
	con->gost_el  = NULL;

	nxs_net_error_con_init(con);

	return NXS_NET_E_OK;
}

/*
 * Закрытие подключения (серверного).
 * Данную функцию после работы с сокетом необходимо вызывать не зависимо от того было установлено соединение или нет, т.к.
 * в данной функции производится очистка выделенной памяти (а само соединение закрывается при необходимости)
 */
int nxs_net_server_close(nxs_process_t *proc, nxs_net_server_t *srv)
{

	if(proc == NULL || srv == NULL) {

		return NXS_NET_E_PTR;
	}

	nxs_net_error_srv_init(srv);

	nxs_string_free(&srv->l_ip);
	nxs_string_free(&srv->l_port);

	srv->l_con_queue = 0;

	nxs_poll_free(&srv->l_poll);

	if(srv->l_sock == NXS_NET_INIT_SOCK) {

		nxs_log_write_debug_net(proc, "server socket is not openned");
	}
	else {

		if(close(srv->l_sock) == -1) {

			nxs_log_write_debug_net(proc, "server socket close error: %s (sock fd: %d)", strerror(errno), srv->l_sock);

			srv->l_sock = NXS_NET_INIT_SOCK;

			return NXS_NET_E_CLOSE;
		}
	}

	srv->l_sock = NXS_NET_INIT_SOCK;

	return NXS_NET_E_OK;
}

/*
 * Закрытие подключения (как для принимающей, так и для инициирующей стороны).
 * Данную функцию после работы с сокетом необходимо вызывать не зависимо от того было установлено соединение или нет, т.к.
 * в данной функции производится очистка выделенной памяти (а само соединение закрывается при необходимости)
 */
int nxs_net_connect_close(nxs_process_t *proc, nxs_net_connect_t *con)
{
	int rc;

	if(proc == NULL || con == NULL) {

		return NXS_NET_E_PTR;
	}

	if(con->type != NXS_NET_SOCK_TYPE_INET) {

		return NXS_NET_E_TYPE;
	}

	rc = NXS_NET_E_OK;

	nxs_net_error_con_init(con);

	con->h_len = 0;

	nxs_buf_free(&con->tmp_buf);

	con->con_retr = 0;

	if(con->sock == NXS_NET_INIT_SOCK) {

		nxs_log_write_debug_net(proc, "socket is not openned");
	}
	else {

		if(close(con->sock) == -1) {

			nxs_log_write_debug_net(proc, "socket close error: %s (sock fd: %d)", strerror(errno), con->sock);

			rc = NXS_NET_E_CLOSE;
			goto error;
		}
	}

error:

	nxs_string_free(&con->peer_ip);
	nxs_string_free(&con->peer_port);
	nxs_string_free(&con->peer_hostname);
	nxs_string_free(&con->header);
	nxs_string_free(&con->sock_path);

	con->sock = NXS_NET_INIT_SOCK;

	con->poll_recv = nxs_poll_destroy(con->poll_recv);
	con->poll_send = nxs_poll_destroy(con->poll_send);

	con->gost_ctx = nxs_crypt_gost_destroy(con->gost_ctx);
	con->gost_el  = NULL;

	return rc;
}

int nxs_net_listen(nxs_process_t *proc, nxs_net_server_t *srv)
{
	int           on_reuseaddr;
	sockaddr_in_t sa;

	if(proc == NULL || srv == NULL) {

		return NXS_NET_E_PTR;
	}

	on_reuseaddr = 1;

	nxs_net_error_srv_init(srv);

	if((srv->l_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {

		nxs_log_write_debug_net(proc, "socket create error: %s", strerror(errno));

		return NXS_NET_E_SOCK_CREATE;
	}

	setsockopt(srv->l_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&on_reuseaddr, sizeof(on_reuseaddr));

	bzero(&sa, sizeof(sa));

	sa.sin_family      = AF_INET;
	sa.sin_addr.s_addr = inet_addr((char *)nxs_string_str(&srv->l_ip));
	sa.sin_port        = htons(nxs_string_atoi(&srv->l_port));

	if(bind(srv->l_sock, (struct sockaddr *)&sa, sizeof(sa)) == -1) {

		nxs_log_write_debug_net(proc, "bind error: %s", strerror(errno));

		return NXS_NET_E_BIND;
	}

	if(listen(srv->l_sock, srv->l_con_queue) == -1) {

		nxs_log_write_debug_net(proc, "listen error: %s", strerror(errno));

		return NXS_NET_E_LISTEN;
	}

	return NXS_NET_E_OK;
}

int nxs_net_accept(nxs_process_t *proc, nxs_net_server_t *srv, nxs_net_connect_t *con)
{
	sockaddr_in_t sa;
	socklen_t     addr_size;
	u_char        port[6];

	if(proc == NULL || srv == NULL || con == NULL) {

		return NXS_NET_E_PTR;
	}

	if(con->type != NXS_NET_SOCK_TYPE_INET) {

		return NXS_NET_E_TYPE;
	}

	nxs_net_error_con_init(con);

	addr_size = sizeof(sa);

	bzero(&sa, addr_size);

	if((con->sock = accept(srv->l_sock, (struct sockaddr *)&sa, &addr_size)) == -1) {

		nxs_log_write_debug_net(proc, "accept error: %s", strerror(errno));

		return NXS_NET_E_ACCEPT;
	}

	nxs_string_char_cpy_dyn(&con->peer_ip, 0, (u_char *)inet_ntoa(sa.sin_addr));

	snprintf((char *)port, 6, "%d", sa.sin_port);
	nxs_string_char_cpy_dyn(&con->peer_port, 0, (u_char *)port);

	con->poll_recv = nxs_poll_malloc();
	nxs_poll_add(con->poll_recv, con->sock, POLLIN);

	con->poll_send = nxs_poll_malloc();
	nxs_poll_add(con->poll_send, con->sock, POLLIN | POLLOUT);

	return NXS_NET_E_OK;
}

/*
 * Аналогична функции nxs_net_accept() с тем отличием, что в данной функции задаются два массива сигналов:
 * * sigunblock - задаёт сигналы, которые будут разблокированы непосредственно перед вызовом accept()
 * * sigblock - задаёт сигналы, которые будут заблокированы сразу после вызова accept()
 */
int nxs_net_accept2(nxs_process_t *proc, nxs_net_server_t *srv, nxs_net_connect_t *con, nxs_array_t *sigunblock, nxs_array_t *sigblock)
{
	sockaddr_in_t sa;
	socklen_t     addr_size;
	u_char        port[6];
	size_t        i;
	int *         sig;

	if(proc == NULL || srv == NULL || con == NULL) {

		return NXS_NET_E_PTR;
	}

	if(con->type != NXS_NET_SOCK_TYPE_INET) {

		return NXS_NET_E_TYPE;
	}

	nxs_net_error_con_init(con);

	addr_size = sizeof(sa);

	bzero(&sa, addr_size);

	for(i = 0; i < nxs_array_count(sigunblock); i++) {

		sig = nxs_array_get(sigunblock, i);

		nxs_proc_signal_unblock(proc, *sig, NXS_PROCESS_SIG_END_ARGS);
	}

	if((con->sock = accept(srv->l_sock, (struct sockaddr *)&sa, &addr_size)) == -1) {

		for(i = 0; i < nxs_array_count(sigblock); i++) {

			sig = nxs_array_get(sigblock, i);

			nxs_proc_signal_block(proc, *sig, NXS_PROCESS_SIG_END_ARGS);
		}

		nxs_log_write_debug_net(proc, "accept error: %s", strerror(errno));

		return NXS_NET_E_ACCEPT;
	}

	for(i = 0; i < nxs_array_count(sigblock); i++) {

		sig = nxs_array_get(sigblock, i);

		nxs_proc_signal_block(proc, *sig, NXS_PROCESS_SIG_END_ARGS);
	}

	nxs_string_char_cpy_dyn(&con->peer_ip, 0, (u_char *)inet_ntoa(sa.sin_addr));

	snprintf((char *)port, 6, "%d", sa.sin_port);
	nxs_string_char_cpy_dyn(&con->peer_port, 0, (u_char *)port);

	con->poll_recv = nxs_poll_malloc();
	nxs_poll_add(con->poll_recv, con->sock, POLLIN);

	con->poll_send = nxs_poll_malloc();
	nxs_poll_add(con->poll_send, con->sock, POLLIN | POLLOUT);

	return NXS_NET_E_OK;
}

int nxs_net_connect(nxs_process_t *proc, nxs_net_connect_t *con)
{
	addrinfo_t     hints, *r = NULL, *p = NULL;
	int            rga, i;
	sockaddr_in_t *ip;

	if(proc == NULL || con == NULL) {

		return NXS_NET_E_PTR;
	}

	if(con->type != NXS_NET_SOCK_TYPE_INET) {

		return NXS_NET_E_TYPE;
	}

	if(con->sock != NXS_NET_INIT_SOCK) {

		nxs_log_write_debug_net(proc, "connect error: socket already openned (sock fd: %d)", con->sock);

		return NXS_NET_E_INIT;
	}

	nxs_net_error_con_init(con);

	con->poll_recv = nxs_poll_destroy(con->poll_recv);
	con->poll_send = nxs_poll_destroy(con->poll_send);

	bzero(&hints, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if((rga = getaddrinfo((char *)nxs_string_str(&con->peer_hostname), (char *)nxs_string_str(&con->peer_port), &hints, &r)) != 0) {

		nxs_log_write_debug_net(proc, "connect error: getaddrinfo error: %s", gai_strerror(rga));

		return NXS_NET_E_GETADDRINFO;
	}

	for(i = 0; i < con->con_retr; i++) {

		for(p = r; p != NULL; p = p->ai_next) {

			if((con->sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {

				nxs_log_write_debug_net(proc, "socket create error: %s", strerror(errno));

				freeaddrinfo(r);

				return NXS_NET_E_SOCK_CREATE;
			}

			if(connect(con->sock, p->ai_addr, p->ai_addrlen) != -1) {

				con->poll_recv = nxs_poll_malloc();
				nxs_poll_add(con->poll_recv, con->sock, POLLIN);

				con->poll_send = nxs_poll_malloc();
				nxs_poll_add(con->poll_send, con->sock, POLLIN | POLLOUT);

				ip = (sockaddr_in_t *)p->ai_addr;
				nxs_string_char_cpy_dyn(&con->peer_ip, 0, (u_char *)inet_ntoa(ip->sin_addr));

				freeaddrinfo(r);

				return NXS_NET_E_OK;
			}

			ip = (sockaddr_in_t *)p->ai_addr;
			nxs_string_char_cpy_dyn(&con->peer_ip, 0, (u_char *)inet_ntoa(ip->sin_addr));

			nxs_log_write_debug_net(proc,
			                        "connect error: %s (ip: %s:%s)",
			                        strerror(errno),
			                        inet_ntoa(ip->sin_addr),
			                        nxs_string_str(&con->peer_port));

			close(con->sock);
			con->sock = NXS_NET_INIT_SOCK;
		}
	}

	freeaddrinfo(r);

	return NXS_NET_E_CONNECT;
}

/*
 * Подключение к серверу с указанием тайм-аута подключения
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK			- подключение прошло успешно
 * * NXS_NET_E_GETADDRINFO	- не удалось получить адрес (getaddrinfo())
 * * NXS_NET_E_SOCK_CREATE	- не удалось создать сокет
 * * NXS_NET_E_FCNTL		- ошибка при задании параметров сокета
 * * NXS_NET_E_CONNECT		- не удалось подключиться к серверу (возможно несколько причин)
 */
int nxs_net_connect2(nxs_process_t *proc, nxs_net_connect_t *con, int timeout)
{
	addrinfo_t     hints, *r = NULL, *p = NULL;
	int            rga, i, r_chk;
	sockaddr_in_t *ip;

	if(proc == NULL || con == NULL) {

		return NXS_NET_E_PTR;
	}

	if(con->type != NXS_NET_SOCK_TYPE_INET) {

		return NXS_NET_E_TYPE;
	}

	if(con->sock != NXS_NET_INIT_SOCK) {

		nxs_log_write_debug_net(proc, "connect error: socket already openned (sock fd: %d)", con->sock);

		return NXS_NET_E_INIT;
	}

	nxs_net_error_con_init(con);

	con->poll_recv = nxs_poll_destroy(con->poll_recv);
	con->poll_send = nxs_poll_destroy(con->poll_send);

	bzero(&hints, sizeof(hints));
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if((rga = getaddrinfo((char *)nxs_string_str(&con->peer_hostname), (char *)nxs_string_str(&con->peer_port), &hints, &r)) != 0) {

		nxs_log_write_debug_net(proc, "connect error: getaddrinfo error: %s", gai_strerror(rga));

		con->_errno = rga;

		return NXS_NET_E_GETADDRINFO;
	}

	for(i = 0; i < con->con_retr; i++) {

		for(p = r; p != NULL; p = p->ai_next) {

			if((con->sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {

				nxs_log_write_debug_net(proc, "socket create error: %s", strerror(errno));

				freeaddrinfo(r);

				return NXS_NET_E_SOCK_CREATE;
			}

			if(nxs_net_sock_set_opt(proc, con, O_NONBLOCK) != NXS_NET_E_OK) {

				nxs_log_write_debug_net(proc, "connect error: fcntl error: %s", nxs_net_strerror(con->_errno));

				close(con->sock);
				con->sock = NXS_NET_INIT_SOCK;

				freeaddrinfo(r);

				return NXS_NET_E_FCNTL;
			}

			if(connect(con->sock, p->ai_addr, p->ai_addrlen) == -1) {

				if(errno != EINPROGRESS) {

					nxs_log_write_debug_net(proc, "connect error: %s", strerror(errno));

					goto next_iteration;
				}
			}

			r_chk = nxs_net_check_conn_ready(proc, con, timeout);

			if(r_chk != NXS_NET_E_OK) {

				if(r_chk == NXS_NET_E_POLL) {

					close(con->sock);
					con->sock = NXS_NET_INIT_SOCK;

					freeaddrinfo(r);

					return NXS_NET_E_CONNECT;
				}
			}
			else {

				if(nxs_net_sock_remove_opt(proc, con, O_NONBLOCK) != NXS_NET_E_OK) {

					nxs_log_write_debug_net(proc, "connect error: fcntl error: %s", nxs_net_strerror(con->_errno));

					close(con->sock);
					con->sock = NXS_NET_INIT_SOCK;

					freeaddrinfo(r);

					return NXS_NET_E_FCNTL;
				}

				con->poll_recv = nxs_poll_malloc();
				nxs_poll_add(con->poll_recv, con->sock, POLLIN);

				con->poll_send = nxs_poll_malloc();
				nxs_poll_add(con->poll_send, con->sock, POLLIN | POLLOUT);

				ip = (sockaddr_in_t *)p->ai_addr;
				nxs_string_char_cpy_dyn(&con->peer_ip, 0, (u_char *)inet_ntoa(ip->sin_addr));

				freeaddrinfo(r);

				return NXS_NET_E_OK;
			}

		next_iteration:

			ip = (sockaddr_in_t *)p->ai_addr;
			nxs_string_char_cpy_dyn(&con->peer_ip, 0, (u_char *)inet_ntoa(ip->sin_addr));

			nxs_log_write_debug_net(
			        proc, "connect attempt failed (ip: %s:%s)", inet_ntoa(ip->sin_addr), nxs_string_str(&con->peer_port));

			close(con->sock);
			con->sock = NXS_NET_INIT_SOCK;
		}
	}

	nxs_log_write_debug_net(proc, "connection create error");

	freeaddrinfo(r);

	return NXS_NET_E_CONNECT;
}

/*
 * Активация отслеживания дескриптора, ожидающего подключения для возможности определять наличие запросов на подключение без вызова
 * блокирующей функции accpet()
 */
int nxs_net_accept_poll_init(nxs_net_server_t *srv)
{

	if(srv == NULL) {

		return NXS_NET_E_PTR;
	}

	if(srv->l_sock == NXS_NET_INIT_SOCK) {

		return NXS_NET_E_INIT;
	}

	if(nxs_poll_add(&srv->l_poll, srv->l_sock, POLLIN) != NXS_POLL_E_OK) {

		return NXS_NET_E_POLL;
	}

	return NXS_NET_E_OK;
}

int nxs_net_accept_poll_free(nxs_net_server_t *srv)
{

	if(srv == NULL) {

		return NXS_NET_E_PTR;
	}

	if(nxs_poll_free(&srv->l_poll) != NXS_POLL_E_OK) {

		return NXS_NET_E_POLL;
	}

	return NXS_NET_E_OK;
}

/*
 * Проверка наличия запросов на подключение
 *
 * Возвращаемые значения:
 * * NXS_NET_E_PTR		- нулевой указатель srv
 * * NXS_NET_E_POLL		- ошибка выполнения poll для дескриптора, либо для дескриптора сработало не известное событие
 * * NXS_NET_E_TIMEOUT	- за время timeout не было получено запросов на подключение
 * * NXS_NET_E_OK		- получен (как минимум один) запрос на подключение
 */
int nxs_net_accept_poll_check(nxs_process_t *proc, nxs_net_server_t *srv, int timeout)
{
	int rc;

	if(srv == NULL) {

		return NXS_NET_E_PTR;
	}

	if((rc = nxs_poll_exec(&srv->l_poll, timeout)) != NXS_POLL_E_OK) {

		if(rc == NXS_POLL_E_TIMEOUT) {

			nxs_log_write_debug_net(proc, "accept poll timeout");

			return NXS_NET_E_TIMEOUT;
		}

		if(rc == NXS_POLL_E_INIT) {

			nxs_log_write_debug_net(proc, "accept poll error: pointer to poll-srv is NULL");
		}
		else {

			nxs_log_write_debug_net(proc, "accept poll error: %s", strerror(rc));
		}

		return NXS_NET_E_POLL;
	}

	if(nxs_poll_check_events(&srv->l_poll, srv->l_sock, POLLIN) == NXS_POLL_EVENTS_TRUE) {

		nxs_log_write_debug_net(proc, "accept poll ready");

		return NXS_NET_E_OK;
	}

	nxs_log_write_debug_net(proc, "accept poll error: unknown event");

	return NXS_NET_E_POLL;
}

int nxs_net_unix_server_init(nxs_process_t *        proc,
                             nxs_net_unix_server_t *srv,
                             u_char *               sock_path,
                             int                    con_queue_size,
                             u_char *               user,
                             u_char *               group,
                             mode_t *               mode)
{
	struct passwd *usr;
	struct group * grp;

	if(srv == NULL || sock_path == NULL) {

		return NXS_NET_E_INIT;
	}

	if(strlen((char *)sock_path) > NXS_NET_UNIX_PATH_MAX - 1) {

		nxs_log_write_debug_net(proc,
		                        "server socket init error: socket path is too long (len: %d, max len: %d, sock path: \"%s\")",
		                        strlen((char *)sock_path),
		                        NXS_NET_UNIX_PATH_MAX - 1,
		                        sock_path);

		return NXS_NET_E_MAX_UNIX_PATH_SIZE;
	}

	/* Задание UID */
	if(user != NULL) {

		if((usr = getpwnam((char *)user)) == NULL) {

			nxs_log_write_debug_net(proc,
			                        "server socket init error: can't resolve user name (sock path: \"%s\", user name: \"%s\")",
			                        sock_path,
			                        user);

			return NXS_NET_E_PWNAM;
		}

		srv->uid = usr->pw_uid;
	}
	else {

		srv->uid = getuid();
	}

	/* Задание GID */
	if(group != NULL) {

		errno = 0;

		if((grp = getgrnam((char *)group)) == NULL) {

			nxs_log_write_debug_net(
			        proc,
			        "server socket init error: can't resolve group name (sock path: \"%s\", group name: \"%s\")",
			        sock_path,
			        group);

			return NXS_NET_E_GRNAM;
		}

		srv->gid = grp->gr_gid;
	}
	else {

		srv->gid = getgid();
	}

	/* Задание MODE */
	if(mode != NULL) {

		srv->mode = *mode;
	}
	else {

		srv->mode = _NXS_NET_DEFAULT_SOCK_PERM;
	}

	nxs_string_init2(&srv->l_sock_path, 0, sock_path);

	srv->l_sock      = NXS_NET_INIT_SOCK;
	srv->l_con_queue = con_queue_size;

	nxs_poll_init(&srv->l_poll);

	nxs_net_unix_error_srv_init(srv);

	return NXS_NET_E_OK;
}

/*
 * Инициализация структуры, с которой будет производиться вся работа по передаче данных.
 * Данная функция вызывается на стороне, ожидающей подключение.
 */
int nxs_net_unix_connect_init(nxs_net_connect_t *con, u_char *header)
{
	int rc;

	if((rc = nxs_net_connect_init(con, header)) != NXS_NET_E_OK) {

		return rc;
	}

	con->type = NXS_NET_SOCK_TYPE_UNIX;

	return NXS_NET_E_OK;
}

/*
 * На текущий момент аргумент 'con_retr' является не значимым (возможно, будет использоваться в будущем)
 */
int nxs_net_unix_connect_out_init(nxs_process_t *proc, nxs_net_connect_t *con, u_char *sock_path, int con_retr, u_char *header)
{

	if(con == NULL || sock_path == NULL || header == NULL) {

		return NXS_NET_E_INIT;
	}

	if(strlen((char *)sock_path) > NXS_NET_UNIX_PATH_MAX - 1) {

		nxs_log_write_debug_net(proc,
		                        "connection init error: socket path is too long (len: %d, max len: %d, sock path: \"%s\")",
		                        strlen((char *)sock_path),
		                        NXS_NET_UNIX_PATH_MAX - 1,
		                        sock_path);

		return NXS_NET_E_MAX_UNIX_PATH_SIZE;
	}

	con->type = NXS_NET_SOCK_TYPE_UNIX;

	nxs_string_init2(&con->peer_hostname, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&con->peer_port, 0, NXS_STRING_EMPTY_STR);
	nxs_string_init2(&con->header, 0, header);
	nxs_string_init2(&con->sock_path, 0, sock_path);
	nxs_string_init2(&con->peer_ip, 0, NXS_STRING_EMPTY_STR);

	nxs_buf_init(&con->tmp_buf, _NXS_NET_TMP_BUF_INIT_SIZE);

	con->h_len = nxs_net_header_len_calc(&con->header);

	con->con_retr = con_retr;

	con->sock      = NXS_NET_INIT_SOCK;
	con->poll_recv = NULL;
	con->poll_send = NULL;

	con->gost_ctx = NULL;
	con->gost_el  = NULL;

	nxs_net_error_con_init(con);

	return NXS_NET_E_OK;
}

/*
 * Закрытие подключения (серверного).
 * Данную функцию после работы с сокетом необходимо вызывать не зависимо от того было установлено соединение или нет, т.к.
 * в данной функции производится очистка выделенной памяти (а само соединение закрывается при необходимости)
 */
int nxs_net_unix_server_close(nxs_process_t *proc, nxs_net_unix_server_t *srv)
{
	int rc;

	rc = NXS_NET_E_OK;

	if(proc == NULL || srv == NULL) {

		return NXS_NET_E_PTR;
	}

	nxs_net_unix_error_srv_init(srv);

	srv->l_con_queue = 0;

	nxs_poll_free(&srv->l_poll);

	srv->uid  = 0;
	srv->gid  = 0;
	srv->mode = 0;

	if(srv->l_sock == NXS_NET_INIT_SOCK) {

		nxs_log_write_debug_net(proc, "server unix socket is not openned");
	}
	else {

		if(close(srv->l_sock) == -1) {

			nxs_log_write_debug_net(proc,
			                        "server socket close error: %s (sock fd: %d, sock path: \"%s\")",
			                        strerror(errno),
			                        srv->l_sock,
			                        nxs_string_str(&srv->l_sock_path));

			rc = NXS_NET_E_CLOSE;
			goto error;
		}
	}

error:

	nxs_string_free(&srv->l_sock_path);

	srv->l_sock = NXS_NET_INIT_SOCK;

	return rc;
}

/*
 * Закрытие подключения (серверного).
 * Данную функцию после работы с сокетом необходимо вызывать не зависимо от того было установлено соединение или нет, т.к.
 * в данной функции производится очистка выделенной памяти (а само соединение закрывается при необходимости)
 */
int nxs_net_unix_server_unlink(nxs_process_t *proc, nxs_net_unix_server_t *srv)
{
	int rc;

	rc = NXS_NET_E_OK;

	if(proc == NULL || srv == NULL) {

		return NXS_NET_E_PTR;
	}

	nxs_net_unix_error_srv_init(srv);

	srv->l_con_queue = 0;

	nxs_poll_free(&srv->l_poll);

	srv->uid  = 0;
	srv->gid  = 0;
	srv->mode = 0;

	if(srv->l_sock == NXS_NET_INIT_SOCK) {

		nxs_log_write_debug_net(proc, "server unix socket is not openned");
	}
	else {

		if(close(srv->l_sock) == -1) {

			nxs_log_write_debug_net(proc,
			                        "server socket close error: %s (sock fd: %d, sock path: \"%s\")",
			                        strerror(errno),
			                        srv->l_sock,
			                        nxs_string_str(&srv->l_sock_path));

			rc = NXS_NET_E_CLOSE;
		}

		if(nxs_fs_unlink(&srv->l_sock_path) < 0) {

			nxs_log_write_debug_net(proc,
			                        "server socket unlink error: %s (sock path: \"%s\")",
			                        strerror(errno),
			                        nxs_string_str(&srv->l_sock_path));

			rc = NXS_NET_E_CLOSE;
			goto error;
		}
	}

error:

	nxs_string_free(&srv->l_sock_path);

	srv->l_sock = NXS_NET_INIT_SOCK;

	return rc;
}

/*
 * Закрытие подключения (как для принимающей, так и для инициирующей стороны).
 * Данную функцию после работы с сокетом необходимо вызывать не зависимо от того было установлено соединение или нет, т.к.
 * в данной функции производится очистка выделенной памяти (а само соединение закрывается при необходимости)
 */
int nxs_net_unix_connect_close(nxs_process_t *proc, nxs_net_connect_t *con)
{
	int rc;

	if(proc == NULL || con == NULL) {

		return NXS_NET_E_PTR;
	}

	if(con->type != NXS_NET_SOCK_TYPE_UNIX) {

		return NXS_NET_E_TYPE;
	}

	rc = NXS_NET_E_OK;

	nxs_net_error_con_init(con);

	con->h_len = 0;

	nxs_buf_free(&con->tmp_buf);

	con->con_retr = 0;

	if(con->sock == NXS_NET_INIT_SOCK) {

		nxs_log_write_debug_net(proc, "socket is not openned");
	}
	else {

		if(close(con->sock) == -1) {

			nxs_log_write_debug_net(proc,
			                        "socket close error: %s (sock fd: %d, sock path: \"%s\")",
			                        strerror(errno),
			                        con->sock,
			                        nxs_string_str(&con->sock_path));

			rc = NXS_NET_E_CLOSE;
			goto error;
		}
	}

error:

	nxs_string_free(&con->peer_ip);
	nxs_string_free(&con->peer_port);
	nxs_string_free(&con->peer_hostname);
	nxs_string_free(&con->header);
	nxs_string_free(&con->sock_path);

	con->sock = NXS_NET_INIT_SOCK;

	con->poll_recv = nxs_poll_destroy(con->poll_recv);
	con->poll_send = nxs_poll_destroy(con->poll_send);

	con->gost_ctx = nxs_crypt_gost_destroy(con->gost_ctx);
	con->gost_el  = NULL;

	return rc;
}

int nxs_net_unix_listen(nxs_process_t *proc, nxs_net_unix_server_t *srv)
{
	sockaddr_un_t sa;
	int           rc;

	if(proc == NULL || srv == NULL) {

		return NXS_NET_E_PTR;
	}

	rc = NXS_NET_E_OK;

	nxs_net_unix_error_srv_init(srv);

	if((srv->l_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {

		nxs_log_write_debug_net(
		        proc, "unix socket create error: %s (sock path: \"%s\")", strerror(errno), nxs_string_str(&srv->l_sock_path));

		return NXS_NET_E_SOCK_CREATE;
	}

	bzero(&sa, sizeof(sa));

	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, (char *)nxs_string_str(&srv->l_sock_path));

	if(bind(srv->l_sock, (struct sockaddr *)&sa, sizeof(sa)) == -1) {

		nxs_log_write_debug_net(proc, "bind error: %s (sock path: \"%s\")", strerror(errno), nxs_string_str(&srv->l_sock_path));

		rc = NXS_NET_E_BIND;
		goto error;
	}

	if(chown((char *)nxs_string_str(&srv->l_sock_path), srv->uid, srv->gid) < 0) {

		nxs_log_write_debug_net(proc,
		                        "unix socket chown error: %s (sock path: \"%s\", uid: %d, gid: %d)",
		                        strerror(errno),
		                        nxs_string_str(&srv->l_sock_path),
		                        srv->uid,
		                        srv->gid);

		rc = NXS_NET_E_CHOWN;
		goto error;
	}

	if(chmod((char *)nxs_string_str(&srv->l_sock_path), srv->mode) < 0) {

		nxs_log_write_debug_net(proc,
		                        "unix socket chmod error: %s (sock path: \"%s\", mode: %d)",
		                        strerror(errno),
		                        nxs_string_str(&srv->l_sock_path),
		                        srv->mode);

		rc = NXS_NET_E_CHMOD;
		goto error;
	}

	if(listen(srv->l_sock, srv->l_con_queue) == -1) {

		nxs_log_write_debug_net(
		        proc, "unix socket listen error: %s (sock path: \"%s\")", strerror(errno), nxs_string_str(&srv->l_sock_path));

		rc = NXS_NET_E_LISTEN;
		goto error;
	}

error:

	if(rc == NXS_NET_E_CHOWN || rc == NXS_NET_E_CHMOD || rc == NXS_NET_E_BIND || rc == NXS_NET_E_LISTEN) {

		if(close(srv->l_sock) == -1) {

			nxs_log_write_debug_net(proc,
			                        "server socket close error: %s (sock fd: %d, sock path: \"%s\")",
			                        strerror(errno),
			                        srv->l_sock,
			                        nxs_string_str(&srv->l_sock_path));
		}
	}

	if(rc == NXS_NET_E_CHOWN || rc == NXS_NET_E_CHMOD || rc == NXS_NET_E_LISTEN) {

		if(nxs_fs_unlink(&srv->l_sock_path) < 0) {

			nxs_log_write_debug_net(proc,
			                        "server socket unlink error: %s (sock path: \"%s\")",
			                        strerror(errno),
			                        nxs_string_str(&srv->l_sock_path));
		}
	}

	return rc;
}

int nxs_net_unix_accept(nxs_process_t *proc, nxs_net_unix_server_t *srv, nxs_net_connect_t *con)
{
	sockaddr_un_t sa;
	socklen_t     addr_size;

	if(proc == NULL || srv == NULL || con == NULL) {

		return NXS_NET_E_PTR;
	}

	if(con->type != NXS_NET_SOCK_TYPE_UNIX) {

		return NXS_NET_E_TYPE;
	}

	nxs_net_error_con_init(con);

	addr_size = sizeof(sa);

	bzero(&sa, addr_size);

	if((con->sock = accept(srv->l_sock, (struct sockaddr *)&sa, &addr_size)) == -1) {

		nxs_log_write_debug_net(proc, "accept error: %s (sock path: \"%s\")", strerror(errno), nxs_string_str(&srv->l_sock_path));

		return NXS_NET_E_ACCEPT;
	}

	nxs_string_char_cpy_dyn(&con->sock_path, 0, nxs_string_str(&srv->l_sock_path));

	con->poll_recv = nxs_poll_malloc();
	nxs_poll_add(con->poll_recv, con->sock, POLLIN);

	con->poll_send = nxs_poll_malloc();
	nxs_poll_add(con->poll_send, con->sock, POLLIN | POLLOUT);

	return NXS_NET_E_OK;
}

int nxs_net_unix_accept2(nxs_process_t *        proc,
                         nxs_net_unix_server_t *srv,
                         nxs_net_connect_t *    con,
                         nxs_array_t *          sigunblock,
                         nxs_array_t *          sigblock)
{
	sockaddr_un_t sa;
	socklen_t     addr_size;
	size_t        i;
	int *         sig;

	if(proc == NULL || srv == NULL || con == NULL) {

		return NXS_NET_E_PTR;
	}

	if(con->type != NXS_NET_SOCK_TYPE_UNIX) {

		return NXS_NET_E_TYPE;
	}

	nxs_net_error_con_init(con);

	addr_size = sizeof(sa);

	bzero(&sa, addr_size);

	for(i = 0; i < nxs_array_count(sigunblock); i++) {

		sig = nxs_array_get(sigunblock, i);

		nxs_proc_signal_unblock(proc, *sig, NXS_PROCESS_SIG_END_ARGS);
	}

	if((con->sock = accept(srv->l_sock, (struct sockaddr *)&sa, &addr_size)) == -1) {

		for(i = 0; i < nxs_array_count(sigblock); i++) {

			sig = nxs_array_get(sigblock, i);

			nxs_proc_signal_block(proc, *sig, NXS_PROCESS_SIG_END_ARGS);
		}

		nxs_log_write_debug_net(proc, "accept error: %s (sock path: \"%s\")", strerror(errno), nxs_string_str(&srv->l_sock_path));

		return NXS_NET_E_ACCEPT;
	}

	for(i = 0; i < nxs_array_count(sigblock); i++) {

		sig = nxs_array_get(sigblock, i);

		nxs_proc_signal_block(proc, *sig, NXS_PROCESS_SIG_END_ARGS);
	}

	nxs_string_char_cpy_dyn(&con->sock_path, 0, nxs_string_str(&srv->l_sock_path));

	con->poll_recv = nxs_poll_malloc();
	nxs_poll_add(con->poll_recv, con->sock, POLLIN);

	con->poll_send = nxs_poll_malloc();
	nxs_poll_add(con->poll_send, con->sock, POLLIN | POLLOUT);

	return NXS_NET_E_OK;
}

int nxs_net_unix_connect(nxs_process_t *proc, nxs_net_connect_t *con)
{
	sockaddr_un_t sa;
	socklen_t     addr_size;

	if(proc == NULL || con == NULL) {

		return NXS_NET_E_PTR;
	}

	if(con->type != NXS_NET_SOCK_TYPE_UNIX) {

		return NXS_NET_E_TYPE;
	}

	if(con->sock != NXS_NET_INIT_SOCK) {

		nxs_log_write_debug_net(proc, "connect error: socket already openned (sock fd: %d)", con->sock);

		return NXS_NET_E_INIT;
	}

	nxs_net_error_con_init(con);

	con->poll_recv = nxs_poll_destroy(con->poll_recv);
	con->poll_send = nxs_poll_destroy(con->poll_send);

	addr_size = sizeof(sa);

	if((con->sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {

		nxs_log_write_debug_net(
		        proc, "socket create error: %s (sock path: \"%s\")", strerror(errno), nxs_string_str(&con->sock_path));

		return NXS_NET_E_SOCK_CREATE;
	}

	bzero(&sa, addr_size);

	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, (char *)nxs_string_str(&con->sock_path));

	if(connect(con->sock, (struct sockaddr *)&sa, addr_size) == -1) {

		nxs_log_write_debug_net(proc, "connect error: %s (sock path: \"%s\")", strerror(errno), nxs_string_str(&con->sock_path));

		close(con->sock);
		con->sock = NXS_NET_INIT_SOCK;

		return NXS_NET_E_CONNECT;
	}

	con->poll_recv = nxs_poll_malloc();
	nxs_poll_add(con->poll_recv, con->sock, POLLIN);

	con->poll_send = nxs_poll_malloc();
	nxs_poll_add(con->poll_send, con->sock, POLLIN | POLLOUT);

	return NXS_NET_E_OK;
}

/*
 * Активация отслеживания дескриптора, ожидающего подключения для возможности определять наличие запросов на подключение без вызова
 * блокирующей функции accpet()
 */
int nxs_net_unix_accept_poll_init(nxs_net_unix_server_t *srv)
{

	if(srv == NULL) {

		return NXS_NET_E_PTR;
	}

	if(srv->l_sock == NXS_NET_INIT_SOCK) {

		return NXS_NET_E_INIT;
	}

	if(nxs_poll_add(&srv->l_poll, srv->l_sock, POLLIN) != NXS_POLL_E_OK) {

		return NXS_NET_E_POLL;
	}

	return NXS_NET_E_OK;
}

int nxs_net_unix_accept_poll_free(nxs_net_unix_server_t *srv)
{

	if(srv == NULL) {

		return NXS_NET_E_PTR;
	}

	if(nxs_poll_free(&srv->l_poll) != NXS_POLL_E_OK) {

		return NXS_NET_E_POLL;
	}

	return NXS_NET_E_OK;
}

/*
 * Проверка наличия запросов на подключение
 *
 * Возвращаемые значения:
 * * NXS_NET_E_PTR		- нулевой указатель srv
 * * NXS_NET_E_POLL		- ошибка выполнения poll для дескриптора, либо для дескриптора сработало не известное событие
 * * NXS_NET_E_TIMEOUT	- за время timeout не было получено запросов на подключение
 * * NXS_NET_E_OK		- получен (как минимум один) запрос на подключение
 */
int nxs_net_unix_accept_poll_check(nxs_process_t *proc, nxs_net_unix_server_t *srv, int timeout)
{
	int rc;

	if(srv == NULL) {

		return NXS_NET_E_PTR;
	}

	if((rc = nxs_poll_exec(&srv->l_poll, timeout)) != NXS_POLL_E_OK) {

		if(rc == NXS_POLL_E_TIMEOUT) {

			nxs_log_write_debug_net(proc, "unix accept poll timeout");

			return NXS_NET_E_TIMEOUT;
		}

		if(rc == NXS_POLL_E_INIT) {

			nxs_log_write_debug_net(proc, "unix accept poll error: pointer to poll-srv is NULL");
		}
		else {

			nxs_log_write_debug_net(proc, "unix accept poll error: %s", strerror(rc));
		}

		return NXS_NET_E_POLL;
	}

	if(nxs_poll_check_events(&srv->l_poll, srv->l_sock, POLLIN) == NXS_POLL_EVENTS_TRUE) {

		nxs_log_write_debug_net(proc, "unix accept poll ready");

		return NXS_NET_E_OK;
	}

	nxs_log_write_debug_net(proc, "unix accept poll error: unknown event");

	return NXS_NET_E_POLL;
}

int nxs_net_crypt_gost_init(nxs_process_t *proc, nxs_net_connect_t *con, nxs_string_t *key)
{

	if(con->gost_ctx != NULL) {

		/*
		 * Если контекст шифрования уже инициализирован
		 */

		nxs_log_write_debug_net(proc, "can't init encryption context GOST 27148-89: already initiated");

		return NXS_NET_E_CRYPT_GOST_INIT;
	}

	if((con->gost_ctx = nxs_crypt_gost_malloc(key)) == NULL) {

		nxs_log_write_debug_net(proc, "can't init encryption context GOST 27148-89: key incorrect");

		return NXS_NET_E_CRYPT_GOST_INIT;
	}

	con->gost_el = nxs_crypt_gost_key_open(con->gost_ctx);

	return NXS_NET_E_OK;
}

int nxs_net_crypt_gost_init2(nxs_process_t *proc, nxs_net_connect_t *con, nxs_string_t *path)
{

	if(con->gost_ctx != NULL) {

		/*
		 * Если контекст шифрования уже инициализирован
		 */

		nxs_log_write_debug_net(proc, "can't init encryption context GOST 27148-89: already initiated");

		return NXS_NET_E_CRYPT_GOST_INIT;
	}

	if((con->gost_ctx = nxs_crypt_gost_malloc2(proc, path)) == NULL) {

		nxs_log_write_debug_net(proc, "can't init encryption context GOST 27148-89: can't read key from file or key is incorrect");

		return NXS_NET_E_CRYPT_GOST_INIT;
	}

	con->gost_el = nxs_crypt_gost_key_open(con->gost_ctx);

	return NXS_NET_E_OK;
}

void nxs_net_crypt_gost_clear(nxs_process_t *proc, nxs_net_connect_t *con)
{

	con->gost_ctx = nxs_crypt_gost_destroy(con->gost_ctx);

	con->gost_el = NULL;
}

void nxs_net_crypt_gost_reset(nxs_process_t *proc, nxs_net_connect_t *con)
{

	nxs_crypt_gost_reset(con->gost_ctx, con->gost_el);
}

/*
 * Активация элемента, производящего ширование.
 * Этим элементом является указатель на первый элементм в списке всех элементов ширования.
 */
void nxs_net_crypt_gost_enable(nxs_process_t *proc, nxs_net_connect_t *con)
{

	if(con->gost_ctx == NULL) {

		return;
	}

	if(nxs_list_count(&con->gost_ctx->els) < 1) {

		con->gost_el = nxs_crypt_gost_key_open(con->gost_ctx);
	}
	else {

		con->gost_el = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, &con->gost_ctx->els);
	}
}

void nxs_net_crypt_gost_disable(nxs_process_t *proc, nxs_net_connect_t *con)
{

	con->gost_el = NULL;
}

nxs_crypt_gost_el_t *nxs_net_crypt_gost_get_el(nxs_net_connect_t *con)
{

	return con->gost_el;
}

/*
 * Получение по сети size байт
 *
 * Возвращаемые значения:
 * * Количество прочитанных байт
 * * NXS_NET_E_POLL - ошибка poll
 * * NXS_NET_E_TIMEOUT - тайм-аут при ожидании данных
 * * NXS_NET_E_PEER_SHUTDOWN - удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKREAD - ошибка чтения сокета
 *
 * Функция изменила своё название с nxs_net_recv_raw в версии v0.2-1 r14
 */
ssize_t nxs_net_recv_core(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, u_char *buf, size_t size)
{
	size_t  tb, lb;
	ssize_t nb;
	int     pe;
	time_t  t;

	tb = 0;
	lb = size;

	t = timeout;

	nxs_net_error_con_init(con);

	while(tb < size) {

		if((pe = nxs_poll_exec(con->poll_recv, t)) != NXS_POLL_E_OK) {

			if(pe == NXS_POLL_E_TIMEOUT) {

				nxs_log_write_debug_net(proc, "recv error: timeout (timeout: %d sec)", t / 1000);

				return NXS_NET_E_TIMEOUT;
			}

			if(pe == NXS_POLL_E_INIT) {

				nxs_log_write_debug_net(proc, "recv error: poll error: pointer to poll-recv is NULL");
			}
			else {

				nxs_log_write_debug_net(proc, "recv error: poll error: %s", strerror(pe));
			}

			return NXS_NET_E_POLL;
		}

		nb = read(con->sock, buf + tb, lb);

		if(nb > 0) {

			tb += nb;
			lb -= nb;
		}
		else {

			if(nb < 0) {

				if(con->type == NXS_NET_SOCK_TYPE_INET) {

					nxs_log_write_debug_net(proc,
					                        "recv error: %s (sock: %d, client: %s:%s, poll: %d)",
					                        strerror(errno),
					                        con->sock,
					                        nxs_string_str(&con->peer_ip),
					                        nxs_string_str(&con->peer_port),
					                        nxs_poll_get_res(con->poll_recv, con->sock));
				}
				else {

					nxs_log_write_debug_net(proc,
					                        "recv error: %s (sock: %d, sock path: \"%s\", poll: %d)",
					                        strerror(errno),
					                        con->sock,
					                        nxs_string_str(&con->sock_path),
					                        nxs_poll_get_res(con->poll_recv, con->sock));
				}

				return NXS_NET_E_SOCKREAD;
			}

			if(con->type == NXS_NET_SOCK_TYPE_INET) {

				nxs_log_write_debug_net(proc,
				                        "recv error: peer closed connection (sock: %d, client: %s:%s, poll: %d)",
				                        con->sock,
				                        nxs_string_str(&con->peer_ip),
				                        nxs_string_str(&con->peer_port),
				                        nxs_poll_get_res(con->poll_recv, con->sock));
			}
			else {

				nxs_log_write_debug_net(proc,
				                        "recv error: peer closed connection (sock: %d, sock path: \"%s\", poll: %d)",
				                        con->sock,
				                        nxs_string_str(&con->sock_path),
				                        nxs_poll_get_res(con->poll_recv, con->sock));
			}

			return NXS_NET_E_PEER_SHUTDOWN;
		}

		t = NXS_NET_TIMEOUT_DEFAULT;
	}

	return tb;
}

/*
 * Отправка по сети size байт
 *
 * Возвращаемые значения:
 * * Количество записанных байт
 * * NXS_NET_E_POLL - ошибка poll
 * * NXS_NET_E_TIMEOUT - тайм-аут при отправке данных
 * * NXS_NET_E_PEER_SHUTDOWN - удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKWRITE - ошибка записи в сокет
 *
 * Функция изменила своё название с nxs_net_send_raw в версии v0.2-1 r14
 */
ssize_t nxs_net_send_core(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, u_char *buf, size_t size)
{
	size_t  tb, lb;
	ssize_t nb;
	int     pe;
	time_t  t;
	char    t_buf[1];

	tb = 0;
	lb = size;

	t = timeout;

	nxs_net_error_con_init(con);

	while(tb < size) {

		if((pe = nxs_poll_exec(con->poll_send, t)) != NXS_POLL_E_OK) {

			if(pe == NXS_POLL_E_TIMEOUT) {

				nxs_log_write_debug_net(proc, "send error: timeout (timeout: %d sec)", t / 1000);

				return NXS_NET_E_TIMEOUT;
			}

			if(pe == NXS_POLL_E_INIT) {

				nxs_log_write_debug_net(proc, "send error: poll error: pointer to poll-send is NULL");
			}
			else {

				nxs_log_write_debug_net(proc, "send error: poll error: %s", strerror(pe));
			}

			return NXS_NET_E_POLL;
		}

		/*
		 * Проверка доступности сокета
		 */
		if(nxs_poll_check_events(con->poll_send, con->sock, POLLIN)) {

			if((nb = recv(con->sock, t_buf, 1, MSG_PEEK)) == 0) {

				if(con->type == NXS_NET_SOCK_TYPE_INET) {

					nxs_log_write_debug_net(proc,
					                        "send error: peer closed connection (sock: %d, client: %s:%s, poll: %d)",
					                        con->sock,
					                        nxs_string_str(&con->peer_ip),
					                        nxs_string_str(&con->peer_port),
					                        nxs_poll_get_res(con->poll_send, con->sock));
				}
				else {

					nxs_log_write_debug_net(
					        proc,
					        "send error: peer closed connection (sock: %d, sock path: \"%s\", poll: %d)",
					        con->sock,
					        nxs_string_str(&con->sock_path),
					        nxs_poll_get_res(con->poll_send, con->sock));
				}

				return NXS_NET_E_PEER_SHUTDOWN;
			}
			else {

				if(nb < 0) {

					if(con->type == NXS_NET_SOCK_TYPE_INET) {

						nxs_log_write_debug_net(proc,
						                        "send error: %s (sock: %d, client: %s:%s, poll: %d)",
						                        strerror(errno),
						                        con->sock,
						                        nxs_string_str(&con->peer_ip),
						                        nxs_string_str(&con->peer_port),
						                        nxs_poll_get_res(con->poll_send, con->sock));
					}
					else {

						nxs_log_write_debug_net(proc,
						                        "send error: %s (sock: %d, sock path: \"%s\", poll: %d)",
						                        strerror(errno),
						                        con->sock,
						                        nxs_string_str(&con->sock_path),
						                        nxs_poll_get_res(con->poll_send, con->sock));
					}

					return NXS_NET_E_SOCKREAD;
				}
			}
		}

		/*
		 * Отправка данных
		 */
		nb = write(con->sock, buf + tb, lb);

		if(nb > 0) {

			tb += nb;
			lb -= nb;
		}
		else {

			if(nb == 0) {

				if(con->type == NXS_NET_SOCK_TYPE_INET) {

					nxs_log_write_debug_net(proc,
					                        "send error: peer closed connection (sock: %d, client: %s:%s, poll: %d)",
					                        con->sock,
					                        nxs_string_str(&con->peer_ip),
					                        nxs_string_str(&con->peer_port),
					                        nxs_poll_get_res(con->poll_send, con->sock));
				}
				else {

					nxs_log_write_debug_net(
					        proc,
					        "send error: peer closed connection (sock: %d, sock path: \"%s\", poll: %d)",
					        con->sock,
					        nxs_string_str(&con->sock_path),
					        nxs_poll_get_res(con->poll_send, con->sock));
				}

				return NXS_NET_E_PEER_SHUTDOWN;
			}

			if(con->type == NXS_NET_SOCK_TYPE_INET) {

				nxs_log_write_debug_net(proc,
				                        "send error: %s (sock: %d, client: %s:%s, poll: %d)",
				                        strerror(errno),
				                        con->sock,
				                        nxs_string_str(&con->peer_ip),
				                        nxs_string_str(&con->peer_port),
				                        nxs_poll_get_res(con->poll_send, con->sock));
			}
			else {

				nxs_log_write_debug_net(proc,
				                        "send error: %s (sock: %d, sock path: \"%s\", poll: %d)",
				                        strerror(errno),
				                        con->sock,
				                        nxs_string_str(&con->sock_path),
				                        nxs_poll_get_res(con->poll_send, con->sock));
			}

			return NXS_NET_E_SOCKWRITE;
		}

		t = NXS_NET_TIMEOUT_DEFAULT;
	}

	return tb;
}

/*
 * Получение из сети данных размером "size" и запись их в "con->tmp_buf" со смещением "offset" относительно начала буфера.
 * После получения данных производится их дешифрование (важно: контекст ширования не сбрасывается в данной функции).
 *
 * Если при этом указатель "data" не равен нулю, то полученная информация размером "size" байт будет записана в область
 * памяти на которую указывает "data".
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK				- данные успешно приняты
 * * NXS_NET_E_POLL				- ошибка poll
 * * NXS_NET_E_TIMEOUT			- тайм-аут при ожидании данных
 * * NXS_NET_E_PEER_SHUTDOWN	- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKREAD			- ошибка чтения сокета
 */
int nxs_net_recv_chunk(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, void *data, size_t offset, size_t size)
{
	ssize_t rc;
	u_char *buf;

	if(nxs_buf_get_size(&con->tmp_buf) < offset + size) {

		nxs_buf_resize(&con->tmp_buf, offset + size);
	}

	buf = nxs_buf_get_subbuf(&con->tmp_buf, offset);

	if((rc = nxs_net_recv_core(proc, con, NXS_NET_TIMEOUT_DEFAULT, buf, size)) < 0) {

		return rc;
	}

	if(nxs_buf_get_len(&con->tmp_buf) < offset + size) {

		nxs_buf_set_len(&con->tmp_buf, offset + size);
	}

	nxs_crypt_gost(nxs_buf_get_subbuf(&con->tmp_buf, offset), size, con->gost_el);

	if(data != NULL) {

		nxs_buf_get_mem(&con->tmp_buf, offset, data, size);
	}

	return NXS_NET_E_OK;
}

ssize_t nxs_net_recv(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, void *data, int type)
{
	nxs_buf_t *     buf;
	nxs_array_t *   arr;
	nxs_list_t *    lst;
	ssize_t         rc;
	nxs_net_hdr_f_t flag;

	nxs_net_error_con_init(con);

	if(data == NULL) {

		return NXS_NET_E_BUF_NULL;
	}

	flag = 0;

	switch(type) {

		case NXS_NET_DATA_TYPE_STRING:

			nxs_net_crypt_gost_reset(proc, con);

			while((flag & NXS_NET_FLAG_LAST) == 0) {

				if((rc = nxs_net_recv_buf(proc, con, timeout, data, &flag)) < 0) {

					return rc;
				}
			}

			break;

		case NXS_NET_DATA_TYPE_ARRAY:

			arr = data;

			if(nxs_array_size(arr) != sizeof(nxs_buf_t)) {

				nxs_log_write_debug_net(proc, "recv error: incorrect array data size");

				return NXS_NET_E_BUF_INCORRECT_TYPE;
			}

			nxs_net_crypt_gost_reset(proc, con);

			while((flag & NXS_NET_FLAG_LAST) == 0) {

				buf = nxs_array_add(arr);

				nxs_buf_init(buf, 1);

				if((rc = nxs_net_recv_buf(proc, con, timeout, buf, &flag)) < 0) {

					return rc;
				}
			}

			break;

		case NXS_NET_DATA_TYPE_LIST:

			lst = data;

			if(nxs_list_size(lst) != sizeof(nxs_buf_t)) {

				nxs_log_write_debug_net(proc, "recv error: incorrect list data size");

				return NXS_NET_E_BUF_INCORRECT_TYPE;
			}

			nxs_net_crypt_gost_reset(proc, con);

			while((flag & NXS_NET_FLAG_LAST) == 0) {

				buf = nxs_list_add_tail(lst);

				nxs_buf_init(buf, 1);

				if((rc = nxs_net_recv_buf(proc, con, timeout, buf, &flag)) < 0) {

					return rc;
				}
			}

			break;

		default:

			nxs_log_write_debug_net(proc, "recv error: unknown data type (type: %d)", type);

			return NXS_NET_E_UNKNOWN_DATATYPE;
	}

	return NXS_NET_E_OK;
}

ssize_t nxs_net_send(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, void *data, int type)
{
	nxs_buf_t *     buf;
	nxs_array_t *   arr;
	nxs_list_t *    lst;
	ssize_t         rc;
	nxs_net_hdr_f_t flag;
	size_t          i;

	if(con == NULL) {

		nxs_log_write_debug_net(proc, "send error: con is NULL");

		return NXS_NET_E_PTR;
	}

	nxs_net_error_con_init(con);

	if(data == NULL) {

		return NXS_NET_E_BUF_NULL;
	}

	switch(type) {

		case NXS_NET_DATA_TYPE_STRING:

			nxs_net_crypt_gost_reset(proc, con);

			if((rc = nxs_net_send_buf(proc, con, timeout, data, NXS_NET_FLAG_BUF | NXS_NET_FLAG_LAST)) < 0) {

				return rc;
			}

			break;

		case NXS_NET_DATA_TYPE_ARRAY:

			arr = data;

			if(nxs_array_size(arr) != sizeof(nxs_buf_t)) {

				nxs_log_write_debug_net(proc, "send error: incorrect array data size");

				return NXS_NET_E_BUF_INCORRECT_TYPE;
			}

			nxs_net_crypt_gost_reset(proc, con);

			for(i = 0, flag = NXS_NET_FLAG_BUF; (buf = nxs_array_get(arr, i)) != NULL; i++) {

				if(i == nxs_array_count(arr) - 1) {

					flag |= NXS_NET_FLAG_LAST;
				}

				if((rc = nxs_net_send_buf(proc, con, timeout, buf, flag)) < 0) {

					return rc;
				}
			}

			break;

		case NXS_NET_DATA_TYPE_LIST:

			lst = data;

			if(nxs_list_size(lst) != sizeof(nxs_buf_t)) {

				nxs_log_write_debug_net(proc, "send error: incorrect list data size");

				return NXS_NET_E_BUF_INCORRECT_TYPE;
			}

			nxs_net_crypt_gost_reset(proc, con);

			for(i = 0, flag = NXS_NET_FLAG_BUF, buf = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, lst); buf != NULL;
			    buf = nxs_list_ptr_next(lst), i++) {

				if(i == nxs_list_count(lst) - 1) {

					flag |= NXS_NET_FLAG_LAST;
				}

				if((rc = nxs_net_send_buf(proc, con, timeout, buf, flag)) < 0) {

					return rc;
				}
			}

			break;

		default:

			nxs_log_write_debug_net(proc, "send error: unknown data type (type: %d)", type);

			return NXS_NET_E_UNKNOWN_DATATYPE;
	}

	return NXS_NET_E_OK;
}

/*
 * Функция появилась в версии v0.2-1 r14
 *
 * Чтение потоковых данных из канала
 *
 * Возвращаемые значения:
 *
 * * Количество прочитанных байт
 * * NXS_NET_E_TIMEOUT			- тайм-аут при ожидании данных
 * * NXS_NET_E_POLL				- ошибка poll
 * * NXS_NET_E_PEER_SHUTDOWN	- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKREAD			- ошибка чтения сокета
 */
ssize_t nxs_net_recv_stream(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_bchain_t *bchain)
{
	int        nba, pe;
	nxs_buf_t *buf = NULL;
	ssize_t    rc;

	nxs_net_error_con_init(con);

	if((pe = nxs_poll_exec(con->poll_recv, timeout)) != NXS_POLL_E_OK) {

		if(pe == NXS_POLL_E_TIMEOUT) {

			nxs_log_write_debug_net(proc, "recv error: timeout (timeout: %d sec)", timeout / 1000);

			return NXS_NET_E_TIMEOUT;
		}

		if(pe == NXS_POLL_E_INIT) {

			nxs_log_write_debug_net(proc, "recv error: poll error: pointer to poll-recv is NULL");
		}
		else {

			nxs_log_write_debug_net(proc, "recv error: poll error: %s", strerror(pe));
		}

		return NXS_NET_E_POLL;
	}

	ioctl(con->sock, FIONREAD, &nba);

	if(nba <= 0) {

		nba = 1;
	}

	buf = nxs_buf_malloc((size_t)nba);

	if((rc = nxs_net_recv_core(proc, con, timeout, nxs_buf_get_subbuf(buf, 0), (size_t)nba)) > 0) {

		nxs_buf_set_len(buf, (size_t)nba);

		nxs_bchain_add_buf(bchain, buf);
	}
	else {

		buf = nxs_buf_destroy(buf);
	}

	return rc;
}

/*
 * Функция появилась в версии v0.2-1 r14
 *
 * TODO: Реализация функции будет произведена в будущем
 */
ssize_t nxs_net_send_stream()
{

	return 0;
}

/*
 * Приём файла
 *
 * Входные аргументы:
 * * opt		- параметры приёма файлов
 * * metadata	- структура для указания метаданных принимаемого файла. Если указатель не NULL - в переданной структуре будут сохранены
 * 					данные передаваемого файла.
 * 					Если metadata не NULL - она НЕ ДОЛЖНА содержать никаких данных, иначе это приведёт к утечке памяти,
 * т.к.
 * 					над metadata выполняется действие nxs_metadata_init_clear()
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK					- файл получен успешно
 *
 * Не критические ошибки (можно продолжать работу, т.к. работа протокола не нарушена):
 * * NXS_NET_E_FILE_TRX_CANCELED	- передающая сторона отменила передачу файла
 * * NXS_NET_E_CHOWN				- ошибка выполнения chown на загружаемый файл
 * * NXS_NET_E_CHMOD				- ошибка выполнения chmod на загружаемый файл
 * * NXS_NET_E_UTIME				- ошибка выполнения utime на загружаемый файл
 *
 * * Критические ошибки (рекомендуется разрыв соединения, т.к. в канале могут остаться данные, которые нельзя обрабоатать):
 * * NXS_NET_E_POLL					- ошибка poll
 * * NXS_NET_E_TIMEOUT				- тайм-аут при ожидании данных
 * * NXS_NET_E_PEER_SHUTDOWN		- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKREAD				- ошибка чтения сокета
 * * NXS_NET_E_HEADER_INCORRECT		- полученный код заголовка не совпадает с заданным "con->header"
 * * NXS_NET_E_OPT					- не верное указание параметров (одного или нескольких) в структуре opt
 * * NXS_NET_E_TYPE					- не верный тип передаваемых от удалённой стороны данных
 * * NXS_NET_E_OPEN_FILE			- ошибка открытия загружаемого файла
 * * NXS_NET_E_WRITE_FILE			- ошибка записи в загружаемый файл
 * * NXS_NET_E_PTR					- указатель на структуру "md" равен нулю (NULL)
 * * NXS_NET_E_METADATA				- ошибка в метданных
 */
int nxs_net_recv_file(nxs_process_t *proc, nxs_net_connect_t *con, nxs_net_opt_file_recv_t *opt, nxs_metadata_t *metadata)
{
	int             ofd;
	ssize_t         rc, ec, wb;
	size_t          hl;
	off_t           offset, dl, lb, eb;
	nxs_net_hdr_f_t hdr_f;
	u_char          bdy_f, rcv_buf[_NXS_NET_SENDFILE_BUF_SIZE];
	nxs_metadata_t  md, *pmd;
	nxs_string_t    path, *name;
	nxs_buf_t       confirm_msg;
	uid_t           uid;
	gid_t           gid;
	mode_t          perm;
	nxs_bool_t      if_confirm, confirm_res;
	struct utimbuf  ftime;

	nxs_string_init(&path);
	nxs_buf_init(&confirm_msg, 1);

	ofd         = -1;
	ec          = NXS_NET_E_OK;
	confirm_res = NXS_YES;

	if(metadata == NULL) {

		pmd = &md;

		nxs_metadata_init_clear(&md);
	}
	else {

		pmd = metadata;

		nxs_metadata_init_clear(metadata);
	}

	if(nxs_net_opt_file_recv_check(proc, opt) != NXS_TRUE) {

		ec = NXS_NET_E_OPT;
		goto error;
	}

	nxs_net_crypt_gost_reset(proc, con);

	/* Получение общего заголовка */

	if((rc = nxs_net_header_recv(proc, con, opt->timeout, &hdr_f, &dl)) != NXS_NET_E_OK) {

		nxs_log_write_debug_net(proc, "file recv error: header error (code: %d)", rc);

		ec = rc;
		goto error;
	}

	/*
	 * Проверка флагов
	 */

	if((hdr_f & NXS_NET_FLAG_FILE) == 0) {

		nxs_log_write_debug_net(proc, "file recv error: transmitting data type is not a file (flag: %" PRIu32 ")", hdr_f);

		ec = NXS_NET_E_TYPE;
		goto error;
	}

	if(hdr_f & NXS_NET_FLAG_FILE_CANCEL) {

		nxs_log_write_debug_net(proc, "file receive canceled before transferring");

		ec = NXS_NET_E_FILE_TRX_CANCELED;
		goto error;
	}

	if(hdr_f & NXS_NET_FLAG_FILE_CONFIRM) {

		if_confirm = NXS_YES;
	}
	else {

		if_confirm = NXS_NO;
	}

	hl = con->h_len;

	/* Получение смещения в передаваемом файле (пока данная переменная не используется) */

	if((rc = (ssize_t)nxs_net_recv_chunk(proc, con, NXS_NET_TIMEOUT_DEFAULT, &offset, hl, sizeof(off_t))) != NXS_NET_E_OK) {

		ec = rc;
		goto error;
	}

	/* Получение метаданных */

	if((rc = nxs_net_metadata_recv(proc, con, NXS_NET_TIMEOUT_DEFAULT, hl + sizeof(off_t), pmd, NULL)) != NXS_NET_E_OK) {

		ec = rc;
		goto error;
	}

	// nxs_metadata_print(proc, pmd);

	/* Формирование пути до файла, в который будет происходить сохранение передаваемых данных */

	name = nxs_metadata_get_name(pmd);

	nxs_string_cpy_dyn(&path, 0, opt->r_dir, 0);

	if(nxs_string_get_char(&path, nxs_string_len(&path) - 1) != '/') {

		nxs_string_char_add_char_dyn(&path, (u_char)'/');
	}

	nxs_string_cat_dyn(&path, name);

	if((ofd = nxs_fs_open(&path, O_WRONLY | O_CREAT | O_TRUNC, _NXS_NET_DEFAULT_PERM)) < 0) {

		nxs_log_write_debug_net(
		        proc, "file recv error: can't open dst file: %s (file: \"%s\")", strerror(errno), nxs_string_str(&path));

		ec = NXS_NET_E_OPEN_FILE;
		goto error;
	}

	nxs_metadata_get_size(pmd, &lb);

	/* Приём данных */

	while(lb > 0) {

		if(lb >= _NXS_NET_SENDFILE_BUF_SIZE) {

			eb = _NXS_NET_SENDFILE_BUF_SIZE;
		}
		else {

			eb = lb;
		}

		if((rc = nxs_net_recv_core(proc, con, NXS_NET_TIMEOUT_DEFAULT, &bdy_f, sizeof(u_char))) < 0) {

			ec = rc;
			goto error;
		}

		nxs_crypt_gost(&bdy_f, sizeof(u_char), con->gost_el);

		if(bdy_f & NXS_NET_TRX_FLAG_CANCEL) {

			nxs_log_write_debug_net(proc, "file receiving canceled by peer (file: \"%s\")", nxs_string_str(&path));

			ec = NXS_NET_E_FILE_TRX_CANCELED;
			goto error;
		}

		if((rc = nxs_net_recv_core(proc, con, NXS_NET_TIMEOUT_DEFAULT, rcv_buf, (size_t)eb)) < 0) {

			ec = rc;
			goto error;
		}

		nxs_crypt_gost(rcv_buf, (size_t)eb, con->gost_el);

		if((wb = write(ofd, rcv_buf, (size_t)eb)) < 0 || wb != (ssize_t)eb) {

			if(wb < 0) {

				nxs_log_write_debug_net(proc,
				                        "file recv error: can't write into dst file: %s (file: \"%s\")",
				                        strerror(errno),
				                        nxs_string_str(&path));
			}
			else {

				nxs_log_write_debug_net(
				        proc,
				        "file recv error: can't write into dst file: wrote less bytes then expected (file: \"%s\")",
				        nxs_string_str(&path));
			}

			ec = NXS_NET_E_WRITE_FILE;
			goto error;
		}

		lb -= eb;
	}

	/*
	 * Установка прав доступа, uid/gid и времени модификации и открытия, если это разрешается
	 *
	 * Если файл существовал в системе - то права на него будут сохранены такими же, как были до передачи файла (т.е. изменится только
	 * содержимое файла и частично метаданные (время правки), права доступа и владец/группа будут сохранены)
	 */

	if(opt->allow_attrs == NXS_YES) {

		if(nxs_metadata_get_uid(pmd, &uid) == NXS_METADATA_E_OK) {

			if(nxs_metadata_get_gid(pmd, &gid) == NXS_METADATA_E_OK) {

				if(chown((char *)nxs_string_str(&path), uid, gid) < 0) {

					nxs_log_write_debug_net(proc,
					                        "file recv error: chown error: %s (file: \"%s\")",
					                        strerror(errno),
					                        nxs_string_str(&path));

					nxs_buf_cpy_dyn(&confirm_msg, 0, _NXS_NET_SENDFILE_C_CHOWN, sizeof(_NXS_NET_SENDFILE_C_CHOWN));
					confirm_res = NXS_NO;

					ec = NXS_NET_E_CHOWN;
					goto confirm;
				}
			}
		}

		if(nxs_metadata_get_perm(pmd, &perm) == NXS_METADATA_E_OK) {

			if(chmod((char *)nxs_string_str(&path), perm) < 0) {

				nxs_log_write_debug_net(
				        proc, "file recv error: chmod error: %s (file: \"%s\")", strerror(errno), nxs_string_str(&path));

				nxs_buf_cpy_dyn(&confirm_msg, 0, _NXS_NET_SENDFILE_C_CHMOD, sizeof(_NXS_NET_SENDFILE_C_CHMOD));
				confirm_res = NXS_NO;

				ec = NXS_NET_E_CHMOD;
				goto confirm;
			}
		}

		if(nxs_metadata_get_atime(pmd, &ftime.actime) == NXS_METADATA_E_OK) {

			if(nxs_metadata_get_mtime(pmd, &ftime.modtime) == NXS_METADATA_E_OK) {

				if(utime((char *)nxs_string_str(&path), &ftime) < 0) {

					nxs_log_write_debug_net(proc,
					                        "file recv error: utime error: %s (file: \"%s\")",
					                        strerror(errno),
					                        nxs_string_str(&path));

					nxs_buf_cpy_dyn(&confirm_msg, 0, _NXS_NET_SENDFILE_C_UTIME, sizeof(_NXS_NET_SENDFILE_C_UTIME));
					confirm_res = NXS_NO;

					ec = NXS_NET_E_UTIME;
					goto confirm;
				}
			}
		}
	}

	nxs_buf_cpy_dyn(&confirm_msg, 0, "", sizeof(u_char));

confirm:

	if(if_confirm == NXS_YES) {

		if((rc = nxs_net_send_confirm(proc, con, NXS_NET_TIMEOUT_DEFAULT, confirm_res, &confirm_msg)) < 0) {

			ec = rc;
			goto error;
		}
	}

error:

	nxs_metadata_free(&md);

	nxs_string_free(&path);
	nxs_buf_free(&confirm_msg);

	if(ofd >= 0) {

		close(ofd);
	}

	return (int)ec;
}

/*
 * Отправка файла
 *
 * В процессе отправки за файлом устанавливается наблюдение. Если в процессе отправки файла он будет удалён или изменён - отправка
 * будет прервана с уведомлением удалённой стороны. В этом случае производить разрыв соединения не требуется.
 *
 * Разрыв соединения рекомендуется производить только в случае сетевых ошибок. Остальные ошибки могут быть корректно обработаны
 * без нарушения протокола передачи файла.
 *
 * Входные аргументы:
 * * opt		- параметры отправки файла (так же, эта структура содержит текст подтверждения (или пустую строку))
 * * path		- путь отправляемого файла
 * * metadata	- структура для указания метаданных отправляемого файла.
 *
 * 					Если указатель не NULL и при этом поле "opt->fill_meta"	имеет значение NXS_METADATA_V_NONE - в
 * переданной
 * структуре
 * 					должны содержаться данные отправляемого файла (на случай, если необходимо строго задать имя
 * отправляемого
 * файла,
 * 					права доступа и др;	важно: размер файла (в структуре "metadata" в любом случае будет
 * переопределён
 * на
 * реальный размер файла)).
 *
 * 					Если указатель не NULL и при этом поле "opt->fill_meta" имеет значение отличное NXS_METADATA_V_NONE
 * (т.е.
 * содержит номер конкретной
 * 					версии) - структура метаданных "metadata" будет очищена и заполнена информацией в соответствии с
 * требуемой
 * версией метаданных
 * 					для конкретного файла. Это может постребоваться для последующей обработки информации по
 * отправленному
 * файлу
 * после завершения данной
 * 					функции.
 *
 * 					Если "metadata" равна NULL - будет использована временна структура метаданных, которая очищается
 * после
 * выполнения функции.
 * 					При этом, если "opt->fill_meta" имеет значение отличное NXS_METADATA_V_NONE (т.е. содержит номер
 * конкретной
 * версии) -
 * 					будут использованы метаданные указанной версии, иначе будут использованы метаданные версии
 * NXS_METADATA_V2
 * 					(т.е. с минимальным набором передаваемой информации).
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK				- ошибок при отправке файла не зафиксировано
 *
 * Не критические ошибки (можно продолжать работу, т.к. работа протокола не нарушена):
 * * NXS_NET_E_OPT				- не верно заполнена структура "opt"
 * * NXS_NET_E_STAT				- не удалось получить размер отправляемого файла
 * * NXS_NET_E_TYPE				- объект ФС по пути "path" не является файлом
 * * NXS_NET_E_METADATA			- ошибка заполнения метаданных
 * * NXS_NET_E_INOTIFY			- ошибка установления наблюдения за отправляемым файлом
 * * NXS_NET_E_OPEN_FILE		- ошибка при открытии отправляемого файла
 * * NXS_NET_E_CHANGED			- отправляемый файл обновлён/изменён в процессе отправки
 * * NXS_NET_E_READ_FILE		- ошибка при чтении отправляемого файла в процессе отправки
 * * NXS_NET_E_CONFIRM_NEGATIVE	- получено негативное подтверждение от удалённой стороны о приёме файла
 *
 * Критические ошибки (рекомендуется разрыв соединения, т.к. в канале могут остаться данные, которые удалённая сторона не сможет
 * обрабоатать):
 * * NXS_NET_E_POLL				- ошибка poll
 * * NXS_NET_E_TIMEOUT			- тайм-аут при отправке данных
 * * NXS_NET_E_PEER_SHUTDOWN	- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKWRITE		- ошибка записи в сокет
 */
int nxs_net_send_file(nxs_process_t *          proc,
                      nxs_net_connect_t *      con,
                      nxs_net_opt_file_send_t *opt,
                      nxs_string_t *           path,
                      nxs_metadata_t *         metadata)
{
	int                 ifd, i_rc;
	size_t              eb;
	off_t               offset, lb;
	ssize_t             rc, ec, rb;
	nxs_net_hdr_f_t     hdr_f;
	u_char              bdy_f, snd_buf[sizeof(u_char) + _NXS_NET_SENDFILE_BUF_SIZE];
	nxs_fs_file_check_t fc;

	ec = NXS_NET_E_OK;

	nxs_string_clear(&opt->confirm_txt);

	ifd = -1;

	offset = 0;

	hdr_f = NXS_NET_FLAG_FILE;

	nxs_net_crypt_gost_reset(proc, con);

	nxs_buf_set_len(&con->tmp_buf, 0);

	nxs_fs_file_check_upd_init_clean(proc, &fc);

	if(nxs_net_opt_file_send_check(proc, opt) != NXS_TRUE) {

		macro_nxs_net_send_file_hdr_emrg(NXS_NET_E_OPT);
	}

	/*
	 * Подготовка и сериализация метаданных для отправляемого файла
	 */

	if((rc = (ssize_t)nxs_net_send_file_metadata_prep(proc, con, path, opt->fill_meta, metadata, &lb)) != NXS_NET_E_OK) {

		macro_nxs_net_send_file_hdr_emrg(rc);
	}

	if((i_rc = nxs_fs_file_check_upd_create(proc, &fc, path)) != NXS_FS_E_OK) {

		nxs_log_write_debug_net(
		        proc, "file send error: can't watch for sending file (fs ret code: %d, path: \"%s\")", i_rc, nxs_string_str(path));

		macro_nxs_net_send_file_hdr_emrg(NXS_NET_E_INOTIFY);
	}

	if((ifd = nxs_fs_open(path, O_RDONLY)) < 0) {

		nxs_log_write_debug_net(proc, "file send error: can't open file: %s (path: \"%s\")", strerror(errno), nxs_string_str(path));

		macro_nxs_net_send_file_hdr_emrg(NXS_NET_E_OPEN_FILE);
	}

	/*
	 * TODO: Проверка возможности докачки файла (элемент будет реализован в будущем).
	 * В соответствии с этими изменениями предполагается корректровка параметра offset.
	 * Пока он всегда равен 0
	 */

	if(opt->confirm == NXS_YES) {

		hdr_f |= NXS_NET_FLAG_FILE_CONFIRM;
	}

	macro_nxs_net_send_file_hdr();

	/* ================================ */

	/*
	 * Процесс отправки файла
	 */

	bdy_f = NXS_NET_TRX_FLAG_CONTINUE;

	while(lb > 0) {

		if((i_rc = nxs_fs_file_check_upd(proc, &fc)) != NXS_FS_E_OK) {

			nxs_log_write_debug_net(proc, "file send error: file changed/updated (err code: %d)", i_rc);

			macro_nxs_net_send_file_cancel(NXS_NET_E_CHANGED);
		}

		if(lb >= _NXS_NET_SENDFILE_BUF_SIZE) {

			eb = _NXS_NET_SENDFILE_BUF_SIZE;
		}
		else {

			eb = lb;
		}

		if((rb = read(ifd, snd_buf + sizeof(u_char), eb)) < 0 || rb != (ssize_t)eb) {

			if(rb < 0) {

				nxs_log_write_debug_net(proc, "file read error: %s", strerror(errno));
			}
			else {

				nxs_log_write_debug_net(proc, "file read error: read less bytes then expected");
			}

			macro_nxs_net_send_file_cancel(NXS_NET_E_READ_FILE);
		}

		nxs_memcpy(snd_buf, &bdy_f, sizeof(u_char));

		nxs_crypt_gost(snd_buf, eb + sizeof(u_char), con->gost_el);

		if((rc = nxs_net_send_core(proc, con, NXS_NET_TIMEOUT_DEFAULT, snd_buf, eb + sizeof(u_char))) < 0) {

			ec = rc;
			goto error;
		}

		lb -= eb;
	}

	if(opt->confirm == NXS_YES) {

		if((rc = nxs_net_recv_confirm(proc, con, NXS_NET_TIMEOUT_DEFAULT)) < 0) {

			nxs_log_write_debug_net(proc, "file send error: confirm receive error (code: %d)", rc);

			ec = rc;
			goto error;
		}

		if(rc == NXS_NO) {

			nxs_log_write_debug_net(proc,
			                        "file send: confirm from peer is negative (confirm text: \"%s\", sending file: \"%s\")",
			                        nxs_buf_get_subbuf(&con->tmp_buf, 0),
			                        nxs_string_str(path));

			ec = NXS_NET_E_CONFIRM_NEGATIVE;
		}
		else {

			nxs_log_write_debug_net(proc,
			                        "file send: confirm from peer is positive (confirm text: \"%s\", sending file: \"%s\")",
			                        nxs_buf_get_subbuf(&con->tmp_buf, 0),
			                        nxs_string_str(path));
		}

		nxs_buf_to_string(&con->tmp_buf, 0, &opt->confirm_txt);
	}

error:

	if(ifd >= 0) {

		close(ifd);
	}

	nxs_fs_file_check_upd_free(proc, &fc);

	return (int)ec;
}

/*
 * Получение подтверждения
 *
 * Не производит сброса контекста шифрования
 *
 * Если длина буфера con->tmp_buf больше нуля - полученное подтверждение содержит текстовое пояснение
 *
 * Возвращаемые значения:
 * * NXS_NO							- получено подтверждение (негативное)
 * * NXS_YES						- получено подтверждение (позитивное)
 *
 * * NXS_NET_E_POLL					- ошибка poll
 * * NXS_NET_E_TIMEOUT				- тайм-аут при ожидании данных
 * * NXS_NET_E_PEER_SHUTDOWN		- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKREAD				- ошибка чтения сокета
 * * NXS_NET_E_HEADER_INCORRECT		- полученный код заголовка не совпадает с заданным "con->header"
 * * NXS_NET_E_BUF_SIZE_EXCEEDED	- размер ожидаемых данных больше допустимого (_NXS_NET_TMP_BUF_MAX_SIZE)
 * * NXS_NET_E_TYPE					- тип принятых данных (т.е. флаг из заголовка пакета) не верный
 */
int nxs_net_recv_confirm(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout)
{
	int             rc;
	nxs_net_hdr_f_t hdr_f;

	if((rc = (int)nxs_net_recv_buf(proc, con, timeout, &con->tmp_buf, &hdr_f)) < 0) {

		return rc;
	}

	/*
	 * Проверка типа передаваемых данных
	 */
	if((hdr_f & NXS_NET_FLAG_CONFIRM_NO) == 0 && (hdr_f & NXS_NET_FLAG_CONFIRM_YES) == 0) {

		nxs_log_write_debug_net(proc, "confirm recv error: transmitting data type is not confimation (flag: %" PRIu32 ")", hdr_f);

		return NXS_NET_E_TYPE;
	}

	if(hdr_f & NXS_NET_FLAG_CONFIRM_NO) {

		return NXS_NO;
	}

	return NXS_YES;
}

/*
 * Отправка подтверждения
 *
 * Не производит сброса контекста шифрования
 *
 * confirm содержит конкретный тип подтверждния:
 * * NXS_NO		- негативное подтверждение
 * * NXS_YES	- позитивное подтверждение
 *
 * "data" содержит текстовые пояснения, отправляемые вместе с подтверждением. Аргумент не должен быть равен NULL
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK				- отправка подтверждения успешна
 * * NXS_NET_E_POLL				- ошибка poll
 * * NXS_NET_E_TIMEOUT			- тайм-аут при отправке данных
 * * NXS_NET_E_PEER_SHUTDOWN	- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKWRITE		- ошибка записи в сокет
 * * NXS_NET_E_BUF_NULL			- указатель на буфер "data" равен NULL
 */
int nxs_net_send_confirm(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_bool_t confirm, nxs_buf_t *data)
{
	int             rc;
	nxs_net_hdr_f_t hdr_f;

	if(data == NULL) {

		nxs_log_write_debug_net(proc, "confirm send error: confirm message buf is NULL");

		return NXS_NET_E_BUF_NULL;
	}

	hdr_f = NXS_NET_FLAG_LAST | NXS_NET_FLAG_BUF;

	if(confirm == NXS_NO) {

		hdr_f |= NXS_NET_FLAG_CONFIRM_NO;
	}
	else {

		hdr_f |= NXS_NET_FLAG_CONFIRM_YES;
	}

	if((rc = nxs_net_send_buf(proc, con, timeout, data, hdr_f)) < 0) {

		return rc;
	}

	return NXS_NET_E_OK;
}

/*
 * Инициализация структуры, содержащей опции, используемые при приёме файла
 */
void nxs_net_opt_file_recv_init(nxs_net_opt_file_recv_t *opt)
{

	if(opt == NULL) {

		return;
	}

	opt->timeout     = NXS_NET_TIMEOUT_DEFAULT;
	opt->r_dir       = NULL;
	opt->allow_attrs = NXS_YES;
}

/*
 * Освобождение структуры, содержащей опции, используемые при приёме файла
 */
void nxs_net_opt_file_recv_free(nxs_net_opt_file_recv_t *opt)
{

	if(opt == NULL) {

		return;
	}

	opt->timeout     = NXS_NET_TIMEOUT_DEFAULT;
	opt->r_dir       = NULL;
	opt->allow_attrs = NXS_YES;
}

/*
 * Функция для проверки правильности заполнения всех нужных полней в структуре opt.
 *
 * Производятся следующие проверки:
 * * Если r_dir == NULL будет возвращён NXS_FALSE
 *
 * Аргумент "proc" нужен для того, чтобы была возможность сделать запись в лог
 *
 * Возвращаемые значения:
 * * NXS_TRUE	- если все опции имеют корректные значения
 * * NXS_FALSE	- в случае ошибки в одной или нескольких опциях (проверка производится до первой найденой ошибки)
 */
nxs_bool_t nxs_net_opt_file_recv_check(nxs_process_t *proc, nxs_net_opt_file_recv_t *opt)
{

	if(opt->r_dir == NULL) {

		nxs_log_write_debug_net(proc, "file recv error: recv dir ptr in options is NULL");

		return NXS_FALSE;
	}

	return NXS_TRUE;
}

/*
 * Инициализация структуры, содержащей опции, используемые при отправке файла
 */
void nxs_net_opt_file_send_init(nxs_net_opt_file_send_t *opt)
{

	if(opt == NULL) {

		return;
	}

	opt->confirm   = NXS_NO;
	opt->fill_meta = NXS_METADATA_V_NONE;
	opt->timeout   = NXS_NET_TIMEOUT_DEFAULT;

	nxs_string_init2(&opt->confirm_txt, 0, NXS_STRING_EMPTY_STR);
}

/*
 * Освобождение структуры, содержащей опции, используемые при отправке файла
 */
void nxs_net_opt_file_send_free(nxs_net_opt_file_send_t *opt)
{

	if(opt == NULL) {

		return;
	}

	opt->confirm   = NXS_NO;
	opt->fill_meta = NXS_METADATA_V_NONE;
	opt->timeout   = NXS_NET_TIMEOUT_DEFAULT;

	nxs_string_free(&opt->confirm_txt);
}

/*
 * Функция для проверки правильности заполнения всех нужных полней в структуре opt.
 * На текущий момент эта функция всегда возвращает NXS_TRUE, т.к. пока список опций такой, что проверки не требуется. Однако, для того,
 * чтобы
 * упростить в будущем внедрение новых опций - делается эта функция-заглушка.
 * Аргумент "proc" нужен для того, чтобы была возможность сделать запись в лог
 */
nxs_bool_t nxs_net_opt_file_send_check(nxs_process_t *proc, nxs_net_opt_file_send_t *opt)
{

	return NXS_TRUE;
}

int nxs_net_get_conn_fd(nxs_net_connect_t *con)
{

	if(con == NULL) {

		return 0;
	}

	return con->sock;
}

int nxs_net_get_srv_conn_fd(nxs_net_server_t *srv)
{

	if(srv == NULL) {

		return 0;
	}

	return srv->l_sock;
}

int nxs_net_get_unix_srv_conn_fd(nxs_net_unix_server_t *srv)
{

	if(srv == NULL) {

		return 0;
	}

	return srv->l_sock;
}

u_char *nxs_net_strerror(int _errno)
{
	u_int i;

	for(i = 0; nxs_net_errors[i].text != NULL; i++) {

		if(nxs_net_errors[i]._errno == _errno) {

			return nxs_net_errors[i].text;
		}
	}

	return _NXS_ERRNO_UNKNOWN;
}

/*
 * Обработка кодов возврата функции nxs_net_recv_file().
 * Т.к. функция возвращает коды, которые, во-первых, представлены большим набором, во-вторых, их не всегда нужно детально разбирать,
 * в-третьих, все коды можно разделить на:
 * * успешный код: файл получен и успешно сохранён
 * * условно успешные: файл получен (но не выполнены последующие действия) или передача отменена удалённой стороной. При этом нарушения
 * 		протокола не произошло и программа может продолжать сетевые коммуникации дальше
 * * не успешные: файл не получен и из-за возможности нарушения протокола - необходимо разорвать соединение
 *
 * Возвращаемые значения:
 * *
 */
int nxs_net_recv_file_err(int ec)
{

	switch(ec) {

		/*
		 * Успешное завершение
		 */

		case NXS_NET_E_OK:

			return NXS_NET_PROTOCOL_E_OK;

		/*
		 * Не критические (с точки зрения протокола) ошибки
		 */

		case NXS_NET_E_FILE_TRX_CANCELED:
		case NXS_NET_E_CHOWN:
		case NXS_NET_E_CHMOD:
		case NXS_NET_E_UTIME:

			return NXS_NET_PROTOCOL_E_WARN;

		/*
		 * Критические (с точки зрения протокола) ошибки
		 */

		case NXS_NET_E_POLL:
		case NXS_NET_E_TIMEOUT:
		case NXS_NET_E_PEER_SHUTDOWN:
		case NXS_NET_E_SOCKREAD:
		case NXS_NET_E_HEADER_INCORRECT:
		case NXS_NET_E_OPT:
		case NXS_NET_E_TYPE:
		case NXS_NET_E_OPEN_FILE:
		case NXS_NET_E_WRITE_FILE:
		case NXS_NET_E_PTR:
		case NXS_NET_E_METADATA:
		default:

			return NXS_NET_PROTOCOL_E_FATAL;
	}
}

/*
 * Обработка кодов возврата функции nxs_net_send_file().
 * Т.к. функция возвращает коды, которые, во-первых, представлены большим набором, во-вторых, их не всегда нужно детально разбирать,
 * в-третьих, все коды можно разделить на:
 * * успешный код: файл отправлен
 * * условно успешные: файл не отправлен, но нарушения протокола не произошло и программа может продолжать сетевые коммуникации дальше
 * * не успешные: файл не отправлен и из-за возможности нарушения протокола - необходимо разорвать соединение
 *
 * Возвращаемые значения:
 * *
 */
int nxs_net_send_file_err(int ec)
{

	switch(ec) {

		/*
		 * Успешное завершение
		 */

		case NXS_NET_E_OK:

			return NXS_NET_PROTOCOL_E_OK;

		/*
		 * Не критические (с точки зрения протокола) ошибки
		 */

		case NXS_NET_E_OPT:
		case NXS_NET_E_STAT:
		case NXS_NET_E_TYPE:
		case NXS_NET_E_METADATA:
		case NXS_NET_E_INOTIFY:
		case NXS_NET_E_OPEN_FILE:
		case NXS_NET_E_CHANGED:
		case NXS_NET_E_READ_FILE:
		case NXS_NET_E_CONFIRM_NEGATIVE:

			return NXS_NET_PROTOCOL_E_WARN;

		/*
		 * Критические (с точки зрения протокола) ошибки
		 */

		case NXS_NET_E_POLL:
		case NXS_NET_E_TIMEOUT:
		case NXS_NET_E_PEER_SHUTDOWN:
		case NXS_NET_E_SOCKWRITE:
		default:

			return NXS_NET_PROTOCOL_E_FATAL;
	}
}

/* Module internal (static) functions */

static size_t nxs_net_header_len_calc(nxs_string_t *header)
{

	return nxs_string_len(header) + sizeof(nxs_net_hdr_f_t) + sizeof(off_t);
}

static void nxs_net_header_prep(nxs_buf_t *buf, nxs_string_t *header, nxs_net_hdr_f_t flag, off_t size)
{

	nxs_buf_set_len(buf, 0);

	/*
	 * header
	 */
	nxs_buf_cat_dyn(buf, nxs_string_str(header), nxs_string_len(header));

	/*
	 * flag
	 */
	nxs_buf_cat_dyn(buf, &flag, sizeof(nxs_net_hdr_f_t));

	/*
	 * size
	 */
	nxs_buf_cat_dyn(buf, &size, sizeof(off_t));
}

static void nxs_net_header_set_hdr(nxs_net_connect_t *con)
{

	nxs_buf_cpy_dyn(&con->tmp_buf, 0, nxs_string_str(&con->header), nxs_string_len(&con->header));
}

static void nxs_net_header_set_flag(nxs_net_connect_t *con, nxs_net_hdr_f_t flag)
{

	nxs_buf_cpy_dyn(&con->tmp_buf, nxs_string_len(&con->header), &flag, sizeof(nxs_net_hdr_f_t));
}

static void nxs_net_header_set_size(nxs_net_connect_t *con, off_t size)
{

	nxs_buf_cpy_dyn(&con->tmp_buf, nxs_string_len(&con->header) + sizeof(nxs_net_hdr_f_t), &size, sizeof(off_t));
}

static void nxs_net_header_set_offset(nxs_net_connect_t *con, off_t offset)
{

	nxs_buf_cpy_dyn(&con->tmp_buf, con->h_len, &offset, sizeof(off_t));
}

/*
 * Сериализация метаданных в буфер
 */
static size_t nxs_net_header_set_metadata(nxs_net_connect_t *con, nxs_metadata_t *md)
{
	off_t dsize; // тип off_t используется для того, чтобы в функции nxs_buf_cpy_dyn() мы записали нужную информацию длиной
	             // sizeof(off_t) байт

	nxs_metadata_to_buf(md, con->h_len + sizeof(off_t) + sizeof(off_t), &con->tmp_buf, (size_t *)&dsize);

	nxs_buf_cpy_dyn(&con->tmp_buf, con->h_len + sizeof(off_t), &dsize, sizeof(off_t));

	return (size_t)dsize;
}

static int nxs_net_header_get_flag(nxs_net_connect_t *con, nxs_net_hdr_f_t *flag)
{
	return nxs_buf_get_mem(&con->tmp_buf, nxs_string_len(&con->header), flag, sizeof(nxs_net_hdr_f_t));
}

static int nxs_net_header_get_size(nxs_net_connect_t *con, off_t *size)
{

	return nxs_buf_get_mem(&con->tmp_buf, nxs_string_len(&con->header) + sizeof(nxs_net_hdr_f_t), size, sizeof(off_t));
}

/*
static int nxs_net_header_get_offset(nxs_net_connect_t *con, size_t *offset)
{

        return nxs_buf_get_mem(&con->tmp_buf, con->h_len, offset, sizeof(size_t));
}

static int nxs_net_header_get_metadata(nxs_net_connect_t *con, nxs_metadata_t *md, size_t *msize)
{

        return nxs_metadata_from_buf(md, con->h_len + sizeof(size_t), &con->tmp_buf, msize);
}

static int nxs_net_header_get_metadata_ver(nxs_net_connect_t *con, nxs_metadata_v_t *version)
{

        return nxs_buf_get_mem(&con->tmp_buf, con->h_len + sizeof(size_t), &version, sizeof(nxs_metadata_v_t));
}*/

#define macro_buf_mem_net_hdr(d, ds)         \
	l = ds;                              \
	nxs_buf_get_mem(buf, _offset, d, l); \
	_offset += l;

/*
 * Выделение из базового заголовка, хранящегося в буфере "buf" значений "flag" и "size"
 *
 * Если указатели "flag" и "size" равны NULL, то это не вызовет ошибки (но и данные не будут сохранены)
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK					- необходимые данные из заголовка получены успешно
 * * NXS_NET_E_BUF_INCORRECT_SIZE	- буфер из которого необходимо выделить требуемые данные имеет меньшую длину, чем необходимо
 * * NXS_NET_E_HEADER_INCORRECT		- полученный код заголовка не совпадает с заданным "con->header"
 */
static int nxs_net_header_parse(nxs_net_connect_t *con, nxs_buf_t *buf, nxs_net_hdr_f_t *flag, off_t *size)
{

	if(nxs_buf_get_len(buf) < con->h_len) {

		return NXS_NET_E_BUF_INCORRECT_SIZE;
	}

	if(nxs_string_char_ncmp(&con->header, 0, nxs_buf_get_subbuf(buf, 0), nxs_string_len(&con->header)) == NXS_STRING_CMP_NE) {

		return NXS_NET_E_HEADER_INCORRECT;
	}

	nxs_net_header_get_flag(con, flag);
	nxs_net_header_get_size(con, size);

	return NXS_NET_E_OK;
}

/*
 * Получение основного заголовка сетевого протокола
 *
 * Полученные данные дешифруются (если надо) и сохраняются в ячейках памяти на которые указывают "flag" и "size"
 *
 * Если указатели "flag" и "size" равны NULL, то это не вызовет ошибки (но и данные не будут сохранены)
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK				- заголовок и необходимые данные из него успешно получены
 * * NXS_NET_E_POLL				- ошибка poll
 * * NXS_NET_E_TIMEOUT			- тайм-аут при ожидании данных
 * * NXS_NET_E_PEER_SHUTDOWN	- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKREAD			- ошибка чтения сокета
 * * NXS_NET_E_HEADER_INCORRECT	- полученный код заголовка не совпадает с заданным "con->header"
 */
static int nxs_net_header_recv(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_net_hdr_f_t *flag, off_t *size)
{
	ssize_t rc;
	size_t  hl;
	u_char *buf;

	hl = con->h_len;

	if(nxs_buf_get_size(&con->tmp_buf) < hl) {

		nxs_buf_resize(&con->tmp_buf, hl);
	}

	buf = nxs_buf_get_subbuf(&con->tmp_buf, 0);

	if((rc = nxs_net_recv_core(proc, con, timeout, buf, hl)) < 0) {

		nxs_buf_set_len(&con->tmp_buf, 0);

		return rc;
	}

	nxs_buf_set_len(&con->tmp_buf, hl);

	nxs_crypt_gost_buf(&con->tmp_buf, con->gost_el);

	if((rc = (ssize_t)nxs_net_header_parse(con, &con->tmp_buf, flag, size)) != NXS_NET_E_OK) {

		return NXS_NET_E_HEADER_INCORRECT;
	}

	return NXS_NET_E_OK;
}

/*
 * Получение буфера из сети.
 * Полученные данные будут записаны в буфер "data", в область памяти, на которую указывает "flag" будут записаны флаги.
 *
 * Если указатель "flag" равен NULL, то это не вызовет ошибки (но и данные не будут сохранены)
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK					- заголовок и необходимые данные из него успешно получены
 * * NXS_NET_E_POLL					- ошибка poll
 * * NXS_NET_E_TIMEOUT				- тайм-аут при ожидании данных
 * * NXS_NET_E_PEER_SHUTDOWN		- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKREAD				- ошибка чтения сокета
 * * NXS_NET_E_HEADER_INCORRECT		- полученный код заголовка не совпадает с заданным "con->header"
 * * NXS_NET_E_BUF_SIZE_EXCEEDED	- размер ожидаемых данных больше допустимого (_NXS_NET_TMP_BUF_MAX_SIZE)
 */
static ssize_t nxs_net_recv_buf(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_buf_t *data, nxs_net_hdr_f_t *flag)
{
	ssize_t rc;
	off_t   dl;

	if((rc = nxs_net_header_recv(proc, con, timeout, flag, &dl)) != NXS_NET_E_OK) {

		nxs_log_write_debug_net(proc, "buf recv error (code: %d)", rc);

		nxs_buf_set_len(data, 0);

		return rc;
	}

	if(dl > _NXS_NET_TMP_BUF_MAX_SIZE) {

		nxs_buf_set_len(data, 0);

		nxs_log_write_debug_net(proc, "buf recv error: max buf transfer size exceeded (code: %d)", NXS_NET_E_BUF_SIZE_EXCEEDED);

		return NXS_NET_E_BUF_SIZE_EXCEEDED;
	}

	/*
	 * Добавляем доп. байт, в который потом можно будет положить '\0' без дополнительных проблем
	 */
	if(nxs_buf_get_size(data) < (size_t)dl + 1) {

		nxs_buf_resize(data, (size_t)dl + 1);
	}

	if((rc = nxs_net_recv_core(proc, con, NXS_NET_TIMEOUT_DEFAULT, nxs_buf_get_subbuf(data, 0), (size_t)dl)) < 0) {

		nxs_buf_set_len(data, 0);

		return rc;
	}

	nxs_buf_set_len(data, (size_t)dl);

	nxs_crypt_gost_buf(data, con->gost_el);

	return NXS_NET_E_OK;
}

/*
 * Отправка буфера
 *
 * Отправляемый буфер - "data", в "flag" - содержатся флаги отправляемого пакета
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK				- отправка буфера успешна
 * * NXS_NET_E_POLL				- ошибка poll
 * * NXS_NET_E_TIMEOUT			- тайм-аут при отправке данных
 * * NXS_NET_E_PEER_SHUTDOWN	- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKWRITE		- ошибка записи в сокет
 */
static ssize_t nxs_net_send_buf(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_buf_t *data, nxs_net_hdr_f_t flag)
{
	ssize_t rc;

	nxs_net_header_prep(&con->tmp_buf, &con->header, flag, nxs_buf_get_len(data));

	nxs_buf_cat_dyn(&con->tmp_buf, nxs_buf_get_subbuf(data, 0), nxs_buf_get_len(data));

	/*
	 * Шифрование передаваемого блока данных (вместе с заголовком прикладного уровня).
	 * Если con->gost_el == NULL, т.е. если шифрование отключено - данные шифроваться не будут (проверка производится в
	 * nxs_crypt_gost_buf())
	 */
	nxs_crypt_gost_buf(&con->tmp_buf, con->gost_el);

	if((rc = nxs_net_send_core(proc, con, timeout, nxs_buf_get_subbuf(&con->tmp_buf, 0), nxs_buf_get_len(&con->tmp_buf))) < 0) {

		return rc;
	}

	return NXS_NET_E_OK;
}

/*
 * Проверка готовности сокета, находящегося в неблокирующем режиме
 *
 * Возвращаемые значения:
 *
 * * NXS_NET_E_OK		- сокет готов к работе
 * * NXS_NET_E_POLL		- ошибка poll (рекомендуется завершить процесс подключения)
 * * NXS_NET_E_TIMEOUT	- за время timeout сокет не сообщил о готовности
 * * NXS_NET_E_CONNECT	- сокет не готов к работе
 */
static int nxs_net_check_conn_ready(nxs_process_t *proc, nxs_net_connect_t *con, int timeout)
{
	nxs_poll_t p;
	int        rc, pe, sock_opt;
	socklen_t  l;

	rc = NXS_NET_E_OK;

	nxs_poll_init(&p);

	if(nxs_poll_add(&p, con->sock, POLLOUT) != NXS_POLL_E_OK) {

		nxs_log_write_debug_net(proc, "connect error: %s", nxs_net_strerror(NXS_NET_ERRNO_BAD_SOCK_VAL));

		rc = NXS_NET_E_POLL;

		goto error;
	}

	if((pe = nxs_poll_exec(&p, timeout)) != NXS_POLL_E_OK) {

		if(pe == NXS_POLL_E_TIMEOUT) {

			nxs_log_write_debug_net(proc, "connect error: socket timeout (timeout: %d sec)", timeout / 1000);

			rc = NXS_NET_E_TIMEOUT;

			goto error;
		}

		nxs_log_write_debug_net(proc, "connect error: poll error: %s", strerror(pe));

		rc = NXS_NET_E_POLL;

		goto error;
	}

	if(nxs_poll_check_events(&p, con->sock, POLLOUT) == NXS_POLL_EVENTS_FALSE) {

		nxs_log_write_debug_net(proc, "connect error: socket not ready to send data");

		rc = NXS_NET_E_CONNECT;

		goto error;
	}

	sock_opt = 0;
	l        = sizeof(sock_opt);

	getsockopt(con->sock, SOL_SOCKET, SO_ERROR, (void *)(&sock_opt), &l);

	if(sock_opt) {

		nxs_log_write_debug_net(proc, "connect error: socket error: %s", strerror(sock_opt));

		rc = NXS_NET_E_CONNECT;

		goto error;
	}

error:

	nxs_poll_free(&p);

	return rc;
}

static int nxs_net_sock_set_opt(nxs_process_t *proc, nxs_net_connect_t *con, int opt)
{
	int sock_arg;

	nxs_net_error_con_init(con);

	if(con == NULL) {

		nxs_net_error_con_set(con, NXS_NET_ERRNO_PTR_NULL);

		nxs_log_write_debug_net(proc, "sock_set_opt error: %s", nxs_net_strerror(NXS_NET_ERRNO_PTR_NULL));

		return NXS_NET_E_FCNTL;
	}

	if(con->sock == NXS_NET_INIT_SOCK) {

		nxs_net_error_con_set(con, NXS_NET_ERRNO_BAD_SOCK_VAL);

		nxs_log_write_debug_net(proc, "sock_set_opt error: %s", nxs_net_strerror(NXS_NET_ERRNO_BAD_SOCK_VAL));

		return NXS_NET_E_FCNTL;
	}

	if((sock_arg = fcntl(con->sock, F_GETFL, NULL)) < 0) {

		nxs_log_write_debug_net(proc, "sock_set_opt error: %s", strerror(errno));

		nxs_net_error_con_set(con, NXS_NET_ERRNO_FCNTL_GET);

		return NXS_NET_E_FCNTL;
	}

	sock_arg |= opt;

	if(fcntl(con->sock, F_SETFL, sock_arg) < 0) {

		nxs_log_write_debug_net(proc, "sock_set_opt error: %s", strerror(errno));

		nxs_net_error_con_set(con, NXS_NET_ERRNO_FCNTL_SET);

		return NXS_NET_E_FCNTL;
	}

	return NXS_NET_E_OK;
}

static int nxs_net_sock_remove_opt(nxs_process_t *proc, nxs_net_connect_t *con, int opt)
{
	int sock_arg;

	nxs_net_error_con_init(con);

	if(con == NULL) {

		nxs_net_error_con_set(con, NXS_NET_ERRNO_PTR_NULL);

		nxs_log_write_debug_net(proc, "sock_remove_opt error: %s", nxs_net_strerror(NXS_NET_ERRNO_PTR_NULL));

		return NXS_NET_E_FCNTL;
	}

	if(con->sock == NXS_NET_INIT_SOCK) {

		nxs_net_error_con_set(con, NXS_NET_ERRNO_BAD_SOCK_VAL);

		nxs_log_write_debug_net(proc, "sock_remove_opt error: %s", nxs_net_strerror(NXS_NET_ERRNO_BAD_SOCK_VAL));

		return NXS_NET_E_FCNTL;
	}

	if((sock_arg = fcntl(con->sock, F_GETFL, NULL)) < 0) {

		nxs_log_write_debug_net(proc, "sock_remove_opt error: %s", strerror(errno));

		nxs_net_error_con_set(con, NXS_NET_ERRNO_FCNTL_GET);

		return NXS_NET_E_FCNTL;
	}

	sock_arg &= (~opt);

	if(fcntl(con->sock, F_SETFL, sock_arg) < 0) {

		nxs_log_write_debug_net(proc, "sock_remove_opt error: %s", strerror(errno));

		nxs_net_error_con_set(con, NXS_NET_ERRNO_FCNTL_SET);

		return NXS_NET_E_FCNTL;
	}

	return NXS_NET_E_OK;
}

/*
 * Проверка правильности пути, содержащегося в поле "name" метаданных "md"
 *
 * Условиями успешной проверки являтся:
 * * В метаданных "md" имеется поле "name"
 * * Длина поля "name" больше нуля
 * * В поле "name" не содержится подстроки ".."
 *
 * Возвращаемые значения:
 * * NXS_TRUE	- проверка пройдена успешно
 * * NXS_FALSE	- проверка не пройдена
 */
static nxs_bool_t nxs_net_send_file_check_path(nxs_metadata_t *md)
{
	nxs_string_t *s;

	if((s = nxs_metadata_get_name(md)) == NULL) {

		return NXS_FALSE;
	}

	if(nxs_string_len(s) == 0) {

		return NXS_FALSE;
	}

	if(nxs_string_find_substr_first(s, 0, (u_char *)"..", 2) != NULL) {

		return NXS_FALSE;
	}

	return NXS_TRUE;
}

/*
 * Заполнение заголовка при отправке файла.
 * Заполняемые поля:
 * * Метаданные (metadata)
 * * Размер метаданных (m_size)
 * * Размер передаваемых данных (d_size)
 *
 * В случае каких-либо ошибок - временный буфер соединения не будет заполнен метаданными.
 * Если данная функция вернула код отличный от NXS_NET_E_OK - необходимо отправить заголовок (файловый) с указанным флагом
 * NXS_NET_FLAG_FILE_CANCEL
 * это отменит дальнейшую передачу файла и позволит сохранить соединение (за счёт того, что в него не будет записана лишняя информация,
 * которую
 * будет нельзя обработать на принимающей стороне)
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK		- заполнение заголовка произошло успешно
 * * NXS_NET_E_STAT		- не удалось получить размер отправляемого файла
 * * NXS_NET_E_TYPE		- объект ФС по пути "path" не является файлом
 * * NXS_NET_E_METADATA	- ошибка заполнения метаданных
 */
static int nxs_net_send_file_metadata_prep(nxs_process_t *    proc,
                                           nxs_net_connect_t *con,
                                           nxs_string_t *     path,
                                           nxs_metadata_v_t   fill_meta,
                                           nxs_metadata_t *   metadata,
                                           off_t *            f_size)
{
	size_t           msize;
	off_t            dsize;
	mode_t           type, mode;
	int              rc, ec;
	nxs_metadata_v_t vmd;
	nxs_metadata_t   md, *pmd;

	ec = NXS_NET_E_OK;

	nxs_metadata_init_clear(&md);

	vmd = fill_meta;

	if(metadata == NULL) {

		pmd = &md;

		if(vmd == NXS_METADATA_V_NONE) {

			vmd = NXS_METADATA_V2;
		}
	}
	else {

		pmd = metadata;
	}

	/*
	 * Проверка соответствия типа отправляемого файла (по заданному пути)
	 */

	if((rc = nxs_fs_get_file_type(path, &type)) != 0) {

		nxs_log_write_debug_net(
		        proc, "file send error: can't stat for file: %s (path: \"%s\")", strerror(errno), nxs_string_str(path));

		ec = NXS_NET_E_STAT;
		goto error;
	}

	if(type != S_IFREG) {

		nxs_log_write_debug_net(proc, "file send error: it is not a regular file (type: %o)", type);

		ec = NXS_NET_E_TYPE;
		goto error;
	}

	/*
	 * Подготовка метаданных
	 */

	if(vmd != NXS_METADATA_V_NONE) {

		/*
		 * Если необходимо инициализировать и заполнить метаданные определённой версии
		 */

		nxs_metadata_free(pmd);

		if((rc = nxs_metadata_fill(pmd, vmd, path)) != NXS_METADATA_E_OK) {

			nxs_log_write_debug_net(
			        proc, "file send error: can't fill metadata (error code: %d, path: \"%s\")", rc, nxs_string_str(path));

			ec = NXS_NET_E_METADATA;
			goto error;
		}

		nxs_metadata_get_size(pmd, f_size);

		msize = nxs_net_header_set_metadata(con, pmd);
	}
	else {

		/*
		 * Если инициализировать и заполнять метаданные не требуется (уже заполнено ранее нужными данными)
		 */

		/*
		 * Проверка задания имени файла
		 */
		if(nxs_net_send_file_check_path(pmd) == NXS_FALSE) {

			nxs_log_write_debug_net(proc, "file send error: empty filename in metadata");

			ec = NXS_NET_E_METADATA;
			goto error;
		}

		/*
		 * Получение размера отправляемого файла
		 */
		if((rc = nxs_fs_get_file_size(path, f_size)) != 0) {

			nxs_log_write_debug_net(
			        proc, "file send error: can't stat for file: %s (path: \"%s\")", strerror(errno), nxs_string_str(path));

			ec = NXS_NET_E_STAT;
			goto error;
		}

		/*
		 * Задание (в метеданных) размера отправляемого файла
		 */
		if((rc = nxs_metadata_set_size(pmd, *f_size)) != NXS_METADATA_E_OK) {

			nxs_log_write_debug_net(proc, "file send error: can't set size in metadata (error code: %d)", rc);

			ec = NXS_NET_E_METADATA;
			goto error;
		}

		/*
		 * Проверка в метаданных правильности указания типа передаваемого файла
		 */
		if((nxs_metadata_get_mode(pmd, &mode)) != NXS_METADATA_E_OK) {

			nxs_log_write_debug_net(proc, "file send error: can't get mode from metadata");

			ec = NXS_NET_E_METADATA;
			goto error;
		}

		if(!S_ISREG(mode)) {

			nxs_log_write_debug_net(proc, "file send error: wrong file type in metadata");

			ec = NXS_NET_E_METADATA;
			goto error;
		}

		msize = nxs_net_header_set_metadata(con, pmd);
	}

error:

	nxs_metadata_free(&md);

	if(ec == NXS_NET_E_OK) {

		/*
		 * Расчёт размера всех данных, которые будут переданы удалённой стороне за исключением основного заголовка (размер файла +
		 * вся служебная информация)
		 *
		 * msize - размер метаданных
		 * f_size - размер файла
		 * f_size / _NXS_NET_SENDFILE_BUF_SIZE + 1 - количество управляющих байт для текущего размера файла (1 байт на каждые 64К
		 * данных)
		 * sizeof(off_t) - размер поля "offset"
		 * sizeof(off_t) - размер поля "m_size" (размер блока с метаданными)
		 */
		dsize = msize + sizeof(off_t) + sizeof(off_t) + *f_size + (*f_size / _NXS_NET_SENDFILE_BUF_SIZE + 1);

		nxs_net_header_set_size(con, dsize);
	}
	else {

		*f_size = 0;

		/* nxs_net_send_file_metadata_prep_empty(con); */
	}

	return ec;
}

/*
//На текущий момент функция не востребована, но пока решено её не удалять
static void nxs_net_send_file_metadata_prep_empty(nxs_net_connect_t *con)
{
        nxs_metadata_t		md;
        size_t				dsize, msize;

        nxs_metadata_init(&md, NXS_METADATA_V2);

        msize = nxs_net_header_set_metadata(con, &md);

        dsize = msize + sizeof(size_t) + sizeof(size_t);

        nxs_metadata_free(&md);

        nxs_net_header_set_size(con, dsize);
}
*/

/*
 * Получение метаданных из сети и заполнение структуры "md".
 *
 * Указатель "md" должен указывать на область памяти, выделенную под размещение структуры метаданных "nxs_metadata_t", при этом структура
 * на которую указывает "md" должна быть инициализирована, либо даже заполнена.
 * Структура будет очищена и инициализирована заново (с нужной версией) в том случае, если память непосредственно под метаданные либо не
 * выделена вовсе, либо
 * имеет версию отличную от той, которая будет принята.
 * Таким образом, перед повтормным вызовом данной функции не следует производить очистку метаданных.
 *
 * Возвращаемые значения:
 * * NXS_NET_E_OK				- метаданные получены успешно
 * * NXS_NET_E_POLL				- ошибка poll
 * * NXS_NET_E_TIMEOUT			- тайм-аут при ожидании данных
 * * NXS_NET_E_PEER_SHUTDOWN	- удалённая сторона закрыла соединение
 * * NXS_NET_E_SOCKREAD			- ошибка чтения сокета
 * * NXS_NET_E_PTR				- указатель на структуру "md" равен нулю (NULL)
 * * NXS_NET_E_METADATA			- ошибка в метданных
 */
static int
        nxs_net_metadata_recv(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, size_t offset, nxs_metadata_t *md, size_t *msize)
{
	int   rc;
	off_t _msize;

	if(md == NULL) {

		return NXS_NET_E_PTR;
	}

	if((rc = nxs_net_recv_chunk(proc, con, timeout, &_msize, offset, sizeof(off_t))) != NXS_NET_E_OK) {

		return rc;
	}

	if((rc = nxs_net_recv_chunk(proc, con, timeout, NULL, offset + sizeof(off_t), (size_t)_msize)) != NXS_NET_E_OK) {

		return rc;
	}

	if((rc = nxs_metadata_from_buf(md, offset + sizeof(off_t), &con->tmp_buf, msize)) != NXS_METADATA_E_OK) {

		switch(rc) {

			case NXS_METADATA_E_VER:

				nxs_log_write_debug_net(proc, "metadata receive error: wrong received version");

				break;

			case NXS_METADATA_E_SIZE:

				nxs_log_write_debug_net(proc, "metadata receive error: wrong received file name len");

				break;

			default:

				nxs_log_write_debug_net(proc, "metadata receive error: unknown error");

				break;
		}

		return NXS_NET_E_METADATA;
	}

	return NXS_NET_E_OK;
}

static void nxs_net_error_con_init(nxs_net_connect_t *con)
{

	if(con != NULL) {

		con->_errno = NXS_NET_ERRNO_NO_DETAIL;
	}
}

static void nxs_net_error_con_set(nxs_net_connect_t *con, int _errno)
{

	if(con != NULL) {

		con->_errno = _errno;
	}
}

static void nxs_net_error_srv_init(nxs_net_server_t *srv)
{

	if(srv != NULL) {

		srv->_errno = NXS_NET_ERRNO_NO_DETAIL;
	}
}

static void nxs_net_unix_error_srv_init(nxs_net_unix_server_t *srv)
{

	if(srv != NULL) {

		srv->_errno = NXS_NET_ERRNO_NO_DETAIL;
	}
}

/*
static void nxs_net_error_srv_set(nxs_net_server_t *srv, int _errno)
{

        if(srv != NULL){

                srv->_errno = _errno;
        }
}
*/
