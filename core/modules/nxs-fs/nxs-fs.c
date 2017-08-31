// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define NXS_FS_BUF_SIZE		4096

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_fs_dir_t *nxs_fs_opendir(nxs_string_t *path)
{

	return (nxs_fs_dir_t *)opendir((char *)nxs_string_get_substr(path, 0));
}

int nxs_fs_closedir(nxs_fs_dir_t *dir)
{

	return closedir((DIR *)dir);
}

nxs_fs_file_t *nxs_fs_fopen(nxs_string_t *path, u_char *mode)
{

	return (nxs_fs_file_t *)fopen((char *)nxs_string_get_substr(path, 0), (char *)mode);
}

int nxs_fs_fclose(nxs_fs_file_t *fp)
{

	return fclose((FILE *)fp);
}

int nxs_fs_mkdir(nxs_string_t *path, mode_t mode)
{

	if(path == NULL) {

		errno = EFAULT;

		return -1;
	}

	if(nxs_string_len(path) == 0) {

		errno = EFAULT;

		return -1;
	}

	return mkdir((char *)nxs_string_str(path), mode);
}

ssize_t nxs_fs_write_buf(int fd, nxs_buf_t *buf)
{
	u_char *b;
	size_t  eb;

	if(buf == NULL) {

		errno = EFAULT;

		return -1;
	}

	b  = nxs_buf_get_subbuf(buf, 0);
	eb = nxs_buf_get_len(buf);

	return write(fd, b, eb);
}

ssize_t nxs_fs_write_buf_n(int fd, nxs_buf_t *buf, size_t n)
{
	u_char *b;

	if(buf == NULL) {

		errno = EFAULT;

		return -1;
	}

	if(n > nxs_buf_get_size(buf)) {

		errno = EFAULT;

		return -1;
	}

	b = nxs_buf_get_subbuf(buf, 0);

	return write(fd, b, n);
}

ssize_t nxs_fs_read_buf(int fd, nxs_buf_t *buf)
{
	u_char *b;
	size_t  eb;
	ssize_t rc;

	if(buf == NULL) {

		errno = EFAULT;

		return -1;
	}

	b  = nxs_buf_get_subbuf(buf, 0);
	eb = nxs_buf_get_size(buf);

	if((rc = read(fd, b, eb)) == -1) {

		nxs_buf_set_len(buf, 0);

		return -1;
	}

	nxs_buf_set_len(buf, rc);

	return rc;
}

ssize_t nxs_fs_read_buf_n(int fd, nxs_buf_t *buf, size_t n)
{
	u_char *b;
	ssize_t rc;

	if(buf == NULL) {

		errno = EFAULT;

		return -1;
	}

	if(n > nxs_buf_get_size(buf)) {

		errno = EFAULT;

		return -1;
	}

	b = nxs_buf_get_subbuf(buf, 0);

	if((rc = read(fd, b, n)) == -1) {

		nxs_buf_set_len(buf, 0);

		return -1;
	}

	nxs_buf_set_len(buf, rc);

	return rc;
}

/*
 * Записать в файл 'path' буфер 'buf'. Если файл не существует, то он будет создан с правами 'mode'.
 * Если файл существует, то он будет перезаписан.
 *
 * В случае ошибки переменная errno будет содержать дополнительный код
 *
 * Возвращаемые ошибки:
 * * NXS_FS_E_FILE_OPEN		- Ошибка открыти файла
 * * NXS_FS_E_FILE_WRITE	- Оибка записи в файл
 * * NXS_FS_E_FILE_CLOSE	- Оибка закрытия файла
 */
ssize_t nxs_fs_write_file(nxs_string_t *path, nxs_buf_t *buf, mode_t mode)
{
	int     fd;
	ssize_t bc;

	if((fd = open((char *)nxs_string_str(path), O_WRONLY | O_CREAT | O_TRUNC, mode)) < 0) {

		return NXS_FS_E_FILE_OPEN;
	}

	if((bc = nxs_fs_write_buf(fd, buf)) < 0) {

		return NXS_FS_E_FILE_WRITE;
	}

	if(close(fd) < 0) {

		return NXS_FS_E_FILE_CLOSE;
	}

	return bc;
}

