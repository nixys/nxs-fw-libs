// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define _NXS_NCURSES_WIN_ARRAY_INIT		3

#define _NXS_NCURSES_WIN_MAX			10000

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

static int nxs_ncurses_mkwin(nxs_ncurses_ctx_t *    ctx,
                             u_int                  index_win,
                             u_int                  nlines,
                             u_int                  ncols,
                             u_int                  y,
                             u_int                  x,
                             nxs_ncurses_type_t     type,
                             nxs_ncurses_win_conf_t w_conf);

static void nxs_ncurses_win_draw(nxs_ncurses_ctx_t *ctx, nxs_ncurses_win_t *w);

static void nxs_ncurses_container_draw(nxs_ncurses_ctx_t *ctx, nxs_ncurses_win_t *w);
static void nxs_ncurses_label_draw(nxs_ncurses_ctx_t *ctx, nxs_ncurses_win_t *w);

static void nxs_ncurses_text_draw(nxs_ncurses_ctx_t *ctx, nxs_ncurses_win_t *w);
static void nxs_ncurses_text_pre_activate(nxs_ncurses_ctx_t *ctx, u_int index);
static void nxs_ncurses_text_pre_deactivate(nxs_ncurses_ctx_t *ctx, u_int index);
static int nxs_ncurses_text_keypad(nxs_ncurses_ctx_t *ctx, u_int index, int key);

static void nxs_ncurses_sig_resize(int sig, void *data);

static nxs_ncurses_win_t *nxs_ncurses_get_win(nxs_ncurses_ctx_t *ctx, u_int index);
static nxs_ncurses_win_t *nxs_ncurses_find_el(nxs_tree_t *t, u_int index);

static void nxs_ncurses_redraw(nxs_ncurses_ctx_t *ctx);
static void nxs_ncurses_redraw_recursive(nxs_ncurses_ctx_t *ctx);

static void nxs_ncurses_show_recursive(nxs_ncurses_ctx_t *ctx, nxs_bool_t recursive);
static void nxs_ncurses_hide_recursive(nxs_tree_t *t);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

// nxs_ncurses_ctx_t *nxs_ncurses_malloc(){}

// nxs_ncurses_ctx_t *nxs_ncurses_destroy(){}

void nxs_ncurses_init(nxs_process_t *    proc,
                      nxs_ncurses_ctx_t *ctx,
                      u_char *           locale,
                      nxs_bool_t         key_mode,
                      nxs_bool_t         keypad_mode,
                      void (*sig_resize)(int, void *))
{
	nxs_ncurses_win_t *w;

	if(ctx == NULL) {

		return;
	}

	if(sig_resize == NULL) {

		ctx->sig_resize = &nxs_ncurses_sig_resize;
	}
	else {

		ctx->sig_resize = sig_resize;
	}

	nxs_proc_signal_set(proc, SIGWINCH, NXS_PROCESS_SA_FLAG_EMPTY, ctx->sig_resize, NULL, NXS_PROCESS_FORCE_YES);

	if(locale == NULL) {

		setlocale(LC_CTYPE, (const char *)NXS_NCURSES_LOCALE_RU);
	}
	else {

		setlocale(LC_CTYPE, (const char *)locale);
	}

	initscr();
	start_color();
	noecho();

	refresh();

	if(key_mode == NXS_NCURSES_KEY_MODE_RAW) {

		raw();

		ctx->key_mode = NXS_NCURSES_KEY_MODE_RAW;
	}
	else {

		cbreak();

		ctx->key_mode = NXS_NCURSES_KEY_MODE_CBREAK;
	}

	if(keypad_mode == NXS_NCURSES_KEYPAD_YES) {

		keypad(stdscr, TRUE);

		ctx->key_mode = NXS_NCURSES_KEYPAD_YES;
	}
	else {

		ctx->key_mode = NXS_NCURSES_KEYPAD_NO;
	}

	ctx->doupdate_manual = NXS_NO;

	ctx->key_global       = NULL;
	ctx->key_global_state = NXS_NCURSES_GKEY_STATE_SKIP;

	nxs_array_init(&ctx->windows, _NXS_NCURSES_WIN_ARRAY_INIT, sizeof(nxs_ncurses_win_t), 0);

	w = nxs_tree_init(&ctx->wtree, sizeof(nxs_ncurses_win_t));

	/*
	 * Определения для основного окна
	 */
	w->visible = NXS_YES;
	w->win     = stdscr;

	ctx->active_win = NXS_NCURSES_ACTIVE_WIN_NONE;

	curs_set(0);
}

