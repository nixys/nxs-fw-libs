#ifndef _INCLUDE_NXS_MYSQL_H
#define _INCLUDE_NXS_MYSQL_H

#include <nxs-core/nxs-core.h>

enum nxs_mysql_err_e
{
	NXS_MYSQL_E_OK,
	NXS_MYSQL_E_ERR
};

enum nxs_mysql_errno_e
{
	NXS_MYSQL_ERRNO_UNDEF,
	NXS_MYSQL_ERRNO_OK,
	NXS_MYSQL_ERRNO_EPORT,
	NXS_MYSQL_ERRNO_EHOST,
	NXS_MYSQL_ERRNO_ENAME,
	NXS_MYSQL_ERRNO_EUSER,
	NXS_MYSQL_ERRNO_EPASS,
	NXS_MYSQL_ERRNO_ECHARSET,
	NXS_MYSQL_ERRNO_EALLOC,
	NXS_MYSQL_ERRNO_ECONNECT,
	NXS_MYSQL_ERRNO_ECHARSET_CONN,
	NXS_MYSQL_ERRNO_EQUERY,
	NXS_MYSQL_ERRNO_ESTORE
};

enum nxs_mysql_query_type_e
{
	NXS_MYSQL_QUERY_TYPE_SELECT,
	NXS_MYSQL_QUERY_TYPE_INSERT,
	NXS_MYSQL_QUERY_TYPE_UPDATE
};


#define NXS_MYSQL_CONN_CHARSET_DEFAULT		"utf8"

struct nxs_mysql_s
{
	/*
	 * Строка, содержащая адрес MySQL-сервера для подключения
	 */
	nxs_string_t		db_host;

	/*
	 * Строка, содержащая имя базы данных MySQL-сервера для подключения
	 */
	nxs_string_t		db_name;

	/*
	 * Строка, содержащая имя пользователя MySQL-сервера для подключения
	 */
	nxs_string_t		db_user;

	/*
	 * Строка, содержащая пароль пользователя MySQL-сервера для подключения
	 */
	nxs_string_t		db_pass;

	/*
	 * Строка, содержащая порт MySQL-сервера для подключения
	 */
	uint16_t			db_port;

	/*
	 * Кодировка подключения
	 */
	nxs_string_t		db_charset;

	/*
	 * Указатель на объект подключения к MySQL
	 */
	MYSQL 				*conn;

	/*
	 * Указатель на текст с ошибкой (если ошибки не было - NULL)
	 */
	u_char				*err_str;

	/*
	 * Номер ошибки (если ошибки не было - NXS_MYSQL_E_OK)
	 */
	nxs_mysql_errno_t	m_errno;
};

struct nxs_mysql_res_s
{
	size_t				fields;
	size_t				rows;

	size_t				last_ins;

	size_t				affected_rows;

	nxs_array_t			*res_tbl;
};

/*
 * Инициализация подключение MySQL
 *
 * Возвращаемые значения:
 * * NXS_MYSQL_E_ERR	- произошла ошибка инициализации
 * * NXS_MYSQL_E_OK		- при инициализации ошибок не возникло
 *
 * Если работа функции завершилась с кодом "NXS_MYSQL_E_OK", то mysql->err_str обнуляется, а mysql->m_errno принимает значение "NXS_MYSQL_ERRNO_OK".
 * Иначе, mysql->m_errno устанавливается в одно из начений доп. кодов, а mysql->err_str содержит указатель на строку с текстом ошибки.
 * Если указатель mysql равен NULL - вернётся "NXS_MYSQL_E_ERR" без использования доп. кодов.
 *
 * Дополнительные коды:
 * * NXS_MYSQL_ERR_EHOST	- параметр "db_host" не задан
 * * NXS_MYSQL_ERR_ENAME	- параметр "db_name" не задан
 * * NXS_MYSQL_ERR_EUSER	- параметр "db_user" не задан
 * * NXS_MYSQL_ERR_EPASS	- параметр "db_pass" не задан
 * * NXS_MYSQL_ERR_EPORT	- параметр "db_port" выходит за границы (0, 65535]
 * * NXS_MYSQL_ERR_ECHARSET	- параметр "db_charset" не задан
 */
