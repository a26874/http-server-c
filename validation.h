#include "http.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
void IsRequestLineValid(Request *request, char *line);
int ValidateGetHttpRequest(Request *request);
int ValidatePostHttpRequest(Request *request);
int CheckIfHttpRequestIsValid(Request *request);
int HasRepeatedHostHeader(Request *request);
