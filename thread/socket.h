#ifndef _SOCKET_H_
#define _SOCKET_H_

#define PORT 2001

struct socket_data {
	int sockfd;
	int client_sockfd;
   	int len;
};

#endif
