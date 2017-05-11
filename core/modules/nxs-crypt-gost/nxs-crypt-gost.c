// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define	NXS_CRYPT_GOST_BITES_LEFT_SHIFT		11

/*
 * Три 8-и раундовых итерации для одного из шагов алгоритма шифрования
 */
#define	NXS_CRYPT_GOST_ENCRYPT_ROUNDS		3

#define	NXS_CRYPT_GOST_MAX_UINT			4294967295U
#define	NXS_CRYPT_GOST_UINT_STR_LEN		11

/*
 * Количество 32-битных регистров в веторе инициализации
 */
#define	NXS_CRYPT_GOST_INIT_VECTOR_LENGTH	2

/* Constants C1 and C2 */
#define	NXS_CRYPT_GOST_C1			16843009
#define	NXS_CRYPT_GOST_C2			16843012

#define NXS_CRYPT_GOST_KEY_STRING_LEN		356

/* Module typedefs */

typedef struct			nxs_crypt_gost_subs_block_els_s		nxs_crypt_gost_subs_block_els_t;
typedef union			nxs_crypt_gost_subs_block_u		nxs_crypt_gost_subs_block_t;

/* Module declarations */

/*
 * Структура, содержащая весемь 4-битных элементов для результирующего блока замен
 */
struct nxs_crypt_gost_subs_block_els_s
{
	uint32_t				el_0:4,
						el_1:4,
						el_2:4,
						el_3:4,
						el_4:4,
						el_5:4,
						el_6:4,
						el_7:4;
};

/*
 * Объединение, содержащее результирующий блок замен.
 * В RFC 5830 (section 6) именуется как "K"
 * Результирующий блок замен - это 32-битный вектор, содержащий восемь 4-битных элементов.
 */
union nxs_crypt_gost_subs_block_u
{
	/*
	 * Результирующий вектор замен
	 */
	uint32_t				vector;

	/*
	 * Весемь 4-битных элементов в результирующем блоке замен
	 */
	nxs_crypt_gost_subs_block_els_t		subs_block_els;
};

/* Module internal (static) functions prototypes */

// clang-format on

static void nxs_crypt_gost_base(nxs_crypt_gost_el_t *nxs_crypt_gost_el);
static void nxs_crypt_gost_ground(u_char key_seq, nxs_crypt_gost_el_t *nxs_crypt_gost_el);

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

nxs_crypt_gost_ctx_t *nxs_crypt_gost_malloc(nxs_string_t *key)
{
	nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx = NULL;

	nxs_crypt_gost_ctx = (nxs_crypt_gost_ctx_t *)nxs_malloc(nxs_crypt_gost_ctx, sizeof(nxs_crypt_gost_ctx_t));

	if(nxs_crypt_gost_init(nxs_crypt_gost_ctx, key) != NXS_CRYPT_GOST_E_OK) {

		return nxs_crypt_gost_destroy(nxs_crypt_gost_ctx);
	}

	return nxs_crypt_gost_ctx;
}

nxs_crypt_gost_ctx_t *nxs_crypt_gost_malloc2(nxs_process_t *proc, nxs_string_t *path)
{
	nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx = NULL;

	nxs_crypt_gost_ctx = (nxs_crypt_gost_ctx_t *)nxs_malloc(nxs_crypt_gost_ctx, sizeof(nxs_crypt_gost_ctx_t));

	if(nxs_crypt_gost_init2(proc, nxs_crypt_gost_ctx, path) != NXS_CRYPT_GOST_E_OK) {

		return nxs_crypt_gost_destroy(nxs_crypt_gost_ctx);
	}

	return nxs_crypt_gost_ctx;
}

nxs_crypt_gost_ctx_t *nxs_crypt_gost_destroy(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx)
{

	if(nxs_crypt_gost_ctx == NULL) {

		return NULL;
	}

	nxs_crypt_gost_free(nxs_crypt_gost_ctx);

	return nxs_free(nxs_crypt_gost_ctx);
}