void nxs_ncurses_set_gkeypad(nxs_ncurses_ctx_t *ctx, int (*key_global)(nxs_ncurses_ctx_t *ctx, int))
{

	ctx->key_global       = key_global;
	ctx->key_global_state = NXS_NCURSES_GKEY_STATE_SKIP;
}

int nxs_ncurses_active_set(nxs_ncurses_ctx_t *ctx, u_int index)
{
	nxs_ncurses_win_t *w;

	if(ctx == NULL) {

		return NXS_NCURSES_E_PTR;
	}

	if(nxs_ncurses_get_win(ctx, index) == NULL) {

		return NXS_NCURSES_E_WIN_INDEX_NOT_EXIST;
	}

	/*
	 * Деактивация текущего активного элемента (если есть)
	 */
	if(ctx->active_win != NXS_NCURSES_ACTIVE_WIN_NONE) {

		w = nxs_ncurses_get_win(ctx, ctx->active_win);

		w->use_color = w->color_pair_inactive;

		if(w->pre_deactivate != NULL) {

			w->pre_deactivate(ctx, index);
		}

		nxs_ncurses_redraw_recursive(ctx);

		if(w->post_deactivate != NULL) {

			w->post_deactivate(ctx, index);
		}
	}

	/*
	 * Аактивация указанного элемента
	 */

	w = nxs_ncurses_get_win(ctx, index);

	ctx->active_win = index;

	w->use_color = w->color_pair_active;

	if(w->pre_activate != NULL) {

		w->pre_activate(ctx, index);
	}

	nxs_ncurses_redraw_recursive(ctx);

	if(w->post_activate != NULL) {

		w->post_activate(ctx, index);
	}

	if(ctx->doupdate_manual == NXS_NO) {

		doupdate();
	}

	return NXS_NCURSES_E_SUCCESS;
}

int nxs_ncurses_active_get_index(nxs_ncurses_ctx_t *ctx)
{

	return ctx->active_win;
}

nxs_bool_t nxs_ncurses_active_check(nxs_ncurses_ctx_t *ctx, u_int index)
{

	if(ctx->active_win == (int)index) {

		return NXS_YES;
	}

	return NXS_NO;
}

int nxs_ncurses_keypress(nxs_ncurses_ctx_t *ctx, int key)
{
	nxs_ncurses_win_t *w;
	int                rc;

	if(ctx == NULL) {

		return NXS_NCURSES_E_PTR;
	}

	if(ctx->key_global != NULL) {

		rc = ctx->key_global(ctx, key);

		if(ctx->key_global_state != NXS_NCURSES_GKEY_STATE_SKIP) {

			return rc;
		}
	}

	if(ctx->active_win == NXS_NCURSES_ACTIVE_WIN_NONE) {

		return NXS_NCURSES_E_SUCCESS;
	}

	if((w = nxs_ncurses_get_win(ctx, ctx->active_win)) == NULL) {

		return NXS_NCURSES_E_WIN_INDEX_NOT_EXIST;
	}

	if(w->keypad != NULL) {

		w->keypad(ctx, ctx->active_win, key);
	}

	return NXS_NCURSES_E_SUCCESS;
}

