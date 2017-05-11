// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define NXS_MYSQL_ERR_EPORT_STR			(u_char *)"db port incorrect"
#define NXS_MYSQL_ERR_EHOST_STR			(u_char *)"db host incorrect"
#define NXS_MYSQL_ERR_ENAME_STR			(u_char *)"db name incorrect"
#define NXS_MYSQL_ERR_EUSER_STR			(u_char *)"db user incorrect"
#define NXS_MYSQL_ERR_EPASS_STR			(u_char *)"db pass incorrect"
#define NXS_MYSQL_ERR_ECHARSET_STR		(u_char *)"db charset incorrect"
#define NXS_MYSQL_ERR_EALLOC_STR		(u_char *)"can't allocate memory"
#define NXS_MYSQL_ERR_ECHARSET_CONN_STR		(u_char *)"can't set charset"
#define NXS_MYSQL_ERR_EQUERY_NULL_CONN_STR	(u_char *)"no connection to mysql-server"

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_mysql_err_t nxs_mysql_init(nxs_mysql_t * mysql,
                               nxs_string_t *db_host,
                               nxs_string_t *db_name,
                               nxs_string_t *db_user,
                               nxs_string_t *db_pass,
                               uint16_t      db_port,
                               nxs_string_t *db_charset)
{

	if(mysql == NULL) {

		return NXS_MYSQL_E_ERR;
	}

	mysql->err_str = NULL;
	mysql->m_errno = NXS_MYSQL_ERRNO_UNDEF;

	if(db_host == NULL) {

		mysql->m_errno = NXS_MYSQL_ERRNO_EHOST;
		mysql->err_str = NXS_MYSQL_ERR_EHOST_STR;

		return NXS_MYSQL_E_ERR;
	}

	if(db_name == NULL) {

		mysql->m_errno = NXS_MYSQL_ERRNO_ENAME;
		mysql->err_str = NXS_MYSQL_ERR_ENAME_STR;

		return NXS_MYSQL_E_ERR;
	}

	if(db_user == NULL) {

		mysql->m_errno = NXS_MYSQL_ERRNO_EUSER;
		mysql->err_str = NXS_MYSQL_ERR_EUSER_STR;

		return NXS_MYSQL_E_ERR;
	}

	if(db_pass == NULL) {

		mysql->m_errno = NXS_MYSQL_ERRNO_EPASS;
		mysql->err_str = NXS_MYSQL_ERR_EPASS_STR;

		return NXS_MYSQL_E_ERR;
	}

	if(db_charset == NULL) {

		mysql->m_errno = NXS_MYSQL_ERRNO_ECHARSET;
		mysql->err_str = NXS_MYSQL_ERR_ECHARSET_STR;

		return NXS_MYSQL_E_ERR;
	}

	nxs_string_init2(&mysql->db_host, 0, nxs_string_str(db_host));
	nxs_string_init2(&mysql->db_name, 0, nxs_string_str(db_name));
	nxs_string_init2(&mysql->db_user, 0, nxs_string_str(db_user));
	nxs_string_init2(&mysql->db_pass, 0, nxs_string_str(db_pass));
	nxs_string_init2(&mysql->db_charset, 0, nxs_string_str(db_charset));

	mysql->db_port = db_port;

	mysql->conn = NULL;

	return NXS_MYSQL_E_OK;
}

void nxs_mysql_free(nxs_mysql_t *mysql)
{

	if(mysql == NULL) {

		return;
	}

	if(mysql->conn != NULL) {

		nxs_mysql_disconnect(mysql);
	}

	nxs_string_free(&mysql->db_host);
	nxs_string_free(&mysql->db_name);
	nxs_string_free(&mysql->db_user);
	nxs_string_free(&mysql->db_pass);
	nxs_string_free(&mysql->db_charset);

	mysql->err_str = NULL;
	mysql->m_errno = NXS_MYSQL_ERRNO_UNDEF;
}

nxs_mysql_err_t nxs_mysql_connect(nxs_mysql_t *mysql)
{

	if(mysql == NULL) {

		return NXS_MYSQL_E_ERR;
	}

	mysql->err_str = NULL;
	mysql->m_errno = NXS_MYSQL_ERRNO_OK;

	if((mysql->conn = mysql_init(NULL)) == NULL) {

		mysql->m_errno = NXS_MYSQL_ERRNO_EALLOC;
		mysql->err_str = NXS_MYSQL_ERR_EALLOC_STR;

		return NXS_MYSQL_E_ERR;
	}

	if(!mysql_real_connect(mysql->conn,
	                       (char *)nxs_string_str(&mysql->db_host),
	                       (char *)nxs_string_str(&mysql->db_user),
	                       (char *)nxs_string_str(&mysql->db_pass),
	                       (char *)nxs_string_str(&mysql->db_name),
	                       (unsigned int)mysql->db_port,
	                       NULL,
	                       0)) {

		mysql->m_errno = NXS_MYSQL_ERRNO_ECONNECT;
		mysql->err_str = (u_char *)mysql_error(mysql->conn);

		mysql_close(mysql->conn);

		mysql->conn = NULL;

		return NXS_MYSQL_E_ERR;
	}

	if(mysql_set_character_set(mysql->conn, (char *)nxs_string_str(&mysql->db_charset))) {

		mysql->err_str = NXS_MYSQL_ERR_ECHARSET_CONN_STR;
		mysql->m_errno = NXS_MYSQL_ERRNO_ECHARSET_CONN;
	}

	return NXS_MYSQL_E_OK;
}

