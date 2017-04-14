#ifndef _INCLUDE_NXS_NET_H
#define _INCLUDE_NXS_NET_H

#include <nxs-core/nxs-core.h>

#define NXS_NET_E_OK					0
#define NXS_NET_E_INIT					-1
#define NXS_NET_E_SOCK_CREATE			-2
#define NXS_NET_E_BIND					-3
#define NXS_NET_E_LISTEN				-4
#define NXS_NET_E_ACCEPT				-5
#define NXS_NET_E_CLOSE					-6
#define NXS_NET_E_POLL					-7
#define NXS_NET_E_TIMEOUT				-8
#define NXS_NET_E_PEER_SHUTDOWN			-9
#define NXS_NET_E_SOCKREAD				-10
#define NXS_NET_E_SOCKWRITE				-11
#define NXS_NET_E_GETADDRINFO			-12
#define NXS_NET_E_CONNECT				-13
#define NXS_NET_E_HEADER_INCORRECT		-14
#define NXS_NET_E_BUF_NULL				-15
#define NXS_NET_E_UNKNOWN_DATATYPE		-16
#define NXS_NET_E_BUF_INCORRECT_TYPE	-17
#define NXS_NET_E_FCNTL					-18
#define NXS_NET_E_CRYPT_GOST_INIT		-19 /* ошибка инициализации контекста шифрования */
#define NXS_NET_E_BUF_INCORRECT_SIZE	-20
#define NXS_NET_E_METADATA				-21 /* ошибка на этапе подготовки метаданных или в самих метаданных*/
#define NXS_NET_E_STAT					-22 /* ошибка при обращении к файлу */
#define NXS_NET_E_TYPE					-23 /* ошибка типов (для каких-либо аргументов) */
#define NXS_NET_E_OPEN_FILE				-24 /* ошибка при открытии файла */
#define NXS_NET_E_WRITE_FILE			-25 /* ошибка при записи файла */
#define NXS_NET_E_READ_FILE				-26 /* ошибка при чтении файла */
#define NXS_NET_E_FILE_TRX_CANCELED		-27 /* отмена передачи файла */
#define NXS_NET_E_CHOWN					-28 /* ошибка выполнения команды chown */
#define NXS_NET_E_CHMOD					-29 /* ошибка выполнения команды chmod */
#define NXS_NET_E_UTIME					-30 /* ошибка выполнения команды utime */
#define NXS_NET_E_INOTIFY				-31 /* ошибка при выполнении одной из функций для работы с inotify */
#define NXS_NET_E_CHANGED				-32 /* наблюдаемый объект был обновлён/изменён */
#define NXS_NET_E_OPT					-33 /* ошибка заполнения полей структуры opt; не все обязательные поля заданы или заданы не так */
#define NXS_NET_E_BUF_SIZE_EXCEEDED		-34 /* превышен максимально допустимый размер буфера */
#define NXS_NET_E_PTR					-35 /* не верный указатель (например, нулевой когда это не допустимо) */
#define NXS_NET_E_CONFIRM_NEGATIVE		-36 /* получено негативное подтверждение */
#define NXS_NET_E_MAX_UNIX_PATH_SIZE	-37 /* превышена допустимая длина имени unix-сокета */
#define NXS_NET_E_PWNAM					-38 /* ошибка получения uid */
#define NXS_NET_E_GRNAM					-39 /* ошибка получения gid */

#define NXS_NET_PROTOCOL_E_OK			0 /* ошибок протокола нет */
#define NXS_NET_PROTOCOL_E_WARN			1 /* ошибки не нарушающие работу протокола */
#define NXS_NET_PROTOCOL_E_FATAL		2 /* фатальные для потокола ошибки */

#define NXS_NET_INIT_SOCK				-1

#define NXS_NET_DATA_TYPE_STRING		0
#define NXS_NET_DATA_TYPE_ARRAY			1
#define NXS_NET_DATA_TYPE_LIST			2

#define	NXS_NET_MAX_CONN_DEFAULT		5

#define NXS_NET_TIMEOUT_DEFAULT			30000
#define NXS_NET_TIMEOUT_INFINITY		-1

#define NXS_NET_FLAG_LAST				1
#define NXS_NET_FLAG_BUF				2
#define NXS_NET_FLAG_FILE				4
#define NXS_NET_FLAG_FILE_RESUME		8
#define NXS_NET_FLAG_FILE_CONFIRM		16
#define NXS_NET_FLAG_FILE_CANCEL		32
#define NXS_NET_FLAG_CONFIRM_NO			64
#define NXS_NET_FLAG_CONFIRM_YES		128

#define NXS_NET_TRX_FLAG_CONTINUE		0
#define NXS_NET_TRX_FLAG_CANCEL			1

#define NXS_NET_TRX_RESUME_DENY			0
#define NXS_NET_TRX_RESUME_ALLOW		1

