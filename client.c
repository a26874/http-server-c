#include <sys/socket.h>
#include <netinet/in.h>
#include "http.h"
#include "parser.h"
#include "validation.h"
#include "header.h"

#define HTTP_SUCCESS "HTTP/1.1 200 OK\r\n\r\n Welcome!!\n"
#define HTTP_FORBIDDEN "HTTP/1.1 400 Bad Request\r\n\r\n"

int handle_client(int client_socket, struct sockaddr_in client_address)
{

    size_t n = 0;
    char buffer[2000];
    char parse_buffer[2000];
    int ret = 0;

    printf("\n--------NEW HANDLE CLIENT--------\n");

    for (;;)
    {
        memset(buffer, 0, sizeof(buffer));

        n = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        if ((int)n == -1)
        {
            perror("read(client)\n");
            ret = RUNTIME_ERROR;
            goto close_handle_client_message;
        }

        if (n == 0)
        {
            printf("connection closed\n");
            break;
        }

        strcpy(parse_buffer, buffer);
        Request request = Parse_Http_Request(parse_buffer);

        if (request.is_valid == 0)
        {
            (void)write(client_socket, HTTP_FORBIDDEN, strlen(HTTP_FORBIDDEN));
            printf("Invalid request line\n");
            ret = INVALID_REQUEST_LINE;
            goto close_handle_client_message;
        }

        printf("Request object\nMethod:%s \nPath:%s \nVersion:%s \n\n", request.method, request.path, request.version);

        int isHttpRequestValid = CheckIfHttpRequestIsValid(&request);
        if (isHttpRequestValid < 0)
        {
            if (isHttpRequestValid == REQUEST_NOT_VALID_GET_HAS_BODY)
            {
                (void)write(client_socket, HTTP_FORBIDDEN, strlen(HTTP_FORBIDDEN));
                printf("A GET Method shouldnt contain a body\n");
            }

            goto close_handle_client_message;
        }

        Print_Headers_And_Body(&request);

        // printf("\n\n\n\nTHE ACTUAL REQUEST BUFFER\n");
        // printf("Request:\n%s", buffer);

        (void)write(client_socket, HTTP_SUCCESS, strlen(HTTP_SUCCESS));
        free(request.body);
        break;
        // printf("Client IP and port: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    }
    goto close_handle_client_message;

close_handle_client_message:
    printf("\n--------FINISHED HANDLE CLIENT--------\n");
    close(client_socket);
    return ret;
}
