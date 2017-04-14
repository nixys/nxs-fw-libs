#include <nxs-core/nxs-core.h>

#define	_NXS_METADATA_E_UNKNWN				"unknown error"
#define	_NXS_METADATA_E_OK					"success"
#define	_NXS_METADATA_E_VER					"wrong version number"
#define	_NXS_METADATA_E_PTR					"wrong pointer to metadata"
#define	_NXS_METADATA_E_STAT				"stat error"
#define	_NXS_METADATA_E_SIZE				"wrong size"
#define	_NXS_METADATA_E_BUF_SIZE			"wrong buffer size"
#define	_NXS_METADATA_E_NOENT				"no such entry"
#define	_NXS_METADATA_E_LOOKUP				"can't lookup entry"

/*
 * Размеры полей при сериализации.
 * Введено, в основном, для совместимости с 32-битной архитектурой
 */
#define _NXS_METADATA_F_VER_LEN				1
#define _NXS_METADATA_F_NAME_LEN			8
#define _NXS_METADATA_F_SIZE_LEN			8
#define _NXS_METADATA_F_MODE_LEN			4
#define _NXS_METADATA_F_UID_LEN				4
#define _NXS_METADATA_F_GID_LEN				4
#define _NXS_METADATA_F_SUNAME_LEN			1
#define _NXS_METADATA_F_SGNAME_LEN			1
#define _NXS_METADATA_F_TIME_LEN			8

typedef struct
{
	int						error;
	u_char					*error_text;
} nxs_metadata_err_t;

typedef struct
{
	nxs_string_t			name;
	mode_t					mode;
	uid_t					uid;
	gid_t					gid;
	off_t					size;
	time_t					atime;
	time_t					mtime;
	time_t					ctime;
} nxs_metadata_v1_t;

typedef struct
{
	nxs_string_t			name;
	mode_t					mode;
	off_t					size;
} nxs_metadata_v2_t;

typedef struct
{
	nxs_string_t			name;
	mode_t					mode;
	off_t					size;
	nxs_string_t			uname;
	nxs_string_t			gname;
	time_t					atime;
	time_t					mtime;
	time_t					ctime;
} nxs_metadata_v3_t;

static nxs_metadata_err_t nxs_metadata_errors[] =
{
		{NXS_METADATA_E_OK,				(u_char *)_NXS_METADATA_E_OK},
		{NXS_METADATA_E_VER,			(u_char *)_NXS_METADATA_E_VER},
		{NXS_METADATA_E_PTR,			(u_char *)_NXS_METADATA_E_PTR},
		{NXS_METADATA_E_STAT,			(u_char *)_NXS_METADATA_E_STAT},
		{NXS_METADATA_E_SIZE,			(u_char *)_NXS_METADATA_E_SIZE},
		{NXS_METADATA_E_BUF_SIZE,		(u_char *)_NXS_METADATA_E_BUF_SIZE},
		{NXS_METADATA_E_NOENT,			(u_char *)_NXS_METADATA_E_NOENT},
		{NXS_METADATA_E_LOOKUP,			(u_char *)_NXS_METADATA_E_LOOKUP},

		{-1, NULL}
};

nxs_metadata_t *nxs_metadata_malloc(nxs_metadata_v_t version)
{
	nxs_metadata_t		*md = NULL;

	md = (nxs_metadata_t *)nxs_malloc(md, sizeof(nxs_metadata_t));

	if(nxs_metadata_init(md, version) != NXS_METADATA_E_OK){

		md = (nxs_metadata_t *)nxs_free(md);
	}

	return md;
}

nxs_metadata_t *nxs_metadata_malloc_clear(void)
{
	nxs_metadata_t		*md = NULL;

	md = (nxs_metadata_t *)nxs_malloc(md, sizeof(nxs_metadata_t));

	if(nxs_metadata_init_clear(md) != NXS_METADATA_E_OK){

		md = (nxs_metadata_t *)nxs_free(md);
	}

	return md;
}