void nxs_ncurses_win_show(nxs_ncurses_ctx_t *ctx, u_int index, nxs_bool_t recursive)
{
	nxs_ncurses_win_t *w;

	if(ctx == NULL) {

		return;
	}

	if((w = nxs_ncurses_get_win(ctx, index)) == NULL) {

		return;
	}

	if(w->pre_show != NULL) {

		w->pre_show(ctx, index);
	}

	nxs_ncurses_show_recursive(ctx, recursive);

	nxs_ncurses_redraw(ctx);

	if(w->post_show != NULL) {

		w->post_show(ctx, index);
	}

	if(ctx->doupdate_manual == NXS_NO) {

		doupdate();
	}
}

void nxs_ncurses_win_hide(nxs_ncurses_ctx_t *ctx, u_int index)
{
	nxs_ncurses_win_t *w;

	if(ctx == NULL) {

		return;
	}

	if((w = nxs_ncurses_get_win(ctx, index)) == NULL) {

		return;
	}

	if(w->visible == NXS_NO) {

		return;
	}

	if(w->pre_hide != NULL) {

		w->pre_hide(ctx, index);
	}

	nxs_ncurses_hide_recursive(&ctx->wtree);

	/*
	 * Этот блок понадобится если при инициализации контекста не будет определяться w->win = stdscr
	 */
	/*
	touchwin(stdscr);
	wnoutrefresh(stdscr);
	*/

	nxs_ncurses_redraw(ctx);

	if(w->post_hide != NULL) {

		w->post_hide(ctx, index);
	}

	if(ctx->doupdate_manual == NXS_NO) {

		doupdate();
	}
}

/*
 * Изменить текущую цветовую схему для указанного окна.
 * Данная схема будет переопределена либо при повторном использовании данной функции (с другим color_pair), либо при активации/деактивации
 * окна
 */
void nxs_ncurses_win_color_change(nxs_ncurses_ctx_t *ctx, u_int index, u_int color_pair)
{
	nxs_ncurses_win_t *w;

	if(ctx == NULL) {

		return;
	}

	if((w = nxs_ncurses_get_win(ctx, index)) == NULL) {

		return;
	}

	w->use_color = color_pair;

	nxs_ncurses_redraw_recursive(ctx);

	if(ctx->doupdate_manual == NXS_NO) {

		doupdate();
	}
}

void nxs_ncurses_conf_clear(nxs_ncurses_win_conf_t *w_conf)
{

	if(w_conf == NULL) {

		return;
	}

	w_conf->visible = NXS_YES;

	w_conf->parent_index = NXS_NCURSES_STDWIN_PARENT;

	w_conf->color_pair_active   = 0;
	w_conf->color_pair_inactive = 0;

	w_conf->border = NXS_NO;

	w_conf->f_resize = NULL;

	w_conf->pre_activate  = NULL;
	w_conf->post_activate = NULL;

	w_conf->pre_deactivate  = NULL;
	w_conf->post_deactivate = NULL;

	w_conf->pre_hide  = NULL;
	w_conf->post_hide = NULL;

	w_conf->pre_show  = NULL;
	w_conf->post_show = NULL;

	w_conf->keypad = NULL;
}

void nxs_ncurses_doupdate_manual(nxs_ncurses_ctx_t *ctx)
{

	if(ctx == NULL) {

		return;
	}

	ctx->doupdate_manual = NXS_YES;
}

void nxs_ncurses_doupdate_auto(nxs_ncurses_ctx_t *ctx)
{

	if(ctx == NULL) {

		return;
	}

	ctx->doupdate_manual = NXS_NO;
}

void nxs_ncurses_doupdate(void)
{

	doupdate();
}

/************************************
 *
 * Window type: container
 *
 ************************************/

void nxs_ncurses_mkcontainer(nxs_ncurses_ctx_t *    ctx,
                             u_int                  index_win,
                             u_int                  nlines,
                             u_int                  ncols,
                             u_int                  y,
                             u_int                  x,
                             nxs_ncurses_win_conf_t w_conf,
                             u_char *               title)
{
	nxs_ncurses_win_t *        w;
	nxs_ncurses_t_container_t *d;

	if(ctx == NULL) {

		return;
	}

	nxs_ncurses_mkwin(ctx, index_win, nlines, ncols, y, x, nxs_ncurses_type_container, w_conf);

	d = (nxs_ncurses_t_container_t *)nxs_malloc(NULL, sizeof(nxs_ncurses_t_container_t));

	if(title == NULL) {

		nxs_string_init2(&d->title, 0, NXS_STRING_EMPTY_STR);
	}
	else {

		nxs_string_init2(&d->title, 0, title);
	}

	w = nxs_ncurses_get_win(ctx, index_win);

	w->data = d;

	nxs_ncurses_container_draw(ctx, w);

	if(ctx->doupdate_manual == NXS_NO) {

		doupdate();
	}
}

