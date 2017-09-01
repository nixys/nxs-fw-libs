#ifndef _INCLUDE_NXS_CORE_H
#define _INCLUDE_NXS_CORE_H

// clang-format off

/** @addtogroup nxs-core
 *
 * @brief Core header, providing typedefs and includes for all other modules.
 *
 *  @{
 */

#define NXS_VERSION		"1.10.0"

#define	_NXS_ERRNO_UNKNOWN	(u_char *)"unknown error"

#define NXS_FALSE		0
#define NXS_TRUE		1

#define NXS_NO			0
#define NXS_YES			1

/*
 * Common includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/wait.h>

#ifdef USE_NXS_MYSQL
#include <mysql/mysql.h>
#endif

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/prctl.h>
#include <poll.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <utime.h>
#include <pwd.h>
#include <grp.h>
#include <sys/inotify.h>
#include <stddef.h>
#include <wchar.h>

#ifdef USE_NXS_NCURSES
#include <ncurses.h>
#include <locale.h>
#endif

#ifdef USE_NXS_XML
#include <libxml/parser.h>
#include <libxml/tree.h>
#endif

#ifdef USE_NXS_JSON
#include <jansson.h>
#endif

#ifdef USE_NXS_REST_API
#include <err.h>
#include <event2/bufferevent.h>
#include <event2/bufferevent_ssl.h>
#include <event2/event.h>
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <sys/queue.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>
#include <openssl/engine.h>
#include <event2/listener.h>
#endif


typedef				_Bool					nxs_bool_t;


/* nxs-core.h */
typedef struct			nxs_errors_s				nxs_errors_t;


/* nxs-list.h */
typedef enum			nxs_list_init_e				nxs_list_init_t;
typedef enum			nxs_list_move_e				nxs_list_move_t;
typedef struct			nxs_list_el_s				nxs_list_el_t;
typedef struct			nxs_list_s 				nxs_list_t;


/* nxs-ilist.h */
#ifdef USE_NXS_ILIST
typedef struct			nxs_ilist_s				nxs_ilist_t;
#endif

/* nxs-stack.h */
typedef struct			nxs_stack_el_s				nxs_stack_el_t;
typedef struct			nxs_stack_s				nxs_stack_t;


/* nxs-tree.h */
typedef struct			nxs_tree_el_s				nxs_tree_el_t;
typedef struct			nxs_tree_s				nxs_tree_t;


/* nxs-array.h */
typedef struct			nxs_array_s				nxs_array_t;


/* nxs-string.h */
typedef enum			nxs_string_err_e			nxs_string_err_t;
typedef enum			nxs_string_escape_types_e		nxs_string_escape_types_t;
typedef struct			nxs_string_s				nxs_string_t;


/* nxs-wstring.h */
typedef struct			nxs_wstring_s				nxs_wstring_t;


/* nxs-buf.h */
typedef enum			nxs_buf_err_e				nxs_buf_err_t;
typedef struct			nxs_buf_s				nxs_buf_t;


/* nxs-bchain.h */
typedef enum			nxs_bchain_err_e			nxs_bchain_err_t;
typedef enum			nxs_bchain_seek_e			nxs_bchain_seek_t;
typedef struct			nxs_bchain_el_s				nxs_bchain_el_t;
typedef struct			nxs_bchain_s				nxs_bchain_t;


/* nxs-pipesem.h */
typedef struct			nxs_pipesem_s				nxs_pipesem_t;


/* nxs-log.h */
typedef struct			nxs_log_s				nxs_log_t;


/* nxs-args.h */
typedef struct			nxs_args_shortopt_s			nxs_args_shortopt_t;
typedef struct			nxs_args_s 				nxs_args_t;


/* nxs-proc.h */
typedef struct			nxs_process_s 				nxs_process_t;


/* nxs-cfg.h */
typedef	struct			nxs_cfg_par_s				nxs_cfg_par_t;
typedef	struct			nxs_cfg_s				nxs_cfg_t;


/* nxs-mysql.h */
#ifdef USE_NXS_MYSQL
typedef enum			nxs_mysql_err_e				nxs_mysql_err_t;
typedef enum			nxs_mysql_errno_e			nxs_mysql_errno_t;
typedef enum			nxs_mysql_query_type_e			nxs_mysql_query_type_t;

typedef	struct			nxs_mysql_s				nxs_mysql_t;
typedef	struct			nxs_mysql_res_s				nxs_mysql_res_t;
#endif


/* nxs-crypt-gost */
typedef struct			nxs_crypt_gost_ctx_s			nxs_crypt_gost_ctx_t;
typedef struct			nxs_crypt_gost_el_s			nxs_crypt_gost_el_t;


/* nxs-metadata.h */
typedef				unsigned char				nxs_metadata_v_t;
typedef	struct			nxs_metadata_s				nxs_metadata_t;


/* nxs-net.h */
typedef				u_int32_t				nxs_net_hdr_f_t;
typedef enum			nxs_net_sock_type_e			nxs_net_sock_type_t;
typedef	struct			nxs_net_server_s			nxs_net_server_t;
typedef	struct			nxs_net_unix_server_s			nxs_net_unix_server_t;
typedef struct			nxs_net_connect_s			nxs_net_connect_t;
typedef	struct			nxs_net_opt_file_send_s			nxs_net_opt_file_send_t;
typedef	struct			nxs_net_opt_file_recv_s			nxs_net_opt_file_recv_t;