void *nxs_metadata_v_malloc(nxs_metadata_v_t version)
{
	nxs_metadata_v1_t	*md_v1 = NULL;
	nxs_metadata_v2_t	*md_v2 = NULL;
	nxs_metadata_v3_t	*md_v3 = NULL;

	switch(version){

		case NXS_METADATA_V1:

			md_v1 = (nxs_metadata_v1_t *)nxs_malloc(md_v1, sizeof(nxs_metadata_v1_t));
			md_v1->mode		= 0;
			md_v1->uid		= 0;
			md_v1->gid		= 0;
			md_v1->size		= 0;
			md_v1->atime	= 0;
			md_v1->mtime	= 0;
			md_v1->ctime	= 0;

			nxs_string_init(&md_v1->name);

			return md_v1;

		case NXS_METADATA_V2:

			md_v2 = (nxs_metadata_v2_t *)nxs_malloc(md_v2, sizeof(nxs_metadata_v2_t));
			md_v2->mode		= 0;
			md_v2->size		= 0;

			nxs_string_init(&md_v2->name);

			return md_v2;

		case NXS_METADATA_V3:

			md_v3 = (nxs_metadata_v3_t *)nxs_malloc(md_v3, sizeof(nxs_metadata_v3_t));
			md_v3->mode		= 0;
			md_v3->size		= 0;
			md_v3->atime	= 0;
			md_v3->mtime	= 0;
			md_v3->ctime	= 0;

			nxs_string_init(&md_v3->name);
			nxs_string_init(&md_v3->uname);
			nxs_string_init(&md_v3->gname);

			return md_v3;
	}

	return NULL;
}

nxs_metadata_t *nxs_metadata_destroy(nxs_metadata_t *md)
{

	if(md == NULL){

		return NULL;
	}

	md->m_data = nxs_metadata_v_destroy(md->m_data, md->version);

	return nxs_free(md);
}

void *nxs_metadata_v_destroy(void *md_v, nxs_metadata_v_t version)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v2_t	*md_v2;
	nxs_metadata_v3_t	*md_v3;

	if(md_v == NULL){

		return NULL;
	}

	switch(version){

		case NXS_METADATA_V1:

			md_v1 = md_v;

			nxs_string_free(&md_v1->name);

			nxs_free(md_v);

			break;

		case NXS_METADATA_V2:

			md_v2 = md_v;

			nxs_string_free(&md_v2->name);

			nxs_free(md_v);

			break;

		case NXS_METADATA_V3:

			md_v3 = md_v;

			nxs_string_free(&md_v3->name);
			nxs_string_free(&md_v3->uname);
			nxs_string_free(&md_v3->gname);

			nxs_free(md_v);

			break;

		default:

			nxs_free(md_v);
	}

	return NULL;
}

int nxs_metadata_init(nxs_metadata_t *md, nxs_metadata_v_t version)
{

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	if((md->m_data = nxs_metadata_v_malloc(version)) == NULL){

		md->version = NXS_METADATA_V_NONE;

		return NXS_METADATA_E_VER;
	}
	else{

		md->version = version;
	}

	return NXS_METADATA_E_OK;
}

int nxs_metadata_init_clear(nxs_metadata_t *md)
{

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	md->version = NXS_METADATA_V_NONE;
	md->m_data = NULL;

	return NXS_METADATA_E_OK;
}

void nxs_metadata_free(nxs_metadata_t *md)
{

	if(md == NULL){

		return;
	}

	md->m_data = nxs_metadata_v_destroy(md->m_data, md->version);
	md->version = NXS_METADATA_V_NONE;
}

void nxs_metadata_clear(nxs_metadata_t *md)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v2_t	*md_v2;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return;
	}

	if(md->m_data == NULL){

		return;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			nxs_string_clear(&md_v1->name);
			md_v1->mode		= 0;
			md_v1->uid		= 0;
			md_v1->gid		= 0;
			md_v1->size		= 0;
			md_v1->atime	= 0;
			md_v1->mtime	= 0;
			md_v1->ctime	= 0;

			break;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			nxs_string_clear(&md_v2->name);
			md_v2->mode		= 0;
			md_v2->size		= 0;

			break;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			nxs_string_clear(&md_v3->name);
			nxs_string_clear(&md_v3->uname);
			nxs_string_clear(&md_v3->gname);
			md_v3->mode		= 0;
			md_v3->size		= 0;
			md_v3->atime	= 0;
			md_v3->mtime	= 0;
			md_v3->ctime	= 0;

			break;
	}
}