/************************************
 *
 * Window type: label
 *
 ************************************/

void nxs_ncurses_mklabel(nxs_ncurses_ctx_t *    ctx,
                         u_int                  index_win,
                         u_int                  nlines,
                         u_int                  ncols,
                         u_int                  y,
                         u_int                  x,
                         nxs_ncurses_win_conf_t w_conf,
                         u_char *               text)
{
	nxs_ncurses_win_t *    w;
	nxs_ncurses_t_label_t *d;

	if(ctx == NULL) {

		return;
	}

	w_conf.border = NXS_NO;

	nxs_ncurses_mkwin(ctx, index_win, nlines, ncols, y, x, nxs_ncurses_type_label, w_conf);

	d = (nxs_ncurses_t_label_t *)nxs_malloc(NULL, sizeof(nxs_ncurses_t_label_t));
	nxs_string_init2(&d->text, 0, text);

	w = nxs_ncurses_get_win(ctx, index_win);

	w->data = d;

	nxs_ncurses_label_draw(ctx, w);

	if(ctx->doupdate_manual == NXS_NO) {

		doupdate();
	}
}

/************************************
 *
 * Window type: text
 *
 ************************************/

void nxs_ncurses_mktext(nxs_ncurses_ctx_t *     ctx,
                        u_int                   index_win,
                        u_int                   nlines,
                        u_int                   ncols,
                        u_int                   y,
                        u_int                   x,
                        nxs_ncurses_win_conf_t  w_conf,
                        nxs_ncurses_conf_text_t t_conf)
{
	nxs_ncurses_win_t *   w;
	nxs_ncurses_t_text_t *d;

	if(ctx == NULL) {

		return;
	}

	w_conf.border = NXS_NO;

	if(w_conf.pre_activate == NULL) {

		w_conf.pre_activate = &nxs_ncurses_text_pre_activate;
	}

	if(w_conf.pre_deactivate == NULL) {

		w_conf.pre_deactivate = &nxs_ncurses_text_pre_deactivate;
	}

	if(w_conf.keypad == NULL) {

		w_conf.keypad = &nxs_ncurses_text_keypad;
	}

	nxs_ncurses_mkwin(ctx, index_win, nlines, ncols, y, x, nxs_ncurses_type_text, w_conf);

	d = (nxs_ncurses_t_text_t *)nxs_malloc(NULL, sizeof(nxs_ncurses_t_text_t));

	nxs_string_init2(&d->text, 0, NXS_STRING_EMPTY_STR);
	d->stype     = t_conf.stype;
	d->maxlen    = t_conf.maxlen;
	d->free_curs = t_conf.free_curs;

	d->curs_y = 0;
	d->curs_x = 0;

	nxs_array_init(&d->newlines, 0, sizeof(nxs_string_t), 1);

	w = nxs_ncurses_get_win(ctx, index_win);

	w->data = d;

	scrollok(w->win, TRUE);

	nxs_ncurses_text_draw(ctx, w);

	if(ctx->doupdate_manual == NXS_NO) {

		doupdate();
	}
}

/* Module internal (static) functions */

static void nxs_ncurses_container_draw(nxs_ncurses_ctx_t *ctx, nxs_ncurses_win_t *w)
{
	nxs_ncurses_t_container_t *d;

	if(w == NULL) {

		return;
	}

	if(w->border == NXS_YES) {

		wborder(w->win, 0, 0, 0, 0, 0, 0, 0, 0);
	}

	d = w->data;

	if(nxs_string_len(&d->title) > 0) {

		mvwprintw(w->win, 0, 2, " %s ", nxs_string_str(&d->title));
	}

	wbkgd(w->win, COLOR_PAIR(w->use_color));

	wnoutrefresh(w->win);
}

