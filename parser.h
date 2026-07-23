#include "http.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
Request Parse_Http_Request(char *buffer, size_t bytes_received);