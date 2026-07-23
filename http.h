#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>

typedef enum ErrorCodesHttpRequest
{
	RUNTIME_ERROR = -1,
	INVALID_REQUEST_LINE = -2,
	HAS_NO_CONTENT_LENGTH = -3,
	HAS_NO_HEADER_HOST = -4,
	HAS_DUPLICATED_HOST = -5,
	INVALID_CONTENT_LENGTH = -6
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