ssize_t nxs_fs_write_file_n(nxs_string_t *path, nxs_buf_t *buf, size_t n, mode_t mode)
{
	int     fd;
	ssize_t bc;

	if((fd = open((char *)nxs_string_str(path), O_WRONLY | O_CREAT | O_TRUNC, mode)) < 0) {

		return NXS_FS_E_FILE_OPEN;
	}

	if((bc = nxs_fs_write_buf_n(fd, buf, n)) < 0) {

		return NXS_FS_E_FILE_WRITE;
	}

	if(close(fd) < 0) {

		return NXS_FS_E_FILE_CLOSE;
	}

	return bc;
}

/*
 * Дописать в файл 'path' буфер 'buf'. Если файл не существует, то он будет создан с правами 'mode'.
 *
 * В случае ошибки переменная errno будет содержать дополнительный код
 *
 * Возвращаемые ошибки:
 * * NXS_FS_E_FILE_OPEN		- Ошибка открыти файла
 * * NXS_FS_E_FILE_WRITE	- Оибка записи в файл
 * * NXS_FS_E_FILE_CLOSE	- Оибка закрытия файла
 */
ssize_t nxs_fs_append_file(nxs_string_t *path, nxs_buf_t *buf, mode_t mode)
{
	int     fd;
	ssize_t bc;

	if((fd = open((char *)nxs_string_str(path), O_WRONLY | O_CREAT | O_APPEND, mode)) < 0) {

		return NXS_FS_E_FILE_OPEN;
	}

	if((bc = nxs_fs_write_buf(fd, buf)) < 0) {

		return NXS_FS_E_FILE_WRITE;
	}

	if(close(fd) < 0) {

		return NXS_FS_E_FILE_CLOSE;
	}

	return bc;
}

ssize_t nxs_fs_append_file_n(nxs_string_t *path, nxs_buf_t *buf, size_t n, mode_t mode)
{
	int     fd;
	ssize_t bc;

	if((fd = open((char *)nxs_string_str(path), O_WRONLY | O_CREAT | O_APPEND, mode)) < 0) {

		return NXS_FS_E_FILE_OPEN;
	}

	if((bc = nxs_fs_write_buf_n(fd, buf, n)) < 0) {

		return NXS_FS_E_FILE_WRITE;
	}

	if(close(fd) < 0) {

		return NXS_FS_E_FILE_CLOSE;
	}

	return bc;
}

/*
 * Считать из файла 'path' данные буфер 'buf'.
 * Размер считанных данных данных будет равен либо размеру файла, либо размеру буфера (в зависимости от того, что меньше)
 *
 * В случае ошибки переменная errno будет содержать дополнительный код
 *
 * Возвращаемые ошибки:
 * * NXS_FS_E_FILE_OPEN		- Ошибка открыти файла
 * * NXS_FS_E_FILE_READ		- Оибка записи в файл
 * * NXS_FS_E_FILE_CLOSE	- Оибка закрытия файла
 */
ssize_t nxs_fs_read_file_to_buf(nxs_string_t *path, nxs_buf_t *buf)
{
	int     fd;
	ssize_t bc;

	if((fd = open((char *)nxs_string_str(path), O_RDONLY)) < 0) {

		return NXS_FS_E_FILE_OPEN;
	}

	if((bc = nxs_fs_read_buf(fd, buf)) < 0) {

		return NXS_FS_E_FILE_READ;
	}

	if(close(fd) < 0) {

		return NXS_FS_E_FILE_CLOSE;
	}

	return bc;
}

