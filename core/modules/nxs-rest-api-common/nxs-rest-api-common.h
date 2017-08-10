#ifndef _INCLUDE_NXS_REST_API_COMMON_H
#define _INCLUDE_NXS_REST_API_COMMON_H

// clang-format off

/** @addtogroup nxs-rest-api-common
 *
 * @brief Module 'nxs-rest-api-common' is a basic module providing functions to work with Rest API common types.
 *
 *  @{
 */

#include <nxs-core/nxs-core.h>

enum nxs_http_code_e {
	NXS_HTTP_CODE_UNKNOWN					= 0,	/* "продолжай" */
	NXS_HTTP_CODE_100_CONTINUE				= 100,	/* "продолжай" */
	NXS_HTTP_CODE_101_SWITCHING_PROTO			= 101,	/* "переключение протоколов" */
	NXS_HTTP_CODE_102_PROCESSING				= 102,	/* "идёт обработка" */
	NXS_HTTP_CODE_200_OK					= 200,	/* "хорошо" */
	NXS_HTTP_CODE_201_CREATED				= 201,	/* "создано" */
	NXS_HTTP_CODE_202_ACCEPTED				= 202,	/* "принято" */
	NXS_HTTP_CODE_203_NON_AUTH_INFO				= 203,	/* "информация не авторитетна" */
	NXS_HTTP_CODE_204_NO_CONTENT				= 204,	/* "нет содержимого" */
	NXS_HTTP_CODE_205_RESET_CONTENT				= 205,	/* "сбросить содержимое" */
	NXS_HTTP_CODE_206_PARTIAL_CONTENT			= 206,	/* "частичное содержимое" */
	NXS_HTTP_CODE_207_MULTI_STATUS				= 207,	/* "многостатусный" */
	NXS_HTTP_CODE_226_IM_USED				= 226,	/* "использовано IM" */
	NXS_HTTP_CODE_300_MULTIPLE_CHOISES			= 300,	/* "множество выборов" */
	NXS_HTTP_CODE_301_MOVED_PERMANENTLY			= 301,	/* "перемещено навсегда" */
	NXS_HTTP_CODE_302_MOVED_TEMPORARILY			= 302,	/* "перемещено временно" */
	NXS_HTTP_CODE_302_FOUND					= 302,	/* "найдено" */
	NXS_HTTP_CODE_303_SEE_OTHER				= 303,	/* "смотреть другое" */
	NXS_HTTP_CODE_304_NOT_MODIFIED				= 304,	/* "не изменялось" */
	NXS_HTTP_CODE_305_USE_PROXY				= 305,	/* "использовать прокси" */
	NXS_HTTP_CODE_307_TEMPORARY_REDIRECT			= 307,	/* "временное перенаправление" */
	NXS_HTTP_CODE_400_BAD_REQUEST				= 400,	/* "плохой, неверный запрос" */
	NXS_HTTP_CODE_401_UNAUTHORIZED				= 401,	/* "не авторизован" */
	NXS_HTTP_CODE_402_PAYMENT_REQUIRED			= 402,	/* "необходима оплата" */
	NXS_HTTP_CODE_403_FORBIDDEN				= 403,	/* "запрещено" */
	NXS_HTTP_CODE_404_NOT_FOUND				= 404,	/* "не найдено" */
	NXS_HTTP_CODE_405_METHOD_NOT_ALLOWED			= 405,	/* "метод не поддерживается" */
	NXS_HTTP_CODE_406_NOT_ACCEPTABLE			= 406,	/* "неприемлемо" */
	NXS_HTTP_CODE_407_PROXY_AUTH_REQUIRED			= 407,	/* "необходима аутентификация прокси" */
	NXS_HTTP_CODE_408_REQUEST_TIMEOUT			= 408,	/* "истекло время ожидания" */
	NXS_HTTP_CODE_409_CONFLICT				= 409,	/* "конфликт" */
	NXS_HTTP_CODE_410_GONE					= 410,	/* "удалён" */
	NXS_HTTP_CODE_411_LENGTH_REQUIRED			= 411,	/* "необходима длина" */
	NXS_HTTP_CODE_412_PRECOND_FAILED			= 412,	/* "условие ложно" */
	NXS_HTTP_CODE_413_REQUEST_ENTITY_TOO_LARGE		= 413,	/* "размер запроса слишком велик" */
	NXS_HTTP_CODE_414_REQUEST_URI_TOO_LARGE			= 414,	/* "запрашиваемый URI слишком длинный" */
	NXS_HTTP_CODE_415_UNSUPPORTED_MEDIA_TYPE		= 415,	/* "неподдерживаемый тип данных" */
	NXS_HTTP_CODE_416_REQUESTED_RANGE_NOT_SATISFIABLE	= 416,	/* "запрашиваемый диапазон не достижим" */
	NXS_HTTP_CODE_417_EXPECTATION_FAILED			= 417,	/* "ожидаемое неприемлемо" */
	NXS_HTTP_CODE_422_UNPROCESSABLE_ENTITY			= 422,	/* "необрабатываемый экземпляр". */
	NXS_HTTP_CODE_423_LOCKED				= 423,	/* "заблокировано". */
	NXS_HTTP_CODE_424_FAILED_DEPENDENCY			= 424,	/* "невыполненная зависимость". */
	NXS_HTTP_CODE_425_UNORDERED_COLLECTION			= 425,	/* "неупорядоченный набор" */
	NXS_HTTP_CODE_426_UPGRADE_REQUIED			= 426,	/* "необходимо обновление". */
	NXS_HTTP_CODE_428_PRECONDITION_REQUIRED			= 428,	/* "необходимо предусловие" */
	NXS_HTTP_CODE_429_TOO_MANY_REQUESTS			= 429,	/* "слишком много запросов" */
	NXS_HTTP_CODE_431_REQUEST_HEADER_FIELD_TOO_LARGE	= 431,	/* "поля заголовка запроса слишком большие" */
	NXS_HTTP_CODE_434_REQUESTED_HOST_UNAVAILABLE		= 434,	/* "Запрашиваемый адрес недоступен" */
	NXS_HTTP_CODE_444_CONNECTION_CLOSED_WITHOUT_RESPONSE	= 444,	/* "Закрывает соединение без передачи заголовка ответа. Нестандартный код" */
	NXS_HTTP_CODE_449_RETRY_WITH				= 449,	/* "повторить с" */
	NXS_HTTP_CODE_451_UNAVAILABLE_FOR_LEGAL_REASONS		= 451,	/* "недоступно по юридическим причинам" */
	NXS_HTTP_CODE_500_INTERNAL_SERVER_ERROR			= 500,	/* "внутренняя ошибка сервера" */
	NXS_HTTP_CODE_501_NOT_IMPLEMENTED			= 501,	/* "не реализовано" */
	NXS_HTTP_CODE_502_BAD_GATEWAY				= 502,	/* "плохой, ошибочный шлюз" */
	NXS_HTTP_CODE_503_SERVICE_ANAVAILABLE			= 503,	/* "сервис недоступен" */
	NXS_HTTP_CODE_504_GATEWAY_TIMEOUT			= 504,	/* "шлюз не отвечает" */
	NXS_HTTP_CODE_505_HTTP_VERSION_NOT_SUPPORTED		= 505,	/* "версия HTTP не поддерживается" */
	NXS_HTTP_CODE_506_VARIANT_ALSO_NEGOTIATES		= 506,	/* "вариант тоже проводит согласование" */
	NXS_HTTP_CODE_507_INSUFFICIENT_STORAGE			= 507,	/* "переполнение хранилища". */
	NXS_HTTP_CODE_508_LOOP_DETECTED				= 508,	/* "обнаружено бесконечное перенаправление" */
	NXS_HTTP_CODE_509_BANDWIDTH_LIMIT_EXCEEDED		= 509,	/* "исчерпана пропускная ширина канала". */
	NXS_HTTP_CODE_510_NOT_EXTENDED				= 510,	/* "не расширено". */
	NXS_HTTP_CODE_511_NETWORK_AUTH_REQUIRED			= 511	/* "требуется сетевая аутентификация" */
};

enum nxs_rest_api_common_cmd_type_e {
	NXS_REST_API_COMMON_CMD_UNKNOWN,
	NXS_REST_API_COMMON_CMD_GET,
	NXS_REST_API_COMMON_CMD_POST,
	NXS_REST_API_COMMON_CMD_HEAD,
	NXS_REST_API_COMMON_CMD_PUT,
	NXS_REST_API_COMMON_CMD_DELETE,
	NXS_REST_API_COMMON_CMD_OPTIONS,
	NXS_REST_API_COMMON_CMD_TRACE,
	NXS_REST_API_COMMON_CMD_CONNECT,
	NXS_REST_API_COMMON_CMD_PATCH,
};

u_char				*nxs_rest_api_common_http_code_to_text	(nxs_http_code_t http_code);
u_char				*nxs_rest_api_common_type_to_text	(nxs_rest_api_common_cmd_type_t type);

/** @} */ // end of nxs-rest-api-common
#endif /* _INCLUDE_NXS_REST_API_COMMON_H */
