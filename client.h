#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "http.h"
#include "parser.h"
#include "header.h"
#include "validation.h"
int handle_client(int client_socket, struct sockaddr_in client_address);