#define NXS_NET_ERRNO_SUCCESS			0
#define NXS_NET_ERRNO_NO_DETAIL			-1
#define	NXS_NET_ERRNO_PTR_NULL			-2
#define	NXS_NET_ERRNO_BAD_SOCK_VAL		-3
#define	NXS_NET_ERRNO_FCNTL_GET			-4
#define	NXS_NET_ERRNO_FCNTL_SET			-5

#define NXS_NET_UNIX_PATH_MAX			108

enum nxs_net_sock_type_e
{
	NXS_NET_SOCK_TYPE_INET,
	NXS_NET_SOCK_TYPE_UNIX
};

struct nxs_net_server_s
{
	nxs_string_t			l_ip;
	nxs_string_t			l_port;
	int						l_sock;
	int						l_con_queue;

	nxs_poll_t				l_poll;

	int						_errno;
};

struct nxs_net_unix_server_s
{
	nxs_string_t			l_sock_path;
	int						l_sock;
	int						l_con_queue;

	nxs_poll_t				l_poll;

	uid_t					uid;
	gid_t					gid;
	mode_t					mode;

	int						_errno;
};

struct nxs_net_connect_s
{
	nxs_net_sock_type_t		type;

	int						sock;

	nxs_string_t			peer_ip;
	nxs_string_t			peer_port;
	nxs_string_t			peer_hostname;

	nxs_string_t			sock_path;

	int						con_retr;

	nxs_string_t			header;
	size_t					h_len;

	nxs_buf_t				tmp_buf;

	nxs_poll_t 				*poll_recv;
	nxs_poll_t 				*poll_send;

	/*
	 * Указатель на контекст шифрования.
	 * При инициализации подключения устанавливается в NULL.
	 * Контекст шифрования определяется дополнительно после инцициализации подключения.
	 */
	nxs_crypt_gost_ctx_t	*gost_ctx;

	/*
	 * Указатель на ключ шифрования (экземляр контекста шифрования) данного соединения.
	 * Если данный элемент имеет значение NULL - шифрование передаваемых данных не производится.
	 */
	nxs_crypt_gost_el_t		*gost_el;

	int						_errno;
};

struct nxs_net_opt_file_recv_s
{
	time_t					timeout;
	nxs_string_t			*r_dir;

	/*
	 * Разрешает или запрещает выставлять на полученный файл атрибуты (права доступа, владелец/группа, время доступа модификации и прочее)
	 * в соответствии с информацией, содержащейся в метадате.
	 */
	nxs_bool_t				allow_attrs;

	/*
	 * Пока решено не рализовывать докачку файла, по этому данный элемент не требуется.
	 * Может понадобиться в будущем.
	 */
	//nxs_bool_t			allow_resume;
};

struct nxs_net_opt_file_send_s
{
	time_t					timeout;
	nxs_metadata_v_t		fill_meta;
	nxs_bool_t				confirm; /* необходимость подтверждения получения файла принимающей стороной */
	nxs_string_t			confirm_txt; /* строка, содержащая текст подтверждения */

	/*
	 * Пока решено не рализовывать докачку файла, по этому данный элемент не требуется.
	 * Может понадобиться в будущем.
	 */
	//nxs_bool_t			resume;
};

/*
 * Функции инициализации и установления соединения
 */
int						nxs_net_server_init						(nxs_net_server_t *srv, u_char *listen_ip, u_char *listen_port, int con_queue_size);
int						nxs_net_connect_init					(nxs_net_connect_t *con, u_char *header);
int						nxs_net_connect_out_init				(nxs_net_connect_t *con, u_char *hostname, u_char *port, int con_retr, u_char *header);
int						nxs_net_server_close					(nxs_process_t *proc, nxs_net_server_t *srv);
int						nxs_net_connect_close					(nxs_process_t *proc, nxs_net_connect_t *con);
int						nxs_net_listen							(nxs_process_t *proc, nxs_net_server_t *srv);
int						nxs_net_accept							(nxs_process_t *proc, nxs_net_server_t *srv, nxs_net_connect_t *con);
int						nxs_net_accept2							(nxs_process_t *proc, nxs_net_server_t *srv, nxs_net_connect_t *con, nxs_array_t *sigunblock, nxs_array_t *sigblock);
int						nxs_net_connect							(nxs_process_t *proc, nxs_net_connect_t *con);
int						nxs_net_connect2						(nxs_process_t *proc, nxs_net_connect_t *con, int timeout);

int						nxs_net_accept_poll_init				(nxs_net_server_t *srv);
int						nxs_net_accept_poll_free				(nxs_net_server_t *srv);
int						nxs_net_accept_poll_check				(nxs_process_t *proc, nxs_net_server_t *srv, int timeout);

