#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>

typedef enum ErrorCodesHttpRequest
{
	RUNTIME_ERROR = -1,
	INVALID_REQUEST_LINE = -2,
	REQUEST_NOT_VALID_GET_HAS_BODY = -3,
	HAS_NO_CONTENT_LENGTH = -4
} ErrorCodesHttpRequest;

typedef struct Header
{
	char name[64];
	char value[512];
} Header;

typedef struct Request
{
	char method[16];
	char path[256];
	char version[16];
	int header_count;
	int is_valid;
	char *body;

	Header headers[100];
	size_t body_length;

} Request;

#endif