int nxs_metadata_fill(nxs_metadata_t *md, nxs_metadata_v_t version, nxs_string_t *path)
{
	nxs_metadata_v1_t		*md_v1;
	nxs_metadata_v2_t		*md_v2;
	nxs_metadata_v3_t		*md_v3;
	nxs_fs_stat_t			f_stat;
	struct passwd			*usr;
	struct group			*grp;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	if(path == NULL){

		return NXS_METADATA_E_PTR;
	}

	if(md->version != version || md->m_data == NULL){

		nxs_metadata_free(md);

		if(nxs_metadata_init(md, version) != NXS_METADATA_E_OK){

			return NXS_METADATA_E_VER;
		}
	}
	else{

		nxs_metadata_clear(md);
	}

	if(nxs_fs_lstat(path, &f_stat) < 0){

		return NXS_METADATA_E_STAT;
	}

	switch(version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			nxs_string_basename(&md_v1->name, path);

			md_v1->mode		= f_stat.st_mode;
			md_v1->uid		= f_stat.st_uid;
			md_v1->gid		= f_stat.st_gid;
			md_v1->size		= f_stat.st_size;
			md_v1->atime	= f_stat.st_atime;
			md_v1->mtime	= f_stat.st_mtime;
			md_v1->ctime	= f_stat.st_ctime;

			break;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			nxs_string_basename(&md_v2->name, path);

			md_v2->mode		= f_stat.st_mode & S_IFMT;
			md_v2->size		= f_stat.st_size;

			break;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			nxs_string_basename(&md_v3->name, path);

			if((usr = getpwuid(f_stat.st_uid)) == NULL){

				nxs_string_set_len(&md_v3->uname, 0);
			}
			else{

				nxs_string_char_cpy_dyn(&md_v3->uname, 0, (u_char *)(usr->pw_name));
			}

			if((grp = getgrgid(f_stat.st_gid)) == NULL){

				nxs_string_set_len(&md_v3->gname, 0);
			}
			else{

				nxs_string_char_cpy_dyn(&md_v3->gname, 0, (u_char *)(grp->gr_name));
			}

			md_v3->mode		= f_stat.st_mode;
			md_v3->size		= f_stat.st_size;
			md_v3->atime	= f_stat.st_atime;
			md_v3->mtime	= f_stat.st_mtime;
			md_v3->ctime	= f_stat.st_ctime;

			break;

		default:

			return NXS_METADATA_E_VER;
	}

	return NXS_METADATA_E_OK;
}

/*
 * Макрос для записи в буфер "buf" данных (d) длиной (ds) со смещением "_offset" относительно его начала.
 * После записи смещение будет увеличено на величину (s)
 */
#define macro_buf_cpy_metadata(d, ds, s)		\
			nxs_buf_cpy_dyn(buf, _offset, d, ds); \
			_offset += s;

/*
 * Запись метаданных "md" в буфер "buf" по смещению "offset" от его начала.
 *
 * msize - это указатель на переменную, в которой хранится количество записанных в буфер байт (размер метаданных).
 * Данная переменная заполняется только в том случае, если указатель на неё имеет не нулевое значение.
 *
 * Возвращаемые значения:
 * * NXS_METADATA_E_OK	- ошибок не возникло, метаданные успешно записаны в буфер
 * * NXS_METADATA_E_PTR	- ошибка с указателем либо на саму структуру метаданных ("md"), либо на структуру "данных" метаданных
 * * NXS_METADATA_E_VER	- указана не верная версия метаданных
 */
