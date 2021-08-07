#include "socket_w.h"
#include "error_w.h"
#include <unistd.h>

int socket_w(int d, int t, int p)
{
	int fd;
    
    err_c((fd = socket(d,t,p)) < 0, "socket"); 
	
    return fd;
}

void bind_w(int s, const struct sockaddr *a, socklen_t a_l)
{
    err_c(bind(s,a,a_l) < 0, "bind");
}

void listen_w(int s, int b)
{
    err_c(listen(s,b) < 0, "listen");
}

int accept_w(int s, struct sockaddr *a, socklen_t *a_l)
{
	int fd;

    err_c((fd = accept(s,a,a_l)) < 0, "accept");
	
    return fd;
}

void setsockopt_w(int s, int l, int o_n, const void *o_v, socklen_t o_l)
{
    err_c(setsockopt(s,l,o_n,o_v,o_l) < 0, "setsockopt");
}

void close_w(int s)
{
    err_c(close(s) < 0,"close");
}
