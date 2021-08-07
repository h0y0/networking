#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 5001
   
typedef struct header {
    int magic;
    int len;
} header_t;

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    header_t h;
    h.magic = 1234;
    char msg[27] = "abcdefghijklmnopqrstuvwxyz";
    unsigned long i;

    for( i = 10; i < sizeof(msg); ++i )
    {
        h.len = sizeof(header_t) + i;
        printf("len: %d\n",h.len);

        write(sock, &h, sizeof(h));
        write(sock, msg, i/2);
        sleep(1);
        write(sock, msg + i/2, i - i/2);
    }

    close(sock);

    return 0;
}
