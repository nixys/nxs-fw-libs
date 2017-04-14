#include <nxs-core/nxs-core.h>

#define NXS_STRING_MAX_UINT_BUF_SIZE		100
#define NXS_STRING_MAX_DOUBLE_DECIMAL_DIG	16

#define NXS_STRING_TMP_STR_SIZE				128
#define NXS_STRING_FLOAT_STR_TPL			"%%%s%zu.%zuf"
#define NXS_STRING_DOUBLE_STR_TPL			"%%%s%zu.%zulf"
#define NXS_STRING_OCT_STR_TPL				"%%%s%zuo"
#define NXS_STRING_HEX_STR_TPL				"%%%s%zux"

#define _NXS_STRING_OK						"nxs string ok"
#define _NXS_STRING_ERROR_NOT_INIT			"nxs string error: string not initialized"
#define _NXS_STRING_ERROR					"nxs string error"
#define _NXS_STRING_ERROR_DST_SIZE			"nxs string error: dst string size exceeded"
#define _NXS_STRING_ERROR_SRC_SIZE			"nxs string error: src string size exceeded"
#define _NXS_STRING_ERROR_DST_LEN			"nxs string error: dst string len exceeded"
#define _NXS_STRING_ERROR_SRC_LEN			"nxs string error: src string size exceeded"
#define _NXS_STRING_ERROR_OFFSET			"nxs string error: offset size more then string len"
#define	_NXS_STRING_ERROR_NOT_CREATED		"nxs string error: string not created"
#define	_NXS_STRING_ERROR_NULL_PTR			"nxs string error: string's or char's ptr is null"
#define	_NXS_STRING_ERROR_SPEC_UNKNOWN		"nxs string error: unknown specifier"
#define	_NXS_STRING_ERROR_SPEC_OVERFLOW		"nxs string error: too many specifiers"
#define	_NXS_STRING_ERROR_UNKNOWN			"nxs string error: unknown"

#define nxs_string_check_not_created(str)	if(str->size == 0 || str->str == NULL){ \
												return NXS_STRING_ERROR_NOT_CREATED; \
											}

struct strftime_s
{
	char		_f;
	int			o;
};

static ssize_t				nxs_string_vprintf_core_dyn						(nxs_string_t *str, size_t offset, const char *fmt, va_list ap);
static void					nxs_string_vprintf_uint							(nxs_string_t *str, nxs_bool_t sign, uint64_t ui64, u_char zero, size_t width, size_t precision);
static void					nxs_string_vprintf_int							(nxs_string_t *str, int64_t i64, u_char zero, size_t width, size_t precision);


/*
 * Выделение памяти под строку str, её инициализация и создание(если требуется).
 * Если new_str == NULL и size > 0 - память под строку будет выделена, строка будет создана нулевой длины (размер строки при этом будет равен size)
 * Если new_str == NULL и size == 0 - память под строку будет выделена и строка будет инициализирована, но не создана (size будет равен 0,
 * str->str будет равно NULL)
 */
nxs_string_t *nxs_string_malloc(size_t size, u_char *new_str)
{
	nxs_string_t	*str = NULL;
	size_t			s;

	str = nxs_calloc(str, sizeof(nxs_string_t));

	if(new_str != NULL){

		if(size == 0){

			s = strlen((const char *)new_str) + 1;
		}
		else{

			s = size;
		}

		nxs_string_create(str, s, new_str);
	}
	else{

		if(size == 0){

			return str;
		}

		nxs_string_create(str, size, NULL);
	}

	return str;
}

nxs_string_t *nxs_string_destroy(nxs_string_t *str)
{

	if(str == NULL){

		return NULL;
	}

	nxs_string_free(str);

	return nxs_free(str);
}

/*
 * Инициализация (обнуление) строки.
 * Рекомендуется использовать эту функцию только при объявлении строки
 */
void nxs_string_init(nxs_string_t *str)
{

	if(str == NULL){

		return;
	}

	str->str = NULL;
	str->size = 0;
	str->len = 0;
}

/*
 * Инициализация (обнуление) строки и одновременное заполнение данным.
 * Рекомендуется использовать эту функцию только при объявлении строки
 *
 * Возвращаемое значение:
 * * Длина новой строки
 * * Коды функции nxs_string_create(). В этом случае строка будет в инициализированном состоянии (т.е. память под строку выделена не будет)
 */
ssize_t nxs_string_init2(nxs_string_t *str, size_t size, u_char *new_str)
{
	ssize_t rc;

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	str->str = NULL;
	str->size = 0;
	str->len = 0;

	if((rc = nxs_string_create(str, size, new_str)) < 0){

		nxs_string_free(str);
	}

	return rc;
}

/*
 * Инициализация (обнуление) строки и одновременное заполнение данным из строки src.
 *
 * Возвращаемое значение:
 * * Длина новой строки
 * * Коды функции nxs_string_create(). В этом случае строка будет в инициализированном состоянии (т.е. память под строку выделена не будет)
 */
