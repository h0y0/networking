#ifndef SOCKET_W_H
#define SOCKET_W_H

#include <sys/socket.h>

int     socket_w(int domain, int type, int protocol);
void	bind_w(int socket, const struct sockaddr *address, socklen_t address_len);
void	listen_w(int socket, int backlog);
int     accept_w(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
void    setsockopt_w(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
void    close_w(int socket);
//int	Getsockopt(int socket, int level, int option_name, void *restrict option_value, socklen_t *restrict option_len);

#endif
