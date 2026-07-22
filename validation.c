
#include"validation.h"
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



int ValidateGetHttpRequest(Request *request)
{
	if (request->body_length > 0)
	{
		{
			return REQUEST_NOT_VALID_GET_HAS_BODY;
		}
	}
	return 1;
}

int ValidatePostHttpRequest(Request *request)
{
	if ((int)request->body_length < 0)
	{
		return HAS_NO_CONTENT_LENGTH;
	}

	return 1;
}

int CheckIfHttpRequestIsValid(Request *request)
{

	if (strcmp(request->method, "GET") == 0)
	{
		return ValidateGetHttpRequest(request);
	}
	else if (strcmp(request->method, "POST") == 0)
	{
		return ValidatePostHttpRequest(request);
	}
	return 1;
}