int nxs_metadata_to_buf(nxs_metadata_t *md, size_t offset, nxs_buf_t *buf, size_t *msize)
{
	nxs_metadata_v1_t		*md_v1;
	nxs_metadata_v2_t		*md_v2;
	nxs_metadata_v3_t		*md_v3;
	size_t					_offset, t;
	u_char					sl;

#if __WORDSIZE == 32
	uint64_t				__v;
	int64_t					__sv;
#endif

	if(md == NULL){

		nxs_buf_set_len(buf, 0);

		return NXS_METADATA_E_PTR;
	}

	if(md->m_data == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			_offset = offset;

			macro_buf_cpy_metadata(&md->version,					sizeof(nxs_metadata_v_t),	_NXS_METADATA_F_VER_LEN)

			t = nxs_string_len(&md_v1->name);

#		if __WORDSIZE == 32

			__v = t;
			macro_buf_cpy_metadata(&__v,							sizeof(__v),				_NXS_METADATA_F_NAME_LEN)
#		else

			macro_buf_cpy_metadata(&t,								sizeof(t),					_NXS_METADATA_F_NAME_LEN)
#		endif

			macro_buf_cpy_metadata(nxs_string_str(&md_v1->name),	t, 							t)

			macro_buf_cpy_metadata(&md_v1->size,					sizeof(off_t),				_NXS_METADATA_F_SIZE_LEN)
			macro_buf_cpy_metadata(&md_v1->mode,					sizeof(mode_t),				_NXS_METADATA_F_MODE_LEN)
			macro_buf_cpy_metadata(&md_v1->uid,						sizeof(uid_t),				_NXS_METADATA_F_UID_LEN)
			macro_buf_cpy_metadata(&md_v1->gid,						sizeof(gid_t),				_NXS_METADATA_F_GID_LEN)

#		if __WORDSIZE == 32

			__sv = md_v1->atime;
			macro_buf_cpy_metadata(&__sv,							sizeof(__sv),				_NXS_METADATA_F_TIME_LEN)

			__sv = md_v1->mtime;
			macro_buf_cpy_metadata(&__sv,							sizeof(__sv),				_NXS_METADATA_F_TIME_LEN)

			__sv = md_v1->ctime;
			macro_buf_cpy_metadata(&__sv,							sizeof(__sv),				_NXS_METADATA_F_TIME_LEN)
#		else

			macro_buf_cpy_metadata(&md_v1->atime,					sizeof(time_t),				_NXS_METADATA_F_TIME_LEN)
			macro_buf_cpy_metadata(&md_v1->mtime,					sizeof(time_t),				_NXS_METADATA_F_TIME_LEN)
			macro_buf_cpy_metadata(&md_v1->ctime,					sizeof(time_t),				_NXS_METADATA_F_TIME_LEN)
#		endif

			if(msize != NULL){

				*msize = _offset - offset;
			}

			break;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			_offset = offset;

			macro_buf_cpy_metadata(&md->version,					sizeof(nxs_metadata_v_t),	_NXS_METADATA_F_VER_LEN)

			t = nxs_string_len(&md_v2->name);

#		if __WORDSIZE == 32

			__v = t;
			macro_buf_cpy_metadata(&__v,							sizeof(__v),				_NXS_METADATA_F_NAME_LEN)
#		else

			macro_buf_cpy_metadata(&t,								sizeof(t),					_NXS_METADATA_F_NAME_LEN)
#		endif

			macro_buf_cpy_metadata(nxs_string_str(&md_v2->name),	t, 							t)
			macro_buf_cpy_metadata(&md_v2->size,					sizeof(off_t),				_NXS_METADATA_F_SIZE_LEN)
			macro_buf_cpy_metadata(&md_v2->mode,					sizeof(mode_t),				_NXS_METADATA_F_MODE_LEN)


			if(msize != NULL){

				*msize = _offset - offset;
			}

			break;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			_offset = offset;

			macro_buf_cpy_metadata(&md->version,					sizeof(nxs_metadata_v_t),	_NXS_METADATA_F_VER_LEN)

			t = nxs_string_len(&md_v3->name);

#		if __WORDSIZE == 32

			__v = t;
			macro_buf_cpy_metadata(&__v,							sizeof(__v),				_NXS_METADATA_F_NAME_LEN)
#		else

			macro_buf_cpy_metadata(&t,								sizeof(t),					_NXS_METADATA_F_NAME_LEN)
#		endif

			macro_buf_cpy_metadata(nxs_string_str(&md_v3->name),	t, 							t)

			macro_buf_cpy_metadata(&md_v3->size,					sizeof(off_t),				_NXS_METADATA_F_SIZE_LEN)
			macro_buf_cpy_metadata(&md_v3->mode,					sizeof(mode_t),				_NXS_METADATA_F_MODE_LEN)

			sl = (u_char)nxs_string_len(&md_v3->uname);
			macro_buf_cpy_metadata(&sl,								sizeof(u_char),				_NXS_METADATA_F_SUNAME_LEN)
			macro_buf_cpy_metadata(nxs_string_str(&md_v3->uname),	sl,							sl)

			sl = (u_char)nxs_string_len(&md_v3->gname);
			macro_buf_cpy_metadata(&sl,								sizeof(u_char),				_NXS_METADATA_F_SGNAME_LEN)
			macro_buf_cpy_metadata(nxs_string_str(&md_v3->gname),	sl,							sl)

#		if __WORDSIZE == 32

			__sv = md_v3->atime;
			macro_buf_cpy_metadata(&__sv,							sizeof(__sv),				_NXS_METADATA_F_TIME_LEN)

			__sv = md_v3->mtime;
			macro_buf_cpy_metadata(&__sv,							sizeof(__sv),				_NXS_METADATA_F_TIME_LEN)

			__sv = md_v3->ctime;
			macro_buf_cpy_metadata(&__sv,							sizeof(__sv),				_NXS_METADATA_F_TIME_LEN)
#		else

			macro_buf_cpy_metadata(&md_v3->atime,					sizeof(time_t),				_NXS_METADATA_F_TIME_LEN)
			macro_buf_cpy_metadata(&md_v3->mtime,					sizeof(time_t),				_NXS_METADATA_F_TIME_LEN)
			macro_buf_cpy_metadata(&md_v3->ctime,					sizeof(time_t),				_NXS_METADATA_F_TIME_LEN)
#		endif

			if(msize != NULL){

				*msize = _offset - offset;
			}

			break;

		default:

			return NXS_METADATA_E_VER;
	}

	return NXS_METADATA_E_OK;
}

