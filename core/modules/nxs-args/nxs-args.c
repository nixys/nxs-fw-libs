// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define	NXS_ARGS_ARGSTR_LEN_CHANK	50

#define	NXS_ARGS_OPT_NOT_FOUND		-1
#define	NXS_ARGS_OPT_NO_ARGUMENT	-2
#define	NXS_ARGS_OPT_UNKNWN_ERR		-3

/* Module typedefs */

typedef struct			nxs_args_argv_s				nxs_args_argv_t;

/* Module declarations */

struct nxs_args_argv_s
{
	nxs_string_t		arg;
	size_t			start;
	size_t			end;
};


/* Module internal (static) functions prototypes */

// clang-format on

static int nxs_args_s_eop(nxs_args_shortopt_t arg_s_el);
static int nxs_args_s_find_opt(nxs_args_shortopt_t *args_s_els, u_char c);
static void nxs_args_s_make_arg_str(nxs_args_shortopt_t *args_s_els, nxs_string_t *arg_str);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

void nxs_args_init(nxs_args_t *  args,
                   nxs_string_t *help_msg,
                   int (*prep_function)(nxs_args_t args, int argc, u_char **argv),
                   int (*post_function)(nxs_args_t args, int argc, u_char **argv),
                   int (*freeargs_function)(nxs_args_t args, int argc, int argind, u_char **argv),
                   nxs_args_shortopt_t *args_s_els,
                   void *               ctx)
{

	nxs_string_init3(&args->help, help_msg);

	args->prep_function     = prep_function;
	args->post_function     = post_function;
	args->freeargs_function = freeargs_function;
	args->args_s_els        = args_s_els;
	args->ctx               = ctx;
}

void nxs_args_free(nxs_args_t *args)
{

	nxs_string_free(&args->help);

	args->prep_function     = NULL;
	args->post_function     = NULL;
	args->freeargs_function = NULL;
	args->args_s_els        = NULL;
	args->ctx               = NULL;
}

void nxs_args_opt_init(nxs_args_shortopt_t **shortopts)
{
	nxs_args_shortopt_t *p;

	p = (nxs_args_shortopt_t *)nxs_malloc(NULL, sizeof(nxs_args_shortopt_t));

	p[0].opt      = 0;
	p[0].have_arg = 0;
	p[0].handler  = NULL;

	*shortopts = p;
}

void nxs_args_opt_free(nxs_args_shortopt_t **shortopts)
{
	nxs_args_shortopt_t *p;

	p = *shortopts;

	p = (nxs_args_shortopt_t *)nxs_free(p);

	*shortopts = p;
}

void nxs_args_opt_add(nxs_args_shortopt_t **shortopts,
                      u_char                opt,
                      int                   is_have_arg,
                      int (*handler)(nxs_args_t args, u_char arg, u_char *optarg))
{
	nxs_args_shortopt_t *p;
	size_t               i;

	p = *shortopts;

	for(i = 0; nxs_args_s_eop(p[i]) == 0; i++)
		;

	p = (nxs_args_shortopt_t *)nxs_realloc(p, sizeof(nxs_args_shortopt_t) * (i + 2));

	p[i].opt      = opt;
	p[i].have_arg = is_have_arg;
	p[i].handler  = handler;

	p[i + 1].opt      = 0;
	p[i + 1].have_arg = 0;
	p[i + 1].handler  = NULL;

	*shortopts = p;
}

ssize_t nxs_args_argv_init(u_char *argv_str, nxs_array_t *argv)
{
	u_char           c, quote;
	size_t           i;
	nxs_args_argv_t *p;
	nxs_bool_t       f, is_arg, is_bslash;

	if(argv_str == NULL || argv == NULL) {

		return -1;
	}

	nxs_array_init(argv, 0, sizeof(nxs_args_argv_t), 1);

	for(quote = 0, is_bslash = NXS_NO, p = NULL, is_arg = NXS_NO, f = NXS_NO, i = 0; argv_str[i] != '\0'; i++) {

		c = argv_str[i];

		switch(is_arg) {

			case NXS_NO:

				if(c == '"') {

					if(quote == 0) {

						quote = c;

						is_arg = NXS_YES;
					}
				}
				else {

					if(c == '\'') {

						if(quote == 0) {

							quote = c;

							is_arg = NXS_YES;
						}
					}
					else {

						if(c == '\\') {

							is_bslash = NXS_YES;

							is_arg = NXS_YES;
						}
						else {

							if(c != ' ' && c != '\t') {

								f = NXS_YES;

								is_arg = NXS_YES;
							}
						}
					}
				}

				if(is_arg == NXS_YES) {

					p = nxs_array_add(argv);

					nxs_string_init2(&p->arg, 0, NXS_STRING_EMPTY_STR);

					if(f == NXS_YES) {

						p->start = i;
						p->end   = i;
					}
					else {

						p->start = i + 1;
						p->end   = i + 1;
					}
				}

				break;

			case NXS_YES:

				if(is_bslash == NXS_YES) {

					/* Если предыдущий символ был '\' - печатаем любой встретившийся в текущей позиции символ */

					f = NXS_YES;

					is_bslash = NXS_NO;

					break;
				}

				if(c == '\\') {

					is_bslash = NXS_YES;

					break;
				}

				if(c == '"' || c == '\'') {

					if(quote == 0) {

						quote = c;
					}
					else {

						if(quote == c) {

							quote = 0;
						}
						else {

							f = NXS_YES;
						}
					}

					break;
				}

				if(c != ' ' && c != '\t') {

					f = NXS_YES;
				}
				else {

					if(quote == 0) {

						is_arg = NXS_NO;

						p = NULL;
					}
					else {

						f = NXS_YES;
					}
				}

				break;
		}

		if(p != NULL && f == NXS_YES) {

			nxs_string_char_add_char(&p->arg, c);

			p->end = i + 1;
		}

		f = NXS_NO;
	}

	if(quote != 0) {

		return -1;
	}

	return (ssize_t)nxs_array_count(argv);
}