int nxs_crypt_gost_init(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx, nxs_string_t *key)
{
	u_int   i, j, k, offset;
	u_char *key_str;

	nxs_memzero(nxs_crypt_gost_ctx, sizeof(nxs_crypt_gost_ctx_t));

	if(nxs_string_len(key) != NXS_CRYPT_GOST_KEY_STRING_LEN) {

		return NXS_CRYPT_GOST_E_KEY_SRC_LEN;
	}

	key_str = nxs_string_str(key);

	nxs_crypt_gost_ctx->init = NXS_CRYPT_GOST_INIT_YES;

	nxs_list_init(&nxs_crypt_gost_ctx->els, sizeof(nxs_crypt_gost_el_t));

	/*
	 * Заполнение основной структуры
	 */

	/*
	 * Чтение ключа (8 элементов по 10 байт)
	 */
	for(i = 0, offset = 0; i < NXS_CRYPT_GOST_KEY_REGISTERS; i++) {

		for(nxs_crypt_gost_ctx->main_el.key[i] = 0, k = 0; k < 10; k++, offset++) {

			nxs_crypt_gost_ctx->main_el.key[i] = nxs_crypt_gost_ctx->main_el.key[i] * 10 + key_str[offset];
		}
	}

	/*
	 * Чтение вектора инициализации (2 элемента по 10 байт)
	 */
	for(i = 0; i < NXS_CRYPT_GOST_INIT_VECTOR_LENGTH; i++) {

		for(nxs_crypt_gost_ctx->main_el.s_vector.reg32[i] = 0, k = 0; k < 10; k++, offset++) {

			nxs_crypt_gost_ctx->main_el.s_vector.reg32[i] =
			        nxs_crypt_gost_ctx->main_el.s_vector.reg32[i] * 10 + key_str[offset];
		}
	}

	/*
	 * Чтение таблицы замен (8 * 16 элементов по 2 байта)
	 */
	for(i = 0; i < NXS_CRYPT_GOST_S_LINES; i++) {

		for(j = 0; j < NXS_CRYPT_GOST_S_VALUES; j++, offset += 2) {

			nxs_crypt_gost_ctx->main_el.subs_table[i][j] = key_str[offset] * 10 + key_str[offset + 1];
		}
	}

	nxs_crypt_gost_base(&nxs_crypt_gost_ctx->main_el);

	nxs_crypt_gost_ctx->main_el.bytes_count = 0;

	return NXS_CRYPT_GOST_E_OK;
}

int nxs_crypt_gost_init2(nxs_process_t *proc, nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx, nxs_string_t *path)
{
	int          rc;
	nxs_string_t key;

	nxs_string_init(&key);

	if((rc = nxs_crypt_gost_key_read(proc, &key, path)) != NXS_CRYPT_GOST_E_OK) {

		nxs_string_free(&key);

		return rc;
	}

	rc = nxs_crypt_gost_init(nxs_crypt_gost_ctx, &key);

	nxs_string_free(&key);

	return rc;
}

void nxs_crypt_gost_free(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx)
{

	if(nxs_crypt_gost_ctx->init != NXS_CRYPT_GOST_INIT_YES) {

		return;
	}

	nxs_list_free(&nxs_crypt_gost_ctx->els);

	nxs_memzero(nxs_crypt_gost_ctx, sizeof(nxs_crypt_gost_ctx_t));
}

nxs_crypt_gost_el_t *nxs_crypt_gost_key_open(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx)
{
	nxs_crypt_gost_el_t *el;

	if(nxs_crypt_gost_ctx->init != NXS_CRYPT_GOST_INIT_YES) {

		return NULL;
	}

	el = nxs_list_add_tail(&nxs_crypt_gost_ctx->els);

	nxs_memcpy(el, &nxs_crypt_gost_ctx->main_el, sizeof(nxs_crypt_gost_el_t));

	return el;
}

nxs_crypt_gost_el_t *nxs_crypt_gost_key_close(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx, nxs_crypt_gost_el_t *key_el)
{
	nxs_crypt_gost_el_t *el;

	if(key_el == NULL) {

		return NULL;
	}

	if(nxs_crypt_gost_ctx->init != NXS_CRYPT_GOST_INIT_YES) {

		return NULL;
	}

	for(el = nxs_list_ptr_init(NXS_LIST_PTR_INIT_TAIL, &nxs_crypt_gost_ctx->els); el != NULL;
	    el = nxs_list_ptr_prev(&nxs_crypt_gost_ctx->els)) {

		if(el == key_el) {

			nxs_list_del(&nxs_crypt_gost_ctx->els, NXS_LIST_MOVE_NEXT);

			return NULL;
		}
	}

	return NULL;
}