ssize_t nxs_fs_read_file_to_buf_n(nxs_string_t *path, nxs_buf_t *buf, size_t n)
{
	int     fd;
	ssize_t bc;

	if((fd = open((char *)nxs_string_str(path), O_RDONLY)) < 0) {

		return NXS_FS_E_FILE_OPEN;
	}

	if((bc = nxs_fs_read_buf_n(fd, buf, n)) < 0) {

		return NXS_FS_E_FILE_READ;
	}

	if(close(fd) < 0) {

		return NXS_FS_E_FILE_CLOSE;
	}

	return bc;
}

/*
 * Считывание файла, расположенного по пути 'path' в строку 'str'.
 * При необходимости размер строки 'str' будет увеличен так, чтобы в неё полностью помещался запрошенный файл
 */
ssize_t nxs_fs_read_file_to_str(nxs_string_t *path, nxs_string_t *str)
{
	int     fd;
	ssize_t bc;
	u_char  buf[NXS_FS_BUF_SIZE];

	if(str == NULL || path == NULL) {

		errno = EFAULT;

		return NXS_FS_E_FILE_BUF;
	}

	nxs_string_clear(str);

	if((fd = open((char *)nxs_string_str(path), O_RDONLY)) < 0) {

		return NXS_FS_E_FILE_OPEN;
	}

	while((bc = read(fd, buf, NXS_FS_BUF_SIZE)) > 0) {

		nxs_string_char_ncat(str, buf, bc);
	}

	if(bc == -1) {

		nxs_string_set_len(str, 0);

		return NXS_FS_E_FILE_READ;
	}

	if(close(fd) < 0) {

		return NXS_FS_E_FILE_CLOSE;
	}

	return (ssize_t)nxs_string_len(str);
}

void nxs_fs_dirent_init(nxs_fs_dirent_t *entry)
{
	entry->d_ino    = 0;
	entry->d_off    = 0;
	entry->d_reclen = 0;
	entry->d_type   = 0;

	entry->d_name = nxs_string_malloc(NXS_FS_D_NAME_SIZE, NULL);
}

void nxs_fs_dirent_free(nxs_fs_dirent_t *entry)
{
	entry->d_name = nxs_string_destroy(entry->d_name);
}

/*
 * Функция чтения директории.
 *
 * mode_set:
 * * NXS_FS_MODE_SET_ALL
 * * NXS_FS_MODE_SET_SKEEP_DOT
 *
 * Возвращаемые значения:
 * 0	- очередная итерация считывания из директории произведена успешно
 * -1	- достигнут конец директории
 * > 0	- произошла ошибка во время чтения директории (в этом случае возвращаемое значение соответствует коду ошибки)
 */
int nxs_fs_readdir(nxs_fs_dir_t *dir, nxs_fs_dirent_t *entry, int mode_set)
{
	struct dirent *dir_entry;

	errno = 0;

	while((dir_entry = readdir((DIR *)dir)) != NULL) {

		switch(mode_set) {

			case NXS_FS_MODE_SET_ALL:

				entry->d_ino    = dir_entry->d_ino;
				entry->d_off    = dir_entry->d_off;
				entry->d_reclen = dir_entry->d_reclen;
				entry->d_type   = dir_entry->d_type;

				nxs_string_char_cpy_static(entry->d_name, 0, (u_char *)dir_entry->d_name);

				return 0;

				break;

			case NXS_FS_MODE_SET_SKEEP_DOT:

				if(strcmp(dir_entry->d_name, ".") && strcmp(dir_entry->d_name, "..")) {

					entry->d_ino    = dir_entry->d_ino;
					entry->d_off    = dir_entry->d_off;
					entry->d_reclen = dir_entry->d_reclen;
					entry->d_type   = dir_entry->d_type;

					nxs_string_char_cpy_static(entry->d_name, 0, (u_char *)dir_entry->d_name);

					return 0;
				}

				break;
		}
	}

	entry->d_ino    = 0;
	entry->d_off    = 0;
	entry->d_reclen = 0;
	entry->d_type   = 0;

	nxs_string_clear(entry->d_name);

	if(errno == 0) {

		return -1;
	}
	else {

		return errno;
	}
}