static void nxs_ncurses_label_draw(nxs_ncurses_ctx_t *ctx, nxs_ncurses_win_t *w)
{
	nxs_ncurses_t_label_t *d;

	d = w->data;

	mvwprintw(w->win, 0, 0, (const char *)nxs_string_str(&d->text));

	wbkgd(w->win, COLOR_PAIR(w->use_color));

	wnoutrefresh(w->win);
}

static void nxs_ncurses_text_pre_activate(nxs_ncurses_ctx_t *ctx, u_int index)
{

	curs_set(1);
}

static void nxs_ncurses_text_pre_deactivate(nxs_ncurses_ctx_t *ctx, u_int index)
{

	curs_set(0);
}

static int nxs_ncurses_text_keypad(nxs_ncurses_ctx_t *ctx, u_int index, int key)
{
	nxs_ncurses_win_t *   w;
	nxs_ncurses_t_text_t *d;
	nxs_string_t *        l;

	if((w = nxs_ncurses_get_win(ctx, index)) == NULL) {

		return NXS_NCURSES_E_WIN_INDEX_EXIST;
	}

	if(w->type != nxs_ncurses_type_text) {

		return NXS_NCURSES_E_WIN_WRONG_TYPE;
	}

	d = w->data;

	switch(key) {

		case KEY_RIGHT:

			if(d->free_curs == NXS_YES) {

				if(d->curs_x + 1 < (u_int)w->ncols) {

					d->curs_x++;
				}

				wmove(w->win, d->curs_y, d->curs_x);
			}

			break;

		case KEY_LEFT:

			if(d->free_curs == NXS_YES) {

				if(d->curs_x > 0) {

					d->curs_x--;
				}

				wmove(w->win, d->curs_y, d->curs_x);
			}

			break;

		case KEY_DOWN:

			if(d->free_curs == NXS_YES) {

				if(d->curs_y + 1 < (u_int)w->nlines) {

					d->curs_y++;
				}

				wmove(w->win, d->curs_y, d->curs_x);
			}

			break;

		case KEY_UP:

			if(d->free_curs == NXS_YES) {

				if(d->curs_y > 0) {

					d->curs_y--;
				}

				wmove(w->win, d->curs_y, d->curs_x);
			}

			break;

		case KEY_ENTER:

			break;

		default:

			if((l = nxs_array_get(&d->newlines, d->curs_y)) == NULL) {

				l = nxs_array_add_i(&d->newlines, d->curs_y);

				// nxs_string_init(l);
			}

			nxs_string_char_add_char_dyn(l, (u_char)key);

			mvwprintw(w->win, d->curs_y, 0, (const char *)nxs_string_str(l));

			break;
	}

	wnoutrefresh(w->win);

	if(ctx->doupdate_manual == NXS_NO) {

		doupdate();
	}

	return NXS_NCURSES_E_SUCCESS;
}

static void nxs_ncurses_text_draw(nxs_ncurses_ctx_t *ctx, nxs_ncurses_win_t *w)
{
	nxs_ncurses_t_text_t *d;

	d = w->data;

	mvwprintw(w->win, 0, 0, (const char *)nxs_string_str(&d->text));

	wbkgd(w->win, COLOR_PAIR(w->use_color));

	wnoutrefresh(w->win);
}

/************************************
 *
 * Static functions
 *
 ************************************/

