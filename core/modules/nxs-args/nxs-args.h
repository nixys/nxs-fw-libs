#ifndef _INCLUDE_NXS_ARGS_H
#define _INCLUDE_NXS_ARGS_H

// clang-format off

/** @addtogroup nxs-args
 *
 * @brief Module 'nxs-args' is a basic module providing functions to work with command line arguments.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

#define	NXS_ARGS_HAVE_ARGS_NO		0
#define	NXS_ARGS_HAVE_ARGS_YES		1

#define NXS_ARGS_CONF_OK		0
#define NXS_ARGS_CONF_ERROR		1
#define NXS_ARGS_CONF_OK_EXIT		2

#define NXS_ARGS_NULL		 	{0, 0, NULL}

struct nxs_args_shortopt_s
{
	u_char			opt;
	int			have_arg;
	int			(*handler)(nxs_args_t args, u_char arg, u_char *optarg);
};

struct nxs_args_s
{
	nxs_string_t		help;
	int			(*prep_function)(nxs_args_t args, int argc, u_char **argv);
	int			(*post_function)(nxs_args_t args, int argc, u_char **argv);
	int			(*freeargs_function)(nxs_args_t args, int argc, int argind, u_char **argv);
	nxs_args_shortopt_t	*args_s_els;
	void			*ctx;
};

#define	nxs_args_get_ctx(args) \
			args.ctx

void				nxs_args_init				(nxs_args_t *args, nxs_string_t *help_msg, int (*prep_function)(nxs_args_t args, int argc, u_char **argv), int (*post_function)(nxs_args_t args, int argc, u_char **argv), int (*freeargs_function)(nxs_args_t args, int argc, int argind, u_char **argv), nxs_args_shortopt_t *args_s_els, void *ctx);
void				nxs_args_free				(nxs_args_t *args);

void				nxs_args_opt_init			(nxs_args_shortopt_t **shortopts);
void				nxs_args_opt_free			(nxs_args_shortopt_t **shortopts);
void				nxs_args_opt_add			(nxs_args_shortopt_t **shortopts, u_char opt, int is_have_arg, int (*handler)(nxs_args_t args, u_char arg, u_char *optarg));

ssize_t				nxs_args_argv_init			(u_char *argv_str, nxs_array_t *argv);
void				nxs_args_argv_free			(nxs_array_t *argv);
nxs_string_t			*nxs_args_argv_get			(nxs_array_t *argv, size_t i);
size_t				nxs_args_argv_pos			(nxs_array_t *argv, size_t pos);

int				nxs_args_handler			(nxs_process_t *proc, nxs_args_t args, int argc, u_char **argv);
int				nxs_args_handler2			(nxs_process_t *proc, nxs_args_t args, nxs_array_t *argv);

/** @} */ // end of nxs-args
#endif /* _INCLUDE_NXS_ARGS_H */
