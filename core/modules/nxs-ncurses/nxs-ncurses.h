#ifndef _INCLUDE_NXS_NCURSES_H
#define _INCLUDE_NXS_NCURSES_H

#include <nxs-core/nxs-core.h>

#define NXS_NCURSES_LOCALE_RU					(u_char *)"ru_RU.UTF-8"

#define NXS_NCURSES_KEY_MODE_RAW				0
#define NXS_NCURSES_KEY_MODE_CBREAK				1

#define NXS_NCURSES_KEYPAD_NO					0
#define NXS_NCURSES_KEYPAD_YES					1

#define NXS_NCURSES_STDWIN_PARENT				0

#define NXS_NCURSES_ACTIVE_WIN_NONE				-1

#define NXS_NCURSES_E_SUCCESS					0
#define NXS_NCURSES_E_PTR						-1
#define NXS_NCURSES_E_WIN_INDEX_WRONG			-2
#define NXS_NCURSES_E_WIN_INDEX_EXIST			-3
#define NXS_NCURSES_E_WIN_INDEX_NOT_EXIST		-4
#define NXS_NCURSES_E_WIN_INDEX_PARENT_WRONG	-5
#define NXS_NCURSES_E_WIN_WRONG_TYPE			-6

#define NXS_NCURSES_GKEY_STATE_SKIP				0 /* Константа значтает, что глобальная функция пропустила обработку клавиши (не требуется) */

enum nxs_ncurses_type_e
{
	nxs_ncurses_type_container,
	nxs_ncurses_type_label,
	nxs_ncurses_type_checkbox,
	nxs_ncurses_type_radio,
	nxs_ncurses_type_button,
	nxs_ncurses_type_text
};

enum nxs_ncurses_subtype_text_e
{
	nxs_ncurses_subtype_text_string,
	nxs_ncurses_subtype_text_area,
	nxs_ncurses_subtype_text_passwd
};

struct nxs_ncurses_win_s
{

	u_int							index;

	int								nlines, ncols, y, x;

	nxs_ncurses_type_t				type;

	nxs_bool_t						visible;

	WINDOW							*win;

	int								parent_index;

	u_int							color_pair_active;
	u_int							color_pair_inactive;
	nxs_bool_t						border;
	u_int							use_color;

	void							(*f_resize)				(u_int nh, u_int nw); /* Изменение размеров родительского окна */

	void							(*pre_activate)			(nxs_ncurses_ctx_t *, u_int);
	void							(*post_activate)		(nxs_ncurses_ctx_t *, u_int);

	void							(*pre_deactivate)		(nxs_ncurses_ctx_t *, u_int);
	void							(*post_deactivate)		(nxs_ncurses_ctx_t *, u_int);

	void							(*pre_hide)				(nxs_ncurses_ctx_t *, u_int);
	void							(*post_hide)			(nxs_ncurses_ctx_t *, u_int);

	void							(*pre_show)				(nxs_ncurses_ctx_t *, u_int);
	void							(*post_show)			(nxs_ncurses_ctx_t *, u_int);

	int								(*keypad)				(nxs_ncurses_ctx_t *, u_int, int);

	void							*data;

};

struct nxs_ncurses_win_conf_s
{

	nxs_bool_t						visible;

	int								parent_index;

	u_int							color_pair_active;
	u_int							color_pair_inactive;

	nxs_bool_t						border;

	void							(*f_resize)				(u_int nh, u_int nw); /* Изменение размеров родительского окна */

	void							(*pre_activate)			(nxs_ncurses_ctx_t *, u_int);
	void							(*post_activate)		(nxs_ncurses_ctx_t *, u_int);

	void							(*pre_deactivate)		(nxs_ncurses_ctx_t *, u_int);
	void							(*post_deactivate)		(nxs_ncurses_ctx_t *, u_int);

	void							(*pre_hide)				(nxs_ncurses_ctx_t *, u_int);
	void							(*post_hide)			(nxs_ncurses_ctx_t *, u_int);

	void							(*pre_show)				(nxs_ncurses_ctx_t *, u_int);
	void							(*post_show)			(nxs_ncurses_ctx_t *, u_int);