int						nxs_net_unix_server_init				(nxs_process_t *proc, nxs_net_unix_server_t *srv, u_char *sock_path, int con_queue_size, u_char *user, u_char *group, mode_t *mode);
int						nxs_net_unix_connect_init				(nxs_net_connect_t *con, u_char *header);
int						nxs_net_unix_connect_out_init			(nxs_process_t *proc, nxs_net_connect_t *con, u_char *sock_path, int con_retr, u_char *header);
int						nxs_net_unix_server_close				(nxs_process_t *proc, nxs_net_unix_server_t *srv);
int						nxs_net_unix_server_unlink				(nxs_process_t *proc, nxs_net_unix_server_t *srv);
int						nxs_net_unix_connect_close				(nxs_process_t *proc, nxs_net_connect_t *con);
int						nxs_net_unix_listen						(nxs_process_t *proc, nxs_net_unix_server_t *srv);
int						nxs_net_unix_accept						(nxs_process_t *proc, nxs_net_unix_server_t *srv, nxs_net_connect_t *con);
int						nxs_net_unix_accept2					(nxs_process_t *proc, nxs_net_unix_server_t *srv, nxs_net_connect_t *con, nxs_array_t *sigunblock, nxs_array_t *sigblock);
int						nxs_net_unix_connect					(nxs_process_t *proc, nxs_net_connect_t *con);

int						nxs_net_unix_accept_poll_init			(nxs_net_unix_server_t *srv);
int						nxs_net_unix_accept_poll_free			(nxs_net_unix_server_t *srv);
int						nxs_net_unix_accept_poll_check			(nxs_process_t *proc, nxs_net_unix_server_t *srv, int timeout);

/*
 * Функции для работы с шифрованием
 */
int						nxs_net_crypt_gost_init					(nxs_process_t *proc, nxs_net_connect_t *con, nxs_string_t *key);
int						nxs_net_crypt_gost_init2				(nxs_process_t *proc, nxs_net_connect_t *con, nxs_string_t *path);
void					nxs_net_crypt_gost_clear				(nxs_process_t *proc, nxs_net_connect_t *con);
void					nxs_net_crypt_gost_reset				(nxs_process_t *proc, nxs_net_connect_t *con);
void					nxs_net_crypt_gost_enable				(nxs_process_t *proc, nxs_net_connect_t *con);
void					nxs_net_crypt_gost_disable				(nxs_process_t *proc, nxs_net_connect_t *con);
nxs_crypt_gost_el_t		*nxs_net_crypt_gost_get_el				(nxs_net_connect_t *con);

/*
 * Функции, используемые для передачи данных
 */
ssize_t					nxs_net_recv_core						(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, u_char *buf, size_t size);
ssize_t					nxs_net_send_core						(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, u_char *buf, size_t size);

int						nxs_net_recv_chunk						(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, void *data, size_t offset, size_t size);

ssize_t					nxs_net_recv							(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, void *data, int type);
ssize_t					nxs_net_send							(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, void *data, int type);

ssize_t					nxs_net_recv_stream						(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_bchain_t *bchain);

int						nxs_net_recv_file						(nxs_process_t *proc, nxs_net_connect_t *con, nxs_net_opt_file_recv_t *opt, nxs_metadata_t *metadata);
int						nxs_net_send_file						(nxs_process_t *proc, nxs_net_connect_t *con, nxs_net_opt_file_send_t *opt, nxs_string_t *path, nxs_metadata_t *metadata);

int						nxs_net_recv_confirm					(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout);
int						nxs_net_send_confirm					(nxs_process_t *proc, nxs_net_connect_t *con, time_t timeout, nxs_bool_t confirm, nxs_buf_t *data);

void					nxs_net_opt_file_recv_init				(nxs_net_opt_file_recv_t *opt);
void					nxs_net_opt_file_recv_free				(nxs_net_opt_file_recv_t *opt);
nxs_bool_t				nxs_net_opt_file_recv_check				(nxs_process_t *proc, nxs_net_opt_file_recv_t *opt);
void					nxs_net_opt_file_send_init				(nxs_net_opt_file_send_t *opt);
void					nxs_net_opt_file_send_free				(nxs_net_opt_file_send_t *opt);
nxs_bool_t				nxs_net_opt_file_send_check				(nxs_process_t *proc, nxs_net_opt_file_send_t *opt);

int						nxs_net_get_conn_fd						(nxs_net_connect_t *con);
int						nxs_net_get_srv_conn_fd					(nxs_net_server_t *srv);
int						nxs_net_get_unix_srv_conn_fd			(nxs_net_unix_server_t *srv);

u_char					*nxs_net_strerror						(int _errno);

int						nxs_net_recv_file_err					(int ec);
int						nxs_net_send_file_err					(int ec);

#endif /* _INCLUDE_NXS_NET_H */
