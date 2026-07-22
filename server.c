#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "http.h"
#include "header.h"
#include "parser.h"
#include "validation.h"
#include "client.h"



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