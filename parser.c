#include "http.h"
#include "validation.h"
#include "header.h"
#include <string.h>
Request Parse_Http_Request(char *buffer)
{
    Request request;
    memset(&request, 0, sizeof(request));

    request.header_count = 0;

    char *after_body = strstr(buffer, "\r\n\r\n");

    if (after_body != NULL)
    {
        *after_body = '\0';
    }

    char *line = strtok(buffer, "\r\n");

    IsRequestLineValid(&request, line);
    if (request.is_valid == 0)
    {
        return request;
    }

    line = strtok(NULL, "\r\n");

    while (line != NULL)
    {
        Header header;
        memset(&header, 0, sizeof(header));

        if (CreateHeader(line, &header, &request) == 1)
        {
            request.headers[request.header_count] = header;
            request.header_count++;
        }

        line = strtok(NULL, "\r\n");
    }

    if (after_body != NULL && request.body_length > 0)
    {
        memcpy(request.body, after_body + 4, request.body_length);
        request.body[request.body_length] = '\0';
    }

    return request;
}