void nxs_args_argv_free(nxs_array_t *argv)
{
	nxs_args_argv_t *p;
	size_t           i;

	if(argv == NULL) {

		return;
	}

	for(i = 0; i < nxs_array_count(argv); i++) {

		p = nxs_array_get(argv, i);

		nxs_string_free(&p->arg);
	}

	nxs_array_free(argv);
}

nxs_string_t *nxs_args_argv_get(nxs_array_t *argv, size_t i)
{
	nxs_args_argv_t *p;

	p = nxs_array_get(argv, i);

	if(p == NULL) {

		return NULL;
	}

	return &p->arg;
}

size_t nxs_args_argv_pos(nxs_array_t *argv, size_t pos)
{
	nxs_args_argv_t *p;
	size_t           i;

	for(i = 0; i < nxs_array_count(argv); i++) {

		p = nxs_array_get(argv, i);

		if(p->end >= pos) {

			return i;
		}
	}

	return i;
}

int nxs_args_handler(nxs_process_t *proc, nxs_args_t args, int argc, u_char **argv)
{
	int          arg, arg_index, ret;
	nxs_string_t arg_str;

	ret = NXS_ARGS_CONF_OK;

	nxs_string_init(&arg_str);
	nxs_string_create(&arg_str, NXS_ARGS_ARGSTR_LEN_CHANK, NXS_STRING_EMPTY_STR);

	nxs_args_s_make_arg_str(args.args_s_els, &arg_str);

	/*
	 * Вызов (если определена пре-конфигурационной функции)
	 */

	if(args.prep_function != NULL) {

		switch(args.prep_function(args, argc, argv)) {

			case NXS_ARGS_CONF_ERROR:

				nxs_log_write_error(proc, "command line config fatal error");

				ret = NXS_ARGS_CONF_ERROR;
				goto error;

				break;

			case NXS_ARGS_CONF_OK_EXIT:

				ret = NXS_ARGS_CONF_OK_EXIT;
				goto error;

				break;
		}
	}

	optind = 0;
	opterr = 0;

	while((arg = getopt(argc, (char **)argv, (char *)nxs_string_get_substr(&arg_str, 0))) != -1) {

		/*
		 * arg может принимать следующие значения:
		 * * -1								- достигнут конец списка опций
		 * * символ, отличный от '?' и ':'	- опция определилась корректно
		 * * символ '?' 					- опция не найдена (значени самой опции находится в переменной
		 * "optopt")
		 * * символ ':' 					- в командной строке всего одна опция, которая ожидает аргумента, но
		 * он
		 * не
		 * указан
		 * 											(значени самой опции находится в
		 * переменной
		 * "optopt")
		 */

		switch((arg_index = nxs_args_s_find_opt(args.args_s_els, arg))) {

			case NXS_ARGS_OPT_NOT_FOUND:

				nxs_log_write_error(proc, "command line config error: unknown option '-%c'", optopt);

				ret = NXS_ARGS_CONF_ERROR;
				goto error;

				break;

			case NXS_ARGS_OPT_NO_ARGUMENT:

				nxs_log_write_error(proc, "command line config error: option '-%c' must have an argument", optopt);

				ret = NXS_ARGS_CONF_ERROR;
				goto error;

				break;

			case NXS_ARGS_OPT_UNKNWN_ERR:

				nxs_log_write_error(proc, "command line config error: unknown error");

				ret = NXS_ARGS_CONF_ERROR;
				goto error;

				break;

			default:

				if(args.args_s_els[arg_index].handler != NULL) {

					if(arg == '?' || arg == ':') {

						switch(args.args_s_els[arg_index].handler(args, (u_char)optopt, (u_char *)optarg)) {

							case NXS_ARGS_CONF_ERROR:

								nxs_log_write_error(proc, "command line config fatal error");

								ret = NXS_ARGS_CONF_ERROR;
								goto error;

								break;

							case NXS_ARGS_CONF_OK_EXIT:

								ret = NXS_ARGS_CONF_OK_EXIT;
								goto error;

								break;
						}
					}
					else {

						switch(args.args_s_els[arg_index].handler(args, (u_char)arg, (u_char *)optarg)) {

							case NXS_ARGS_CONF_ERROR:

								nxs_log_write_error(proc, "command line config fatal error");

								ret = NXS_ARGS_CONF_ERROR;
								goto error;

								break;

							case NXS_ARGS_CONF_OK_EXIT:

								ret = NXS_ARGS_CONF_OK_EXIT;
								goto error;

								break;
						}
					}
				}
				else {

					nxs_log_write_error(
					        proc, "command line config internal error: no handler specified for option '%c'", arg);

					ret = NXS_ARGS_CONF_ERROR;
					goto error;
				}

				break;
		}
	}

	/*
	 * Определение и обработка свободных аргументов
	 */

	if(args.freeargs_function != NULL) {

		switch(args.freeargs_function(args, argc, optind, (u_char **)argv)) {

			case NXS_ARGS_CONF_ERROR:

				nxs_log_write_error(proc, "command line config fatal error");

				ret = NXS_ARGS_CONF_ERROR;
				goto error;

				break;

			case NXS_ARGS_CONF_OK_EXIT:

				ret = NXS_ARGS_CONF_OK_EXIT;
				goto error;

				break;
		}
	}

	/*
	 * Вызов (если определена пост-конфигурационной функции)
	 */

	if(args.post_function != NULL) {

		switch(args.post_function(args, argc, argv)) {

			case NXS_ARGS_CONF_ERROR:

				nxs_log_write_error(proc, "command line config fatal error");

				ret = NXS_ARGS_CONF_ERROR;
				goto error;

				break;

			case NXS_ARGS_CONF_OK_EXIT:

				ret = NXS_ARGS_CONF_OK_EXIT;
				goto error;

				break;
		}
	}

error:

	nxs_string_free(&arg_str);

	return ret;
}

