#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <pthread.h>

#define BUFFER_MAX 1024
#define CLIENT_MAX 1000

typedef struct peer peer_t;
typedef struct server server_t; 
typedef struct header header_t;

struct header
{
    int identifier;
    int len;
};

struct peer
{
    int socket, buflen, newdata;
    struct sockaddr_in addr;
    char buf[BUFFER_MAX];
    pthread_t worker;
    pthread_mutex_t bufmutex;
    pthread_cond_t nonEmpty;
    int done;
};

struct server
{
    int socket;
    struct sockaddr_in addr;
    peer_t client_list[CLIENT_MAX];
};

server_t    *make_s(int port);
void        free_s(server_t *server);

void        start_s(server_t *server);
void        close_s(server_t *server);

peer_t      *accept_p(server_t *server);
int         get_unused_p(server_t *server);

void        close_p(peer_t *peer);
int         read_p(peer_t *peer);
int         write_p(peer_t *peer);

#endif
