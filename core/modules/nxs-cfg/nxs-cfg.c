// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define NXS_CFG_CONF_READ_OK			0
#define NXS_CFG_CONF_READ_ERROR			1
#define NXS_CFG_CONF_READ_EOF			2

#define	NXS_CFG_READ_STATUS_WAIT_OPT		0
#define	NXS_CFG_READ_STATUS_B_COMMENT		1
#define	NXS_CFG_READ_STATUS_READ_OPT		2
#define	NXS_CFG_READ_STATUS_WAIT_SEP		3
#define	NXS_CFG_READ_STATUS_READ_SEP		4
#define	NXS_CFG_READ_STATUS_WAIT_VAL		5
#define	NXS_CFG_READ_STATUS_READ_VAL		6
#define	NXS_CFG_READ_STATUS_WAIT_EOL		7
#define	NXS_CFG_READ_STATUS_E_COMMENT		8
#define	NXS_CFG_READ_STATUS_EOL			9
#define	NXS_CFG_READ_STATUS_READ_QUETED_VAL	10

#define	NXS_CFG_OPT_VAL_CHANK_SIZE		10

/* Module typedefs */

typedef struct			nxs_cfg_parse_info_s			nxs_cfg_parse_info_t;

/* Module declarations */

struct nxs_cfg_parse_info_s
{
	int			line;
	int			pos;
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_cfg_required_list_fill(nxs_list_t *required_list, nxs_cfg_par_t *cfg_par);
static void nxs_cfg_required_list_del(nxs_list_t *required_list, nxs_cfg_par_t cfg_par, int option_id);
static int nxs_cfg_required_list_check(nxs_process_t *proc, nxs_list_t *required_list, nxs_cfg_par_t *cfg_par);
static void nxs_cfg_definition_list_add(nxs_list_t *definition_list, int option_id);
static int nxs_cfg_definition_list_check(nxs_list_t *definition_list, nxs_cfg_par_t *cfg_par, int option_id);
static int nxs_cfg_process_option(nxs_process_t *      proc,
                                  nxs_cfg_t            cfg,
                                  nxs_cfg_parse_info_t parse_info,
                                  nxs_list_t *         required_list,
                                  nxs_list_t *         definition_list,
                                  nxs_string_t *       opt,
                                  nxs_string_t *       value);
static int nxs_cfg_eop(nxs_cfg_par_t cfg);
static int nxs_cfg_check_delimiter(unsigned char c);
static int nxs_cfg_check_charset(unsigned char c, unsigned char *charset_srt);
static void nxs_cfg_optval_add_char(nxs_string_t *str, unsigned char c);
static int nxs_cfg_read_char(int fd, nxs_cfg_parse_info_t *parse_info, unsigned char *c);
static int nxs_cfg_read_next(int fd, nxs_cfg_parse_info_t *parse_info, nxs_string_t *opt, nxs_string_t *value);

static int nxs_cfg_type_handler_int(nxs_process_t *proc, nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par);
static int nxs_cfg_type_handler_string(nxs_process_t *proc, nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par);
static int nxs_cfg_type_handler_num(nxs_process_t *proc, nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par);
static int
        nxs_cfg_type_handler_list(nxs_process_t *proc, nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par, nxs_bool_t distinct);
static int nxs_cfg_type_handler_list_num(nxs_process_t *proc,
                                         nxs_string_t * opt,
                                         nxs_string_t * val,
                                         nxs_cfg_par_t *cfg_par,
                                         nxs_bool_t     distinct);

// clang-format off

/* Module initializations */

static unsigned char nxs_cfg_char_delimiter[]	= {' ', '\t', '\0'};
static unsigned char nxs_cfg_char_eol		= '\n';
static unsigned char nxs_cfg_char_sep		= '=';
static unsigned char nxs_cfg_char_comment	= '#';
static unsigned char nxs_cfg_char_quote		= '"';
static unsigned char nxs_cfg_char_opt_set[]	= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890/_.-:*";
static unsigned char nxs_cfg_char_val_set[]	= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZабвгдеёжзийклмнопрстуфхцчшщьыъэюяАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЬЫЪЭЮЯ1234567890|\\/~`!@$%^&*:;.,()-_=+№?'[]{}<>";

/* Module global functions */

// clang-format on

/*
 * Чтение конфигурационного файла "cfg.cfg_path"
 */
int nxs_cfg_read(nxs_process_t *proc, nxs_cfg_t cfg)
{
	int                  fd, rc, ret;
	nxs_string_t         opt, value;
	nxs_cfg_parse_info_t parse_info = {1, 0};
	nxs_list_t           required_list, definition_list;

	if((fd = nxs_fs_open(&cfg.cfg_path, O_RDONLY)) == -1) {

		nxs_log_write_debug(
		        proc, "config error: can't open config file: %s (config file: %s)", strerror(errno), nxs_string_str(&cfg.cfg_path));

		return NXS_CFG_CONF_ERROR;
	}

	ret = NXS_CFG_CONF_OK;

	nxs_string_init(&opt);
	nxs_string_init(&value);

	nxs_cfg_required_list_fill(&required_list, cfg.cfg_par);

	nxs_list_init(&definition_list, sizeof(int));

	/*
	 * Если задано - производим вызов начальной функции, которая произведёт подготовительные действия
	 */
	if(cfg.prep_function != NULL) {

		if(cfg.prep_function(cfg) != NXS_CFG_CONF_OK) {

			nxs_log_write_error(proc, "fatal config error");

			ret = NXS_CFG_CONF_ERROR;
			goto error;
		}
	}

	/*
	 * Обработка (чтение) параметров конфигурационного файла
	 */

	while((rc = nxs_cfg_read_next(fd, &parse_info, &opt, &value)) == NXS_CFG_CONF_READ_OK) {

		if(nxs_cfg_process_option(proc, cfg, parse_info, &required_list, &definition_list, &opt, &value) != NXS_CFG_CONF_OK) {

			ret = NXS_CFG_CONF_ERROR;
			goto error;
		}
	}

	if(rc == NXS_CFG_CONF_READ_ERROR) {

		nxs_log_write_error(
		        proc, "fatal config error: unexpected character (line: %d, pos: %d)", parse_info.line, parse_info.pos - 1);

		ret = NXS_CFG_CONF_ERROR;
		goto error;
	}
	else {

		if(nxs_string_len(&opt) > 0) {

			if(nxs_cfg_process_option(proc, cfg, parse_info, &required_list, &definition_list, &opt, &value) !=
			   NXS_CFG_CONF_OK) {

				ret = NXS_CFG_CONF_ERROR;
				goto error;
			}
		}
	}

	/*
	 * Проверка на определение обязательных опций
	 */
	if(nxs_cfg_required_list_check(proc, &required_list, cfg.cfg_par) != NXS_CFG_CONF_OK) {

		nxs_log_write_error(proc, "fatal config error");

		ret = NXS_CFG_CONF_ERROR;
		goto error;
	}

	/*
	 * Если задано - производим вызов постконфигурационной функции, которая произведёт завершающие (проверочные) действия
	 */
	if(cfg.post_function != NULL) {

		if(cfg.post_function(cfg) != NXS_CFG_CONF_OK) {

			nxs_log_write_error(proc, "fatal config error");

			ret = NXS_CFG_CONF_ERROR;
			goto error;
		}
	}

error:

	nxs_fs_close(fd);

	nxs_string_free(&opt);
	nxs_string_free(&value);

	nxs_list_free(&required_list);
	nxs_list_free(&definition_list);

	return ret;
}

/* Module internal (static) functions */

/*
 * Заполнение списка номерами параметров, которые являются обязательными
 */
static void nxs_cfg_required_list_fill(nxs_list_t *required_list, nxs_cfg_par_t *cfg_par)
{
	int *index, i;

	nxs_list_init(required_list, sizeof(int));

	for(i = 0; !nxs_cfg_eop(cfg_par[i]); i++) {

		if(cfg_par[i].required == NXS_CFG_REQUIRED_YES) {

			index = nxs_list_add_tail(required_list);

			*index = i;
		}
	}
}

/*
 * Удаление из списка обязательных опций той опции, которая была определена
 */
static void nxs_cfg_required_list_del(nxs_list_t *required_list, nxs_cfg_par_t cfg_par, int option_id)
{
	int *index;

	if(cfg_par.required == NXS_CFG_REQUIRED_YES) {

		for(index = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, required_list); index != NULL;
		    index = nxs_list_ptr_next(required_list)) {

			if(*index == option_id) {

				nxs_list_del(required_list, NXS_LIST_MOVE_NEXT);

				break;
			}
		}
	}
}

/*
 * Проверка остались ли в списке обязательные параметры, которые не были определены.
 * Если остались - выводим эти параметры.
 *
 * Возвращаемые значения:
 * NXS_CFG_CONF_OK		- обязательных параметров в списке нет
 * NXS_CFG_CONF_ERROR	- обязательные параметры в списке остались
 */
static int nxs_cfg_required_list_check(nxs_process_t *proc, nxs_list_t *required_list, nxs_cfg_par_t *cfg_par)
{
	int *index;

	if(nxs_list_count(required_list) == 0) {

		return NXS_CFG_CONF_OK;
	}

	for(index = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, required_list); index != NULL; index = nxs_list_ptr_next(required_list)) {

		nxs_log_write_error(proc, "required option was not defined (option: \"%s\")", nxs_string_str(&cfg_par[*index].name));
	}

	return NXS_CFG_CONF_ERROR;
}

/*
 * Добавление в список определённых опций новую опцию
 */
static void nxs_cfg_definition_list_add(nxs_list_t *definition_list, int option_id)
{
	int *index;

	index = nxs_list_add_tail(definition_list);

	*index = option_id;
}

/*
 * Проверка наличия в списке определённых опций наличия добавляемой опции.
 * Если добавляемая опция присутствует в списке и её не разрешено определять дважды - будет возвращена ошибка
 *
 * Возвращаемые значения:
 * NXS_CFG_CONF_OK		- опция либо отсутствует в списке, либо её разрешено определять повторно
 * NXS_CFG_CONF_ERROR	- опцию не разрешено определять повторно и она присутствует в списке
 */
static int nxs_cfg_definition_list_check(nxs_list_t *definition_list, nxs_cfg_par_t *cfg_par, int option_id)
{
	int *index;

	if(cfg_par[option_id].twice_def == NXS_CFG_TWICE_DEFINITION_YES) {

		return NXS_CFG_CONF_OK;
	}

	for(index = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, definition_list); index != NULL; index = nxs_list_ptr_next(definition_list)) {

		if(*index == option_id) {

			return NXS_CFG_CONF_ERROR;
		}
	}

	return NXS_CFG_CONF_OK;
}

/*
 * Обработка конкретной опции
 */
static int nxs_cfg_process_option(nxs_process_t *      proc,
                                  nxs_cfg_t            cfg,
                                  nxs_cfg_parse_info_t parse_info,
                                  nxs_list_t *         required_list,
                                  nxs_list_t *         definition_list,
                                  nxs_string_t *       opt,
                                  nxs_string_t *       value)
{
	int flag_found_opt, i;

	for(flag_found_opt = 0, i = 0; !nxs_cfg_eop(cfg.cfg_par[i]); i++) {

		if(nxs_string_cmp(&cfg.cfg_par[i].name, NXS_STRING_NO_OFFSET, opt, NXS_STRING_NO_OFFSET) == NXS_STRING_CMP_EQ) {

			flag_found_opt = 1;

			break;
		}
	}

	if(flag_found_opt == 0) {

		nxs_log_write_error(proc,
		                    "fatal config error: unknown option at line %d (option name: \"%s\")",
		                    parse_info.line - 1,
		                    nxs_string_str(opt));

		return NXS_CFG_CONF_ERROR;
	}
	else {

		/*
		 * Проверка наличия этой опции в списке ранее определённых опций
		 */
		if(nxs_cfg_definition_list_check(definition_list, cfg.cfg_par, i) != NXS_CFG_CONF_OK) {

			nxs_log_write_error(proc,
			                    "fatal config error: double definition option at line %d (option name: \"%s\")",
			                    parse_info.line - 1,
			                    nxs_string_str(opt));

			return NXS_CFG_CONF_ERROR;
		}

		if(cfg.cfg_par[i].init_function != NULL) {

			if(cfg.cfg_par[i].init_function(opt, value, &cfg.cfg_par[i]) != NXS_CFG_CONF_OK) {

				nxs_log_write_error(proc, "fatal config error at line %d", parse_info.line - 1);

				return NXS_CFG_CONF_ERROR;
			}
		}
		else {

			switch(cfg.cfg_par[i].type) {

				case NXS_CFG_TYPE_VOID:

					nxs_log_write_error(proc,
					                    "fatal config error: options with type 'void' must have handlers (option name: "
					                    "\"%s\", line: %d)",
					                    nxs_string_str(opt),
					                    parse_info.line - 1);

					return NXS_CFG_CONF_ERROR;

					break;

				case NXS_CFG_TYPE_INT:

					if(nxs_cfg_type_handler_int(proc, opt, value, &cfg.cfg_par[i]) != NXS_CFG_CONF_OK) {

						nxs_log_write_error(proc, "fatal config error at line %d", parse_info.line - 1);

						return NXS_CFG_CONF_ERROR;
					}

					break;

				case NXS_CFG_TYPE_STRING:

					if(nxs_cfg_type_handler_string(proc, opt, value, &cfg.cfg_par[i]) != NXS_CFG_CONF_OK) {

						nxs_log_write_error(proc, "fatal config error at line %d", parse_info.line - 1);

						return NXS_CFG_CONF_ERROR;
					}

					break;

				case NXS_CFG_TYPE_NUM:

					if(nxs_cfg_type_handler_num(proc, opt, value, &cfg.cfg_par[i]) != NXS_CFG_CONF_OK) {

						nxs_log_write_error(proc, "fatal config error at line %d", parse_info.line - 1);

						return NXS_CFG_CONF_ERROR;
					}

					break;

				case NXS_CFG_TYPE_LIST:

					if(nxs_cfg_type_handler_list(proc, opt, value, &cfg.cfg_par[i], NXS_NO) != NXS_CFG_CONF_OK) {

						nxs_log_write_error(proc, "fatal config error at line %d", parse_info.line - 1);

						return NXS_CFG_CONF_ERROR;
					}

					break;

				case NXS_CFG_TYPE_LIST_DISTINCT:

					if(nxs_cfg_type_handler_list(proc, opt, value, &cfg.cfg_par[i], NXS_YES) != NXS_CFG_CONF_OK) {

						nxs_log_write_error(proc, "fatal config error at line %d", parse_info.line - 1);

						return NXS_CFG_CONF_ERROR;
					}

					break;

				case NXS_CFG_TYPE_LIST_NUM:

					if(nxs_cfg_type_handler_list_num(proc, opt, value, &cfg.cfg_par[i], NXS_NO) != NXS_CFG_CONF_OK) {

						nxs_log_write_error(proc, "fatal config error at line %d", parse_info.line - 1);

						return NXS_CFG_CONF_ERROR;
					}

					break;

				case NXS_CFG_TYPE_LIST_NUM_DISTINCT:

					if(nxs_cfg_type_handler_list_num(proc, opt, value, &cfg.cfg_par[i], NXS_YES) != NXS_CFG_CONF_OK) {

						nxs_log_write_error(proc, "fatal config error at line %d", parse_info.line - 1);

						return NXS_CFG_CONF_ERROR;
					}

					break;

				default:

					nxs_log_write_error(proc,
					                    "fatal config error: unknown type of option (option name: \"%s\", line: %d)",
					                    nxs_string_str(opt),
					                    parse_info.line - 1);

					return NXS_CFG_CONF_ERROR;

					break;
			}
		}

		/*
		 * Добавление опции в список опции, которая была только что добавлена
		 */
		nxs_cfg_definition_list_add(definition_list, i);

		/*
		 * Проверка является ли данная опция обязательной и если да - удаление её из списка, который содержит обязательные для
		 * определения опции
		 */
		nxs_cfg_required_list_del(required_list, cfg.cfg_par[i], i);
	}

	return NXS_CFG_CONF_OK;
}

/*
 * Проверка достижения конца списка параметров
 *
 * Возвращаемые значения:
 * 0 - конец списка параметров ещё не достигнут
 * 1 - конец списка параметров достигнут
 */
static int nxs_cfg_eop(nxs_cfg_par_t cfg)
{

	if(nxs_string_str(&cfg.name) == NULL) {

		return 1;
	}

	return 0;
}

/*
 * Проверка символа на соответствие множеству символов-разделителей
 *
 * Возвращаемые значения:
 * 0 - символ не соответствует никакому из символов-разделителей
 * 1 - символ соответствует одному из символов-разделителей
 */
static int nxs_cfg_check_delimiter(unsigned char c)
{
	int i;

	for(i = 0; nxs_cfg_char_delimiter[i] != '\0'; i++) {

		if(c == nxs_cfg_char_delimiter[i]) {

			return 1;
		}
	}

	return 0;
}

/*
 * Проверка символа на соответствие множеству символов charset_srt (где charset_srt - это множество допустимых символов)
 *
 * Возвращаемые значения:
 * 0 - символ не соответствует никакому из множества символов charset_srt
 * 1 - символ соответствует одному из множества символов charset_srt
 */
static int nxs_cfg_check_charset(unsigned char c, unsigned char *charset_srt)
{
	int i;

	for(i = 0; charset_srt[i] != '\0'; i++) {

		if(c == charset_srt[i]) {

			return 1;
		}
	}

	return 0;
}

/*
 * Добавить символ "c" к строке str.
 * Если строка str не достаточного размера, чтобы вместить новый символ - её размер будет увеличен (шаг увеличения:
 * NXS_CFG_OPT_VAL_CHANK_SIZE)
 */
static void nxs_cfg_optval_add_char(nxs_string_t *str, unsigned char c)
{

	while(nxs_string_char_add_char(str, (u_char)c) == NXS_STRING_ERROR_DST_SIZE) {

		nxs_string_resize(str, nxs_string_size(str) + NXS_CFG_OPT_VAL_CHANK_SIZE);
	}
}

/*
 * Считать следующий символ из конфигурационного файла.
 * При этом значения полей структуры, содержащей информацию по текущим позициям чтения файла будет изменена соответствующим образом
 */
static int nxs_cfg_read_char(int fd, nxs_cfg_parse_info_t *parse_info, unsigned char *c)
{
	int        rb;
	static int f_utf8 = 0;

	rb = read(fd, c, 1);

	if(rb > 0) {

		if(*c == '\n') {

			parse_info->line++;
			parse_info->pos = 0;
		}
		else {

			/*
			 * Подсчёт позиции с учётом киррилических символов
			 */

			if(*c == 0xd0) {

				f_utf8 = 1;

				parse_info->pos++;

				return rb;
			}

			if(*c == 0xd1) {

				f_utf8 = 2;

				parse_info->pos++;

				return rb;
			}

			if((f_utf8 == 1 && *c >= 0x80 && *c <= 0xbf) || (f_utf8 == 2 && *c >= 0x80 && *c <= 0x9f)) {

				f_utf8 = 0;

				return rb;
			}

			f_utf8 = 0;

			parse_info->pos++;

			return rb;
		}
	}

	return rb;
}

/*
 * Считать следующую пару "опция - значение"
 *
 * Возвращаемые значения:
 * NXS_CFG_CONF_READ_OK			- чтение произведено успешно
 * NXS_CFG_CONF_READ_ERROR		- синтаксическая ошибка или ошибка ввода/вывода
 * NXS_CFG_CONF_READ_EOF		- достигнут конец файла (чтение произведено успешно, ошибок нет)
 */
static int nxs_cfg_read_next(int fd, nxs_cfg_parse_info_t *parse_info, nxs_string_t *opt, nxs_string_t *value)
{
	unsigned char c;
	int           status, rb;

	nxs_string_clear(opt);
	nxs_string_clear(value);

	status = NXS_CFG_READ_STATUS_WAIT_OPT;

	rb = nxs_cfg_read_char(fd, parse_info, &c);

	parse_info->pos = 0;

	while(rb > 0 && status != NXS_CFG_READ_STATUS_EOL) {

		switch(status) {

			case NXS_CFG_READ_STATUS_WAIT_OPT:

				/*
				 * Ожидание начала имени опции.
				 * Все символы-разделители, включая символ '\n' пропускаются
				 */

				if(nxs_cfg_check_delimiter(c) == 1) {

					rb = nxs_cfg_read_char(fd, parse_info, &c);
				}
				else {

					if(c == nxs_cfg_char_eol) {

						/*
						 * EOL
						 */

						rb = nxs_cfg_read_char(fd, parse_info, &c);
					}
					else {

						if(c == nxs_cfg_char_comment) {

							status = NXS_CFG_READ_STATUS_B_COMMENT;
						}
						else {

							if(nxs_cfg_check_charset(c, nxs_cfg_char_opt_set) == 1) {

								status = NXS_CFG_READ_STATUS_READ_OPT;
							}
							else {

								/*
								 * ERROR
								 */

								return NXS_CFG_CONF_READ_ERROR;
							}
						}
					}
				}

				break;

			case NXS_CFG_READ_STATUS_B_COMMENT:

				if(c == nxs_cfg_char_eol) {

					/*
					 * EOL
					 */

					status = NXS_CFG_READ_STATUS_WAIT_OPT;
				}

				rb = nxs_cfg_read_char(fd, parse_info, &c);

				break;

			case NXS_CFG_READ_STATUS_READ_OPT:

				/*
				 * Чтение имени опции до первого символа, который не находится во множестве допустимых символов для имени
				 * опций
				 */

				if(nxs_cfg_check_charset(c, nxs_cfg_char_opt_set) == 1) {

					nxs_cfg_optval_add_char(opt, c);

					rb = nxs_cfg_read_char(fd, parse_info, &c);
				}
				else {

					if(nxs_cfg_check_delimiter(c) == 1) {

						status = NXS_CFG_READ_STATUS_WAIT_SEP;
					}
					else {

						if(c == nxs_cfg_char_sep) {

							status = NXS_CFG_READ_STATUS_READ_SEP;
						}
						else {

							/*
							 * ERROR
							 */

							return NXS_CFG_CONF_READ_ERROR;
						}
					}
				}

				break;

			case NXS_CFG_READ_STATUS_WAIT_SEP:

				if(nxs_cfg_check_delimiter(c) == 1) {

					rb = nxs_cfg_read_char(fd, parse_info, &c);
				}
				else {

					if(c == nxs_cfg_char_sep) {

						status = NXS_CFG_READ_STATUS_READ_SEP;
					}
					else {

						/*
						 * ERROR
						 */

						return NXS_CFG_CONF_READ_ERROR;
					}
				}

				break;

			case NXS_CFG_READ_STATUS_READ_SEP:

				if(c == nxs_cfg_char_sep) {

					rb = nxs_cfg_read_char(fd, parse_info, &c);

					status = NXS_CFG_READ_STATUS_WAIT_VAL;
				}
				else {

					/*
					 * ERROR
					 */

					return NXS_CFG_CONF_READ_ERROR;
				}

				break;

			case NXS_CFG_READ_STATUS_WAIT_VAL:

				if(nxs_cfg_check_delimiter(c) == 1) {

					rb = nxs_cfg_read_char(fd, parse_info, &c);
				}
				else {

					if(nxs_cfg_check_charset(c, nxs_cfg_char_val_set) == 1) {

						status = NXS_CFG_READ_STATUS_READ_VAL;
					}
					else {

						if(c == nxs_cfg_char_quote) {

							rb = nxs_cfg_read_char(fd, parse_info, &c);

							status = NXS_CFG_READ_STATUS_READ_QUETED_VAL;
						}
						else {

							printf("ERROR: %c\n", c);

							/*
							 * ERROR
							 */

							return NXS_CFG_CONF_READ_ERROR;
						}
					}
				}

				break;

			case NXS_CFG_READ_STATUS_READ_VAL:

				if(nxs_cfg_check_charset(c, nxs_cfg_char_val_set) == 1) {

					nxs_cfg_optval_add_char(value, c);

					rb = nxs_cfg_read_char(fd, parse_info, &c);
				}
				else {

					if(nxs_cfg_check_delimiter(c) == 1) {

						status = NXS_CFG_READ_STATUS_WAIT_EOL;
					}
					else {

						if(c == nxs_cfg_char_comment) {

							status = NXS_CFG_READ_STATUS_E_COMMENT;
						}
						else {

							if(c == nxs_cfg_char_eol) {

								status = NXS_CFG_READ_STATUS_EOL;
							}
							else {

								/*
								 * ERROR
								 */

								return NXS_CFG_CONF_READ_ERROR;
							}
						}
					}
				}

				break;

			case NXS_CFG_READ_STATUS_READ_QUETED_VAL:

				if(nxs_cfg_check_charset(c, nxs_cfg_char_val_set) == 1 || nxs_cfg_check_delimiter(c) == 1 ||
				   c == nxs_cfg_char_comment || c == nxs_cfg_char_eol) {

					nxs_cfg_optval_add_char(value, c);

					rb = nxs_cfg_read_char(fd, parse_info, &c);
				}
				else {

					if(c == nxs_cfg_char_quote) {

						rb = nxs_cfg_read_char(fd, parse_info, &c);

						status = NXS_CFG_READ_STATUS_WAIT_EOL;
					}
					else {

						/*
						 * ERROR
						 */

						return NXS_CFG_CONF_READ_ERROR;
					}
				}

				break;

			case NXS_CFG_READ_STATUS_WAIT_EOL:

				if(nxs_cfg_check_delimiter(c) == 1) {

					rb = nxs_cfg_read_char(fd, parse_info, &c);
				}
				else {

					if(c == nxs_cfg_char_eol) {

						status = NXS_CFG_READ_STATUS_EOL;
					}
					else {

						if(c == nxs_cfg_char_comment) {

							status = NXS_CFG_READ_STATUS_E_COMMENT;
						}
						else {

							/*
							 * ERROR
							 */

							return NXS_CFG_CONF_READ_ERROR;
						}
					}
				}

				break;

			case NXS_CFG_READ_STATUS_E_COMMENT:

				if(c != nxs_cfg_char_eol) {

					rb = nxs_cfg_read_char(fd, parse_info, &c);
				}
				else {

					/*
					 * EOL
					 */

					status = NXS_CFG_READ_STATUS_EOL;
				}

				break;

			case NXS_CFG_READ_STATUS_EOL:

				/*
				 * EMPTY
				 */

				break;
		}
	}

	if(rb == 0) {

		if(status == NXS_CFG_READ_STATUS_WAIT_OPT || status == NXS_CFG_READ_STATUS_B_COMMENT ||
		   status == NXS_CFG_READ_STATUS_WAIT_EOL || status == NXS_CFG_READ_STATUS_READ_VAL ||
		   status == NXS_CFG_READ_STATUS_E_COMMENT || status == NXS_CFG_READ_STATUS_EOL) {

			return NXS_CFG_CONF_READ_EOF;
		}
		else {

			return NXS_CFG_CONF_READ_ERROR;
		}
	}
	else {

		return NXS_CFG_CONF_READ_OK;
	}
}

static int nxs_cfg_type_handler_int(nxs_process_t *proc, nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par)
{
	int *  var = nxs_cfg_get_val(cfg_par);
	size_t i;

	for(i = 0; i < nxs_string_len(val); i++) {

		if(nxs_string_get_char(val, i) != '-' && (nxs_string_get_char(val, i) < '0' || nxs_string_get_char(val, i) > '9')) {

			nxs_log_write_error(proc,
			                    "config error: bad value for integer option (option: \"%s\", value: \"%s\")",
			                    nxs_string_str(opt),
			                    nxs_string_str(val));

			return NXS_CFG_CONF_ERROR;
		}
	}

	*var = nxs_string_atoi(val);

	if(cfg_par->min_val != 0 || cfg_par->max_val != 0) {

		if(*var < cfg_par->min_val) {

			nxs_log_write_error(proc,
			                    "config error: value of integer option less then allowed minimum value (option: \"%s\", value: "
			                    "\"%s\", allowed values: from '%d' till '%d')",
			                    nxs_string_str(opt),
			                    nxs_string_str(val),
			                    cfg_par->min_val,
			                    cfg_par->max_val);

			return NXS_CFG_CONF_ERROR;
		}

		if(*var > cfg_par->max_val) {

			nxs_log_write_error(proc,
			                    "config error: value of integer option more then allowed maximum value (option: \"%s\", value: "
			                    "\"%s\", allowed values: from '%d' till '%d')",
			                    nxs_string_str(opt),
			                    nxs_string_str(val),
			                    cfg_par->min_val,
			                    cfg_par->max_val);

			return NXS_CFG_CONF_ERROR;
		}
	}

	return NXS_CFG_CONF_OK;
}

static int nxs_cfg_type_handler_string(nxs_process_t *proc, nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par)
{
	nxs_string_t *var = nxs_cfg_get_val(cfg_par);

	nxs_string_cpy_dyn(var, 0, val, 0);

	return NXS_CFG_CONF_OK;
}

static int nxs_cfg_type_handler_num(nxs_process_t *proc, nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par)
{
	size_t        i;
	nxs_string_t *var = nxs_cfg_get_val(cfg_par);

	for(i = 0; i < nxs_string_len(val); i++) {

		if(nxs_string_get_char(val, i) < '0' || nxs_string_get_char(val, i) > '9') {

			nxs_log_write_error(proc,
			                    "config error: bad value for integer option (option: \"%s\", value: \"%s\")",
			                    nxs_string_str(opt),
			                    nxs_string_str(val));

			return NXS_CFG_CONF_ERROR;
		}
	}

	nxs_string_cpy_dyn(var, 0, val, 0);

	return NXS_CFG_CONF_OK;
}

static int nxs_cfg_type_handler_list(nxs_process_t *proc, nxs_string_t *opt, nxs_string_t *val, nxs_cfg_par_t *cfg_par, nxs_bool_t distinct)
{
	nxs_list_t *  var = nxs_cfg_get_val(cfg_par);
	nxs_string_t *el, *p;

	if(distinct == NXS_YES) {

		for(p = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, var); p != NULL; p = nxs_list_ptr_next(var)) {

			if(nxs_string_cmp(p, NXS_STRING_NO_OFFSET, val, NXS_STRING_NO_OFFSET) == NXS_STRING_CMP_EQ) {

				return NXS_CFG_CONF_OK;
			}
		}
	}

	el = nxs_list_add_tail(var);

	nxs_string_init2(el, 0, nxs_string_str(val));

	return NXS_CFG_CONF_OK;
}

static int nxs_cfg_type_handler_list_num(nxs_process_t *proc,
                                         nxs_string_t * opt,
                                         nxs_string_t * val,
                                         nxs_cfg_par_t *cfg_par,
                                         nxs_bool_t     distinct)
{
	size_t        i;
	nxs_list_t *  var = nxs_cfg_get_val(cfg_par);
	nxs_string_t *el, *p;

	if(distinct == NXS_YES) {

		for(p = nxs_list_ptr_init(NXS_LIST_PTR_INIT_HEAD, var); p != NULL; p = nxs_list_ptr_next(var)) {

			if(nxs_string_cmp(p, NXS_STRING_NO_OFFSET, val, NXS_STRING_NO_OFFSET) == NXS_STRING_CMP_EQ) {

				return NXS_CFG_CONF_OK;
			}
		}
	}

	for(i = 0; i < nxs_string_len(val); i++) {

		if(nxs_string_get_char(val, i) < '0' || nxs_string_get_char(val, i) > '9') {

			nxs_log_write_error(proc,
			                    "config error: bad value for integer option (option: \"%s\", value: \"%s\")",
			                    nxs_string_str(opt),
			                    nxs_string_str(val));

			return NXS_CFG_CONF_ERROR;
		}
	}

	el = nxs_list_add_tail(var);

	nxs_string_init2(el, 0, nxs_string_str(val));

	return NXS_CFG_CONF_OK;
}