nxs_mysql_err_t						nxs_mysql_init				(nxs_mysql_t *mysql, nxs_string_t *db_host, nxs_string_t *db_name, nxs_string_t *db_user, nxs_string_t *db_pass, uint16_t db_port, nxs_string_t *db_charset);

void								nxs_mysql_free				(nxs_mysql_t *mysql);

/*
 * Подключение к MySQL с параметрами, заданными при инициализации подключения.
 *
 * Возвращаемые значения:
 * * NXS_MYSQL_E_ERR	- произошла ошибка подключения
 * * NXS_MYSQL_E_OK		- при подключении ошибок не возникло
 *
 * Если работа функции завершилась с кодом "NXS_MYSQL_E_OK", то mysql->err_str обнуляется, а mysql->m_errno принимает значение "NXS_MYSQL_ERRNO_OK".
 * Иначе, mysql->m_errno устанавливается в одно из начений доп. кодов, а mysql->err_str содержит указатель на строку с текстом ошибки.
 * Если указатель mysql равен NULL - вернётся "NXS_MYSQL_E_ERR" без использования доп. кодов.
 *
 * Дополнительные коды:
 * * NXS_MYSQL_ERR_ECONNECT			- ошибка подключения
 * * NXS_MYSQL_ERR_ECHARSET_CONN	- ошибка установления требуемой кодировки подключения (не является критической ошибкой, будет использована
 * 										кодировка по умолчанию).
 */
nxs_mysql_err_t						nxs_mysql_connect			(nxs_mysql_t *mysql);

/*
 * Завершение соединения с MySQL.
 *
 * Возвращаемые значения:
 * * NXS_MYSQL_E_ERR	- произошла ошибка отключения
 * * NXS_MYSQL_E_OK		- при отключении от MySQL ошибок не возникло
 *
 * Если работа функции завершилась с кодом "NXS_MYSQL_E_OK", то mysql->err_str обнуляется, а mysql->m_errno принимает значение "NXS_MYSQL_ERRNO_OK".
 * Иначе, mysql->m_errno устанавливается в одно из начений доп. кодов, а mysql->err_str содержит указатель на строку с текстом ошибки.
 * Если указатель mysql равен NULL - вернётся "NXS_MYSQL_E_ERR" без использования доп. кодов.
 *
 * Дополнительные коды:
 * Отсутствуют.
 */
nxs_mysql_err_t						nxs_mysql_disconnect		(nxs_mysql_t *mysql);

/*
 * Проверка соединения с MySQL.
 * Если подключения ещё не было остановлено - будет произведена попытка подключения.
 * Если подключение уже было установлено - будет произведена его проверка и при необходимости произойдёт переустановка подключения.
 *
 * Возвращаемые значения:
 * * NXS_MYSQL_E_ERR	- произошла ошибка подключения
 * * NXS_MYSQL_E_OK		- при проверки соединения и/или при переподключении ошибок не возникло
 *
 * Если работа функции завершилась с кодом "NXS_MYSQL_E_OK", то mysql->err_str обнуляется, а mysql->m_errno принимает значение "NXS_MYSQL_ERRNO_OK".
 * Иначе, mysql->m_errno устанавливается в одно из начений доп. кодов, а mysql->err_str содержит указатель на строку с текстом ошибки.
 * Если указатель mysql равен NULL - вернётся "NXS_MYSQL_E_ERR" без использования доп. кодов.
 *
 * Дополнительные коды:
 * * Дополнительные коды функции nxs_mysql_connect()
 */
nxs_mysql_err_t						nxs_mysql_check_conn		(nxs_mysql_t *mysql);

