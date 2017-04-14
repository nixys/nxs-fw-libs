#ifndef _INCLUDE_NXS_XML_H
#define _INCLUDE_NXS_XML_H

#include <nxs-core/nxs-core.h>

#define NXS_XML_E_OK					0
#define NXS_XML_E_NULL_PTR				1
#define NXS_XML_E_READ_XML_FILE			2
#define NXS_XML_E_READ_XML_MEM			3
#define NXS_XML_E_GET_ROOT				4

#define NXS_XML_C_IND_INIT				-1

struct nxs_xml_attr_s
{
	nxs_string_t	attr_name;
	nxs_string_t	attr_value;
};

struct nxs_xml_el_s
{
	nxs_string_t	name;
	nxs_string_t	content;

	nxs_array_t		childs;		/* Массив дочерних элеменетов типа nxs_xml_el_t */

	nxs_array_t		attrs;		/* Массив атрибутов (тип элементов nxs_xml_attr_s)*/
};

struct nxs_xml_s
{
	nxs_string_t	version;

	nxs_xml_el_t	*root;		/* Корневой элемент */
};

nxs_xml_t				*nxs_xml_malloc					(void);
nxs_xml_t				*nxs_xml_destroy				(nxs_xml_t *xml);

void					nxs_xml_init					(nxs_xml_t *xml);
void					nxs_xml_free					(nxs_xml_t *xml);

void					nxs_xml_clear					(nxs_xml_t *xml);

int						nxs_xml_read_file				(nxs_xml_t *xml, nxs_string_t *filename, nxs_string_t *encoding, int options);
int						nxs_xml_read_mem				(nxs_xml_t *xml, nxs_buf_t *buf, nxs_string_t *url, nxs_string_t *encoding, int options);

nxs_xml_el_t			*nxs_xml_root_get				(nxs_xml_t *xml);

nxs_array_t				*nxs_xml_element_get			(nxs_xml_el_t *xml_el, u_char *el_name, ...);
nxs_xml_el_t 			*nxs_xml_element_get_first		(nxs_xml_el_t *xml_el, u_char *el_name, ...);
nxs_string_t			*nxs_xml_element_get_name		(nxs_xml_el_t *xml_el);
nxs_string_t			*nxs_xml_element_get_content	(nxs_xml_el_t *xml_el);

nxs_string_t			*nxs_xml_attr_get_name			(nxs_xml_attr_t *attr);
nxs_string_t			*nxs_xml_attr_get_value			(nxs_xml_attr_t *attr);

nxs_xml_el_t			*nxs_xml_element_res_get		(nxs_array_t *res, size_t index);
nxs_xml_attr_t			*nxs_xml_element_res_get_attr	(nxs_xml_el_t *xml_el, nxs_string_t attr);
nxs_array_t				*nxs_xml_element_res_free		(nxs_array_t *res);

void					nxs_xml_print					(nxs_process_t *proc, nxs_xml_el_t *xml_el);

#endif /* _INCLUDE_NXS_XML_H */
