#ifndef _INCLUDE_NXS_JSON_H
#define _INCLUDE_NXS_JSON_H

#include <nxs-core/nxs-core.h>

#define NXS_JSON_E_OK					0
#define NXS_JSON_E_NULL_PTR				1
#define NXS_JSON_E_NOT_NULL_PTR			2
#define NXS_JSON_E_READ_JSON_FILE		3
#define NXS_JSON_E_READ_JSON_MEM		4
#define NXS_JSON_E_PARSE				5

#define NXS_JSON_PRINT_CONSOLE			0
#define NXS_JSON_PRINT_LOG				1

#define NXS_JSON_INT_FORMAT				JSON_INTEGER_FORMAT

typedef					json_int_t							nxs_json_int_t;

enum nxs_json_type_e {
	NXS_JSON_TYPE_OBJECT,
	NXS_JSON_TYPE_ARRAY,
	NXS_JSON_TYPE_STRING,
	NXS_JSON_TYPE_INTEGER,
	NXS_JSON_TYPE_REAL,
	NXS_JSON_TYPE_TRUE,
	NXS_JSON_TYPE_FALSE,
	NXS_JSON_TYPE_NULL
};

struct nxs_json_s {
	nxs_json_type_t		type;
	nxs_string_t		key;
	void				*value;
};


void					nxs_json_init						(nxs_json_t **json);
void					nxs_json_free						(nxs_json_t **json);

int						nxs_json_read_file					(nxs_process_t *proc, nxs_json_t **json, nxs_string_t *filename, size_t flags);
int						nxs_json_read_mem					(nxs_process_t *proc, nxs_json_t **json, nxs_buf_t *buf, size_t flags);

nxs_json_t				*nxs_json_child_get_by_index		(nxs_json_t *obj, size_t index);
nxs_json_t				*nxs_json_child_get_by_key			(nxs_json_t *obj, u_char *key);
size_t					nxs_json_child_get_count			(nxs_json_t *obj);

nxs_string_t			*nxs_json_get_key					(nxs_json_t *obj);

nxs_string_t			*nxs_json_string_val				(nxs_json_t *obj);
nxs_json_int_t			nxs_json_integer_val				(nxs_json_t *obj);
double					nxs_json_real_val					(nxs_json_t *obj);
nxs_bool_t				nxs_json_bool_val					(nxs_json_t *obj);

nxs_json_type_t			nxs_json_type_get					(nxs_json_t *obj);
u_char					*nxs_json_type_string				(nxs_json_t *obj);

nxs_json_t				*nxs_json_clone						(nxs_json_t *obj);

void					nxs_json_print						(nxs_process_t *proc, int log_dst, nxs_json_t *obj);


#endif /* _INCLUDE_NXS_JSON_H */