static int nxs_ncurses_mkwin(nxs_ncurses_ctx_t *    ctx,
                             u_int                  index_win,
                             u_int                  nlines,
                             u_int                  ncols,
                             u_int                  y,
                             u_int                  x,
                             nxs_ncurses_type_t     type,
                             nxs_ncurses_win_conf_t w_conf)
{
	nxs_ncurses_win_t *nw, *pw;

	if(ctx == NULL) {

		return NXS_NCURSES_E_PTR;
	}

	if(index_win > _NXS_NCURSES_WIN_MAX) {

		return NXS_NCURSES_E_WIN_INDEX_WRONG;
	}

	if(nxs_ncurses_get_win(ctx, index_win) != NULL) {

		return NXS_NCURSES_E_WIN_INDEX_EXIST;
	}

	/*
	 * Определение корня (родителя) для нового элемента
	 */
	if((pw = nxs_ncurses_get_win(ctx, w_conf.parent_index)) == NULL) {

		return NXS_NCURSES_E_WIN_INDEX_PARENT_WRONG;
	}

	nw = (nxs_ncurses_win_t *)nxs_tree_add(&ctx->wtree);

	nw->index        = index_win;
	nw->parent_index = w_conf.parent_index;

	nw->nlines = nlines;
	nw->ncols  = ncols;
	nw->y      = y;
	nw->x      = x;

	nw->border              = w_conf.border;
	nw->color_pair_active   = w_conf.color_pair_active;
	nw->color_pair_inactive = w_conf.color_pair_inactive;
	nw->use_color           = w_conf.color_pair_inactive;
	nw->visible             = w_conf.visible;

	nw->type = type;
	nw->data = NULL;

	/*
	 * Установка функций окна
	 */

	nw->f_resize = w_conf.f_resize;

	nw->pre_activate  = w_conf.pre_activate;
	nw->post_activate = w_conf.post_activate;

	nw->pre_deactivate  = w_conf.pre_deactivate;
	nw->post_deactivate = w_conf.post_deactivate;

	nw->pre_show  = w_conf.pre_show;
	nw->post_show = w_conf.post_show;

	nw->pre_hide  = w_conf.pre_hide;
	nw->post_hide = w_conf.post_hide;

	nw->keypad = w_conf.keypad;

	/*
	 * Применение заданных параметров
	 */

	if(nw->visible == NXS_YES) {

		if(nw->parent_index == NXS_NCURSES_STDWIN_PARENT) {

			nw->win = newwin(nlines, ncols, y, x);
		}
		else {

			/*
			 * С использованием derwin() не удалось добиться корректной перерисовки родительского окна, удалённое окно
			 * продолжало отображаться.
			 * Помогал только wclear(), но затраты на проработку модуля таким методом были слишком большими.
			 *
			 * upd: если отказаться от derwin() и перейти к использованию только newwin (а иерархию организовать в данном
			 * модуле), то
			 * получаются проблемы при активации окна - не перерисовываются дочерние окна.
			 */
			nw->win = derwin(pw->win, nlines, ncols, y, x);
		}
	}

	return NXS_NCURSES_E_SUCCESS;
}

static nxs_ncurses_win_t *nxs_ncurses_get_win(nxs_ncurses_ctx_t *ctx, u_int index)
{

	if(ctx == NULL) {

		return NULL;
	}

	if(index > _NXS_NCURSES_WIN_MAX) {

		return NULL;
	}

	nxs_tree_ptr_c_root_init(&ctx->wtree);

	return nxs_ncurses_find_el(&ctx->wtree, index);
}

static nxs_ncurses_win_t *nxs_ncurses_find_el(nxs_tree_t *t, u_int index)
{
	nxs_ncurses_win_t *w;
	int                i;

	w = nxs_tree_get_c_root(t);

	if(w->index == index) {

		return w;
	}

	for(i = 0; i < nxs_tree_get_c_root_cc(t); i++) {

		nxs_tree_ptr_c_el_set(t, i);

		nxs_tree_chroot(t);

		if((w = nxs_ncurses_find_el(t, index)) != NULL) {

			return w;
		}

		nxs_tree_chroot_up(t);
	}

	return NULL;
}

