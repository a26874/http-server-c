#include "header.h"

Header *GetHeader(Request *request, char *name_of_header)
{
    for (int i = 0; i < request->header_count; i++)
    {
        if (strcmp(request->headers[i].name, name_of_header) == 0)
        {
            return &request->headers[i];
        }
    }
    return NULL;
}

void TrimHeaderValue(Header *header)
{
    if (header->value[0] == '\0')
    {
        return;
    }

    char buffer[512];
    int first_count = 0;
    int max_size = strlen(header->value) - 1;
    memset(&buffer, 0, sizeof(buffer));

    while (isspace(header->value[first_count]))
    {
        first_count++;
    }

    while (max_size >= first_count && isspace((unsigned char)header->value[max_size]))
    {
        max_size--;
    }

    strncpy(buffer, header->value + first_count, (max_size + 1) - first_count);

    strcpy(header->value, buffer);
}

int CreateHeader(char *line, Header *header, Request *request)
{
    char line_copy[2000];
    strcpy(line_copy, line);

    char *get_value = strchr(line_copy, ':');

    if (get_value == NULL)
    {
        return 0;
    }

    *get_value = '\0';

    strcpy(header->name, line_copy);
    strcpy(header->value, get_value + 1);

    if (strcmp(header->name, "Content-Length") == 0)
    {
        char *endptr;
        int parsed_to_int = strtol(header->value, &endptr, 0);
        request->body_length = parsed_to_int;
        request->body = malloc(request->body_length + 1);
    }

    TrimHeaderValue(header);

    return 1;
}

void Print_Headers_And_Body(Request *request)
{
    if (request->header_count > 0)
    {
        printf("----------HEADERS START----------\n");
        for (int i = 0; i < request->header_count; i++)
        {
            printf("Name: %s\nValue: %s\n", request->headers[i].name, request->headers[i].value);
        }
        printf("----------HEADERS FINISH----------\n");
    }
    if (request->body != NULL)
    {

        printf("\n----------BODY START----------\n");
        printf("%s", request->body);
        printf("\n----------BODY FINISH----------\n");
    }
}