nxs_mysql_err_t nxs_mysql_disconnect(nxs_mysql_t *mysql)
{

	if(mysql == NULL) {

		return NXS_MYSQL_E_ERR;
	}

	if(mysql->conn != NULL) {

		mysql_close(mysql->conn);
	}

	mysql->err_str = NULL;
	mysql->m_errno = NXS_MYSQL_ERRNO_OK;

	mysql->conn = NULL;

	return NXS_MYSQL_E_OK;
}

nxs_mysql_err_t nxs_mysql_check_conn(nxs_mysql_t *mysql)
{

	if(mysql == NULL) {

		return NXS_MYSQL_E_ERR;
	}

	if(mysql->conn == NULL) {

		return nxs_mysql_connect(mysql);
	}

	if(mysql_ping(mysql->conn) != 0) {

		mysql_close(mysql->conn);

		return nxs_mysql_connect(mysql);
	}

	mysql->err_str = NULL;
	mysql->m_errno = NXS_MYSQL_ERRNO_OK;

	return NXS_MYSQL_E_OK;
}

void nxs_mysql_escape_string(nxs_mysql_t *mysql, nxs_string_t *str_to, nxs_string_t *str_from)
{
	nxs_string_t tmp;
	size_t       len;

	if(mysql == NULL || str_to == NULL) {

		return;
	}

	nxs_string_init(&tmp);

	if(str_from != NULL) {

		nxs_string_cpy_dyn(&tmp, 0, str_from, 0);
	}
	else {

		nxs_string_cpy_dyn(&tmp, 0, str_to, 0);
	}

	len = nxs_string_len(&tmp);

	nxs_string_resize(str_to, len * 2 + 1);

	len = mysql_real_escape_string(mysql->conn, (char *)nxs_string_str(str_to), (char *)nxs_string_str(&tmp), len);

	nxs_string_set_len(str_to, len);

	nxs_string_free(&tmp);
}

nxs_mysql_err_t nxs_mysql_query(nxs_mysql_t *mysql, nxs_mysql_query_type_t type, nxs_mysql_res_t *res, const char *query, ...)
{
	size_t         n, i;
	nxs_string_t   q;
	unsigned long *lens;
	va_list        arg;
	MYSQL_RES *    r_mysql;
	MYSQL_ROW      row_mysql;
	nxs_array_t *  r;
	nxs_string_t * field;

	mysql->err_str = NULL;
	mysql->m_errno = NXS_MYSQL_ERRNO_OK;

	nxs_mysql_res_clear(res);

	if(mysql == NULL) {

		return NXS_MYSQL_E_ERR;
	}

	if(mysql->conn == NULL) {

		mysql->err_str = NXS_MYSQL_ERR_EQUERY_NULL_CONN_STR;
		mysql->m_errno = NXS_MYSQL_ERRNO_EQUERY;

		return NXS_MYSQL_E_ERR;
	}

	/*
	 * Подготовка запроса
	 */

	nxs_string_init(&q);

	va_start(arg, query);
	n = nxs_string_vprintf_dyn(&q, query, arg);
	va_end(arg);

	/*
	 * Выполнение запроса
	 */

	if(mysql_real_query(mysql->conn, (char *)nxs_string_str(&q), n) != 0) {

		mysql->m_errno = NXS_MYSQL_ERRNO_EQUERY;
		mysql->err_str = (u_char *)mysql_error(mysql->conn);

		nxs_string_free(&q);

		return NXS_MYSQL_E_ERR;
	}

	nxs_string_free(&q);

	switch(type) {

		case NXS_MYSQL_QUERY_TYPE_UPDATE:
		default:

			if(res == NULL) {

				return NXS_MYSQL_E_OK;
			}

			res->affected_rows = (size_t)mysql_affected_rows(mysql->conn);

			return NXS_MYSQL_E_OK;

		case NXS_MYSQL_QUERY_TYPE_INSERT:

			if(res == NULL) {

				return NXS_MYSQL_E_OK;
			}

			res->last_ins      = (size_t)mysql_insert_id(mysql->conn);
			res->affected_rows = (size_t)mysql_affected_rows(mysql->conn);

			return NXS_MYSQL_E_OK;

		case NXS_MYSQL_QUERY_TYPE_SELECT:

			if(res == NULL) {

				return NXS_MYSQL_E_OK;
			}

			if((r_mysql = mysql_store_result(mysql->conn)) == NULL) {

				mysql->m_errno = NXS_MYSQL_ERRNO_ESTORE;
				mysql->err_str = (u_char *)mysql_error(mysql->conn);

				return NXS_MYSQL_E_ERR;
			}

			res->fields  = mysql_num_fields(r_mysql);
			res->res_tbl = nxs_array_malloc(0, sizeof(nxs_array_t), 1);

			while((row_mysql = mysql_fetch_row(r_mysql)) != NULL) {

				res->rows++;

				r = nxs_array_add(res->res_tbl);
				nxs_array_init(r, res->fields, sizeof(nxs_string_t), 1);

				lens = mysql_fetch_lengths(r_mysql);

				for(i = 0; i < res->fields; i++) {

					field = nxs_array_add(r);

					nxs_string_init(field);

					if(lens[i] == 0) {

						if(row_mysql[i] == NULL) {

							nxs_string_create(field, 0, (u_char *)"NULL");
						}
						else {

							nxs_string_create(field, 0, NXS_STRING_EMPTY_STR);
						}
					}
					else {

						nxs_string_create(field, lens[i] + 1, (u_char *)row_mysql[i]);
					}
				}
			}

			mysql_free_result(r_mysql);

			return NXS_MYSQL_E_OK;
	}
}