int nxs_fs_stat(nxs_string_t *file, nxs_fs_stat_t *buf)
{

	return stat((char *)nxs_string_get_substr(file, 0), (struct stat *)buf);
}

int nxs_fs_lstat(nxs_string_t *file, nxs_fs_stat_t *buf)
{

	return lstat((char *)nxs_string_get_substr(file, 0), (struct stat *)buf);
}

int nxs_fs_unlink(nxs_string_t *file)
{

	return unlink((char *)nxs_string_get_substr(file, 0));
}

int nxs_fs_rmdir(nxs_string_t *dir)
{

	return rmdir((char *)nxs_string_get_substr(dir, 0));
}

/*
 * Копирование файла из src в dst
 *
 * bs		- размер блока данных. Если bs == 0, то будет использован стандартный размер блока NXS_FS_CP_BS_DEFAULT.
 * _errno	- указатель на переменну, в которую будет записан вспомогательный код ошибки
 * buf		- указатель на буфер, который будет использоваться при копировании. Если buf == NULL - будет использован "местный" буфер.
 * 				в проектах, где операции копирования файлов являются частыми - лучше использовать "внешний" буфер и
 * передавать
 * 				его в функцию в качестве аргумента, это позволит сэкономить время на выделении памяти.
 */
int nxs_fs_cp(nxs_string_t *src, nxs_string_t *dst, mode_t mode, size_t bs, int *_errno, nxs_buf_t *buf)
{
	nxs_buf_t _buf, *b;
	size_t    _bs;
	ssize_t   rc_in, rc_out;
	int       fd_in, fd_out, ec;

	fd_in  = -1;
	fd_out = -1;

	ec = NXS_FS_E_OK;

	*_errno = 0;

	if(bs == 0) {

		_bs = NXS_FS_CP_BS_DEFAULT;
	}
	else {

		_bs = bs;
	}

	if(buf == NULL) {

		nxs_buf_init(&_buf, _bs);

		b = &_buf;
	}
	else {

		b = buf;
	}

	if((fd_in = nxs_fs_open(src, O_RDONLY)) == -1) {

		*_errno = errno;

		ec = NXS_FS_E_CP_OPEN_SRC;
		goto error;
	}

	if((fd_out = nxs_fs_open(dst, O_RDWR | O_CREAT | O_TRUNC, mode)) == -1) {

		*_errno = errno;

		ec = NXS_FS_E_CP_OPEN_DST;
		goto error;
	}

	while((rc_in = nxs_fs_read_buf(fd_in, b)) > 0) {

		if((rc_out = nxs_fs_write_buf(fd_out, b)) == -1) {

			*_errno = errno;

			ec = NXS_FS_E_CP_WRITE;
			goto error;
		}

		if(rc_in != rc_out) {

			*_errno = EIO;

			ec = NXS_FS_E_CP_WRITE;
			goto error;
		}
	}

	if(rc_in == -1) {

		*_errno = errno;

		ec = NXS_FS_E_CP_WRITE;
		goto error;
	}

error:

	if(fd_in != -1) {

		nxs_fs_close(fd_in);
	}

	if(fd_out != -1) {

		nxs_fs_close(fd_out);
	}

	if(buf == NULL) {

		nxs_buf_free(&_buf);
	}

	return ec;
}

int nxs_fs_check_dir(nxs_string_t *path)
{
	nxs_fs_stat_t file_stat;

	if(path == NULL) {

		return NXS_FS_CHECK_DIR_ERROR_NOENT;
	}

	if(nxs_fs_lstat(path, &file_stat) < 0) {

		return NXS_FS_CHECK_DIR_ERROR_NOENT;
	}
	else {

		if(S_ISDIR(file_stat.st_mode)) {

			return NXS_FS_CHECK_DIR_OK;
		}
	}

	return NXS_FS_CHECK_DIR_ERROR_NOENT;
}

