#ifndef SERVER_H
#define SERVER_H

#include "peer.h"

typedef struct server server_t;

server_t    *make_s(int port);
void        free_s(server_t *server);

void        setup_s(server_t *server);
void        close_s(server_t *server);

peer_t      *accept_p(server_t *server);
void        close_p(server_t *server);

#endif