static void nxs_ncurses_show_recursive(nxs_ncurses_ctx_t *ctx, nxs_bool_t recursive)
{
	nxs_ncurses_win_t *w, *pw;
	int                i;

	if((w = nxs_tree_get_c_root(&ctx->wtree)) == NULL) {

		return;
	}

	if(w->visible == NXS_NO) {

		pw = nxs_tree_get_c_root_parent(&ctx->wtree);

		if(w->parent_index == NXS_NCURSES_STDWIN_PARENT || pw == NULL) {

			w->win = newwin(w->nlines, w->ncols, w->y, w->x);
		}
		else {

			w->win = derwin(pw->win, w->nlines, w->ncols, w->y, w->x);
		}

		w->visible = NXS_YES;
	}

	if(recursive == NXS_YES) {

		for(i = 0; i < nxs_tree_get_c_root_cc(&ctx->wtree); i++) {

			nxs_tree_ptr_c_el_set(&ctx->wtree, i);

			nxs_tree_chroot(&ctx->wtree);

			nxs_ncurses_show_recursive(ctx, recursive);

			nxs_tree_chroot_up(&ctx->wtree);
		}
	}

	return;
}

static void nxs_ncurses_hide_recursive(nxs_tree_t *t)
{
	nxs_ncurses_win_t *w;
	int                i;

	if((w = nxs_tree_get_c_root(t)) == NULL) {

		return;
	}

	if(w->visible == NXS_NO) {

		return;
	}

	delwin(w->win);

	w->win     = NULL;
	w->visible = NXS_NO;

	for(i = 0; i < nxs_tree_get_c_root_cc(t); i++) {

		nxs_tree_ptr_c_el_set(t, i);

		nxs_tree_chroot(t);

		nxs_ncurses_hide_recursive(t);

		nxs_tree_chroot_up(t);
	}

	return;
}

/*
 * Перерисовать все видимые элементы
 */
static void nxs_ncurses_redraw(nxs_ncurses_ctx_t *ctx)
{

	/*
	 * Устанавливается корень дерева (главный корень) от которого будет производиться обход всех элементов
	 */
	if(nxs_ncurses_get_win(ctx, NXS_NCURSES_STDWIN_PARENT) == NULL) {

		return;
	}

	nxs_ncurses_redraw_recursive(ctx);
}

static void nxs_ncurses_redraw_recursive(nxs_ncurses_ctx_t *ctx)
{
	nxs_ncurses_win_t *w;
	int                i;

	if((w = nxs_tree_get_c_root(&ctx->wtree)) == NULL) {

		return;
	}

	if(w->visible == NXS_NO) {

		return;
	}

	if(w->win != NULL) {

		if(w->index > 0) {

			werase(w->win);

			nxs_ncurses_win_draw(ctx, w);
		}
		else {

			touchwin(w->win);

			wnoutrefresh(w->win);
		}
	}

	for(i = 0; i < nxs_tree_get_c_root_cc(&ctx->wtree); i++) {

		nxs_tree_ptr_c_el_set(&ctx->wtree, i);

		nxs_tree_chroot(&ctx->wtree);

		nxs_ncurses_redraw_recursive(ctx);

		nxs_tree_chroot_up(&ctx->wtree);
	}

	return;
}

static void nxs_ncurses_win_draw(nxs_ncurses_ctx_t *ctx, nxs_ncurses_win_t *w)
{

	if(ctx == NULL || w == NULL) {

		return;
	}

	if(w->visible == NXS_NO) {

		return;
	}

	switch(w->type) {

		case nxs_ncurses_type_container:

			nxs_ncurses_container_draw(ctx, w);

			break;

		case nxs_ncurses_type_label:

			nxs_ncurses_label_draw(ctx, w);

			break;

		case nxs_ncurses_type_checkbox:

			break;

		case nxs_ncurses_type_radio:

			break;

		case nxs_ncurses_type_button:

			break;

		case nxs_ncurses_type_text:

			nxs_ncurses_text_draw(ctx, w);

			break;
	}
}

static void nxs_ncurses_sig_resize(int sig, void *data)
{
	//	u_int			h, w;
	struct winsize size;

	refresh();

	ioctl(fileno(stdout), TIOCGWINSZ, &size);

	resizeterm(size.ws_row, size.ws_col);

	//	getmaxyx(stdscr, h, w);
}
