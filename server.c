#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define HTTP_SUCCESS "HTTP/1.1 200 OK\r\n\r\n Welcome!!\n"
#define HTTP_FORBIDDEN "HTTP/1.1 400 Bad Request\r\n\r\n"

typedef enum ErrorCodesHttpRequest
{
	RUNTIME_ERROR = -1,
	INVALID_REQUEST_LINE = -2,
	REQUEST_NOT_VALID_GET_HAS_BODY = -3
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

	Header headers[100];
	int header_count;
	int is_valid;
} Request;

Header *GetHeader(Request request, char *name_of_header)
{
	for (int i = 0; i < request.header_count; i++)
	{
		if (strcmp(request.headers[i].name, name_of_header) == 0)
		{
			return &request.headers[i];
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

int CreateHeader(char *line, Header *header)
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

	TrimHeaderValue(header);

	return 1;
}

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

Request Parse_Http_Request(char *buffer)
{
	Request request;
	memset(&request, 0, sizeof(request));

	request.header_count = 0;
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

		if (CreateHeader(line, &header) == 1)
		{
			request.headers[request.header_count] = header;
			request.header_count++;
		}

		line = strtok(NULL, "\r\n");
	}

	return request;
}

void Print_Headers(Request request)
{
	if (request.header_count > 0)
	{
		printf("----------HEADERS START----------\n");
		for (int i = 0; i < request.header_count; i++)
		{
			printf("Name: %s\nValue: %s\n", request.headers[i].name, request.headers[i].value);
		}
		printf("----------HEADERS FINISH----------\n");
	}
}

int CheckIfHttpRequestIsValid(Request request)
{

	if (strcmp(request.method, "GET") == 0)
	{
		return ValidateGetHttpRequest(request);
	}
	return 1;
}

int ValidateGetHttpRequest(Request request)
{
	if (GetHeader(request, "Content-Length") != NULL && GetHeader(request, "Content-Type") != NULL)
	{
		{
			return REQUEST_NOT_VALID_GET_HAS_BODY;
		}
	}
	return 1;
}
int handle_client(int client_socket, struct sockaddr_in client_address)
{

	ssize_t n = 0;
	char buffer[2000];
	char parse_buffer[2000];
	int ret = 0;

	printf("\n--------NEW HANDLE CLIENT--------\n");

	for (;;)
	{
		memset(buffer, 0, sizeof(buffer));

		n = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

		if (n < 0)
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

		int isHttpRequestValid = CheckIfHttpRequestIsValid(request);
		if (isHttpRequestValid < 0)
		{
			if (isHttpRequestValid == REQUEST_NOT_VALID_GET_HAS_BODY)
			{
				(void)write(client_socket, HTTP_FORBIDDEN, strlen(HTTP_FORBIDDEN));
				printf("A GET Method shouldnt contain a body\n");
			}

			goto close_handle_client_message;
		}

		Print_Headers(request);

		// printf("\n\n\n\nTHE ACTUAL REQUEST BUFFER\n");
		// printf("Request:\n%s", buffer);

		(void)write(client_socket, HTTP_SUCCESS, strlen(HTTP_SUCCESS));
		break;
		// printf("Client IP and port: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
	}
	goto close_handle_client_message;

close_handle_client_message:
	printf("\n--------FINISHED HANDLE CLIENT--------\n");
	close(client_socket);
	return ret;
}

int main()
{

	// variable declaration
	int rc = 0;
	struct sockaddr_in bind_addr, client_addr;
	int tcp_socket = 0;
	int client_socket = 0;
	int ret = 0;
	socklen_t client_addr_len = sizeof(client_addr);
	// logic

	// Creates a socket with domain, type and protocol
	tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_socket == -1)
	{
		perror("socket()");
		return RUNTIME_ERROR;
	}
	printf("Socket created successfully\n");

	memset(&bind_addr, 0, sizeof(bind_addr));

	bind_addr.sin_port = htons(7281);
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;
	if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt\n");
		return RUNTIME_ERROR;
	}

	rc = bind(tcp_socket, (const struct sockaddr *)&bind_addr, sizeof(bind_addr));
	if (rc < 0)
	{
		perror("bind()");
		ret = 1;
		return RUNTIME_ERROR;
	}
	printf("bind succeeded\n");

	rc = listen(tcp_socket, SOMAXCONN);

	if (rc < 0)
	{
		perror("listen()");
		ret = 1;
		goto exit;
	}
	printf("listen succeeded\n");

	for (;;)
	{
		printf("waiting for connection ... \n");
		client_socket = accept(tcp_socket, (struct sockaddr *)&client_addr, &client_addr_len);

		if (client_socket == -1)
		{
			perror("Accept");
			continue;
		}

		printf("Got a connection\n");
		rc = handle_client(client_socket, client_addr);
	}

exit:
	close(tcp_socket);
	return ret;
}