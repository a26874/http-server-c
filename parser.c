#include "http.h"
#include "validation.h"
#include "header.h"
#include <string.h>
Request Parse_Http_Request(char *buffer, size_t bytes_received)
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

        int header_result = CreateHeader(line, &header, &request);
        if (header_result == 1)
        {
            request.headers[request.header_count] = header;
            request.header_count++;
        }
        else
        {
            request.is_valid = 0;
        }

        line = strtok(NULL, "\r\n");
    }

    if (after_body != NULL && (int)request.body_length > 0)
    {
        if ((int)(bytes_received - ((after_body + 4) - buffer)) != (int)request.body_length)
        {
            request.is_valid = 0;
        }
        else
        {
            memcpy(request.body, after_body + 4, request.body_length);
            request.body[request.body_length] = '\0';
        }
    }

    return request;
}
