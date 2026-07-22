#include "http.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

Header *GetHeader(Request *request, char *name_of_header);
void TrimHeaderValue(Header *header);
int CreateHeader(char *line, Header *header, Request *request);
void Print_Headers_And_Body(Request *request);