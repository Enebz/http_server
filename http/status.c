#include "status.h"

// Function Definitions
HttpStatusCode http_status_code(int code, const char *reason)
{
	HttpStatusCode status;
	status.code = code;
	status.reason = reason;
	return status;
}