/*
 * Подготовка SQL-валидную строку (экранирование соответствующих символов).
 * Результат будет сохранён в строке str_to
 *
 * Если указатель на строку str_from != NULL, то это значит, что исходная строка, которую необходимо подготовить, это str_from.
 * Это необходимо на тот случай, когда необходимо исходную строку оставить в неизменном виде (например, для дальнейшего использования).
 */
void								nxs_mysql_escape_string		(nxs_mysql_t *mysql, nxs_string_t *str_to, nxs_string_t *str_from);

/*
 * Выполнение запроса к MySQL.
 *
 * Возвращаемые значения:
 * * NXS_MYSQL_E_OK		- при выполнении запросов типа "NXS_MYSQL_QUERY_TYPE_UPDATE" и "NXS_MYSQL_QUERY_TYPE_SELECT"  ошибок не возникло
 * * NXS_MYSQL_E_ERR	- ошибка при выолнении запроса
 *
 * Если работа функции завершилась с кодом "NXS_MYSQL_E_OK", то mysql->err_str обнуляется, а mysql->m_errno принимает значение "NXS_MYSQL_ERRNO_OK".
 * Иначе, mysql->m_errno устанавливается в одно из начений доп. кодов, а mysql->err_str содержит указатель на строку с текстом ошибки.
 * Если указатель mysql равен NULL - вернётся "NXS_MYSQL_E_ERR" без использования доп. кодов.
 *
 * Дополнительные коды:
 * * NXS_MYSQL_ERR_EQUERY	- ошибка при выолнении запроса
 * * NXS_MYSQL_ERR_ESTORE	- ошибка получения данных в запросе типа "NXS_MYSQL_QUERY_TYPE_SELECT"
 */
nxs_mysql_err_t						nxs_mysql_query				(nxs_mysql_t *mysql, nxs_mysql_query_type_t type, nxs_mysql_res_t *res, const char *query, ...);

/*
 * Создание структуры для хранения результата запроса MySQL типа "NXS_MYSQL_QUERY_TYPE_SELECT".
 * Выделенная память будет инициализирована (обнулена).
 *
 * Возвращаемые значения:
 * * Указатель на созданный элемент
 */
nxs_mysql_res_t						*nxs_mysql_res_malloc		();

/*
 * Очистить структуру для хранения результата запроса MySQL типа "NXS_MYSQL_QUERY_TYPE_SELECT" и освободить память, выделенную ранее под указатель.
 *
 * Возвращаемые значения:
 * * NULL
 */
nxs_mysql_res_t						*nxs_mysql_res_destroy		(nxs_mysql_res_t *res);

/*
 * Инициализация структуры для хранения результата запроса MySQL типа "NXS_MYSQL_QUERY_TYPE_SELECT"
 *
 * Возвращаемые значения:
 * * Отсутствуют
 */
void								nxs_mysql_res_init			(nxs_mysql_res_t *res);

/*
 * Освобождение памяти под элементы структуры для хранения результата запроса MySQL типа "NXS_MYSQL_QUERY_TYPE_SELECT".
 *
 * Возвращаемые значения:
 * * Отсутствуют
 */
void								nxs_mysql_res_free			(nxs_mysql_res_t *res);
void								nxs_mysql_res_clear			(nxs_mysql_res_t *res);

/*
 * Получение элемента таблицы (строка типа "nxs_string_t") с заданными коррдинатами.
 *
 * Возвращаемые значения:
 * * Указатель на строку
 * * NULL					- если элемент с заданными индексами не найден или сам элемент равен NULL
 */
nxs_string_t						*nxs_mysql_res_get			(nxs_mysql_res_t *res, size_t row, size_t col);
size_t								nxs_mysql_res_rows			(nxs_mysql_res_t *res);
size_t								nxs_mysql_res_fields		(nxs_mysql_res_t *res);
size_t								nxs_mysql_res_last_ins		(nxs_mysql_res_t *res);
size_t								nxs_mysql_res_affected		(nxs_mysql_res_t *res);

#endif /* _INCLUDE_NXS_MYSQL_H */
