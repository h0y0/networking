#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "server.h"
#include "socket_w.h"

server_t *make_s(int p)
{
    int i;

    // allocate
    server_t *s = malloc(sizeof(server_t));
    memset(&(s->addr),0,sizeof(s->addr));

    // address
    s->addr.sin_family      = AF_INET;
    s->addr.sin_addr.s_addr = htonl(INADDR_ANY); // inet_addr(LOOPBACK);
    s->addr.sin_port        = htons(p);

    // server
    for(i = 0; i < sizeof(s->client_list)/sizeof(peer_t); ++i)
    {
        s->client_list[i].socket = -1;
    }

    return s;
}

void free_s(server_t *s)
{
    free(s);
}

void start_s(server_t *s)
{
    s->socket = socket_w(AF_INET, SOCK_STREAM, 0);

    int reuse = 1;
    setsockopt_w(s->socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)); 

    bind_w(s->socket, (struct sockaddr *)&(s->addr), sizeof(struct sockaddr));

    listen_w(s->socket, 10);
}

void close_s(server_t *s)
{
    int i;

    for(i = 0; i < sizeof(s->client_list)/sizeof(peer_t); ++i)
        if(s->client_list[i].socket > 0)
            close_w(s->client_list[i].socket);
    
    close_w(s->socket);
}

peer_t *accept_p(server_t *s)
{
    for(int i = 0; i < sizeof(s->client_list)/sizeof(peer_t); ++i)
    {
        peer_t *p = &(s->client_list[i]);
        if(p->socket < 0)
        {
            memset(&(p->addr),0,sizeof(p->addr));
            memset(&(p->buf),0,sizeof(p->buf));
            socklen_t client_len = sizeof(p->addr);
            p->socket = accept_w(s->socket,(struct sockaddr *)&(p->addr),&client_len);
            p->buflen = 0;
            return p;
        }
    }
    return NULL;
}

int get_unused_p(server_t *s)
{
    for(int i = 0; i < sizeof(s->client_list)/sizeof(peer_t); ++i)
        if(s->client_list[i].socket < 0)
            return i;
    return -1;
}

void close_p(peer_t *p)
{
    close_w(p->socket);
    p->socket = -1;
}