int nxs_fs_get_file_size(nxs_string_t *path, off_t *size)
{
	int           rc;
	nxs_fs_stat_t file_stat;

	if(size == NULL) {

		errno = EFAULT;

		return -1;
	}

	if(path == NULL) {

		*size = 0;

		errno = ENOENT;

		return -1;
	}

	if((rc = nxs_fs_lstat(path, &file_stat)) < 0) {

		*size = 0;

		return rc;
	}
	else {

		*size = file_stat.st_size;
	}

	return 0;
}

int nxs_fs_get_file_type(nxs_string_t *path, mode_t *type)
{
	int           rc;
	nxs_fs_stat_t file_stat;

	if(path == NULL) {

		*type = 0;

		errno = ENOENT;

		return -1;
	}

	if((rc = nxs_fs_lstat(path, &file_stat)) < 0) {

		*type = 0;

		return rc;
	}
	else {

		*type = file_stat.st_mode & S_IFMT;
	}

	return 0;
}

int nxs_fs_getcwd(nxs_string_t *pwd)
{
	u_char c_pwd[2 * FILENAME_MAX + 1];

	if(getcwd((char *)c_pwd, 2 * FILENAME_MAX) == NULL) {

		return -1;
	}

	nxs_string_char_cpy(pwd, 0, c_pwd);

	nxs_string_char_add_char(pwd, (u_char)'/');

	return 0;
}

/*
 * Инициализация с очисткой структуры fc, содержащей информцию по наблюдаемому файлу
 */
void nxs_fs_file_check_upd_init_clean(nxs_process_t *proc, nxs_fs_file_check_t *fc)
{

	if(fc == NULL) {

		return;
	}

	fc->init = NXS_FALSE;
	fc->path = NULL;

	nxs_poll_init(&fc->pll);
}

/*
 * Инициализация структуры fc, содержащей информцию по наблюдаемому файлу вместе с процессом заполнения структуры
 *
 * Возвращаемые значения:
 * * NXS_FS_E_OK		- инициализация прошла успешно
 * * NXS_FS_E_INOTIFY	- ошибка inotify при инициализации (не удалось установить наблюдение за файлом)
 * * NXS_FS_E_PTR_NULL	- указатель "fc" равен NULL
 */
int nxs_fs_file_check_upd_init(nxs_process_t *proc, nxs_fs_file_check_t *fc, nxs_string_t *path)
{

	if(fc == NULL) {

		return NXS_FS_E_PTR_NULL;
	}

	fc->init = NXS_FALSE;
	fc->path = path;

	nxs_poll_init(&fc->pll);

	if((fc->fd = inotify_init()) < 0) {

		nxs_log_write_warn(proc,
		                   "file check upd error: inotify error(init): %s (file path: \"%s\")",
		                   strerror(errno),
		                   nxs_string_str(fc->path));

		return NXS_FS_E_INOTIFY;
	}

	if((fc->wd = inotify_add_watch(fc->fd, (char *)nxs_string_str(fc->path), IN_CLOSE_WRITE | IN_MOVE_SELF | IN_MODIFY)) < 0) {

		nxs_log_write_warn(proc,
		                   "file check upd error: inotify error(add watch): %s (file path: \"%s\")",
		                   strerror(errno),
		                   nxs_string_str(fc->path));

		return NXS_FS_E_INOTIFY;
	}

	nxs_poll_add(&fc->pll, fc->fd, POLLIN);

	fc->init = NXS_TRUE;

	return NXS_FS_E_OK;
}

/*
 * Заполнение структуры fc, содержащей информцию по наблюдаемому файлу.
 * Заполнение производится только для очищенной (после использования) структуры fc или для инициазированной с помощью
 * функции nxs_fs_file_check_upd_init_clean()
 *
 * Возвращаемые значения:
 * * NXS_FS_E_OK		- создание/заполнение структуры "fc" прошло успешно
 * * NXS_FS_E_PTR_NULL	- указатель "fc" == NULL
 * * NXS_FS_E_INIT		- структура "fc" уже была инициализирована
 * * NXS_FS_E_INOTIFY	- ошибка inotify при инициализации (не удалось установить наблюдение за файлом)
 */