	int								(*keypad)				(nxs_ncurses_ctx_t *, u_int, int);

};

struct nxs_ncurses_conf_text_s
{

	nxs_ncurses_subtype_text_t		stype;

	u_int							maxlen;
	nxs_bool_t						free_curs;
};

struct nxs_ncurses_ctx_s
{


	nxs_bool_t						key_mode;
	nxs_bool_t						keypad_mode;

	int								(*key_global)(nxs_ncurses_ctx_t *ctx, int);		/* Функция-обработчик нажатия клавиш (глобальная для конкретного контекста) */
	int								key_global_state;								/* Состояние обработки глобальных клавиш. Требуется для того, чтобы иметь возможность обрабатывать сложные сочетания клавиш */

	int								active_win;										/* Индекс активного окна */

	nxs_array_t						windows;

	nxs_tree_t						wtree;

	nxs_bool_t						doupdate_manual;

	void							(*sig_resize)(int, void *);

};

struct nxs_ncurses_t_container_s
{

	nxs_string_t					title;

};

struct nxs_ncurses_t_label_s
{

	nxs_string_t					text;

};

struct nxs_ncurses_t_text_s
{

	nxs_ncurses_subtype_text_t		stype;
	u_int							maxlen;
	nxs_bool_t						free_curs;

	u_int							curs_y;
	u_int							curs_x;

	nxs_array_t						newlines;

	nxs_string_t					text;

};

void						nxs_ncurses_init						(nxs_process_t *proc, nxs_ncurses_ctx_t *ctx, u_char *locale, nxs_bool_t key_mode, nxs_bool_t keypad_mode, void (*f_resize)(int, void *));
void						nxs_ncurses_set_gkeypad					(nxs_ncurses_ctx_t *ctx, int (*key_global)(nxs_ncurses_ctx_t *ctx, int));

//int							nxs_ncurses_mkwin						(nxs_ncurses_ctx_t *ctx, u_int index_win, u_int nlines, u_int ncols, u_int y, u_int x, nxs_bool_t active, nxs_ncurses_win_conf_t w_conf);

int							nxs_ncurses_active_set					(nxs_ncurses_ctx_t *ctx, u_int index);
void						nxs_ncurses_active_next					(nxs_ncurses_ctx_t *ctx);
void						nxs_ncurses_active_prev					(nxs_ncurses_ctx_t *ctx);
int							nxs_ncurses_active_get_index			(nxs_ncurses_ctx_t *ctx);
nxs_bool_t					nxs_ncurses_active_check				(nxs_ncurses_ctx_t *ctx, u_int index);

int							nxs_ncurses_keypress					(nxs_ncurses_ctx_t *ctx, int key);

void						nxs_ncurses_mkcontainer					(nxs_ncurses_ctx_t *ctx, u_int index_win, u_int nlines, u_int ncols, u_int y, u_int x, nxs_ncurses_win_conf_t w_conf, u_char *title);
void						nxs_ncurses_mklabel						(nxs_ncurses_ctx_t *ctx, u_int index_win, u_int nlines, u_int ncols, u_int y, u_int x, nxs_ncurses_win_conf_t w_conf, u_char *text);
void						nxs_ncurses_mktext						(nxs_ncurses_ctx_t *ctx, u_int index_win, u_int nlines, u_int ncols, u_int y, u_int x, nxs_ncurses_win_conf_t w_conf, nxs_ncurses_conf_text_t t_conf);

void						nxs_ncurses_win_show					(nxs_ncurses_ctx_t *ctx, u_int index, nxs_bool_t recursive);
void						nxs_ncurses_win_hide					(nxs_ncurses_ctx_t *ctx, u_int index);
void						nxs_ncurses_win_color_change			(nxs_ncurses_ctx_t *ctx, u_int index, u_int color_pair);

void						nxs_ncurses_conf_clear					(nxs_ncurses_win_conf_t *w_conf);

void						nxs_ncurses_doupdate					(void);
void						nxs_ncurses_doupdate_auto				(nxs_ncurses_ctx_t *ctx);
void						nxs_ncurses_doupdate_manual				(nxs_ncurses_ctx_t *ctx);

#endif /* _INCLUDE_NXS_NCURSES_H */