nxs_mysql_res_t *nxs_mysql_res_malloc(void)
{
	nxs_mysql_res_t *res = NULL;

	res = (nxs_mysql_res_t *)nxs_malloc(res, sizeof(nxs_mysql_res_t));

	nxs_mysql_res_init(res);

	return res;
}

nxs_mysql_res_t *nxs_mysql_res_destroy(nxs_mysql_res_t *res)
{

	if(res == NULL) {

		return NULL;
	}

	nxs_mysql_res_free(res);

	return nxs_free(res);
}

void nxs_mysql_res_init(nxs_mysql_res_t *res)
{

	if(res == NULL) {

		return;
	}

	res->fields        = 0;
	res->rows          = 0;
	res->last_ins      = 0;
	res->affected_rows = 0;
	res->res_tbl       = NULL;
}

void nxs_mysql_res_free(nxs_mysql_res_t *res)
{
	size_t        i, j, c;
	nxs_array_t * r;
	nxs_string_t *str;

	if(res == NULL) {

		return;
	}

	for(i = 0; i < res->rows; i++) {

		r = nxs_array_get(res->res_tbl, i);
		c = nxs_array_count(r);

		for(j = 0; j < c; j++) {

			str = nxs_array_get(r, j);

			nxs_string_free(str);
		}

		nxs_array_free(r);
	}

	res->res_tbl = nxs_array_destroy(res->res_tbl);

	res->fields        = 0;
	res->rows          = 0;
	res->last_ins      = 0;
	res->affected_rows = 0;
}

void nxs_mysql_res_clear(nxs_mysql_res_t *res)
{
	size_t        i, j, c;
	nxs_array_t * r;
	nxs_string_t *str;

	if(res == NULL) {

		return;
	}

	if(res->res_tbl != NULL) {

		for(i = 0; i < res->rows; i++) {

			r = nxs_array_get(res->res_tbl, i);
			c = nxs_array_count(r);

			for(j = 0; j < c; j++) {

				str = nxs_array_get(r, j);

				nxs_string_free(str);
			}

			nxs_array_free(r);
		}

		res->res_tbl = nxs_array_destroy(res->res_tbl);
	}

	res->fields        = 0;
	res->rows          = 0;
	res->last_ins      = 0;
	res->affected_rows = 0;
}

nxs_string_t *nxs_mysql_res_get(nxs_mysql_res_t *res, size_t row, size_t col)
{
	nxs_array_t *r;

	r = nxs_array_get(res->res_tbl, row);

	if(r != NULL) {

		return nxs_array_get(r, col);
	}

	return NULL;
}

size_t nxs_mysql_res_rows(nxs_mysql_res_t *res)
{
	if(res == NULL) {

		return 0;
	}

	return res->rows;
}

size_t nxs_mysql_res_fields(nxs_mysql_res_t *res)
{
	if(res == NULL) {

		return 0;
	}

	return res->fields;
}

size_t nxs_mysql_res_last_ins(nxs_mysql_res_t *res)
{
	if(res == NULL) {

		return 0;
	}

	return res->last_ins;
}

size_t nxs_mysql_res_affected(nxs_mysql_res_t *res)
{
	if(res == NULL) {

		return 0;
	}

	return res->affected_rows;
}

/* Module internal (static) functions */
