#include "select_s.h"
#include "error_w.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>

#define THREAD_MAX 1000
#define BUFFER_MAX 1024

void *peer_thread(void *arg);
void *main_thread(void *arg);

void dumpBuffer(uint8_t* buffer, size_t len) {
    printf("current buffer:");
    for(size_t i=0; i<len; ++i) printf("%02X ",buffer[i]);
    printf("\n");
}

void run_s(server_t *s)
{
    pthread_t server_thread;
    pthread_create(&server_thread,NULL,(void *)main_thread,s);
    pthread_join(server_thread,NULL);
}

void *main_thread(void *arg)
{
    server_t *s = (server_t *)arg;

    fd_set r_set, w_set, e_set; 
    fd_set active_fd_set;

    FD_ZERO(&r_set);
    FD_ZERO(&w_set);
    FD_ZERO(&e_set);

    FD_ZERO(&active_fd_set);
    FD_SET(s->socket, &active_fd_set);

    int socket_max = s->socket;
    int ready_count;

    for(;;)
    {
        r_set = active_fd_set;
        ready_count = select(socket_max + 1, &r_set, NULL,NULL,NULL);//&w_set, &e_set, NULL);

        if(ready_count < 0)
        {
            close_s(s);
            err_e("select");
        }

        if(FD_ISSET(s->socket,&r_set))
        {
            peer_t *p = accept_p(s);
            printf("new peer %s:%d\n",
                    inet_ntoa(*(struct in_addr *)(&(p->addr))),
                    ntohs(p->addr.sin_port));
            if(p == NULL)
                printf("rejected, too many clients\n");
            else
            {
                FD_SET(p->socket,&active_fd_set);
                if(p->socket > socket_max)
                    socket_max = p->socket;

                pthread_create(&(p->worker), NULL, (void *)peer_thread, p);

                printf("accepted at %ld\n",(long)p - (long)(s->client_list));
            }

            if(--ready_count <= 0)
                continue;
        }

        for(int i = 0; i < sizeof(s->client_list)/sizeof(peer_t); ++i)
        {
            peer_t *p = &(s->client_list[i]);
            if(p->socket < 0) {
                if (i < 3) printf("skip %d\n", i);
                continue;
            }
            if(FD_ISSET(p->socket, &r_set))
            {
                if (i < 3) printf("right before lock of sock %d\n", i);
                pthread_mutex_lock(&(p->bufmutex));
                int readbytes = read(p->socket,p->buf+p->buflen,sizeof(p->buf) - p->buflen);
                printf("socket %d has data %d bytes\n", i, readbytes);
                pthread_mutex_unlock(&(p->bufmutex));
                if (i < 3) printf("right after lock of sock %d\n", i);

                if(readbytes > 0)
                {
                    p->buflen += readbytes;
                    p->newdata = 1;
                    printf("enqueue to socket %d\n", i);
                    pthread_cond_signal(&(p->nonEmpty));
                }
                else
                {
                    FD_CLR(p->socket, &active_fd_set);
                    p->done = 1;
                    //pthread_cancel(p->worker);
                    //pthread_join(p->worker, NULL);
                    printf("closed peer %s:%d\n", inet_ntoa(*(struct in_addr *)&(p->addr)), ntohs(p->addr.sin_port));
                    close_p(p);
                }

                if(--ready_count <= 0)
                    break;
            }
        }
    }
    return 0;
}

void handle_message(char *buf, int len)
{
    //printf("payload: %*.*s\n",len,len,buf);
    printf("payload: %*.*s\n",len,len,buf);
}

void *peer_thread(void *arg)
{
    peer_t *p = (peer_t *)arg;
    for(;;)
    {
        if (p->done) break;
        pthread_mutex_lock(&(p->bufmutex));
        while(p->newdata == 0)
            pthread_cond_wait(&(p->nonEmpty), &(p->bufmutex));
        p->newdata = 0;
        process_p((peer_t *)arg);
        pthread_mutex_unlock(&(p->bufmutex));
    }
    return 0;
}


void process_p(peer_t *p)
{ 
    //printf("read count: %d\n",left_n);
    //printf("buffer length:\t%d\n",p->buflen);

    while(p->buflen > 0)
    {
        //uint64_t tid;
        //pthread_threadid_np(p->worker,&tid);
        //printf("thread id: %llu\t",tid);
        printf("thread id: %lu\t",pthread_self());
        // fill header
        header_t *h = (header_t *)(p->buf);

        // invalid header
        if(h->identifier != 1234 || h->len > sizeof(p->buf))
        {
            printf("invalid packet\n");
            p->buflen = 0;
            return;
        }

        // partial packet
        if(h->len > p->buflen)
        {
            printf("partial packet\n");
            return;
        }
        
        // full message
        else
        {
            printf("complete packet from %s:%d\n",
                    inet_ntoa(*(struct in_addr *)&(p->addr)),
                    ntohs(p->addr.sin_port));
            //printf("packet length:\t%d\n",h->len);
            //printf("buffer length:\t%d\n",p->buflen);

            int strlen = h->len - sizeof(header_t);
            printf("payload:%*.*s\n",strlen,strlen,p->buf + sizeof(header_t));

            p->buflen -= h->len;
            if(p->buflen > 0)
                memmove(p->buf, p->buf + h->len, p->buflen);
            else
            {
                p->buflen = 0;
                return;
            }
        }
    }
}

int main(int argc, char **argv)
{
    server_t *server = make_s(5001);
    start_s(server);
    run_s(server);
    close_s(server);
    free_s(server);
    return 0;
}