/* nxs-poll.h */
typedef struct			nxs_poll_s				nxs_poll_t;


/* nxs-fs.h */
typedef struct 			nxs_fs_dirent_s				nxs_fs_dirent_t;
typedef struct 			nxs_fs_file_check_s			nxs_fs_file_check_t;


/* nxs-ncurses.h */
#ifdef USE_NXS_NCURSES
typedef enum			nxs_ncurses_type_e			nxs_ncurses_type_t;
typedef enum			nxs_ncurses_subtype_text_e		nxs_ncurses_subtype_text_t;
typedef struct			nxs_ncurses_win_s			nxs_ncurses_win_t;
typedef struct			nxs_ncurses_win_conf_s			nxs_ncurses_win_conf_t;
typedef struct			nxs_ncurses_conf_text_s			nxs_ncurses_conf_text_t;
typedef struct			nxs_ncurses_ctx_s			nxs_ncurses_ctx_t;
typedef struct			nxs_ncurses_t_container_s		nxs_ncurses_t_container_t;
typedef struct			nxs_ncurses_t_label_s			nxs_ncurses_t_label_t;
typedef struct			nxs_ncurses_t_text_s			nxs_ncurses_t_text_t;
#endif


/* nxs-xml.h */
#ifdef USE_NXS_XML
typedef struct			nxs_xml_s				nxs_xml_t;
typedef struct			nxs_xml_el_s				nxs_xml_el_t;
typedef struct			nxs_xml_attr_s				nxs_xml_attr_t;
#endif


/* nxs-json.h */
#ifdef USE_NXS_JSON
typedef enum			nxs_json_type_e				nxs_json_type_t;
typedef struct			nxs_json_s				nxs_json_t;

typedef enum			nxs_cfg_json_state_e			nxs_cfg_json_state_t;
typedef enum			nxs_cfg_json_type_e			nxs_cfg_json_type_t;
typedef struct			nxs_cfg_json_par_s			nxs_cfg_json_par_t;
typedef struct			nxs_cfg_json_s				nxs_cfg_json_t;
#endif


/* nxs-rest-api-common.h */
typedef enum			nxs_http_code_e				nxs_http_code_t;
typedef enum			nxs_rest_api_common_cmd_type_e		nxs_rest_api_common_cmd_type_t;


/* nxs-rest-api.h */
#ifdef USE_NXS_REST_API
typedef enum			nxs_rest_api_err_e			nxs_rest_api_err_t;
typedef enum			nxs_rest_api_format_err_e		nxs_rest_api_format_err_t;
typedef enum			nxs_rest_api_log_stage_e		nxs_rest_api_log_stage_t;

typedef struct			nxs_rest_api_ctx_s			nxs_rest_api_ctx_t;
typedef struct			nxs_rest_api_request_s			nxs_rest_api_request_t;
#endif


/* nxs-curl.h */
#ifdef USE_NXS_CURL
typedef struct			nxs_curl_s				nxs_curl_t;
typedef enum			nxs_curl_log_e				nxs_curl_log_t;
#endif


/*
 * Core defines
 */

struct nxs_errors_s
{
	int			_errno;
	u_char			*text;
};

#define nxs_error(rc, code, label)	rc = code; \
						goto label;

/*
 * Nxs includes
 */
#include <nxs-mem/nxs-mem.h>
#include <nxs-list/nxs-list.h>

#ifdef USE_NXS_ILIST
#include <nxs-ilist/nxs-ilist.h>
#endif

#include <nxs-stack/nxs-stack.h>
#include <nxs-tree/nxs-tree.h>
#include <nxs-array/nxs-array.h>

#ifdef USE_NXS_MPOOL
#include <nxs-mpool/nxs-mpool.h>
#endif

#include <nxs-string/nxs-string.h>
#include <nxs-wstring/nxs-wstring.h>
#include <nxs-buf/nxs-buf.h>
#include <nxs-bchain/nxs-bchain.h>
#include <nxs-pipesem/nxs-pipesem.h>
#include <nxs-log/nxs-log.h>
#include <nxs-args/nxs-args.h>
#include <nxs-proc/nxs-proc.h>
#include <nxs-cfg/nxs-cfg.h>

#ifdef USE_NXS_MYSQL
#include <nxs-mysql/nxs-mysql.h>
#endif

#include <nxs-crypt-gost/nxs-crypt-gost.h>
#include <nxs-metadata/nxs-metadata.h>
#include <nxs-poll/nxs-poll.h>
#include <nxs-net/nxs-net.h>
#include <nxs-fs/nxs-fs.h>

#ifdef USE_NXS_NCURSES
#include <nxs-ncurses/nxs-ncurses.h>
#endif

#ifdef USE_NXS_XML
#include <nxs-xml/nxs-xml.h>
#endif

#ifdef USE_NXS_JSON
#include <nxs-json/nxs-json.h>
#include <nxs-cfg-json/nxs-cfg-json.h>
#endif

#include <nxs-rest-api-common/nxs-rest-api-common.h>

#ifdef USE_NXS_REST_API
#include <nxs-rest-api/nxs-rest-api.h>
#endif

#ifdef USE_NXS_CURL
#include <nxs-curl/nxs-curl.h>
#endif

#include <nxs-base64/nxs-base64.h>

/** @} */ // end of nxs-core
#endif /* _INCLUDE_NXS_CORE_H */
