// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */



/* Module typedefs */

typedef struct			nxs_rest_api_common_http_text_s		nxs_rest_api_common_http_text_t;
typedef struct			nxs_rest_api_common_type_text_s		nxs_rest_api_common_type_text_t;

/* Module declarations */

struct nxs_rest_api_common_http_text_s
{
	nxs_http_code_t				http_code;
	char					*text;
};

struct nxs_rest_api_common_type_text_s
{
	nxs_rest_api_common_cmd_type_t		type;
	char					*text;
};

/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */

nxs_rest_api_common_http_text_t nxs_rest_api_common_http_text[] =
{
	{NXS_HTTP_CODE_100_CONTINUE,				"Continue "},
	{NXS_HTTP_CODE_101_SWITCHING_PROTO,			"Switching Protocols"},
	{NXS_HTTP_CODE_102_PROCESSING,				"Processing"},
	{NXS_HTTP_CODE_200_OK,					"OK"},
	{NXS_HTTP_CODE_201_CREATED,				"Created"},
	{NXS_HTTP_CODE_202_ACCEPTED,				"Accepted"},
	{NXS_HTTP_CODE_203_NON_AUTH_INFO,			"Non-Authoritative Information"},
	{NXS_HTTP_CODE_204_NO_CONTENT,				"No Content"},
	{NXS_HTTP_CODE_205_RESET_CONTENT,			"Reset Content"},
	{NXS_HTTP_CODE_206_PARTIAL_CONTENT,			"Partial Content"},
	{NXS_HTTP_CODE_207_MULTI_STATUS,			"Multi-Status"},
	{NXS_HTTP_CODE_226_IM_USED,				"IM Used"},
	{NXS_HTTP_CODE_300_MULTIPLE_CHOISES,			"Multiple Choices"},
	{NXS_HTTP_CODE_301_MOVED_PERMANENTLY,			"Moved Permanently"},
	{NXS_HTTP_CODE_302_MOVED_TEMPORARILY,			"Moved Temporarily"},
	{NXS_HTTP_CODE_302_FOUND,				"Found"},
	{NXS_HTTP_CODE_303_SEE_OTHER,				"See Other"},
	{NXS_HTTP_CODE_304_NOT_MODIFIED,			"Not Modified"},
	{NXS_HTTP_CODE_305_USE_PROXY,				"Use Proxy"},
	{NXS_HTTP_CODE_307_TEMPORARY_REDIRECT,			"Temporary Redirect"},
	{NXS_HTTP_CODE_400_BAD_REQUEST,				"Bad Request"},
	{NXS_HTTP_CODE_401_UNAUTHORIZED,			"Unauthorized"},
	{NXS_HTTP_CODE_402_PAYMENT_REQUIRED,			"Payment Required"},
	{NXS_HTTP_CODE_403_FORBIDDEN,				"Forbidden"},
	{NXS_HTTP_CODE_404_NOT_FOUND,				"Not Found"},
	{NXS_HTTP_CODE_405_METHOD_NOT_ALLOWED,			"Method Not Allowed"},
	{NXS_HTTP_CODE_406_NOT_ACCEPTABLE,			"Not Acceptable"},
	{NXS_HTTP_CODE_407_PROXY_AUTH_REQUIRED,			"Proxy Authentication Required"},
	{NXS_HTTP_CODE_408_REQUEST_TIMEOUT,			"Request Timeout"},
	{NXS_HTTP_CODE_409_CONFLICT,				"Conflict"},
	{NXS_HTTP_CODE_410_GONE,				"Gone"},
	{NXS_HTTP_CODE_411_LENGTH_REQUIRED,			"Length Required"},
	{NXS_HTTP_CODE_412_PRECOND_FAILED,			"Precondition Failed"},
	{NXS_HTTP_CODE_413_REQUEST_ENTITY_TOO_LARGE,		"Request Entity Too Large"},
	{NXS_HTTP_CODE_414_REQUEST_URI_TOO_LARGE,		"Request-URI Too Large"},
	{NXS_HTTP_CODE_415_UNSUPPORTED_MEDIA_TYPE,		"Unsupported Media Type"},
	{NXS_HTTP_CODE_416_REQUESTED_RANGE_NOT_SATISFIABLE,	"Requested Range Not Satisfiable"},
	{NXS_HTTP_CODE_417_EXPECTATION_FAILED,			"Expectation Failed"},
	{NXS_HTTP_CODE_422_UNPROCESSABLE_ENTITY,		"Unprocessable Entity"},
	{NXS_HTTP_CODE_423_LOCKED,				"Locked"},
	{NXS_HTTP_CODE_424_FAILED_DEPENDENCY,			"Failed Dependency"},
	{NXS_HTTP_CODE_425_UNORDERED_COLLECTION,		"Unordered Collection"},
	{NXS_HTTP_CODE_426_UPGRADE_REQUIED,			"Upgrade Required"},
	{NXS_HTTP_CODE_428_PRECONDITION_REQUIRED,		"Precondition Required"},
	{NXS_HTTP_CODE_429_TOO_MANY_REQUESTS,			"Too Many Requests"},
	{NXS_HTTP_CODE_431_REQUEST_HEADER_FIELD_TOO_LARGE,	"Request Header Fields Too Large"},
	{NXS_HTTP_CODE_434_REQUESTED_HOST_UNAVAILABLE,		"Requested host unavailable."},
	{NXS_HTTP_CODE_444_CONNECTION_CLOSED_WITHOUT_RESPONSE,	"Connection closed without response"},
	{NXS_HTTP_CODE_449_RETRY_WITH,				"Retry With"},
	{NXS_HTTP_CODE_451_UNAVAILABLE_FOR_LEGAL_REASONS,	"Unavailable For Legal Reasons"},
	{NXS_HTTP_CODE_500_INTERNAL_SERVER_ERROR,		"Internal Server Error"},
	{NXS_HTTP_CODE_501_NOT_IMPLEMENTED,			"Not Implemented"},
	{NXS_HTTP_CODE_502_BAD_GATEWAY,				"Bad Gateway"},
	{NXS_HTTP_CODE_503_SERVICE_ANAVAILABLE,			"Service Unavailable"},
	{NXS_HTTP_CODE_504_GATEWAY_TIMEOUT,			"Gateway Timeout"},
	{NXS_HTTP_CODE_505_HTTP_VERSION_NOT_SUPPORTED,		"HTTP Version Not Supported"},
	{NXS_HTTP_CODE_506_VARIANT_ALSO_NEGOTIATES,		"Variant Also Negotiates"},
	{NXS_HTTP_CODE_507_INSUFFICIENT_STORAGE,		"Insufficient Storage"},
	{NXS_HTTP_CODE_508_LOOP_DETECTED,			"Loop Detected"},
	{NXS_HTTP_CODE_509_BANDWIDTH_LIMIT_EXCEEDED,		"Bandwidth Limit Exceeded"},
	{NXS_HTTP_CODE_510_NOT_EXTENDED,			"Not Extended"},
	{NXS_HTTP_CODE_511_NETWORK_AUTH_REQUIRED,		"Network Authentication Required"},

	{NXS_HTTP_CODE_UNKNOWN,					"UNKNOWN"}
};

