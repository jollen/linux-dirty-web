/*
   web.c,
   a dirty embedded Web server.
*/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <netinet/in.h>
#include "http.h"

#define CR '\r'
#define LF '\n'

#define MAX 4096

int http_get;

int http_parser(char *s)
{
    if (s[0] == CR && s[1] == LF)
	return -1;
    else if (strncmp(s, "GET", 3) == 0)
	http_get = 1;
    return 0;
}

struct http_operations *http_ops[MAX_OPS];

void *http_thead(void *argu)
{
    struct http_operations *ops = (struct http_operations *)argu;
	char buf[MAX];
	int ret;

    while (1) {
	   printf("ops->read\n");
	   ret = ops->read(ops, buf);
	   printf("client: %s [%d]\n", buf, ret);
           http_parser(buf);
	   if (buf[0] == CR && buf[1] == LF)
	      break;
	}

	if (http_get) {
	  ops->write(ops, "HTTP/1.1 200 OK\n", 16);
	  ops->write(ops, "Content-Type: text/html\n", 24);
	  ops->write(ops, "\n", 1);
	  ops->write(ops, "<h1>hihi</h1>", 13);
	}

	printf("ops->close\n");
	ops->close(ops);
}

void *http_main(int n)
{
   	pthread_t thread_id1;
	struct http_operations *ops = http_data[n].fops;

	/* blocking open */
	printf("ops->open\n");
	if (ops == NULL)
	   printf("ops = NULL\n");
	if (ops->open) {
	   ops->open(ops);
	} else {
	   printf("ops->open = NULL\n");
	}
	printf("exit ops->open\n");

	// HTTP main thread
	pthread_create(thread_id1, NULL, http_thead, ops);

	return NULL;
}

int http_register(struct http_operations *ops, int opsno)
{
	switch (opsno) {
		case SOCKET_OPS:
			   http_data[SOCKET_OPS].fops = ops;
			   http_main(SOCKET_OPS);
			break;
		case FILE_OPS:
			http_ops[FILE_OPS] = ops;
			break;
		default:
			printf("error\n");
			return -1;
	}
	printf("exit http_register\n");

	return 0;
}
