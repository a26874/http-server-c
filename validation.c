#include "header.h"
#include "validation.h"
void IsRequestLineValid(Request *request, char *line)
{

	int parsed_request_line_itens = sscanf(line, "%15s %255s %15s", request->method, request->path, request->version);

	if (parsed_request_line_itens == 3)
	{
		request->is_valid = 1;
	}
	else
	{
		request->is_valid = 0;
	}
}

int ValidatePostHttpRequest(Request *request)
{
	if ((int)request->body_length < 0)
	{
		return HAS_NO_CONTENT_LENGTH;
	}

	return 1;
}

int HasRepeatedHostHeader(Request *request)
{

	if (request->headers == NULL)
	{
		return 0;
	}

	int counter = 0;

	for (int i = 0; i < request->header_count; i++)
	{

		if (strcmp(request->headers[i].name, "Host") == 0)
		{
			counter++;
		}

		if (counter > 1)
		{
			return HAS_DUPLICATED_HOST;
		}
	}

	return 1;
}

int IsContentLengthNonValid(Request *request)
{

	Header *content_length_header = GetHeader(request, "Content-Length");

	int counter = 0;
	if (content_length_header != NULL)
	{
		while (content_length_header->value[counter] != '\0')
		{
			if (isdigit(content_length_header->value[counter]) == 0)
			{
				return 0;
			}
			counter++;
		}
	}
	return 1;
}

int CheckIfHttpRequestIsValid(Request *request)
{

	Header *host_header = GetHeader(request, "Host");
	if (host_header == NULL)
	{
		return HAS_NO_HEADER_HOST;
	}

	if ((int)request->body_length < 0 || IsContentLengthNonValid(request) == 0)
	{
		return INVALID_CONTENT_LENGTH;
	}

	int has_repeated_host_header = HasRepeatedHostHeader(request);

	if (has_repeated_host_header <= 0)
	{
		return has_repeated_host_header;
	}

	if (strcmp(request->method, "POST") == 0)
	{
		return ValidatePostHttpRequest(request);
	}
	return 1;
}