int nxs_args_handler2(nxs_process_t *proc, nxs_args_t args, nxs_array_t *argv)
{
	u_char **     argv_str;
	size_t        i, l;
	nxs_string_t *s;
	int           ret;

	l = nxs_array_count(argv);

	argv_str = (u_char **)nxs_malloc(NULL, sizeof(u_char *) * (l + 1));

	argv_str[l] = NULL;

	for(i = 0; i < l; i++) {

		s = nxs_array_get(argv, i);

		argv_str[i] = (u_char *)nxs_malloc(NULL, sizeof(u_char) * nxs_string_len(s) + 1);

		nxs_memcpy(argv_str[i], nxs_string_str(s), nxs_string_len(s) + 1);
	}

	ret = nxs_args_handler(proc, args, (int)l, argv_str);

	for(i = 0; i < l; i++) {

		argv_str[i] = (u_char *)nxs_free(argv_str[i]);
	}

	argv_str = (u_char **)nxs_free(argv_str);

	return ret;
}

/* Module internal (static) functions */

/*
 * Проверка достижения конца списка параметров
 *
 * Возвращаемые значения:
 * 0 - конец списка параметров ещё не достигнут
 * 1 - конец списка параметров достигнут
 */
static int nxs_args_s_eop(nxs_args_shortopt_t arg_s_el)
{

	if(arg_s_el.opt == 0) {

		return 1;
	}

	return 0;
}

static int nxs_args_s_find_opt(nxs_args_shortopt_t *args_s_els, u_char c)
{
	int i;

	for(i = 0; !nxs_args_s_eop(args_s_els[i]); i++) {

		if(args_s_els[i].opt == c) {

			return i;
		}
	}

	switch(c) {

		case '?':

			return NXS_ARGS_OPT_NOT_FOUND;

		case ':':

			return NXS_ARGS_OPT_NO_ARGUMENT;
	}

	return NXS_ARGS_OPT_UNKNWN_ERR;
}

static void nxs_args_s_make_arg_str(nxs_args_shortopt_t *args_s_els, nxs_string_t *arg_str)
{
	int i;

	nxs_string_char_add_char(arg_str, (u_char)':');

	for(i = 0; !nxs_args_s_eop(args_s_els[i]); i++) {

		if(args_s_els[i].opt != (u_char)'?' && args_s_els[i].opt != (u_char)':') {

			nxs_string_char_add_char(arg_str, args_s_els[i].opt);

			if(args_s_els[i].have_arg == NXS_ARGS_HAVE_ARGS_YES) {

				nxs_string_char_add_char(arg_str, (u_char)':');
			}
		}
	}
}