int nxs_crypt_gost_key_read(nxs_process_t *proc, nxs_string_t *key, nxs_string_t *path)
{
	int     sf_fd, i;
	u_char  key_buf[NXS_CRYPT_GOST_KEY_STRING_LEN];
	ssize_t br;

	if((sf_fd = open((char *)nxs_string_str(path), O_RDONLY)) < 0) {

		nxs_log_write_warn(proc, "can't open GOST secret file: %s (path: \"%s\")", strerror(errno), nxs_string_str(path));

		return NXS_CRYPT_GOST_E_KEY_READ;
	}

	if((br = read(sf_fd, key_buf, NXS_CRYPT_GOST_KEY_STRING_LEN)) < 0) {

		nxs_log_write_warn(proc, "can't read GOST secret file: %s (path: \"%s\")", strerror(errno), nxs_string_str(path));

		close(sf_fd);

		return NXS_CRYPT_GOST_E_KEY_READ;
	}

	close(sf_fd);

	if(br < NXS_CRYPT_GOST_KEY_STRING_LEN) {

		nxs_log_write_warn(
		        proc, "can't read GOST secret file: secret file has size less then expected (path: \"%s\")", nxs_string_str(path));

		return NXS_CRYPT_GOST_E_KEY_READ;
	}

	for(i = 0; i < NXS_CRYPT_GOST_KEY_STRING_LEN; i++) {

		if(key_buf[i] < '0' || key_buf[i] > '9') {

			nxs_log_write_warn(proc,
			                   "can't read GOST secret file: secret file has unexpected char (pos: %d, path: \"%s\")",
			                   i,
			                   nxs_string_str(path));

			return NXS_CRYPT_GOST_E_KEY_READ;
		}
	}

	nxs_string_char_ncpy_dyn(key, NXS_STRING_NO_OFFSET, key_buf, NXS_CRYPT_GOST_KEY_STRING_LEN);

	return NXS_CRYPT_GOST_E_OK;
}

void nxs_crypt_gost_key_gen(nxs_string_t *key)
{
	uint32_t i, j, rnd;
	u_char   keyBuf[NXS_CRYPT_GOST_UINT_STR_LEN];

	nxs_string_create(key, NXS_CRYPT_GOST_KEY_STRING_LEN + 1, NULL);

	srand(time(NULL));

	for(i = 0; i < NXS_CRYPT_GOST_KEY_REGISTERS + NXS_CRYPT_GOST_INIT_VECTOR_LENGTH; i++) {

		rnd = rand() % NXS_CRYPT_GOST_MAX_UINT;

		sprintf((char *)keyBuf, "%.10u", rnd);

		nxs_string_char_cat_dyn(key, keyBuf);
	}

	for(i = 0; i < NXS_CRYPT_GOST_S_LINES; i++) {

		for(j = 0; j < NXS_CRYPT_GOST_S_VALUES; j++) {

			rnd = rand() % (NXS_CRYPT_GOST_S_MAX_VALUE + 1);

			sprintf((char *)keyBuf, "%.2u", rnd);

			nxs_string_char_cat_dyn(key, keyBuf);
		}
	}
}

void nxs_crypt_gost_reset(nxs_crypt_gost_ctx_t *nxs_crypt_gost_ctx, nxs_crypt_gost_el_t *nxs_crypt_gost_el)
{

	if(nxs_crypt_gost_ctx == NULL || nxs_crypt_gost_el == NULL) {

		return;
	}

	nxs_crypt_gost_el->s_vector.reg32[0] = nxs_crypt_gost_ctx->main_el.s_vector.reg32[0];
	nxs_crypt_gost_el->s_vector.reg32[1] = nxs_crypt_gost_ctx->main_el.s_vector.reg32[1];

	nxs_crypt_gost_el->bytes_count = 0;
}

/*
 * При шифровании у активного элемента контекста (активной копии контекста) меняется только "s_vector".
 * Данная функция задаёт "s_vector" активного элемента значения, указанные в контексте, т.е. переводит его начальное состояние.
 * Это позволяет избегать повторных копирований всего элемента.
 */