nxs_rest_api_common_type_text_t nxs_rest_api_common_type_text[] =
{
	{NXS_REST_API_COMMON_CMD_GET,				"GET"},
	{NXS_REST_API_COMMON_CMD_POST,				"POST"},
	{NXS_REST_API_COMMON_CMD_HEAD,				"HEAD"},
	{NXS_REST_API_COMMON_CMD_PUT,				"PUT"},
	{NXS_REST_API_COMMON_CMD_DELETE,			"DELETE"},
	{NXS_REST_API_COMMON_CMD_OPTIONS,			"OPTIONS"},
	{NXS_REST_API_COMMON_CMD_TRACE,				"TRACE"},
	{NXS_REST_API_COMMON_CMD_CONNECT,			"CONNECT"},
	{NXS_REST_API_COMMON_CMD_PATCH,				"PATCH"},
	{NXS_REST_API_COMMON_CMD_UNKNOWN,			"UNKNOWN"}
};


/* Module global functions */

// clang-format on

/* Module internal (static) functions */

u_char *nxs_rest_api_common_http_code_to_text(nxs_http_code_t http_code)
{
	size_t i;

	for(i = 0; nxs_rest_api_common_http_text[i].http_code != 0; i++) {

		if(nxs_rest_api_common_http_text[i].http_code == http_code) {

			return (u_char *)nxs_rest_api_common_http_text[i].text;
		}
	}

	return (u_char *)nxs_rest_api_common_http_text[i].text;
}

u_char *nxs_rest_api_common_type_to_text(nxs_rest_api_common_cmd_type_t type)
{
	size_t i;

	for(i = 0; nxs_rest_api_common_type_text[i].type != 0; i++) {

		if(nxs_rest_api_common_type_text[i].type == type) {

			return (u_char *)nxs_rest_api_common_type_text[i].text;
		}
	}

	return (u_char *)nxs_rest_api_common_type_text[i].text;
}
