#ifndef _INCLUDE_NXS_METADATA_H
#define _INCLUDE_NXS_METADATA_H

#include <nxs-core/nxs-core.h>

#define	NXS_METADATA_V_NONE			0
#define	NXS_METADATA_V1				1
#define	NXS_METADATA_V2				2
#define	NXS_METADATA_V3				3

#define	NXS_METADATA_E_OK			0
#define	NXS_METADATA_E_VER			1
#define	NXS_METADATA_E_PTR			2
#define	NXS_METADATA_E_STAT			3
#define	NXS_METADATA_E_SIZE			4
#define	NXS_METADATA_E_BUF_SIZE		5
#define	NXS_METADATA_E_NOENT		6
#define	NXS_METADATA_E_LOOKUP		7

struct nxs_metadata_s
{
	nxs_metadata_v_t		version;
	void					*m_data;
};

nxs_metadata_t					*nxs_metadata_malloc				(nxs_metadata_v_t version);
nxs_metadata_t					*nxs_metadata_malloc_clear			(void);
void							*nxs_metadata_v_malloc				(nxs_metadata_v_t version);
nxs_metadata_t					*nxs_metadata_destroy				(nxs_metadata_t *md);
void							*nxs_metadata_v_destroy				(void *md_v, nxs_metadata_v_t version);
int								nxs_metadata_init					(nxs_metadata_t *md, nxs_metadata_v_t version);
int								nxs_metadata_init_clear				(nxs_metadata_t *md);
void							nxs_metadata_free					(nxs_metadata_t *md);
void							nxs_metadata_clear					(nxs_metadata_t *md);
int								nxs_metadata_fill					(nxs_metadata_t *md, nxs_metadata_v_t version, nxs_string_t *path);
int								nxs_metadata_to_buf					(nxs_metadata_t *md, size_t offset, nxs_buf_t *buf, size_t *msize);
int								nxs_metadata_from_buf				(nxs_metadata_t *md, size_t offset, nxs_buf_t *buf, size_t *msize);

int								nxs_metadata_set_name				(nxs_metadata_t *md, nxs_string_t *name);
int								nxs_metadata_set_uname				(nxs_metadata_t *md, nxs_string_t *uname);
int								nxs_metadata_set_gname				(nxs_metadata_t *md, nxs_string_t *gname);
int								nxs_metadata_set_perm				(nxs_metadata_t *md, mode_t mode, uid_t uid, gid_t gid);
int								nxs_metadata_set_time				(nxs_metadata_t *md, time_t atime, time_t mtime, time_t ctime);
int								nxs_metadata_set_size				(nxs_metadata_t *md, off_t size);

nxs_metadata_v_t				nxs_metadata_get_version			(nxs_metadata_t *md);
nxs_string_t					*nxs_metadata_get_name				(nxs_metadata_t *md);
nxs_string_t					*nxs_metadata_get_uname				(nxs_metadata_t *md);
nxs_string_t					*nxs_metadata_get_gname				(nxs_metadata_t *md);
int								nxs_metadata_get_mode				(nxs_metadata_t *md, mode_t *mode);
int								nxs_metadata_get_perm				(nxs_metadata_t *md, mode_t *perm);
int								nxs_metadata_get_uid				(nxs_metadata_t *md, uid_t *uid);
int								nxs_metadata_get_gid				(nxs_metadata_t *md, gid_t *gid);
int								nxs_metadata_get_size				(nxs_metadata_t *md, off_t *size);
int								nxs_metadata_get_atime				(nxs_metadata_t *md, time_t *atime);
int								nxs_metadata_get_mtime				(nxs_metadata_t *md, time_t *mtime);
int								nxs_metadata_get_ctime				(nxs_metadata_t *md, time_t *ctime);

u_char							*nxs_metadata_error					(int error);

void							nxs_metadata_print					(nxs_process_t *proc, nxs_metadata_t *md);


#endif /* _INCLUDE_NXS_METADATA_H */
