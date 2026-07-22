server: server.c client.c parser.c header.c validation.c
	gcc -Wall -Wextra -g server.c client.c parser.c header.c validation.c -o server