/*
 * Макрос для получения из буфера "buf" данных (d) длиной (ds) со смещением "_offset" относительно его начала.
 * После записи смещение будет увеличено на величину (s)
 */
#define macro_buf_mem_metadata(d, ds, s)		\
			if(nxs_buf_get_mem(buf, _offset, d, ds) != NXS_BUF_E_OK){ \
				nxs_metadata_free(md); \
				return NXS_METADATA_E_BUF_SIZE; \
			} \
			_offset += s;

/*
 * Получение метаданных из переданного буфера со смещением "offset" относительно начала буфера и заполнение структуры "md".
 * Если указатель "msize" не NULL - в данную область памяти будет записан размер считанных метаданных.
 *
 * Возвращаемые значения:
 * * NXS_METADATA_E_OK		- метаданные из буфера получены успешно
 * * NXS_METADATA_E_PTR		- нулевой указатель на структуру "md"
 * * NXS_METADATA_E_VER		- полученная из буфера версия метаданных имеет не верное значение
 * * NXS_METADATA_E_SIZE	- считанный размер строки (с именем файла) превышает размер всего буфера
 */
int nxs_metadata_from_buf(nxs_metadata_t *md, size_t offset, nxs_buf_t *buf, size_t *msize)
{
	nxs_metadata_v_t		version;
	nxs_metadata_v1_t		*md_v1;
	nxs_metadata_v2_t		*md_v2;
	nxs_metadata_v3_t		*md_v3;
	size_t					_offset, t;
	u_char					*_buf, sl;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	_offset = offset;

	macro_buf_mem_metadata(&version,					sizeof(nxs_metadata_v_t),		_NXS_METADATA_F_VER_LEN)

	if(md->version != version || md->m_data == NULL){

		nxs_metadata_free(md);

		if(nxs_metadata_init(md, version)!= NXS_METADATA_E_OK){

			return NXS_METADATA_E_VER;
		}
	}
	else{

		nxs_metadata_clear(md);
	}

	switch(version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			macro_buf_mem_metadata(&t,					sizeof(t),						_NXS_METADATA_F_NAME_LEN)

			if(t > nxs_buf_get_len(buf)){

				/*
				 * Переданный размер строки превышает размер всего буфера - это ошибка
				 */

				nxs_metadata_free(md);

				return NXS_METADATA_E_SIZE;
			}

			if(nxs_string_size(&md_v1->name) < t + 1){

				nxs_string_resize(&md_v1->name, t + 1);
			}
			_buf = nxs_string_str(&md_v1->name);
			macro_buf_mem_metadata(_buf,				t,								t)
			nxs_string_set_len(&md_v1->name, t);

			macro_buf_mem_metadata(&md_v1->size,		sizeof(off_t),					_NXS_METADATA_F_SIZE_LEN)
			macro_buf_mem_metadata(&md_v1->mode,		sizeof(mode_t),					_NXS_METADATA_F_MODE_LEN)
			macro_buf_mem_metadata(&md_v1->uid,			sizeof(uid_t),					_NXS_METADATA_F_UID_LEN)
			macro_buf_mem_metadata(&md_v1->gid,			sizeof(gid_t),					_NXS_METADATA_F_GID_LEN)
			macro_buf_mem_metadata(&md_v1->atime,		sizeof(time_t),					_NXS_METADATA_F_TIME_LEN)
			macro_buf_mem_metadata(&md_v1->mtime,		sizeof(time_t),					_NXS_METADATA_F_TIME_LEN)
			macro_buf_mem_metadata(&md_v1->ctime,		sizeof(time_t),					_NXS_METADATA_F_TIME_LEN)

			if(msize != NULL){

				*msize = _offset - offset;
			}

			break;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			macro_buf_mem_metadata(&t,					sizeof(t),						_NXS_METADATA_F_NAME_LEN)

			if(t > nxs_buf_get_len(buf)){

				/*
				 * Переданный размер строки превышает размер всего буфера - это ошибка
				 */

				nxs_metadata_free(md);

				return NXS_METADATA_E_SIZE;
			}

			if(nxs_string_size(&md_v2->name) < t + 1){

				nxs_string_resize(&md_v2->name, t + 1);
			}
			_buf = nxs_string_str(&md_v2->name);
			macro_buf_mem_metadata(_buf, 				t,								t)
			nxs_string_set_len(&md_v2->name,t);

			macro_buf_mem_metadata(&md_v2->size,		sizeof(off_t),					_NXS_METADATA_F_SIZE_LEN)
			macro_buf_mem_metadata(&md_v2->mode,		sizeof(mode_t),					_NXS_METADATA_F_MODE_LEN)

			if(msize != NULL){

				*msize = _offset - offset;
			}

			break;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			macro_buf_mem_metadata(&t,					sizeof(t),						_NXS_METADATA_F_NAME_LEN)

			if(t > nxs_buf_get_len(buf)){

				/*
				 * Переданный размер строки превышает размер всего буфера - это ошибка
				 */

				nxs_metadata_free(md);

				return NXS_METADATA_E_SIZE;
			}

			if(nxs_string_size(&md_v3->name) < t + 1){

				nxs_string_resize(&md_v3->name, t + 1);
			}
			_buf = nxs_string_str(&md_v3->name);
			macro_buf_mem_metadata(_buf, 				t,								t)
			nxs_string_set_len(&md_v3->name, t);

			macro_buf_mem_metadata(&md_v3->size,		sizeof(off_t),					_NXS_METADATA_F_SIZE_LEN)
			macro_buf_mem_metadata(&md_v3->mode,		sizeof(mode_t),					_NXS_METADATA_F_MODE_LEN)

			macro_buf_mem_metadata(&sl,					sizeof(u_char),					_NXS_METADATA_F_SUNAME_LEN)
			if(nxs_string_size(&md_v3->uname) < (size_t)sl + 1){

				nxs_string_resize(&md_v3->uname, (size_t)sl + 1);
			}
			_buf = nxs_string_str(&md_v3->uname);
			macro_buf_mem_metadata(_buf,				sl,								sl)
			nxs_string_set_len(&md_v3->uname, sl);

			macro_buf_mem_metadata(&sl,					sizeof(u_char),					_NXS_METADATA_F_SGNAME_LEN)
			if(nxs_string_size(&md_v3->gname) < (size_t)sl + 1){

				nxs_string_resize(&md_v3->gname, (size_t)sl + 1);
			}
			_buf = nxs_string_str(&md_v3->gname);
			macro_buf_mem_metadata(_buf,				sl,								sl)
			nxs_string_set_len(&md_v3->gname, sl);

			macro_buf_mem_metadata(&md_v3->atime,		sizeof(time_t),					_NXS_METADATA_F_TIME_LEN)
			macro_buf_mem_metadata(&md_v3->mtime,		sizeof(time_t),					_NXS_METADATA_F_TIME_LEN)
			macro_buf_mem_metadata(&md_v3->ctime,		sizeof(time_t),					_NXS_METADATA_F_TIME_LEN)

			if(msize != NULL){

				*msize = _offset - offset;
			}

			break;

		default:

			return NXS_METADATA_E_VER;
	}

	return NXS_METADATA_E_OK;
}