ssize_t nxs_string_init3(nxs_string_t *str, nxs_string_t *src)
{
	ssize_t rc;

	if(str == NULL || src == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	str->str = NULL;
	str->size = 0;
	str->len = 0;

	if((rc = nxs_string_create(str, 0, nxs_string_str(src))) < 0){

		nxs_string_free(str);
	}

	return rc;
}

/*
 * Создание строки.
 * Выделение памяти под строку (необходимо инициализировать строку до использования этой функции).
 * Если new_str != NULL и при этом size == 0, то размер новой строки будет вычислен автоматически как длина строки new_str + 1 ('\0')
 * Если new_str == NULL, а size > 0 - будет создана строка размером size и нулевой длиной.
 * Если и new_str == NULL, и size == 0 - строка создана не будет, функция вернёт ошибку "NXS_STRING_ERROR_NOT_CREATED".
 *
 * Возвращаемое значение:
 * * Длина новой строки
 * * NXS_STRING_ERROR_NOT_INIT		- если строка не инициализирована (для предотвращения утечек памяти)
 * * NXS_STRING_ERROR_DST_SIZE		- строка new_str имеет длину, превышающую размер size, заявленный для строки str
 * * NXS_STRING_ERROR_NOT_CREATED	- строка не создана, если new_str == NULL и size == 0 (не выделяется память нулевой длины)
 * * NXS_STRING_ERROR_NULL_PTR		- указатель на строку "str" равен NULL
 */
ssize_t nxs_string_create(nxs_string_t *str, size_t size, u_char *new_str)
{
	size_t		s;
	ssize_t		len;

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(str->str != NULL){

		return NXS_STRING_ERROR_NOT_INIT;
	}

	if(new_str != NULL){

		if(size == 0){

			/*
			 * Необходимо вычислить размер под новую строку
			 */

			s = strlen((const char *)new_str) + 1;
		}
		else{

			s = size;
		}

		str->str = nxs_calloc(NULL, s);
		str->size = s;

		if((len = nxs_string_char_ncpy(str, NXS_STRING_NO_OFFSET, new_str, strlen((const char *)new_str))) < 0){

			return len;
		}

		str->str[len] = '\0';

		str->len = len;
	}
	else{

		if(size == 0){

			/*
			 * Если требуемый размер строки 0
			 */

			return NXS_STRING_ERROR_NOT_CREATED;
		}

		str->str = nxs_calloc(NULL, size);
		str->size = size;
		nxs_string_clear(str);
	}

	return str->len;
}

/*
 * Освобождение памяти, выделенной под строку и инициализация строки для повторного использования)
 *
 * Возвращаемое значение:
 * * NXS_STRING_OK					- если ошибок не возникло
 * * NXS_STRING_ERROR_NOT_CREATED	- если строка не созадана
 * * NXS_STRING_ERROR_NULL_PTR		- указатель на строку "str" равен NULL
 */
int nxs_string_free(nxs_string_t *str)
{

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(str->size == 0 || str->str == NULL){

		return NXS_STRING_ERROR_NOT_CREATED;
	}

	str->str = nxs_free(str->str);

	str->len = 0;
	str->size = 0;

	return NXS_STRING_OK;
}

/*
 * Изменение размера строки str до нового размера new_size.
 * Если новый рамер строки меньше текущей длины строки - строка будет обрезана (с учётом завершающего символа)
 * Если new_size == 0 - память, выделенная под строку, будет освобождена, а строка инициализирована.
 * Если строка ещё не создана - она будет создана с размером new_size и нулевой длиной
 *
 * Возвращаемое значение:
 * * Новый рамер строки str
 * * NXS_STRING_ERROR_NULL_PTR		- указатель на строку "str" равен NULL
 */
ssize_t nxs_string_resize(nxs_string_t *str, size_t new_size)
{

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	/*
	 * Если new_size == 0 - память, выделенная под строку, будет освобождена, а строка инициализирована.
	 */
	if(new_size == 0){

		nxs_string_free(str);

		return str->size;
	}

	if(str->size == 0 || str->str == NULL){

		/*
		 * Если строка ещё не создана - создаём её
		 */

		nxs_string_create(str, new_size, NULL);
	}
	else{

		/*
		 * Если строка уже создана - изменяем её размер
		 */

		str->str = nxs_realloc(str->str, new_size);

		if(new_size < str->len + 1){

			str->len = new_size - 1;

			str->str[str->len] = '\0';
		}

		str->size = new_size;
	}

	return str->size;
}

/*
 * Очистка строки (выделенная память не освобождается)
 *
 * Возвращаемое значение:
 * * NXS_STRING_OK					- если ошибок не возникло
 * * NXS_STRING_ERROR_NOT_CREATED	- если строка не созадана
 * * NXS_STRING_ERROR_NULL_PTR		- указатель на строку "str" равен NULL
 */
int nxs_string_clear(nxs_string_t *str)
{

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(str->size == 0 || str->str == NULL){

		return NXS_STRING_ERROR_NOT_CREATED;
	}

	str->str[0] = '\0';
	str->len = 0;

	return NXS_STRING_OK;
}

ssize_t nxs_string_printf_dyn(nxs_string_t *str, const char *msg, ...)
{
	size_t		k;
	va_list		msg_args;

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	va_start(msg_args, msg);
	k = nxs_string_vprintf_core_dyn(str, 0, msg, msg_args);
	va_end(msg_args);

	return k;
}

/*
 * nxs_string_printf* format:
 *
 * %[0][width]zu				- size_t
 * %[0][width]zd				- ssize_t
 * %[0][width]d					- int
 * %[0][width]u					- unsigned int
 * %[0][width]lu				- unsigned long int
 * %[0][width]ld				- long int
 * %[0][width]llu				- unsigned long long int
 * %[0][width]lld				- long long int
 * %[0][width]o					- oct
 * %[0][width]x					- hex
 * %[0][width][.precision]f		- double
 * %c							- char
 * %s							- char * (null-terminated string)
 * %r							- nxs_string_t *
 * %R							- nxs_buf_t *
 * %%							- %
 */

ssize_t nxs_string_printf2_dyn(nxs_string_t *str, size_t offset, const char *msg, ...)
{
	size_t		k;
	va_list		msg_args;

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	va_start(msg_args, msg);
	k = nxs_string_vprintf_core_dyn(str, offset, msg, msg_args);
	va_end(msg_args);

	return k;
}

ssize_t nxs_string_vprintf_dyn(nxs_string_t *str, const char *fmt, va_list ap)
{

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	return nxs_string_vprintf_core_dyn(str, 0, fmt, ap);
}

/*
 * На текущий момент данные фукнции не используются
 *
 *
 *ssize_t nxs_string_printf(nxs_string_t *str, const char *msg, ...)
{
	size_t		k;
	va_list		msg_args;

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	va_start(msg_args, msg);
	k = vsnprintf((char *)str->str, str->size, msg, msg_args);
	va_end(msg_args);

	if(k >= str->size){

		str->len = str->size - 1;

		return NXS_STRING_ERROR_DST_SIZE;
	}

	str->len = k;

	return k;
}

ssize_t nxs_string_printf2(nxs_string_t *str, size_t offset, const char *msg, ...)
{
	size_t		k;
	va_list		msg_args;

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(offset + 1 >= str->size){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	va_start(msg_args, msg);
	k = vsnprintf((char *)str->str + offset, str->size - offset, msg, msg_args);
	va_end(msg_args);

	if(k >= str->size - offset){

		str->len = str->size - 1;

		return NXS_STRING_ERROR_DST_SIZE;
	}

	str->len = k + offset;

	return k;
}

ssize_t nxs_string_vprintf(nxs_string_t *str, const char *format, va_list ap)
{
	size_t		k;

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	k = vsnprintf((char *)str->str, str->size, format, ap);

	if(k >= str->size){

		str->len = str->size - 1;

		return NXS_STRING_ERROR_DST_SIZE;
	}

	str->len = k;

	return k;
}
*/

/*
 * Копирование подстроки src со смещением offset_src в строку dst со смещением offset_dst от её начала
 *
 * Возвращаемые значения:
 * * Новая длина строки dst
 * * NXS_STRING_ERROR_OFFSET	- смещение offset_dst или offset_src больше длин соответствующих строк
 * * NXS_STRING_ERROR_DST_SIZE	- результирующая длина строки dst больше, чем допустимый размер строки dst
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "dst" или "src" равен NULL
 */
ssize_t nxs_string_cpy(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src)
{

	if(dst == NULL || src == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(offset_src > src->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(offset_dst > dst->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(src->len - offset_src + 1 > dst->size - offset_dst){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	nxs_memcpy(dst->str + offset_dst, src->str + offset_src, src->len - offset_src);

	dst->len = offset_dst + src->len - offset_src;
	dst->str[dst->len] = '\0';

	return dst->len;
}

/*
 * Функция появилась с версии v0.2-0 r13
 */
ssize_t nxs_string_cpy_dyn(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src)
{
	ssize_t rc;

	if((rc = nxs_string_cpy(dst, offset_dst, src, offset_src)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(dst, src->len - offset_src + offset_dst + 1);

		rc = nxs_string_cpy(dst, offset_dst, src, offset_src);
	}

	return rc;
}

/*
 * Копирование n символов подстроки src со смещением offset_src в строку dst со смещением offset_dst от её начала
 *
 * Возвращаемые значения:
 * * Новая длина строки dst
 * * NXS_STRING_ERROR_OFFSET	- смещение offset_dst или offset_src больше длин соответствующих строк
 * * NXS_STRING_ERROR_SRC_SIZE	- количество байт n, которые требуется скопироваться со смещением offset_src превышает длину строки src
 * * NXS_STRING_ERROR_DST_SIZE	- результирующая длина строки dst больше, чем допустимый размер строки dst
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "dst" или "src" равен NULL
 */
ssize_t nxs_string_ncpy(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src, size_t n)
{

	if(dst == NULL || src == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(offset_src > src->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(offset_dst > dst->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(offset_src + n > src->len){

		return NXS_STRING_ERROR_SRC_SIZE;
	}

	if(n + 1 > dst->size - offset_dst){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	nxs_memcpy(dst->str + offset_dst, src->str + offset_src, n);

	dst->len = offset_dst + n;
	dst->str[dst->len] = '\0';

	return dst->len;
}

/*
 * Функция появилась с версии v0.2-0 r13
 */
ssize_t nxs_string_ncpy_dyn(nxs_string_t *dst, size_t offset_dst, nxs_string_t *src, size_t offset_src, size_t n)
{
	ssize_t rc;

	if((rc = nxs_string_ncpy(dst, offset_dst, src, offset_src, n)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(dst, offset_dst + n + 1);

		rc = nxs_string_ncpy(dst, offset_dst, src, offset_src, n);
	}

	return rc;
}

/*
 * Добавление строки src в конец строки dst
 *
 * Возвращаемое значение:
 * * Новая длина строки dst
 * * NXS_STRING_ERROR_DST_SIZE - если результирующий размер строки превышает допустимый для использования размер строки dst
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "dst" или "src" равен NULL
 */
ssize_t nxs_string_cat(nxs_string_t *dst, nxs_string_t *src)
{

	if(dst == NULL || src == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(dst->len + src->len + 1 > dst->size){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	nxs_memcpy(dst->str + dst->len, src->str, src->len);

	dst->len += src->len;
	dst->str[dst->len] = '\0';

	return dst->len;
}

/*
 * Функция появилась с версии v0.2-0 r13
 */
ssize_t nxs_string_cat_dyn(nxs_string_t *dst, nxs_string_t *src)
{
	ssize_t rc;

	if((rc = nxs_string_cat(dst, src)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(dst, dst->len + src->len + 1);

		rc = nxs_string_cat(dst, src);
	}

	return rc;
}

/*
 * Добавление n байт строки src в конец строки dst
 *
 * Возвращаемое значение:
 * * Новая длина строки dst
 * * NXS_STRING_ERROR_SRC_SIZE	- если требуемое количество байт 'n' превышает длину строки src
 * * NXS_STRING_ERROR_DST_SIZE	- если результирующий размер строки превышает допустимый для использования размер строки dst
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "dst" или "src" равен NULL
 */
ssize_t nxs_string_ncat(nxs_string_t *dst, nxs_string_t *src, size_t n)
{

	if(dst == NULL || src == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(n > src->len){

		return NXS_STRING_ERROR_SRC_SIZE;
	}

	if(dst->len + n + 1 > dst->size){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	nxs_memcpy(dst->str + dst->len, src->str, n);

	dst->len += n;
	dst->str[dst->len] = '\0';

	return dst->len;
}

/*
 * Функция появилась с версии v0.2-0 r13
 */
ssize_t nxs_string_ncat_dyn(nxs_string_t *dst, nxs_string_t *src, size_t n)
{
	ssize_t rc;

	if((rc = nxs_string_ncat(dst, src, n)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(dst, dst->len + n + 1);

		rc = nxs_string_ncat(dst, src, n);
	}

	return rc;
}

/*
 * Стравнение строки str1 со смещением  offset1 байт от начала со строкой str2 со смещением offset2 байт
 *
 * Возвращаемое значение:
 * * NXS_STRING_CMP_EQ			- сравниваемые подстроки одинаковы
 * * NXS_STRING_CMP_NE			- сравниваемые подстроки различны (хотя бы в одном символе)
 * * NXS_STRING_ERROR_OFFSET	- заданное смещение offset1 или offset2 больше длин соответствующих строк
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str1" или "str2" равен NULL
 */
int nxs_string_cmp(nxs_string_t *str1, size_t offset1, nxs_string_t *str2, size_t offset2)
{
	size_t i;

	if(str1 == NULL || str2 == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(offset1 > str1->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(offset2 > str2->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(str1->len - offset1 != str2->len - offset2){

		return NXS_STRING_CMP_NE;
	}

	for(i = 0; i < str1->len - offset1; i++){

		if(str1->str[offset1 + i] != str2->str[offset2 + i]){

			return NXS_STRING_CMP_NE;
		}
	}

	return NXS_STRING_CMP_EQ;
}

/*
 * Стравнение строки str1 со смещением  offset1 байт от начала со строкой str2 со смещением offset2 байт, сравниваются n первых байт
 *
 * Возвращаемое значение:
 * * NXS_STRING_CMP_EQ			- сравниваемые подстроки одинаковы
 * * NXS_STRING_CMP_NE			- сравниваемые подстроки различны (хотя бы в одном символе)
 * * NXS_STRING_ERROR_OFFSET	- заданное смещение offset1 или offset2 больше длин соответствующих строк
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str1" или "str2" равен NULL
 */
int nxs_string_ncmp(nxs_string_t *str1, size_t offset1, nxs_string_t *str2, size_t offset2, size_t n)
{
	size_t i;

	if(str1 == NULL || str2 == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(offset1 > str1->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(offset2 > str2->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(n > str1->len - offset1 || n > str2->len - offset2){

		return NXS_STRING_CMP_NE;
	}

	for(i = 0; i < n; i++){

		if(str1->str[offset1 + i] != str2->str[offset2 + i]){

			return NXS_STRING_CMP_NE;
		}
	}

	return NXS_STRING_CMP_EQ;
}

/*
 * Скопировать строку ch_str в строку str со смещением offset в строке str относительно её начала
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_STRING_ERROR_OFFSET	- если смещение offset больше длины строки str
 * * NXS_STRING_ERROR_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str" или "ch_str" равен NULL
 */
ssize_t nxs_string_char_cpy(nxs_string_t *str, size_t offset, u_char *ch_str)
{
	size_t len;

	if(str == NULL || ch_str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(offset > str->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	len = strlen((const char *)ch_str);

	if(offset + len + 1 > str->size){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	nxs_memcpy(str->str + offset, ch_str, len);

	str->len = offset + len;
	str->str[str->len] = '\0';

	return str->len;
}

/*
 * Функция появилась с версии v0.2-0 r13
 */
ssize_t nxs_string_char_cpy_dyn(nxs_string_t *str, size_t offset, u_char *ch_str)
{
	size_t		len;
	ssize_t		rc;

	if(ch_str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	len = strlen((const char *)ch_str);

	if((rc = nxs_string_char_cpy(str, offset, ch_str)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(str, offset + len + 1);

		rc = nxs_string_char_cpy(str, offset, ch_str);
	}

	return rc;
}

/*
 * Скопировать n-байт строки ch_str в строку str со смещением offset в строке str относительно её начала
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_STRING_ERROR_OFFSET	- если смещение offset больше длины строки str
 * * NXS_STRING_ERROR_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str" или "ch_str" равен NULL
 *
 * Функция изменилась в версии v0.2-0 r13
 */
ssize_t nxs_string_char_ncpy(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n)
{

	if(str == NULL || ch_str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(offset > str->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(offset + n + 1 > str->size){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	nxs_memcpy(str->str + offset, ch_str, n);

	str->len = offset + n;
	str->str[str->len] = '\0';

	return str->len;
}

/*
 * Функция появилась с версии v0.2-0 r13
 */
ssize_t nxs_string_char_ncpy_dyn(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n)
{
	ssize_t rc;

	if((rc = nxs_string_char_ncpy(str, offset, ch_str, n)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(str, offset + n + 1);

		rc = nxs_string_char_ncpy(str, offset, ch_str, n);
	}

	return rc;
}

/*
 * Добавить строку ch_str к строке str
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_STRING_ERROR_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str" или "ch_str" равен NULL
 */
ssize_t nxs_string_char_cat(nxs_string_t *str, u_char *ch_str)
{
	size_t len;

	if(str == NULL || ch_str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	len = strlen((const char *)ch_str);

	if(str->len + len + 1 > str->size){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	nxs_memcpy(str->str + str->len, ch_str, len);

	str->len += len;
	str->str[str->len] = '\0';

	return str->len;
}

/*
 * Функция появилась с версии v0.2-0 r13
 */
ssize_t nxs_string_char_cat_dyn(nxs_string_t *str, u_char *ch_str)
{
	size_t		len;
	ssize_t		rc;

	if(ch_str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	len = strlen((const char *)ch_str);

	if((rc = nxs_string_char_cat(str, ch_str)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(str, str->len + len + 1);

		rc = nxs_string_char_cat(str, ch_str);
	}

	return rc;
}

/*
 * Добавить n первых байт строки ch_str к строке str
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_STRING_ERROR_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str" или "ch_str" равен NULL
 *
 * Функция изменилась в версии v0.2-0 r13
 */
ssize_t nxs_string_char_ncat(nxs_string_t *str, u_char *ch_str, size_t n)
{

	if(str == NULL || ch_str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(str->len + n + 1 > str->size){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	nxs_memcpy(str->str + str->len, ch_str, n);

	str->len += n;
	str->str[str->len] = '\0';

	return str->len;
}

/*
 * Функция появилась с версии v0.2-0 r13
 */
ssize_t nxs_string_char_ncat_dyn(nxs_string_t *str, u_char *ch_str, size_t n)
{
	ssize_t rc;

	if((rc = nxs_string_char_ncat(str, ch_str, n)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(str, str->len + n + 1);

		rc = nxs_string_char_ncat(str, ch_str, n);
	}

	return rc;
}

/*
 * Стравнение строки str со смещением  offset байт от начала со строкой ch_str (u_char)
 *
 * Возвращаемое значение:
 * * NXS_STRING_CMP_EQ			- сравниваемые подстроки одинаковы
 * * NXS_STRING_CMP_NE			- сравниваемые подстроки различны (хотя бы в одном символе)
 * * NXS_STRING_ERROR_OFFSET	- заданное смещение offset больше длины строки str
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str" или "ch_str" равен NULL
 */
int nxs_string_char_cmp(nxs_string_t *str, size_t offset, u_char *ch_str)
{
	size_t i, len;

	if(str == NULL || ch_str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(offset + 1 > str->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	len = strlen((const char *)ch_str);

	if(str->len - offset != len){

		return NXS_STRING_CMP_NE;
	}

	for(i = 0; i < str->len - offset; i++){

		if(str->str[offset + i] != ch_str[i]){

			return NXS_STRING_CMP_NE;
		}
	}

	return NXS_STRING_CMP_EQ;
}

/*
 * Стравнение строки str со смещением  offset байт от начала со строкой ch_str, сравниваются n первых байт
 *
 * Возвращаемое значение:
 * * NXS_STRING_CMP_EQ			- сравниваемые подстроки одинаковы
 * * NXS_STRING_CMP_NE			- сравниваемые подстроки различны (хотя бы в одном символе)
 * * NXS_STRING_ERROR_OFFSET	- заданное смещение offset больше длины строки str
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str" или "ch_str" равен NULL
 *
 * Функция изменилась в версии v0.2-0 r13
 */
int nxs_string_char_ncmp(nxs_string_t *str, size_t offset, u_char *ch_str, size_t n)
{
	size_t i;

	if(str == NULL || ch_str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(offset + 1 > str->len){

		return NXS_STRING_ERROR_OFFSET;
	}

	if(n > str->len){

		return NXS_STRING_CMP_NE;
	}

	for(i = 0; i < n; i++){

		if(str->str[offset + i] != ch_str[i]){

			return NXS_STRING_CMP_NE;
		}
	}

	return NXS_STRING_CMP_EQ;
}

/*
 * Добавить символ "c" к строке str
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_STRING_ERROR_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str" или "ch_str" равен NULL
 */
ssize_t nxs_string_char_add_char(nxs_string_t *str, u_char c)
{

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(c == '\0'){

		if(str->len + 1 > str->size){

			return NXS_STRING_ERROR_DST_SIZE;
		}

		str->str[str->len] = c;
	}
	else{

		if(str->len + 2 > str->size){

			return NXS_STRING_ERROR_DST_SIZE;
		}

		str->str[str->len] = c;

		str->len++;
		str->str[str->len] = '\0';
	}

	return str->len;
}

/*
 * Функция появилась с версии v0.2-0 r13
 */
ssize_t nxs_string_char_add_char_dyn(nxs_string_t *str, u_char c)
{
	ssize_t rc;

	if((rc = nxs_string_char_add_char(str, c)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(str, str->len + 2);

		rc = nxs_string_char_add_char(str, c);
	}

	return rc;
}

void nxs_string_basename(nxs_string_t *dst, nxs_string_t *path)
{
	size_t 		i, len;
	ssize_t		l;

	if(dst == NULL || path == NULL){

		return;
	}

	len = path->len;

	if(len == 0){

		nxs_string_char_cpy_dyn(dst, 0, (u_char *)".");

		return;
	}

	while(len > 1 && path->str[len - 1] == '/'){

		len--;
	}

	if(len == 1){

		nxs_string_ncpy_dyn(dst, 0, path, 0, 1);

		return;
	}

	for(l = len - 1; l >= 0 && path->str[l] != '/'; l--);

	l++;

	if(dst->size < len - l + 1){

		nxs_string_resize(dst, len - l + 1);
	}

	for(i = l; i < len; i++){

		dst->str[i - l] = path->str[i];
	}

	dst->str[i - l] = '\0';
	dst->len = i - l;
}

void nxs_string_dirname(nxs_string_t *dst, nxs_string_t *path)
{
	ssize_t 		l, i;

	if(dst == NULL || path == NULL){

		return;
	}

	for(l = path->len - 1; l >= 0 && path->str[l] == '/'; l--);
	for(                 ; l >= 0 && path->str[l] != '/'; l--);
	for(                 ; l >= 0 && path->str[l] == '/'; l--);

	if(l < 0){

		if(path->str[0] == '/'){

			nxs_string_char_cpy_dyn(dst, 0, (u_char *)"/");
		}
		else{

			nxs_string_char_cpy_dyn(dst, 0, (u_char *)".");
		}
	}
	else{

		if(dst->size < (size_t)l + 2){

			nxs_string_resize(dst, l + 2);
		}

		for(i = 0; i <= l; i++){

			dst->str[i] = path->str[i];
		}

		dst->str[i] = '\0';
		dst->len = i;
	}
}

/*
 * Поместить символ 'c' в строку str в позицию pos (символ, находящийся в позиции pos будет заменён новым).
 * Если символ добавляется в конец строки - длина строки будет увеличина в рамках размеров строки
 *
 * Возвращаемое значение:
 * * Длина результирующей строки
 * * NXS_STRING_ERROR_NOT_CREATED	- строка не создана
 * * NXS_STRING_ERROR_DST_LEN		- требуемая позиция символа находится за границами строки (больше её длины)
 * * NXS_STRING_ERROR_DST_SIZE		- в строке нет доступного места для добавления символа
 * * NXS_STRING_ERROR_NULL_PTR		- указатель на строку "str" равен NULL
 */
ssize_t nxs_string_set_char(nxs_string_t *str, size_t pos, u_char c)
{

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	nxs_string_check_not_created(str);

	if(pos > str->len){

		return NXS_STRING_ERROR_DST_LEN;
	}

	if(pos == str->len){

		return nxs_string_char_add_char(str, c);
	}

	str->str[pos] = c;

	return str->len;
}

/*
 * Вставить симпол 'c' в строку str в позицию pos (символы, находящиеся справа от позиции pos будут смещены вправо на один символ).
 */
ssize_t nxs_string_ins_char(nxs_string_t *str, size_t pos, u_char c)
{
	size_t	i;

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	nxs_string_check_not_created(str);

	if(pos > str->len){

		return NXS_STRING_ERROR_DST_LEN;
	}

	if(str->len + 2 > str->size){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	for(i = str->len + 1; i > pos; i--){

		str->str[i] = str->str[i - 1];
	}

	str->str[pos] = c;

	str->len++;

	return str->len;
}

ssize_t nxs_string_ins_char_dyn(nxs_string_t *str, size_t pos, u_char c)
{
	ssize_t rc;

	if((rc = nxs_string_ins_char(str, pos, c)) == NXS_STRING_ERROR_DST_SIZE){

		nxs_string_resize(str, str->len + 2);

		rc = nxs_string_ins_char(str, pos, c);
	}

	return rc;
}

/*
 * Функция появилась с версии v0.2-0 r13
 * 
 * Задать новую длину строки. Будет установлен символ конца строки в позицию len (соответствующим образом будет изменено значение длины строки).
 * Проверка наличия других символов '\0' в строке не проверяется.
 *
 * Возвращаемое значение:
 * * Длина результирующей строки
 * * NXS_STRING_ERROR_DST_SIZE	- новая длина строки больше допустимого размера строки str
 * * NXS_STRING_ERROR_NULL_PTR	- указатель на строку "str" равен NULL
 */
ssize_t nxs_string_set_len(nxs_string_t *str, size_t len)
{

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(len >= str->size){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	str->str[len] = '\0';

	str->len = len;

	return str->len;
}

/*
 * Установить терминальный символ '\0' в позицию, соответствующую значению длины строки.
 *
 * Данная функция создаётся для того, чтобы было возможно корректно использовать строки при для сетевом обмене, т.к. при передаче
 * по сети данные передаются как буфер, т.е. игнорируется завершающий символ '\0'. При этом значение длины строки (str->len) верное,
 * а её размер достаточный для добавления терминачального симовола (последнее гарантируетя функцией приёма буфера).
 */
ssize_t nxs_string_len_fix(nxs_string_t *str)
{

	if(str == NULL){

		return NXS_STRING_ERROR_NULL_PTR;
	}

	if(str->size == 0){

		return NXS_STRING_ERROR_DST_SIZE;
	}

	str->str[str->len] = '\0';

	return str->len;
}

/*
 * Получить символ строки str, расположенный в позиции pos
 *
 * Возвращаемое значение:
 * * Требуемый символ
 * * 0 - если pos больше длины строки, либо указатель на строку "str" равен NULL
 */
u_char nxs_string_get_char(nxs_string_t *str, size_t pos)
{

	if(str == NULL){

		return 0;
	}

	if(pos >= str->len){

		return 0;
	}

	return str->str[pos];
}

/*
 * Получить подстроку str со смещением offset
 *
 * Возвращаемое значение:
 * * Указатель на подстроку с требуемым смещением
 * * NULL - если запрошенное смещение больше размера строки или str равно NULL
 *
 * Функция изменилась в версии v0.2-0 r13
 */
u_char *nxs_string_get_substr(nxs_string_t *str, size_t offset)
{

	if(str == NULL){

		return NULL;
	}

	if(offset >= str->size){

		return NULL;
	}

	return str->str + offset;
}

/*
 * Поиск первого вхождения подстроки f_str длиной f_str_len в строку str со смещением offset
 *
 * Возвращаемое значение:
 * * Указатель на найденную подстроку в строке str
 * * NULL - если подстрока не найдена, длина искомой строки больше длины строки str или смещение offset в строке str больше длины строки str, либо
 * 				хотя бы один из указателей "str" или "f_str" равен NULL
 */
u_char *nxs_string_find_substr_first(nxs_string_t *str, size_t offset, u_char *f_str, size_t f_str_len)
{
	size_t i, j;

	if(str == NULL || f_str == NULL){

		return NULL;
	}

	if(f_str_len > str->len){

		return NULL;
	}

	if(offset + 1 > str->len){

		return NULL;
	}

	for(i = offset, j = 0; i < str->len; i++){

		if(str->str[i] == f_str[j]){

			if(j + 1 == f_str_len){

				return str->str + i - j;
			}

			j++;
		}
		else{

			i -= j;

			j = 0;
		}

		/*
		 * Если в строке str осталось для проверки символов меньше, чем необхо проверить в строке f_str
		 */
		if(str->len - (i + 1) < f_str_len - j){

			return NULL;
		}
	}

	return NULL;
}

/*
 * Поиск последнего вхождения подстроки f_str длиной f_str_len в строку str со смещением offset
 *
 * Возвращаемое значение:
 * * Указатель на найденную подстроку в строке str
 * * NULL - если подстрока не найдена, длина искомой строки больше длины строки str или смещение offset в строке str больше длины строки str, либо
 * 				хотя бы один из указателей "str" или "f_str" равен NULL
 */
u_char *nxs_string_find_substr_last(nxs_string_t *str, size_t offset, u_char *f_str, size_t f_str_len)
{
	size_t i, j;

	if(str == NULL || f_str == NULL){

		return NULL;
	}

	if(str->len < f_str_len){

		return NULL;
	}

	if(str->len < offset + 1){

		return NULL;
	}

	for(i = str->len - 1, j = f_str_len - 1; i >= offset; i--){

		if(str->str[i] == f_str[j]){

			if(j == 0){

				return str->str + i;
			}

			j--;
		}
		else{

			i += (f_str_len - 1 - j);

			j = f_str_len - 1;
		}

		/*
		 * Если в строке str осталось для проверки символов меньше, чем необхо проверить в строке f_str
		 */
		if(i - offset < j + 1){

			return NULL;
		}
	}

	return NULL;
}

/*
 * Поиск первого вхождения символа 'c' в строку str со смещением  offset
 *
 * Возвращаемое значение:
 * * Указатель на найденную подстроку в строке str, которая начинается с симвмола 'c'
 * * NULL - если символ не найден или смещение offset в строке str больше длины строки str либо указатель "str" на строку равен NULL
 */
u_char *nxs_string_find_char_first(nxs_string_t *str, size_t offset, u_char c)
{
	size_t i;

	if(str == NULL){

		return NULL;
	}

	if(str->len < offset + 1){

		return NULL;
	}

	for(i = offset; i < str->len; i++){

		if(str->str[i] == c){

			return str->str + i;
		}
	}

	return NULL;
}

/*
 * Поиск последнего вхождения символа 'c' в строку str со смещением  offset
 *
 * Возвращаемое значение:
 * * Указатель на найденную подстроку в строке str, которая начинается с симвмола 'c'
 * * NULL - если символ не найден или смещение offset в строке str больше длины строки str либо указатель "str" на строку равен NULL
 */
u_char *nxs_string_find_char_last(nxs_string_t *str, size_t offset, u_char c)
{
	size_t i;

	if(str == NULL){

		return NULL;
	}

	if(str->len < offset + 1){

		return NULL;
	}

	for(i = str->len - 1; i >= offset; i--){

		if(str->str[i] == c){

			return str->str + i;
		}
	}

	return NULL;
}

/*
 * Заменить в строке 'str' все вхождения подстроки 'f_str' на 'd_str' и записать результат в строку 'dst'.
 * Если указатель на 'dst' == NULL - результат будет записан в 'src' (содержимое строки 'src' при этому будет заменено).
 *
 * max_count означает максимальное число замен, которое будет произведено. max_count == 0 снимает ограничение на число замен.
 *
 * Возвращаемые значения:
 * Количество произведённых замен
 */
size_t nxs_string_subs(nxs_string_t *src, nxs_string_t *dst, nxs_string_t *f_str, nxs_string_t *d_str, size_t max_count)
{
	size_t			count, o;
	u_char			*c;
	nxs_string_t	tmp, *s;

	if(src == NULL || f_str == NULL || d_str == NULL){

		return 0;
	}

	if(dst == NULL){

		nxs_string_init(&tmp);

		s = &tmp;
	}
	else{

		s = dst;
	}

	count	= 0;
	o		= 0;

	while((c = nxs_string_find_substr_first(src, o, f_str->str, f_str->len)) != NULL && (max_count == 0 || count < max_count)){

		nxs_string_ncpy_dyn(s, s->len, src, o, c - src->str - o);

		nxs_string_cat_dyn(s, d_str);

		o = c - src->str + f_str->len;

		count++;
	}

	nxs_string_ncpy_dyn(s, s->len, src, o, src->len - o);

	if(dst == NULL){

		nxs_string_cpy_dyn(src, 0, s, 0);
	}

	return count;
}

/*
 * Функция появилась с версии v0.2-0 r13
 * 
 * Буфер будет расширен динамически
 */
u_char *nxs_string_to_buf(nxs_string_t *str, size_t offset, nxs_buf_t *buf)
{

	if(str == NULL){

		return NULL;
	}

	if(buf == NULL){

		return NULL;
	}

	if(offset > str->len){

		return NULL;
	}

	nxs_buf_cpy_dyn(buf, 0, nxs_string_get_substr(str, offset), str->len - offset + 1);

	return nxs_buf_get_subbuf(buf, 0);
}

/*
 * Получения текста ошибки по её номеру при работе со строками
 */
u_char *nxs_string_strerror(int nxs_str_errno)
{

	switch(nxs_str_errno){

		case NXS_STRING_OK:
			return (u_char *)_NXS_STRING_OK;

		case NXS_STRING_ERROR:
			return (u_char *)_NXS_STRING_ERROR;

		case NXS_STRING_ERROR_NOT_INIT:
			return (u_char *)_NXS_STRING_ERROR_NOT_INIT;

		case NXS_STRING_ERROR_DST_SIZE:
			return (u_char *)_NXS_STRING_ERROR_DST_SIZE;

		case NXS_STRING_ERROR_SRC_SIZE:
			return (u_char *)_NXS_STRING_ERROR_SRC_SIZE;

		case NXS_STRING_ERROR_DST_LEN:
			return (u_char *)_NXS_STRING_ERROR_DST_LEN;

		case NXS_STRING_ERROR_SRC_LEN:
			return (u_char *)_NXS_STRING_ERROR_SRC_LEN;

		case NXS_STRING_ERROR_OFFSET:
			return (u_char *)_NXS_STRING_ERROR_OFFSET;

		case NXS_STRING_ERROR_NOT_CREATED:
			return (u_char *)_NXS_STRING_ERROR_NOT_CREATED;

		case NXS_STRING_ERROR_NULL_PTR:
			return (u_char *)_NXS_STRING_ERROR_NULL_PTR;

		case NXS_STRING_ERROR_SPEC_UNKNOWN:
			return (u_char *)_NXS_STRING_ERROR_SPEC_UNKNOWN;

		case NXS_STRING_ERROR_SPEC_OVERFLOW:
			return (u_char *)_NXS_STRING_ERROR_SPEC_OVERFLOW;
	}

	return (u_char *)_NXS_STRING_ERROR_UNKNOWN;
}

/*
 * Получение длины строки str
 */
size_t nxs_string_len(nxs_string_t *str)
{

	if(str == NULL){

		return 0;
	}

	return str->len;
}

/*
 * Получение размера строки.
 * Выводимое значение на единицу больше допустимого для использования, т.к. в строке присутствует символ '\n'
 */
size_t nxs_string_size(nxs_string_t *str)
{

	if(str == NULL){

		return 0;
	}

	return str->size;
}

/*
 * Получение информации о заполненности строки.
 *
 * Возвращаемое значение:
 * NXS_STRING_NOT_FULL	- в строке есть доступное для использования место
 * NXS_STRING_FULL		- строка полная, доступного для использования места нет
 */
int nxs_string_check_space(nxs_string_t *str)
{

	if(str == NULL){

		return NXS_STRING_FULL;
	}

	if(str->size - str->len > 1){

		return NXS_STRING_NOT_FULL;
	}

	return NXS_STRING_FULL;
}

ssize_t nxs_string_strftime(nxs_string_t *str, u_char *fmt, time_t t)
{
	char		s[1024];
	struct tm	*_t;

	_t = localtime(&t);
	strftime(s, 1024, (const char *)fmt, _t);

	return nxs_string_printf_dyn(str, "%s", s);
}

ssize_t nxs_string_strftime_sec(nxs_string_t *str, u_char *fmt, time_t sec)
{
	u_int			i, j, o, s, v[4];
	nxs_string_t	_fmt;
	ssize_t			rc;
	nxs_bool_t		f;

	struct strftime_s strftime_v[] =
	{
		{'d',	-1},
		{'h',	-1},
		{'m',	-1},
		{'s',	-1},
		{'\0',	-1},
	};

	nxs_string_init2(&_fmt, 0, fmt);

	for(i = 0; i < 4; i++){

		v[i] = 0;
	}

	for(i = 0, s = 0, o = 0; i < _fmt.len; i++){

		switch(s){

			case 0:

				if(_fmt.str[i] == '%'){

					s = 1;
				}

				break;

			case 1:

				if(_fmt.str[i] == '%'){

					s = 0;
				}
				else{

					if(_fmt.str[i] < '0' || _fmt.str[i] > '9'){

						for(f = NXS_FALSE, j = 0; f == NXS_FALSE && strftime_v[j]._f != '\0'; j++){

							if(_fmt.str[i] == strftime_v[j]._f){

								if(o > 3 || strftime_v[j].o != -1){

									rc = NXS_STRING_ERROR_SPEC_OVERFLOW;
									goto error;
								}

								f = NXS_TRUE;

								_fmt.str[i] = 'u';

								strftime_v[j].o = o++;
							}
						}

						if(f == NXS_FALSE){

							rc = NXS_STRING_ERROR_SPEC_UNKNOWN;
							goto error;
						}

						s = 0;
					}
				}

				break;
		}
	}

	if(strftime_v[0].o >= 0){

		v[strftime_v[0].o] = sec / 86400;

		sec -= v[strftime_v[0].o] * 86400;
	}

	if(strftime_v[1].o >= 0){

		v[strftime_v[1].o] = sec / 3600;

		sec -= v[strftime_v[1].o] * 3600;
	}

	if(strftime_v[2].o >= 0){

		v[strftime_v[2].o] = sec / 60;

		sec -= v[strftime_v[2].o] * 60;
	}

	if(strftime_v[3].o >= 0){

		v[strftime_v[3].o] = sec;
	}

	rc = nxs_string_printf_dyn(str, (const char *)nxs_string_str(&_fmt), v[0], v[1], v[2], v[3]);

error:

	nxs_string_free(&_fmt);

	return rc;
}

void nxs_string_escape(nxs_string_t *str_to, nxs_string_t *str_from, nxs_string_escape_types_t type)
{
	size_t			i;
	nxs_string_t	tmp_str;
	nxs_bool_t		f;

	if(str_to == NULL){

		return;
	}

	nxs_string_init(&tmp_str);

	if(str_from != NULL){

		nxs_string_cpy_dyn(&tmp_str, 0, str_from, 0);
	}
	else{

		nxs_string_cpy_dyn(&tmp_str, 0, str_to, 0);
	}

	nxs_string_clear(str_to);

	for(i = 0; i < tmp_str.len; i++){

		f = NXS_NO;

		switch(tmp_str.str[i]){

			case (u_char)'\\':

				nxs_string_char_cat_dyn(str_to, (u_char *)"\\\\");
				break;

			case (u_char)'"':

				nxs_string_char_cat_dyn(str_to, (u_char *)"\\\"");
				break;

			case (u_char)'\'':

				if(type == NXS_STRING_ESCAPE_TYPE_JSON){

					f = NXS_YES;
				}
				else{

					nxs_string_char_cat_dyn(str_to, (u_char *)"\\\'");
				}

				break;

			case (u_char)'/':

				nxs_string_char_cat_dyn(str_to, (u_char *)"\\/");
				break;

			case (u_char)'\b':

				nxs_string_char_cat_dyn(str_to, (u_char *)"\\b");
				break;

			case (u_char)'\f':

				nxs_string_char_cat_dyn(str_to, (u_char *)"\\f");
				break;

			case (u_char)'\n':

				nxs_string_char_cat_dyn(str_to, (u_char *)"\\n");
				break;

			case (u_char)'\r':

				nxs_string_char_cat_dyn(str_to, (u_char *)"\\r");
				break;

			case (u_char)'\t':

				nxs_string_char_cat_dyn(str_to, (u_char *)"\\t");
				break;

			default:

				f = NXS_YES;

				break;
		}

		if(f == NXS_YES){

			nxs_string_char_add_char_dyn(str_to, tmp_str.str[i]);
		}
	}

	nxs_string_free(&tmp_str);
}

static ssize_t nxs_string_vprintf_core_dyn(nxs_string_t *str, size_t offset, const char *fmt, va_list ap)
{
	size_t						i, j, k;
	u_char						zero, *c;
	size_t						width, precision;
	int64_t						i64;
	uint64_t					ui64;
	nxs_bool_t					f;
	nxs_string_t				*s;
	nxs_buf_t					*b;
	char						tmp_tpl[NXS_STRING_TMP_STR_SIZE], tmp_str[NXS_STRING_TMP_STR_SIZE];
	int							d;
	double						lf;

	if(str == NULL){

		return 0;
	}

	if(offset + 1 >= str->size){

		nxs_string_resize(str, offset + 2);
	}

	nxs_string_set_len(str, offset);

	for(i = 0; fmt[i] != '\0'; i++){

		if(fmt[i] == '%'){

			zero		= ' ';
			width		= 0;
			precision	= 6;
			k			= 1;
			f			= NXS_NO;

			/* zeroes fill */
			if(fmt[i + k] != '\0' && fmt[i + k] == '0'){

				zero = '0';

				k++;
			}

			/* width */
			for(; fmt[i + k] != '\0' && fmt[i + k] >= '0' && fmt[i + k] <= '9'; k++){

				width = width * 10 + (fmt[i + k] - '0');
			}

			/* precision */
			if(fmt[i + k] != '\0' && fmt[i + k] == '.'){

				for(precision = 0, k++; fmt[i + k] != '\0' && fmt[i + k] >= '0' && fmt[i + k] <= '9'; k++){

					precision = precision * 10 + (fmt[i + k] - '0');
				}
			}

			if(fmt[i + k] != '\0'){

				/* types */
				switch(fmt[i + k]){

					case 'z':

						k++;

						if(fmt[i + k] != '\0'){

							switch(fmt[i + k]){

								case 'd':

									/* zd */

									i64 = (int64_t)va_arg(ap, ssize_t);

									nxs_string_vprintf_int(str, i64, zero, width, 0);

									f = NXS_YES;

									break;

								case 'u':

									/* zu */

									ui64 = (uint64_t)va_arg(ap, size_t);

									nxs_string_vprintf_uint(str, NXS_NO, ui64, zero, width, 0);

									f = NXS_YES;

									break;
							}
						}

						break;

					case 'l':

						k++;

						if(fmt[i + k] != '\0'){

							switch(fmt[i + k]){

								case 'd':

									/* ld */

									i64 = (int64_t)va_arg(ap, long);

									nxs_string_vprintf_int(str, i64, zero, width, 0);

									f = NXS_YES;

									break;

								case 'u':

									/* lu */

									ui64 = (uint64_t)va_arg(ap, u_long);

									nxs_string_vprintf_uint(str, NXS_NO, ui64, zero, width, 0);

									f = NXS_YES;

									break;

								case 'f':

									lf = (double)va_arg(ap, double);

									sprintf(tmp_tpl, NXS_STRING_DOUBLE_STR_TPL, zero == '0' ? "0" : "", width, precision);
									sprintf(tmp_str, (const char *)tmp_tpl, lf);

									nxs_string_char_cat_dyn(str, (u_char *)tmp_str);

									f = NXS_YES;

									break;

								case 'l':

									k++;

									if(fmt[i + k] != '\0'){

										switch(fmt[i + k]){

											case 'd':

												/* lld */

												i64 = (int64_t)va_arg(ap, long long int);

												nxs_string_vprintf_int(str, i64, zero, width, 0);

												f = NXS_YES;

												break;

											case 'u':

												/* llu */

												ui64 = (int64_t)va_arg(ap, unsigned long long int);

												nxs_string_vprintf_uint(str, NXS_NO, ui64, zero, width, 0);

												f = NXS_YES;

												break;
										}
									}

									break;
							}
						}

						break;

					case 'u':

						ui64 = (uint64_t)va_arg(ap, u_int);

						nxs_string_vprintf_uint(str, NXS_NO, ui64, zero, width, 0);

						f = NXS_YES;

						break;

					case 'd':

						i64 = (int64_t)va_arg(ap, int);

						nxs_string_vprintf_int(str, i64, zero, width, 0);

						f = NXS_YES;

						break;

					case 'c':

						d = (int)va_arg(ap, int);

						nxs_string_char_add_char_dyn(str, (u_char)(d & 0xff));

						f = NXS_YES;

						break;

					case 's':

						c = (u_char *)va_arg(ap, u_char *);

						for(j = 0; c[j] != (u_char)'\0'; j++){

							nxs_string_char_add_char_dyn(str, c[j]);
						}

						f = NXS_YES;

						break;

					case 'r':

						s = (nxs_string_t *)va_arg(ap, nxs_string_t *);

						nxs_string_cat_dyn(str, s);

						f = NXS_YES;

						break;

					case 'R':

						b = (nxs_buf_t *)va_arg(ap, nxs_buf_t *);

						nxs_string_char_ncat_dyn(str, nxs_buf_get_subbuf(b, 0), nxs_buf_get_len(b));

						f = NXS_YES;

						break;

					case 'f':

						/* Не мудрствуя лукаво ... */

						lf = (double)va_arg(ap, double);

						sprintf(tmp_tpl, NXS_STRING_FLOAT_STR_TPL, zero == '0' ? "0" : "", width, precision);
						sprintf(tmp_str, (const char *)tmp_tpl, lf);

						nxs_string_char_cat_dyn(str, (u_char *)tmp_str);

						f = NXS_YES;

						break;

					case 'x':


						i64 = (int64_t)va_arg(ap, int);

						sprintf(tmp_tpl, NXS_STRING_HEX_STR_TPL, zero == '0' ? "0" : "", width);
						sprintf(tmp_str, (const char *)tmp_tpl, i64);

						nxs_string_char_cat_dyn(str, (u_char *)tmp_str);

						f = NXS_YES;

						break;

					case 'o':


						i64 = (int64_t)va_arg(ap, int);

						sprintf(tmp_tpl, NXS_STRING_OCT_STR_TPL, zero == '0' ? "0" : "", width);
						sprintf(tmp_str, (const char *)tmp_tpl, i64);

						nxs_string_char_cat_dyn(str, (u_char *)tmp_str);

						f = NXS_YES;

						break;

					case '%':

						if(k == 1){

							nxs_string_char_add_char_dyn(str, (u_char)'%');

							f = NXS_YES;
						}
						else{

							f = NXS_NO;
						}

						break;
				}
			}

			if(f == NXS_YES){

				i += k;
			}
			else{

				nxs_string_char_add_char_dyn(str, (u_char)fmt[i]);
			}
		}
		else{

			nxs_string_char_add_char_dyn(str, (u_char)fmt[i]);
		}
	}

	return str->len;
}

static void nxs_string_vprintf_uint(nxs_string_t *str, nxs_bool_t sign, uint64_t ui64, u_char zero, size_t width, size_t precision)
{
	u_char		tmp[NXS_STRING_MAX_UINT_BUF_SIZE + 1], *s;
	uint32_t	ui32;
	size_t		l, ls;

	s = tmp + NXS_STRING_MAX_UINT_BUF_SIZE;

	if(ui64 <= 0xffffffff){

		ui32 = (uint32_t)ui64;

		do{

			*(--s) = (u_char)(ui32 % 10 + '0');
		}
		while(ui32 /= 10);
	}
	else{

		do{

			*(--s) = (u_char)(ui64 % 10 + '0');
		}
		while(ui64 /= 10);
	}

	if(sign == NXS_YES){

		*(--s) = (u_char)'-';
	}

	for(l = (tmp + NXS_STRING_MAX_UINT_BUF_SIZE) - s; l < width; l++){

		nxs_string_char_add_char_dyn(str, zero);
	}

	ls	= nxs_string_len(str);
	l	= (tmp + NXS_STRING_MAX_UINT_BUF_SIZE) - s;

	nxs_string_char_ncpy_dyn(str, ls, s, l);

	for(l = (tmp + NXS_STRING_MAX_UINT_BUF_SIZE) - s; l < precision; l++){

		nxs_string_char_add_char_dyn(str, '0');
	}
}

static void nxs_string_vprintf_int(nxs_string_t *str, int64_t i64, u_char zero, size_t width, size_t precision)
{
	uint64_t	ui64;
	nxs_bool_t	sign;

	if(i64 < 0){

		ui64 = (uint64_t)(-i64);

		sign = NXS_YES;
	}
	else{

		ui64 = (uint64_t)(i64);

		sign = NXS_NO;
	}

	nxs_string_vprintf_uint(str, sign, ui64, zero, width, precision);
}