int nxs_fs_file_check_upd_create(nxs_process_t *proc, nxs_fs_file_check_t *fc, nxs_string_t *path)
{

	if(fc == NULL) {

		return NXS_FS_E_PTR_NULL;
	}

	if(fc->init == NXS_TRUE) {

		return NXS_FS_E_INIT;
	}

	return nxs_fs_file_check_upd_init(proc, fc, path);
}

/*
 * Очистка структуры fc, содержащей инфорацию по наблюдаемому файлу
 */
void nxs_fs_file_check_upd_free(nxs_process_t *proc, nxs_fs_file_check_t *fc)
{

	if(fc == NULL) {

		return;
	}

	if(fc->init == NXS_TRUE) {

		inotify_rm_watch(fc->fd, fc->wd);
		close(fc->fd);

		fc->path = NULL;
		nxs_poll_free(&fc->pll);

		fc->init = NXS_FALSE;
	}
}

/*
 * Проверка изменения наблюдаемого файла, определённого в структуре fc
 *
 * Файл считается обновлённым если:
 * * Для файла сработал inotify
 * * Для файла не удалось выполнить stat(). Распрастраняется только на ошибку ENOENT.
 *
 * Возвращаемые значения:
 * * NXS_FS_E_OK		- файл не изменился
 * * NXS_FS_E_INIT		- структура fc не инициализирована (ни за каким файлом не ведётся наблюдение)
 * * NXS_FS_E_STAT		- для наблюдаемого файла не удалось выполнить stat(). При этом произошла ошибка отличная от ENOENT
 * * NXS_FS_E_POLL		- ошибка при выполнении poll()
 * * NXS_FS_E_CHANGED	- файл изменился
 */
int nxs_fs_file_check_upd(nxs_process_t *proc, nxs_fs_file_check_t *fc)
{
	int           rc;
	nxs_fs_stat_t f_stat;

	if(fc == NULL) {

		nxs_log_write_warn(proc, "file check upd: file check struct pointer is NULL");

		return NXS_FS_E_PTR_NULL;
	}

	if(fc->init == NXS_FALSE) {

		nxs_log_write_warn(proc, "file check upd: file check struct not initialized (file path: \"%s\")", nxs_string_str(fc->path));

		return NXS_FS_E_INIT;
	}

	if(nxs_fs_lstat(fc->path, &f_stat) < 0) {

		if(errno == ENOENT) {

			nxs_log_write_debug(proc, "file check upd: file was deleted (file path: \"%s\")", nxs_string_str(fc->path));

			return NXS_FS_E_CHANGED;
		}

		nxs_log_write_warn(
		        proc, "file check upd error: stat error: %s (file path: \"%s\")", strerror(errno), nxs_string_str(fc->path));

		return NXS_FS_E_STAT;
	}

	if((rc = nxs_poll_exec(&fc->pll, 0)) == NXS_POLL_E_POLL) {

		nxs_log_write_warn(proc,
		                   "file check upd error: poll error(add watch): %s (file path: \"%s\")",
		                   strerror(errno),
		                   nxs_string_str(fc->path));

		return NXS_FS_E_POLL;
	}

	if(rc == NXS_POLL_E_TIMEOUT) {

		return NXS_FS_E_OK;
	}

	if(nxs_poll_check_events(&fc->pll, fc->fd, POLLIN) == NXS_POLL_EVENTS_TRUE) {

		nxs_log_write_debug(proc, "file check upd: file was modified (file path: \"%s\")", nxs_string_str(fc->path));

		return NXS_FS_E_CHANGED;
	}

	return NXS_FS_E_OK;
}

/* Module internal (static) functions */
