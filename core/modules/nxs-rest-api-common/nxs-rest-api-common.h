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

#define NXS_HTTP_100_CONTINUE					100 /* "продолжай" */
#define NXS_HTTP_101_SWITCHING_PROTO				101 /* "переключение протоколов" */
#define NXS_HTTP_102_PROCESSING					102 /* "идёт обработка" */
#define NXS_HTTP_200_OK						200 /* "хорошо" */
#define NXS_HTTP_201_CREATED					201 /* "создано" */
#define NXS_HTTP_202_ACCEPTED					202 /* "принято" */
#define NXS_HTTP_202_ACCEPTED					202 /* "принято" */
#define NXS_HTTP_203_NON_AUTH_INFO				203 /* "информация не авторитетна" */
#define NXS_HTTP_204_NO_CONTENT					204 /* "нет содержимого" */
#define NXS_HTTP_205_RESET_CONTENT				205 /* "сбросить содержимое" */
#define NXS_HTTP_206_PARTIAL_CONTENT				206 /* "частичное содержимое" */
#define NXS_HTTP_207_MULTI_STATUS				207 /* "многостатусный" */
#define NXS_HTTP_226_IM_USED					226 /* "использовано IM" */
#define NXS_HTTP_300_MULTIPLE_CHOISES				300 /* "множество выборов" */
#define NXS_HTTP_301_MOVED_PERMANENTLY				301 /* "перемещено навсегда" */
#define NXS_HTTP_302_MOVED_TEMPORARILY				302 /* "перемещено временно" */
#define NXS_HTTP_302_FOUND					302 /* "найдено" */
#define NXS_HTTP_303_SEE_OTHER					303 /* "смотреть другое" */
#define NXS_HTTP_304_NOT_MODIFIED				304 /* "не изменялось" */
#define NXS_HTTP_305_USE_PROXY					305 /* "использовать прокси" */
#define NXS_HTTP_307_TEMPORARY_REDIRECT				307 /* "временное перенаправление" */
#define NXS_HTTP_400_BAD_REQUEST				400 /* "плохой, неверный запрос" */
#define NXS_HTTP_401_UNAUTHORIZED				401 /* "не авторизован" */
#define NXS_HTTP_402_PAYMENT_REQUIRED				402 /* "необходима оплата" */
#define NXS_HTTP_403_FORBIDDEN					403 /* "запрещено" */
#define NXS_HTTP_404_NOT_FOUND					404 /* "не найдено" */
#define NXS_HTTP_405_METHOD_NOT_ALLOWED				405 /* "метод не поддерживается" */
#define NXS_HTTP_406_NOT_ACCEPTABLE				406 /* "неприемлемо" */
#define NXS_HTTP_407_PROXY_AUTH_REQUIRED			407 /* "необходима аутентификация прокси" */
#define NXS_HTTP_408_REQUEST_TIMEOUT				408 /* "истекло время ожидания" */
#define NXS_HTTP_409_CONFLICT					409 /* "конфликт" */
#define NXS_HTTP_410_GONE					410 /* "удалён" */
#define NXS_HTTP_411_LENGTH_REQUIRED				411 /* "необходима длина" */
#define NXS_HTTP_412_PRECOND_FAILED				412 /* "условие ложно" */
#define NXS_HTTP_413_REQUEST_ENTITY_TOO_LARGE			413 /* "размер запроса слишком велик" */
#define NXS_HTTP_414_REQUEST_URI_TOO_LARGE			414 /* "запрашиваемый URI слишком длинный" */
#define NXS_HTTP_415_UNSUPPORTED_MEDIA_TYPE			415 /* "неподдерживаемый тип данных" */
#define NXS_HTTP_416_REQUESTED_RANGE_NOT_SATISFIABLE		416 /* "запрашиваемый диапазон не достижим" */
#define NXS_HTTP_417_EXPECTATION_FAILED				417 /* "ожидаемое неприемлемо" */
#define NXS_HTTP_422_UNPROCESSABLE_ENTITY			422 /* "необрабатываемый экземпляр". */
#define NXS_HTTP_423_LOCKED					423 /* "заблокировано". */
#define NXS_HTTP_424_FAILED_DEPENDENCY				424 /* "невыполненная зависимость". */
#define NXS_HTTP_425_UNORDERED_COLLECTION			425 /* "неупорядоченный набор" */
#define NXS_HTTP_426_UPGRADE_REQUIED				426 /* "необходимо обновление". */
#define NXS_HTTP_428_PRECONDITION_REQUIRED			428 /* "необходимо предусловие" */
#define NXS_HTTP_429_TOO_MANY_REQUESTS				429 /* "слишком много запросов" */
#define NXS_HTTP_431_REQUEST_HEADER_FIELD_TOO_LARGE		431 /* "поля заголовка запроса слишком большие" */
#define NXS_HTTP_434_REQUESTED_HOST_UNAVAILABLE			434 /* "Запрашиваемый адрес недоступен" */
#define NXS_HTTP_444_CONNECTION_CLOSED_WITHOUT_RESPONSE		444 /* "Закрывает соединение без передачи заголовка ответа. Нестандартный код" */
#define NXS_HTTP_449_RETRY_WITH					449 /* "повторить с" */
#define NXS_HTTP_451_UNAVAILABLE_FOR_LEGAL_REASONS		451 /* "недоступно по юридическим причинам" */
#define NXS_HTTP_500_INTERNAL_SERVER_ERROR			500 /* "внутренняя ошибка сервера" */
#define NXS_HTTP_501_NOT_IMPLEMENTED				501 /* "не реализовано" */
#define NXS_HTTP_502_BAD_GATEWAY				502 /* "плохой, ошибочный шлюз" */
#define NXS_HTTP_503_SERVICE_ANAVAILABLE			503 /* "сервис недоступен" */
#define NXS_HTTP_504_GATEWAY_TIMEOUT				504 /* "шлюз не отвечает" */
#define NXS_HTTP_505_HTTP_VERSION_NOT_SUPPORTED			505 /* "версия HTTP не поддерживается" */
#define NXS_HTTP_506_VARIANT_ALSO_NEGOTIATES			506 /* "вариант тоже проводит согласование" */
#define NXS_HTTP_507_INSUFFICIENT_STORAGE			507 /* "переполнение хранилища". */
#define NXS_HTTP_508_LOOP_DETECTED				508 /* "обнаружено бесконечное перенаправление" */
#define NXS_HTTP_509_BANDWIDTH_LIMIT_EXCEEDED			509 /* "исчерпана пропускная ширина канала". */
#define NXS_HTTP_510_NOT_EXTENDED				510 /* "не расширено". */
#define NXS_HTTP_511_NETWORK_AUTH_REQUIRED			511 /* "требуется сетевая аутентификация" */

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

u_char				*nxs_rest_api_common_http_code_to_text	(int http_code);
u_char				*nxs_rest_api_common_type_to_text	(nxs_rest_api_common_cmd_type_t type);

/** @} */ // end of nxs-rest-api-common
#endif /* _INCLUDE_NXS_REST_API_COMMON_H */