void nxs_crypt_gost(u_char *data, size_t len, nxs_crypt_gost_el_t *nxs_crypt_gost_el)
{
	size_t i;

	if(nxs_crypt_gost_el == NULL) {

		return;
	}

	/*
	 * Цикл по массиву данных для их шифрования/дешифрования.
	 */
	for(i = 0; len - i > 0; i++) {

		/*
		 * Если ключ был использован для 64 бит данных,
		 * либо это его первое использвание -
		 * производим очередной цикл генерации ключей.
		 */
		if(nxs_crypt_gost_el->bytes_count == 0) {

			nxs_crypt_gost_el->s_vector.reg32[0] = nxs_crypt_gost_el->s_vector.reg32[0] + NXS_CRYPT_GOST_C1;
			nxs_crypt_gost_el->s_vector.reg32[1] =
			        (nxs_crypt_gost_el->s_vector.reg32[1] + NXS_CRYPT_GOST_C2 - 1) % (NXS_CRYPT_GOST_MAX_UINT) + 1;

			/*
			 * Базовый шаг крипто-преобразования
			 */
			nxs_crypt_gost_base(nxs_crypt_gost_el);
		}

		/*
		 * Шифруем/дешифруем очередной байт данных
		 */
		data[i] ^= nxs_crypt_gost_el->s_vector.reg8[nxs_crypt_gost_el->bytes_count++];

		/*
		 * Если данным набором ключей были зашифрованы все
		 * 64 бита данных - обнуляем счётчик для того, чтобы
		 * запустить новый цикл генерации ключей.
		 */
		if(nxs_crypt_gost_el->bytes_count == NXS_CRYPT_GOST_64_REG8) {

			nxs_crypt_gost_el->bytes_count = 0;
		}
	}
}

/* Module internal (static) functions */

static void nxs_crypt_gost_base(nxs_crypt_gost_el_t *nxs_crypt_gost_el)
{
	u_char   rounds, key_seq;
	uint32_t tmp_reg32;

	/*
	 * 24 раунда (цикл из трёх итераций по 8 раундов в каждом) алгоритма ГОСТ 28147-89
	 */
	for(rounds = 0; rounds < NXS_CRYPT_GOST_ENCRYPT_ROUNDS; rounds++) {

		for(key_seq = 0; key_seq < NXS_CRYPT_GOST_KEY_REGISTERS; key_seq++) {

			nxs_crypt_gost_ground(key_seq, nxs_crypt_gost_el);
		}
	}

	/*
	 * Последние восемь раундов (ключи считываются в обратном порядке)
	 */
	for(key_seq = NXS_CRYPT_GOST_KEY_REGISTERS; key_seq > 0; key_seq--) {

		nxs_crypt_gost_ground(key_seq - 1, nxs_crypt_gost_el);
	}

	/* N1 <-> N2 */
	tmp_reg32                            = nxs_crypt_gost_el->s_vector.reg32[1];
	nxs_crypt_gost_el->s_vector.reg32[1] = nxs_crypt_gost_el->s_vector.reg32[0];
	nxs_crypt_gost_el->s_vector.reg32[0] = tmp_reg32;
}

static void nxs_crypt_gost_ground(u_char key_seq, nxs_crypt_gost_el_t *nxs_crypt_gost_el)
{
	nxs_crypt_gost_subs_block_t K;

	K.vector = nxs_crypt_gost_el->key[key_seq] + nxs_crypt_gost_el->s_vector.reg32[0];

	/*
	 * Преобразование результирующего вектора замен наложением таблицы замен
	 */
	K.subs_block_els.el_0 = nxs_crypt_gost_el->subs_table[0][K.subs_block_els.el_0];
	K.subs_block_els.el_1 = nxs_crypt_gost_el->subs_table[1][K.subs_block_els.el_1];
	K.subs_block_els.el_2 = nxs_crypt_gost_el->subs_table[2][K.subs_block_els.el_2];
	K.subs_block_els.el_3 = nxs_crypt_gost_el->subs_table[3][K.subs_block_els.el_3];
	K.subs_block_els.el_4 = nxs_crypt_gost_el->subs_table[4][K.subs_block_els.el_4];
	K.subs_block_els.el_5 = nxs_crypt_gost_el->subs_table[5][K.subs_block_els.el_5];
	K.subs_block_els.el_6 = nxs_crypt_gost_el->subs_table[6][K.subs_block_els.el_6];
	K.subs_block_els.el_7 = nxs_crypt_gost_el->subs_table[7][K.subs_block_els.el_7];

	/*
	 * Цикличный сдвиг на 11 бит влево
	 */
	K.vector = K.vector << NXS_CRYPT_GOST_BITES_LEFT_SHIFT;

	K.vector ^= nxs_crypt_gost_el->s_vector.reg32[1];

	nxs_crypt_gost_el->s_vector.reg32[1] = nxs_crypt_gost_el->s_vector.reg32[0];
	nxs_crypt_gost_el->s_vector.reg32[0] = K.vector;
}