int nxs_metadata_set_name(nxs_metadata_t *md, nxs_string_t *name)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v2_t	*md_v2;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	if(md->m_data == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			nxs_string_cpy_dyn(&md_v1->name, 0, name, 0);

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			nxs_string_cpy_dyn(&md_v2->name, 0, name, 0);

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			nxs_string_cpy_dyn(&md_v3->name, 0, name, 0);

			return NXS_METADATA_E_OK;
	}

	return NXS_METADATA_E_VER;
}

int nxs_metadata_set_uname(nxs_metadata_t *md, nxs_string_t *uname)
{
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	if(md->m_data == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V2:

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			nxs_string_cpy_dyn(&md_v3->uname, 0, uname, 0);

			return NXS_METADATA_E_OK;
	}

	return NXS_METADATA_E_VER;
}

int nxs_metadata_set_gname(nxs_metadata_t *md, nxs_string_t *gname)
{
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	if(md->m_data == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V2:

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			nxs_string_cpy_dyn(&md_v3->gname, 0, gname, 0);

			return NXS_METADATA_E_OK;
	}

	return NXS_METADATA_E_VER;
}

int nxs_metadata_set_perm(nxs_metadata_t *md, mode_t mode, uid_t uid, gid_t gid)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v2_t	*md_v2;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	if(md->m_data == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			md_v1->mode	= mode;
			md_v1->uid	= uid;
			md_v1->gid	= gid;

			break;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			md_v2->mode	= mode;

			break;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			md_v3->mode	= mode;

			break;

		default:

			return NXS_METADATA_E_VER;
	}

	return NXS_METADATA_E_OK;
}

