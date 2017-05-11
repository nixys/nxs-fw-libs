// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */

#define nxs_wstring_check_not_created(str)	if(str->size == 0 || str->str == NULL){ \
							return NXS_WSTRING_E_NOT_CREATED; \
						}

/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

void nxs_wstring_ctx_init(void)
{

	setlocale(LC_ALL, "");
}

nxs_wstring_t *nxs_wstring_malloc(size_t size, wchar_t *new_str)
{
	nxs_wstring_t *str = NULL;
	size_t         s;

	str = nxs_calloc(str, sizeof(nxs_wstring_t));

	if(new_str != NULL) {

		if(size == 0) {

			s = wcslen((const wchar_t *)new_str) + 1;
		}
		else {

			s = size;
		}

		nxs_wstring_create(str, s, new_str);
	}
	else {

		if(size == 0) {

			return str;
		}

		nxs_wstring_create(str, size, NULL);
	}

	return str;
}

nxs_wstring_t *nxs_wstring_destroy(nxs_wstring_t *str)
{

	if(str == NULL) {

		return NULL;
	}

	nxs_wstring_free(str);

	return nxs_free(str);
}

/*
 * Инициализация (обнуление) строки.
 * Рекомендуется использовать эту функцию только при объявлении строки
 */
void nxs_wstring_init(nxs_wstring_t *str)
{

	if(str == NULL) {

		return;
	}

	str->str  = NULL;
	str->size = 0;
	str->len  = 0;
}

/*
 * Инициализация (обнуление) строки и одновременное заполнение данным.
 * Рекомендуется использовать эту функцию только при объявлении строки
 *
 * Возвращаемое значение:
 * * Длина новой строки
 * * Коды функции nxs_string_create(). В этом случае строка будет в инициализированном состоянии (т.е. память под строку выделена не будет)
 */
