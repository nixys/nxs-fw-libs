#ifndef _INCLUDE_NXS_FS_H
#define _INCLUDE_NXS_FS_H

#include <nxs-core/nxs-core.h>

typedef			DIR			nxs_fs_dir_t;
typedef struct		stat			nxs_fs_stat_t;
typedef			FILE			nxs_fs_file_t;

#define NXS_FS_E_OK				0
#define NXS_FS_E_INIT				1
#define NXS_FS_E_INOTIFY			2
#define NXS_FS_E_STAT				3
#define NXS_FS_E_POLL				4
#define NXS_FS_E_CHANGED			5
#define NXS_FS_E_PTR_NULL			6
#define NXS_FS_E_CP_OPEN_SRC			7
#define NXS_FS_E_CP_OPEN_DST			8
#define NXS_FS_E_CP_READ			9
#define NXS_FS_E_CP_WRITE			10

#define NXS_FS_E_FILE_BUF			-1
#define NXS_FS_E_FILE_OPEN			-2
#define NXS_FS_E_FILE_WRITE			-3
#define NXS_FS_E_FILE_READ			-4
#define NXS_FS_E_FILE_CLOSE			-5

#define NXS_FS_MODE_SET_ALL			0
#define NXS_FS_MODE_SET_SKEEP_DOT		1

#define	NXS_FS_D_NAME_SIZE			256

#define NXS_FS_CHECK_DIR_OK			0
#define NXS_FS_CHECK_DIR_ERROR_NOENT		1

#define NXS_FS_CP_BS_DEFAULT			4096

struct nxs_fs_dirent_s
{

	ino_t			d_ino;		/* inode number */
	off_t			d_off;		/* offset to the next dirent */
	unsigned short		d_reclen;	/* length of this record */
	unsigned char		d_type;		/* type of file; not supported by all file system types */
	nxs_string_t		*d_name;	/* filename */
};

struct nxs_fs_file_check_s
{
	nxs_bool_t		init;
	int			fd;
	int			wd;
	nxs_string_t		*path;
	nxs_poll_t		pll;
};

#define nxs_fs_open(path, oflag...) \
			open((char *)nxs_string_get_substr(path, NXS_STRING_NO_OFFSET), oflag)

#define nxs_fs_close(fd) \
			close(fd)

#define nxs_fs_write(fd, char_buf, n) \
			write(fd, char_buf, n)

#define nxs_fs_read(fd, char_buf, n) \
			read(fd, char_buf, n)

nxs_fs_dir_t				*nxs_fs_opendir					(nxs_string_t *path);
int					nxs_fs_closedir					(nxs_fs_dir_t *dir);
nxs_fs_file_t				*nxs_fs_fopen					(nxs_string_t *path, u_char *mode);
int					nxs_fs_fclose					(nxs_fs_file_t *fp);

int					nxs_fs_mkdir					(nxs_string_t *path, mode_t mode);

ssize_t					nxs_fs_write_buf				(int fd, nxs_buf_t *buf);
ssize_t					nxs_fs_write_buf_n				(int fd, nxs_buf_t *buf, size_t n);
ssize_t					nxs_fs_read_buf					(int fd, nxs_buf_t *buf);
ssize_t					nxs_fs_read_buf_n				(int fd, nxs_buf_t *buf, size_t n);

ssize_t					nxs_fs_write_file				(nxs_string_t *path, nxs_buf_t *buf, mode_t mode);
ssize_t					nxs_fs_write_file_n				(nxs_string_t *path, nxs_buf_t *buf, size_t n, mode_t mode);
ssize_t					nxs_fs_append_file				(nxs_string_t *path, nxs_buf_t *buf, mode_t mode);
ssize_t					nxs_fs_append_file_n				(nxs_string_t *path, nxs_buf_t *buf, size_t n, mode_t mode);
ssize_t					nxs_fs_read_file_to_buf				(nxs_string_t *path, nxs_buf_t *buf);
ssize_t					nxs_fs_read_file_to_buf_n			(nxs_string_t *path, nxs_buf_t *buf, size_t n);
ssize_t					nxs_fs_read_file_to_str				(nxs_string_t *path, nxs_string_t *str);

void					nxs_fs_dirent_init				(nxs_fs_dirent_t *entry);
void					nxs_fs_dirent_free				(nxs_fs_dirent_t *entry);
int					nxs_fs_readdir					(nxs_fs_dir_t *dir, nxs_fs_dirent_t *entry, int mode_set);
int					nxs_fs_stat							(nxs_string_t *file, nxs_fs_stat_t *buf);
int					nxs_fs_lstat					(nxs_string_t *file, nxs_fs_stat_t *buf);
int					nxs_fs_unlink					(nxs_string_t *file);
int					nxs_fs_rmdir					(nxs_string_t *dir);
int					nxs_fs_cp							(nxs_string_t *src, nxs_string_t *dst, mode_t mode, size_t bs, int *_errno, nxs_buf_t *buf);
int					nxs_fs_check_dir				(nxs_string_t *path);
int					nxs_fs_get_file_size				(nxs_string_t *path, off_t *size);
int					nxs_fs_get_file_type				(nxs_string_t *path, mode_t *type);

int					nxs_fs_getcwd					(nxs_string_t *pwd);

void					nxs_fs_file_check_upd_init_clean		(nxs_process_t *proc, nxs_fs_file_check_t *fc);
int					nxs_fs_file_check_upd_init			(nxs_process_t *proc, nxs_fs_file_check_t *fc, nxs_string_t *path);
int					nxs_fs_file_check_upd_create			(nxs_process_t *proc, nxs_fs_file_check_t *fc, nxs_string_t *path);
void					nxs_fs_file_check_upd_free			(nxs_process_t *proc, nxs_fs_file_check_t *fc);
int					nxs_fs_file_check_upd				(nxs_process_t *proc, nxs_fs_file_check_t *fc);

#endif /* _INCLUDE_NXS_FS_H */