int nxs_metadata_set_time(nxs_metadata_t *md, time_t atime, time_t mtime, time_t ctime)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	if(md->m_data == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			md_v1->atime	= atime;
			md_v1->mtime	= mtime;
			md_v1->ctime	= ctime;

			break;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			md_v3->atime	= atime;
			md_v3->mtime	= mtime;
			md_v3->ctime	= ctime;

			break;

		default:

			return NXS_METADATA_E_VER;
	}

	return NXS_METADATA_E_OK;
}

int nxs_metadata_set_size(nxs_metadata_t *md, off_t size)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v2_t	*md_v2;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	if(md->m_data == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			md_v1->size	= size;

			break;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			md_v2->size	= size;

			break;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			md_v3->size	= size;

			break;

		default:

			return NXS_METADATA_E_VER;
	}

	return NXS_METADATA_E_OK;
}

nxs_metadata_v_t nxs_metadata_get_version(nxs_metadata_t *md)
{

	if(md == NULL){

		return NXS_METADATA_V_NONE;
	}

	return md->version;
}

nxs_string_t *nxs_metadata_get_name(nxs_metadata_t *md)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v2_t	*md_v2;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NULL;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			return &md_v1->name;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			return &md_v2->name;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			return &md_v3->name;
	}

	return NULL;
}

nxs_string_t *nxs_metadata_get_uname(nxs_metadata_t *md)
{
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NULL;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			return NULL;

		case NXS_METADATA_V2:

			return NULL;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			return &md_v3->uname;
	}

	return NULL;
}

nxs_string_t *nxs_metadata_get_gname(nxs_metadata_t *md)
{
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NULL;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			return NULL;

		case NXS_METADATA_V2:

			return NULL;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			return &md_v3->gname;
	}

	return NULL;
}

int nxs_metadata_get_mode(nxs_metadata_t *md, mode_t *mode)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v2_t	*md_v2;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			*mode = md_v1->mode;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			*mode = md_v2->mode;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			*mode = md_v3->mode;

			return NXS_METADATA_E_OK;
	}

	*mode = 0;

	return NXS_METADATA_E_VER;
}

int nxs_metadata_get_perm(nxs_metadata_t *md, mode_t *perm)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			*perm = md_v1->mode & ~S_IFMT;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V2:

			*perm = 0;

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			*perm = md_v3->mode & ~S_IFMT;

			return NXS_METADATA_E_OK;
	}

	*perm = 0;

	return NXS_METADATA_E_VER;
}

int nxs_metadata_get_uid(nxs_metadata_t *md, uid_t *uid)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v3_t	*md_v3;
	struct passwd		*usr;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			*uid = md_v1->uid;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V2:

			*uid = 0;

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			if((usr = getpwnam((char *)nxs_string_str(&md_v3->uname))) == NULL){

				*uid = 0;

				return NXS_METADATA_E_LOOKUP;
			}

			*uid = usr->pw_uid;

			return NXS_METADATA_E_OK;
	}

	*uid = 0;

	return NXS_METADATA_E_VER;
}