ssize_t nxs_wstring_init2(nxs_wstring_t *str, size_t size, wchar_t *new_str)
{
	ssize_t rc;

	if(str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	str->str  = NULL;
	str->size = 0;
	str->len  = 0;

	if((rc = nxs_wstring_create(str, size, new_str)) < 0) {

		nxs_wstring_free(str);
	}

	return rc;
}

/*
 * Создание строки.
 * Выделение памяти под строку (необходимо инициализировать строку до использования этой функции).
 * Если new_str != NULL и при этом size == 0, то размер новой строки будет вычислен автоматически как длина строки new_str + 1 ('\0')
 * Если new_str == NULL, а size > 0 - будет создана строка размером size и нулевой длиной.
 * Если и new_str == NULL, и size == 0 - строка создана не будет, функция вернёт ошибку "NXS_WSTRING_E_NOT_CREATED".
 *
 * Возвращаемое значение:
 * * Длина новой строки
 * * NXS_WSTRING_E_INIT			- если строка не инициализирована (для предотвращения утечек памяти)
 * * NXS_WSTRING_E_DST_SIZE		- строка new_str имеет длину, превышающую размер size, заявленный для строки str
 * * NXS_WSTRING_E_NOT_CREATED	- строка не создана, если new_str == NULL и size == 0 (не выделяется память нулевой длины)
 * * NXS_WSTRING_E_PTR			- указатель на строку "str" равен NULL
 */
ssize_t nxs_wstring_create(nxs_wstring_t *str, size_t size, wchar_t *new_str)
{
	size_t  s;
	ssize_t len;

	if(str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(str->str != NULL) {

		return NXS_WSTRING_E_INIT;
	}

	if(new_str != NULL) {

		if(size == 0) {

			/*
			 * Необходимо вычислить размер под новую строку
			 */

			s = wcslen((const wchar_t *)new_str) + 1;
		}
		else {

			s = size;
		}

		str->str  = nxs_calloc(NULL, sizeof(wchar_t) * s);
		str->size = s;

		if((len = nxs_wstring_wchar_cpy(str, 0, new_str)) < 0) {

			return len;
		}

		str->len = len;
	}
	else {

		if(size == 0) {

			/*
			 * Если требуемый размер строки 0
			 */

			return NXS_WSTRING_E_NOT_CREATED;
		}

		str->str  = nxs_calloc(NULL, sizeof(wchar_t) * size);
		str->size = size;
		nxs_wstring_clear(str);
	}

	return str->len;
}

/*
 * Освобождение памяти, выделенной под строку и инициализация строки для повторного использования)
 *
 * Возвращаемое значение:
 * * NXS_WSTRING_OK				- если ошибок не возникло
 * * NXS_WSTRING_E_NOT_CREATED	- если строка не созадана
 * * NXS_WSTRING_E_PTR			- указатель на строку "str" равен NULL
 */
int nxs_wstring_free(nxs_wstring_t *str)
{

	if(str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(str->size == 0 || str->str == NULL) {

		return NXS_WSTRING_E_NOT_CREATED;
	}

	str->str = nxs_free(str->str);

	str->len  = 0;
	str->size = 0;

	return NXS_WSTRING_E_OK;
}

/*
 * Изменение размера строки str до нового размера new_size.
 * Если новый рамер строки меньше текущей длины строки - строка будет обрезана (с учётом завершающего символа)
 * Если new_size == 0 - память, выделенная под строку, будет освобождена, а строка инициализирована.
 * Если строка ещё не создана - она будет создана с размером new_size и нулевой длиной
 *
 * Возвращаемое значение:
 * * Новый рамер строки str
 * * NXS_WSTRING_E_PTR		- указатель на строку "str" равен NULL
 */
ssize_t nxs_wstring_resize(nxs_wstring_t *str, size_t new_size)
{

	if(str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	/*
	 * Если new_size == 0 - память, выделенная под строку, будет освобождена, а строка инициализирована.
	 */
	if(new_size == 0) {

		nxs_wstring_free(str);

		return str->size;
	}

	if(str->size == 0 || str->str == NULL) {

		/*
		 * Если строка ещё не создана - создаём её
		 */

		nxs_wstring_create(str, new_size, NULL);
	}
	else {

		/*
		 * Если строка уже создана - изменяем её размер
		 */

		str->str = nxs_realloc(str->str, sizeof(wchar_t) * new_size);

		if(new_size < str->len + 1) {

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
 * * NXS_WSTRING_OK				- если ошибок не возникло
 * * NXS_WSTRING_E_NOT_CREATED	- если строка не созадана
 * * NXS_WSTRING_E_PTR			- указатель на строку "str" равен NULL
 */
int nxs_wstring_clear(nxs_wstring_t *str)
{

	if(str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(str->size == 0 || str->str == NULL) {

		return NXS_WSTRING_E_NOT_CREATED;
	}

	str->str[0] = '\0';
	str->len    = 0;

	return NXS_WSTRING_E_OK;
}

/*
 * Копирование подстроки src со смещением offset_src в строку dst со смещением offset_dst от её начала
 *
 * Возвращаемые значения:
 * * Новая длина строки dst
 * * NXS_WSTRING_E_OFFSET	- смещение offset_dst или offset_src больше длин соответствующих строк
 * * NXS_WSTRING_E_DST_SIZE	- результирующая длина строки dst больше, чем допустимый размер строки dst
 * * NXS_WSTRING_E_PTR		- указатель на строку "dst" или "src" равен NULL
 */
ssize_t nxs_wstring_cpy(nxs_wstring_t *dst, size_t offset_dst, nxs_wstring_t *src, size_t offset_src)
{

	if(dst == NULL || src == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(offset_src > src->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(offset_dst > dst->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(src->len - offset_src + 1 > dst->size - offset_dst) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	nxs_memcpy(dst->str + offset_dst, src->str + offset_src, sizeof(wchar_t) * (src->len - offset_src));

	dst->len           = offset_dst + src->len - offset_src;
	dst->str[dst->len] = '\0';

	return dst->len;
}

ssize_t nxs_wstring_cpy_dyn(nxs_wstring_t *dst, size_t offset_dst, nxs_wstring_t *src, size_t offset_src)
{
	ssize_t rc;

	if((rc = nxs_wstring_cpy(dst, offset_dst, src, offset_src)) == NXS_WSTRING_E_DST_SIZE) {

		nxs_wstring_resize(dst, src->len - offset_src + offset_dst + 1);

		rc = nxs_wstring_cpy(dst, offset_dst, src, offset_src);
	}

	return rc;
}

/*
 * Копирование n символов подстроки src со смещением offset_src в строку dst со смещением offset_dst от её начала
 *
 * Возвращаемые значения:
 * * Новая длина строки dst
 * * NXS_WSTRING_E_OFFSET	- смещение offset_dst или offset_src больше длин соответствующих строк
 * * NXS_WSTRING_E_SRC_SIZE	- количество байт n, которые требуется скопироваться со смещением offset_src превышает длину строки src
 * * NXS_WSTRING_E_DST_SIZE	- результирующая длина строки dst больше, чем допустимый размер строки dst
 * * NXS_WSTRING_E_PTR		- указатель на строку "dst" или "src" равен NULL
 */
ssize_t nxs_wstring_ncpy(nxs_wstring_t *dst, size_t offset_dst, nxs_wstring_t *src, size_t offset_src, size_t n)
{

	if(dst == NULL || src == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(offset_src > src->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(offset_dst > dst->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(offset_src + n > src->len) {

		return NXS_WSTRING_E_SRC_SIZE;
	}

	if(n + 1 > dst->size - offset_dst) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	nxs_memcpy(dst->str + offset_dst, src->str + offset_src, sizeof(wchar_t) * n);

	dst->len           = offset_dst + n;
	dst->str[dst->len] = '\0';

	return dst->len;
}

ssize_t nxs_wstring_ncpy_dyn(nxs_wstring_t *dst, size_t offset_dst, nxs_wstring_t *src, size_t offset_src, size_t n)
{
	ssize_t rc;

	if((rc = nxs_wstring_ncpy(dst, offset_dst, src, offset_src, n)) == NXS_WSTRING_E_DST_SIZE) {

		nxs_wstring_resize(dst, offset_dst + n + 1);

		rc = nxs_wstring_ncpy(dst, offset_dst, src, offset_src, n);
	}

	return rc;
}

/*
 * Добавление строки src в конец строки dst
 *
 * Возвращаемое значение:
 * * Новая длина строки dst
 * * NXS_WSTRING_E_DST_SIZE	- если результирующий размер строки превышает допустимый для использования размер строки dst
 * * NXS_WSTRING_E_PTR		- указатель на строку "dst" или "src" равен NULL
 */
ssize_t nxs_wstring_cat(nxs_wstring_t *dst, nxs_wstring_t *src)
{

	if(dst == NULL || src == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(dst->len + src->len + 1 > dst->size) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	nxs_memcpy(dst->str + dst->len, src->str, sizeof(wchar_t) * src->len);

	dst->len += src->len;
	dst->str[dst->len] = '\0';

	return dst->len;
}

ssize_t nxs_wstring_cat_dyn(nxs_wstring_t *dst, nxs_wstring_t *src)
{
	ssize_t rc;

	if((rc = nxs_wstring_cat(dst, src)) == NXS_WSTRING_E_DST_SIZE) {

		nxs_wstring_resize(dst, dst->len + src->len + 1);

		rc = nxs_wstring_cat(dst, src);
	}

	return rc;
}

/*
 * Добавление n байт строки src в конец строки dst
 *
 * Возвращаемое значение:
 * * Новая длина строки dst
 * * NXS_WSTRING_E_SRC_SIZE	- если требуемое количество байт 'n' превышает длину строки src
 * * NXS_WSTRING_E_DST_SIZE	- если результирующий размер строки превышает допустимый для использования размер строки dst
 * * NXS_WSTRING_E_PTR		- указатель на строку "dst" или "src" равен NULL
 */
ssize_t nxs_wstring_ncat(nxs_wstring_t *dst, nxs_wstring_t *src, size_t n)
{

	if(dst == NULL || src == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(n > src->len) {

		return NXS_WSTRING_E_SRC_SIZE;
	}

	if(dst->len + n + 1 > dst->size) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	nxs_memcpy(dst->str + dst->len, src->str, sizeof(wchar_t) * n);

	dst->len += n;
	dst->str[dst->len] = '\0';

	return dst->len;
}

ssize_t nxs_wstring_ncat_dyn(nxs_wstring_t *dst, nxs_wstring_t *src, size_t n)
{
	ssize_t rc;

	if((rc = nxs_wstring_ncat(dst, src, n)) == NXS_WSTRING_E_DST_SIZE) {

		nxs_wstring_resize(dst, dst->len + n + 1);

		rc = nxs_wstring_ncat(dst, src, n);
	}

	return rc;
}

/*
 * Стравнение строки str1 со смещением  offset1 байт от начала со строкой str2 со смещением offset2 байт
 *
 * Возвращаемое значение:
 * * NXS_WSTRING_CMP_EQ			- сравниваемые подстроки одинаковы
 * * NXS_WSTRING_CMP_NE			- сравниваемые подстроки различны (хотя бы в одном символе)
 * * NXS_WSTRING_E_OFFSET		- заданное смещение offset1 или offset2 больше длин соответствующих строк
 * * NXS_WSTRING_E_PTR			- указатель на строку "str1" или "str2" равен NULL
 */
int nxs_wstring_cmp(nxs_wstring_t *str1, size_t offset1, nxs_wstring_t *str2, size_t offset2)
{
	size_t i;

	if(str1 == NULL || str2 == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(offset1 > str1->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(offset2 > str2->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(str1->len - offset1 != str2->len - offset2) {

		return NXS_WSTRING_CMP_NE;
	}

	for(i = 0; i < str1->len - offset1; i++) {

		if(str1->str[offset1 + i] != str2->str[offset2 + i]) {

			return NXS_WSTRING_CMP_NE;
		}
	}

	return NXS_WSTRING_CMP_EQ;
}

/*
 * Стравнение строки str1 со смещением  offset1 байт от начала со строкой str2 со смещением offset2 байт, сравниваются n первых байт
 *
 * Возвращаемое значение:
 * * NXS_WSTRING_CMP_EQ			- сравниваемые подстроки одинаковы
 * * NXS_WSTRING_CMP_NE			- сравниваемые подстроки различны (хотя бы в одном символе)
 * * NXS_WSTRING_E_OFFSET		- заданное смещение offset1 или offset2 больше длин соответствующих строк
 * * NXS_WSTRING_E_PTR			- указатель на строку "str1" или "str2" равен NULL
 */
int nxs_wstring_ncmp(nxs_wstring_t *str1, size_t offset1, nxs_wstring_t *str2, size_t offset2, size_t n)
{
	size_t i;

	if(str1 == NULL || str2 == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(offset1 > str1->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(offset2 > str2->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(n > str1->len - offset1 || n > str2->len - offset2) {

		return NXS_WSTRING_CMP_NE;
	}

	for(i = 0; i < n; i++) {

		if(str1->str[offset1 + i] != str2->str[offset2 + i]) {

			return NXS_WSTRING_CMP_NE;
		}
	}

	return NXS_WSTRING_CMP_EQ;
}

/*
 * Скопировать строку ch_str в строку str со смещением offset в строке str относительно её начала
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_WSTRING_E_OFFSET	- если смещение offset больше длины строки str
 * * NXS_WSTRING_E_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_WSTRING_E_PTR		- указатель на строку "str" или "ch_str" равен NULL
 */
ssize_t nxs_wstring_wchar_cpy(nxs_wstring_t *str, size_t offset, wchar_t *ch_str)
{
	size_t len;

	if(str == NULL || ch_str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(offset > 0 && offset > str->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	len = wcslen((const wchar_t *)ch_str);

	if(offset + len + 1 > str->size) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	nxs_memcpy(str->str + offset, ch_str, sizeof(wchar_t) * len);

	str->len           = offset + len;
	str->str[str->len] = '\0';

	return str->len;
}

ssize_t nxs_wstring_wchar_cpy_dyn(nxs_wstring_t *str, size_t offset, wchar_t *ch_str)
{
	size_t  len;
	ssize_t rc;

	if(ch_str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	len = wcslen((const wchar_t *)ch_str);

	if((rc = nxs_wstring_wchar_cpy(str, offset, ch_str)) == NXS_WSTRING_E_DST_SIZE) {

		nxs_wstring_resize(str, offset + len + 1);

		rc = nxs_wstring_wchar_cpy(str, offset, ch_str);
	}

	return rc;
}

/*
 * Скопировать n-байт строки ch_str в строку str со смещением offset в строке str относительно её начала
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_WSTRING_E_OFFSET	- если смещение offset больше длины строки str
 * * NXS_WSTRING_E_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_WSTRING_E_PTR		- указатель на строку "str" или "ch_str" равен NULL
 *
 * Функция изменилась в версии v0.2-0 r13
 */
ssize_t nxs_wstring_wchar_ncpy(nxs_wstring_t *str, size_t offset, wchar_t *ch_str, size_t n)
{

	if(str == NULL || ch_str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(offset > 0 && offset > str->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(offset + n + 1 > str->size) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	nxs_memcpy(str->str + offset, ch_str, sizeof(wchar_t) * n);

	str->len           = offset + n;
	str->str[str->len] = '\0';

	return str->len;
}

ssize_t nxs_wstring_wchar_ncpy_dyn(nxs_wstring_t *str, size_t offset, wchar_t *ch_str, size_t n)
{
	ssize_t rc;

	if((rc = nxs_wstring_wchar_ncpy(str, offset, ch_str, n)) == NXS_WSTRING_E_DST_SIZE) {

		nxs_wstring_resize(str, offset + n + 1);

		rc = nxs_wstring_wchar_ncpy(str, offset, ch_str, n);
	}

	return rc;
}

/*
 * Добавить строку ch_str к строке str
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_WSTRING_E_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_WSTRING_E_PTR		- указатель на строку "str" или "ch_str" равен NULL
 */
ssize_t nxs_wstring_wchar_cat(nxs_wstring_t *str, wchar_t *ch_str)
{
	size_t len;

	if(str == NULL || ch_str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	len = wcslen((const wchar_t *)ch_str);

	if(str->len + len + 1 > str->size) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	nxs_memcpy(str->str + str->len, ch_str, sizeof(wchar_t) * len);

	str->len += len;
	str->str[str->len] = '\0';

	return str->len;
}

ssize_t nxs_wstring_wchar_cat_dyn(nxs_wstring_t *str, wchar_t *ch_str)
{
	size_t  len;
	ssize_t rc;

	if(ch_str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	len = wcslen((const wchar_t *)ch_str);

	if((rc = nxs_wstring_wchar_cat(str, ch_str)) == NXS_WSTRING_E_DST_SIZE) {

		nxs_wstring_resize(str, str->len + len + 1);

		rc = nxs_wstring_wchar_cat(str, ch_str);
	}

	return rc;
}

/*
 * Добавить n первых байт строки ch_str к строке str
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_WSTRING_E_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_WSTRING_E_PTR		- указатель на строку "str" или "ch_str" равен NULL
 *
 * Функция изменилась в версии v0.2-0 r13
 */
ssize_t nxs_wstring_wchar_ncat(nxs_wstring_t *str, wchar_t *ch_str, size_t n)
{

	if(str == NULL || ch_str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(str->len + n + 1 > str->size) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	nxs_memcpy(str->str + str->len, ch_str, sizeof(wchar_t) * n);

	str->len += n;
	str->str[str->len] = '\0';

	return str->len;
}

ssize_t nxs_wstring_wchar_ncat_dyn(nxs_wstring_t *str, wchar_t *ch_str, size_t n)
{
	ssize_t rc;

	if((rc = nxs_wstring_wchar_ncat(str, ch_str, n)) == NXS_WSTRING_E_DST_SIZE) {

		nxs_wstring_resize(str, str->len + n + 1);

		rc = nxs_wstring_wchar_ncat(str, ch_str, n);
	}

	return rc;
}

/*
 * Стравнение строки str со смещением  offset байт от начала со строкой ch_str (u_char)
 *
 * Возвращаемое значение:
 * * NXS_WSTRING_CMP_EQ			- сравниваемые подстроки одинаковы
 * * NXS_WSTRING_CMP_NE			- сравниваемые подстроки различны (хотя бы в одном символе)
 * * NXS_WSTRING_E_OFFSET		- заданное смещение offset больше длины строки str
 * * NXS_WSTRING_E_PTR			- указатель на строку "str" или "ch_str" равен NULL
 */
int nxs_wstring_wchar_cmp(nxs_wstring_t *str, size_t offset, wchar_t *ch_str)
{
	size_t i, len;

	if(str == NULL || ch_str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(offset + 1 > str->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	len = wcslen((const wchar_t *)ch_str);

	if(str->len - offset != len) {

		return NXS_WSTRING_CMP_NE;
	}

	for(i = 0; i < str->len - offset; i++) {

		if(str->str[offset + i] != ch_str[i]) {

			return NXS_WSTRING_CMP_NE;
		}
	}

	return NXS_WSTRING_CMP_EQ;
}

/*
 * Стравнение строки str со смещением  offset байт от начала со строкой ch_str, сравниваются n первых байт
 *
 * Возвращаемое значение:
 * * NXS_WSTRING_CMP_EQ			- сравниваемые подстроки одинаковы
 * * NXS_WSTRING_CMP_NE			- сравниваемые подстроки различны (хотя бы в одном символе)
 * * NXS_WSTRING_E_OFFSET		- заданное смещение offset больше длины строки str
 * * NXS_WSTRING_E_PTR			- указатель на строку "str" или "ch_str" равен NULL
 *
 */
int nxs_wstring_wchar_ncmp(nxs_wstring_t *str, size_t offset, wchar_t *ch_str, size_t n)
{
	size_t i;

	if(str == NULL || ch_str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(offset + 1 > str->len) {

		return NXS_WSTRING_E_OFFSET;
	}

	if(n > str->len) {

		return NXS_WSTRING_CMP_NE;
	}

	for(i = 0; i < n; i++) {

		if(str->str[offset + i] != ch_str[i]) {

			return NXS_WSTRING_CMP_NE;
		}
	}

	return NXS_WSTRING_CMP_EQ;
}

/*
 * Добавить символ "c" к строке str
 *
 * Возвращаемое значение:
 * * Новое значение длины строки str
 * * NXS_WSTRING_E_DST_SIZE	- если результирующая длина строки больше допустимого размера строки str
 * * NXS_WSTRING_E_PTR		- указатель на строку "str" или "ch_str" равен NULL
 */
ssize_t nxs_wstring_wchar_add_wchar(nxs_wstring_t *str, wchar_t c)
{

	if(str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(c == '\0') {

		if(str->len + 1 > str->size) {

			return NXS_WSTRING_E_DST_SIZE;
		}

		str->str[str->len] = c;
	}
	else {

		if(str->len + 2 > str->size) {

			return NXS_WSTRING_E_DST_SIZE;
		}

		str->str[str->len] = c;

		str->len++;
		str->str[str->len] = '\0';
	}

	return str->len;
}

ssize_t nxs_wstring_wchar_add_wchar_dyn(nxs_wstring_t *str, wchar_t c)
{
	ssize_t rc;

	if((rc = nxs_wstring_wchar_add_wchar(str, c)) == NXS_WSTRING_E_DST_SIZE) {

		nxs_wstring_resize(str, str->len + 2);

		rc = nxs_wstring_wchar_add_wchar(str, c);
	}

	return rc;
}

void nxs_wstring_basename(nxs_wstring_t *dst, nxs_wstring_t *path)
{
	size_t  i, len;
	ssize_t l;

	if(dst == NULL || path == NULL) {

		return;
	}

	len = path->len;

	if(len == 0) {

		nxs_wstring_wchar_cpy_dyn(dst, 0, (wchar_t *)".");

		return;
	}

	while(len > 1 && path->str[len - 1] == L'/') {

		len--;
	}

	if(len == 1) {

		nxs_wstring_ncpy_dyn(dst, 0, path, 0, 1);

		return;
	}

	for(l = len - 1; l >= 0 && path->str[l] != L'/'; l--)
		;

	l++;

	if(dst->size < len - l + 1) {

		nxs_wstring_resize(dst, len - l + 1);
	}

	for(i = l; i < len; i++) {

		dst->str[i - l] = path->str[i];
	}

	dst->str[i - l] = L'\0';
	dst->len        = i - l;
}

void nxs_wstring_dirname(nxs_wstring_t *dst, nxs_wstring_t *path)
{
	ssize_t l, i;

	if(dst == NULL || path == NULL) {

		return;
	}

	for(l = path->len - 1; l >= 0 && path->str[l] == L'/'; l--)
		;
	for(; l >= 0 && path->str[l] != L'/'; l--)
		;
	for(; l >= 0 && path->str[l] == L'/'; l--)
		;

	if(l < 0) {

		if(path->str[0] == L'/') {

			nxs_wstring_wchar_cpy_dyn(dst, 0, (wchar_t *)"/");
		}
		else {

			nxs_wstring_wchar_cpy_dyn(dst, 0, (wchar_t *)".");
		}
	}
	else {

		if(dst->size < (size_t)l + 2) {

			nxs_wstring_resize(dst, l + 2);
		}

		for(i = 0; i <= l; i++) {

			dst->str[i] = path->str[i];
		}

		dst->str[i] = L'\0';
		dst->len    = i;
	}
}

/*
 * Поместить символ 'c' в строку str в позицию pos (символ, находящийся в позиции pos будет заменён новым).
 * Если символ добавляется в конец строки - длина строки будет увеличина в рамках размеров строки
 *
 * Возвращаемое значение:
 * * Длина результирующей строки
 * * NXS_WSTRING_E_NOT_CREATED	- строка не создана
 * * NXS_WSTRING_E_DST_LEN		- требуемая позиция символа находится за границами строки (больше её длины)
 * * NXS_WSTRING_E_DST_SIZE		- в строке нет доступного места для добавления символа
 * * NXS_WSTRING_E_PTR			- указатель на строку "str" равен NULL
 */
ssize_t nxs_wstring_set_wchar(nxs_wstring_t *str, size_t pos, wchar_t c)
{

	if(str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	nxs_wstring_check_not_created(str);

	if(pos > str->len) {

		return NXS_WSTRING_E_DST_LEN;
	}

	if(pos == str->len) {

		return nxs_wstring_wchar_add_wchar(str, c);
	}

	str->str[pos] = c;

	return str->len;
}

/*
 * Задать новую длину строки. Будет установлен символ конца строки в позицию len (соответствующим образом будет изменено значение длины
 * строки).
 * Проверка наличия других символов '\0' в строке не проверяется.
 *
 * Возвращаемое значение:
 * * Длина результирующей строки
 * * NXS_WSTRING_E_DST_SIZE	- новая длина строки больше допустимого размера строки str
 * * NXS_WSTRING_E_PTR		- указатель на строку "str" равен NULL
 */
ssize_t nxs_wstring_set_len(nxs_wstring_t *str, size_t len)
{

	if(str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(len >= str->size) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	str->str[len] = L'\0';

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
ssize_t nxs_wstring_len_fix(nxs_wstring_t *str)
{

	if(str == NULL) {

		return NXS_WSTRING_E_PTR;
	}

	if(str->size == 0) {

		return NXS_WSTRING_E_DST_SIZE;
	}

	str->str[str->len] = L'\0';

	return str->len;
}

/*
 * Получить символ строки str, расположенный в позиции pos
 *
 * Возвращаемое значение:
 * * Требуемый символ
 * * 0 - если pos больше длины строки, либо указатель на строку "str" равен NULL
 */
wchar_t nxs_wstring_get_wchar(nxs_wstring_t *str, size_t pos)
{

	if(str == NULL) {

		return 0;
	}

	if(pos > str->len - 1) {

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
 */
wchar_t *nxs_wstring_get_substr(nxs_wstring_t *str, size_t offset)
{

	if(str == NULL) {

		return NULL;
	}

	if(offset >= str->size) {

		return NULL;
	}

	return str->str + offset;
}

/*
 * Поиск первого вхождения подстроки f_str длиной f_str_len в строку str со смещением offset
 *
 * Возвращаемое значение:
 * * Указатель на найденную подстроку в строке str
 * * NULL - если подстрока не найдена, длина искомой строки больше длины строки str или смещение offset в строке str больше длины строки
 * str, либо
 * 				хотя бы один из указателей "str" или "f_str" равен NULL
 */
wchar_t *nxs_wstring_find_substr_first(nxs_wstring_t *str, size_t offset, wchar_t *f_str, size_t f_str_len)
{
	size_t i, j;

	if(str == NULL || f_str == NULL) {

		return NULL;
	}

	if(f_str_len > str->len) {

		return NULL;
	}

	if(offset + 1 > str->len) {

		return NULL;
	}

	for(i = offset, j = 0; i < str->len; i++) {

		if(str->str[i] == f_str[j]) {

			if(j + 1 == f_str_len) {

				return str->str + i - j;
			}

			j++;
		}
		else {

			j = 0;
		}

		/*
		 * Если в строке str осталось для проверки символов меньше, чем необхо проверить в строке f_str
		 */
		if(str->len - (i + 1) < f_str_len - j) {

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
 * * NULL - если подстрока не найдена, длина искомой строки больше длины строки str или смещение offset в строке str больше длины строки
 * str, либо
 * 				хотя бы один из указателей "str" или "f_str" равен NULL
 */
wchar_t *nxs_wstring_find_substr_last(nxs_wstring_t *str, size_t offset, wchar_t *f_str, size_t f_str_len)
{
	size_t i, j;

	if(str == NULL || f_str == NULL) {

		return NULL;
	}

	if(str->len < f_str_len) {

		return NULL;
	}

	if(str->len < offset + 1) {

		return NULL;
	}

	for(i = str->len - 1, j = f_str_len - 1; i >= offset; i--) {

		if(str->str[i] == f_str[j]) {

			if(j == 0) {

				return str->str + i;
			}

			j--;
		}
		else {

			j = f_str_len - 1;
		}

		/*
		 * Если в строке str осталось для проверки символов меньше, чем необхо проверить в строке f_str
		 */
		if(i - offset < j + 1) {

			return NULL;
		}
	}

	return NULL;
}
/*
size_t	nxs_wstring_to_string(nxs_wstring_t *src, nxs_string_t *dst)
{
        size_t n;

        n = wcstombs(dst->str, src->str, dst->size - 1);


}
*/

/* Module internal (static) functions */