int nxs_metadata_get_gid(nxs_metadata_t *md, gid_t *gid)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v3_t	*md_v3;
	struct group		*grp;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			*gid = md_v1->gid;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V2:

			*gid = 0;

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			if((grp = getgrnam((char *)nxs_string_str(&md_v3->gname))) == NULL){

				*gid = 0;

				return NXS_METADATA_E_LOOKUP;
			}

			*gid = grp->gr_gid;

			return NXS_METADATA_E_OK;
	}

	*gid = 0;

	return NXS_METADATA_E_VER;
}

int nxs_metadata_get_size(nxs_metadata_t *md, off_t *size)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v2_t	*md_v2;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	if(size == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			*size = md_v1->size;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			*size = md_v2->size;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			*size = md_v3->size;

			return NXS_METADATA_E_OK;
	}

	*size = 0;

	return NXS_METADATA_E_VER;
}

int nxs_metadata_get_atime(nxs_metadata_t *md, time_t *atime)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			*atime = md_v1->atime;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V2:

			*atime = 0;

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			*atime = md_v3->atime;

			return NXS_METADATA_E_OK;
	}

	return NXS_METADATA_E_VER;
}

int nxs_metadata_get_mtime(nxs_metadata_t *md, time_t *mtime)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			*mtime = md_v1->mtime;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V2:

			*mtime = 0;

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			*mtime = md_v3->mtime;

			return NXS_METADATA_E_OK;
	}

	return NXS_METADATA_E_VER;
}

int nxs_metadata_get_ctime(nxs_metadata_t *md, time_t *ctime)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return NXS_METADATA_E_PTR;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			*ctime = md_v1->ctime;

			return NXS_METADATA_E_OK;

		case NXS_METADATA_V2:

			*ctime = 0;

			return NXS_METADATA_E_NOENT;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			*ctime = md_v3->ctime;

			return NXS_METADATA_E_OK;
	}

	return NXS_METADATA_E_VER;
}

u_char *nxs_metadata_error(int error)
{
	int i;

	for(i = 0; nxs_metadata_errors[i].error_text != NULL; i++){

		if(nxs_metadata_errors[i].error == error){

			return nxs_metadata_errors[i].error_text;
		}
	}

	return (u_char *)_NXS_METADATA_E_UNKNWN;
}

void nxs_metadata_print(nxs_process_t *proc, nxs_metadata_t *md)
{
	nxs_metadata_v1_t	*md_v1;
	nxs_metadata_v2_t	*md_v2;
	nxs_metadata_v3_t	*md_v3;

	if(md == NULL){

		return;
	}

	if(md->m_data == NULL){

		return;
	}

	switch(md->version){

		case NXS_METADATA_V1:

			md_v1 = md->m_data;

			nxs_log_write_debug(proc, "printing metadata information\n\n"
											"version:\t%d\n"
											"name:\t\t\"%s\"\n"
											"size:\t\t%" PRIu64 "\n"
											"mode:\t\t%o\n"
											"uid:\t\t%d\n"
											"gid:\t\t%d\n"
											"atime:\t\t%ld\n"
											"mtime:\t\t%ld\n"
											"ctime:\t\t%ld\n", md->version,
															nxs_string_str(&md_v1->name),
															md_v1->size,
															md_v1->mode,
															md_v1->uid,
															md_v1->gid,
															md_v1->atime,
															md_v1->mtime,
															md_v1->ctime);

			break;

		case NXS_METADATA_V2:

			md_v2 = md->m_data;

			nxs_log_write_debug(proc, "printing metadata information\n\n"
											"version:\t%d\n"
											"name:\t\t\"%s\"\n"
											"size:\t\t%" PRIu64 "\n"
											"mode:\t\t%o\n", md->version,
															nxs_string_str(&md_v2->name),
															md_v2->size,
															md_v2->mode);

			break;

		case NXS_METADATA_V3:

			md_v3 = md->m_data;

			nxs_log_write_debug(proc, "printing metadata information\n\n"
											"version:\t%d\n"
											"name:\t\t\"%s\"\n"
											"size:\t\t%" PRIu64 "\n"
											"mode:\t\t%o\n"
											"uname:\t\t\"%s\"\n"
											"gname:\t\t\"%s\"\n"
											"atime:\t\t%ld\n"
											"mtime:\t\t%ld\n"
											"ctime:\t\t%ld\n", md->version,
															nxs_string_str(&md_v3->name),
															md_v3->size,
															md_v3->mode,
															nxs_string_str(&md_v3->uname),
															nxs_string_str(&md_v3->gname),
															md_v3->atime,
															md_v3->mtime,
															md_v3->ctime);

			break;
	}
}
