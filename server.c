/* A simple TCP server that forks a child process per connection,
   logs messages with timestamps, and echoes an acknowledgment back. */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void dostuff(int, const char *);

void
error(const char *msg)
{
    perror(msg);
    exit(1);
}

int
main(int argc, char *argv[])
{
    int                sockfd, newsockfd, portno, pid;
    socklen_t          clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int                opt = 1;
    char               client_ip[INET_ADDRSTRLEN];

    if(argc < 2)
    {
        fprintf(stderr, "usage: %s port\n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("ERROR opening socket");

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno                    = atoi(argv[1]);
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(portno);

    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Listening on port %d\n", portno);

    while(1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if(newsockfd < 0)
            error("ERROR on accept");

        inet_ntop(AF_INET, &cli_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Connection from %s\n", client_ip);

        pid = fork();
        if(pid < 0)
            error("ERROR on fork");
        if(pid == 0)
        {
            close(sockfd);
            dostuff(newsockfd, client_ip);
            exit(0);
        }
        else
            close(newsockfd);
    }

    close(sockfd);
    return 0;
}

void
dostuff(int sock, const char *client_ip)
{
    int        n;
    char       buffer[256];
    time_t     now;
    struct tm *t;
    char       timestamp[32];
    FILE      *fp;

    while(1)
    {
        bzero(buffer, 256);
        n = read(sock, buffer, 255);
        if(n <= 0)
            break;

        buffer[strcspn(buffer, "\n")] = '\0';
        if(strlen(buffer) == 0)
            continue;

        now = time(NULL);
        t   = localtime(&now);
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

        printf("[%s] %s: %s\n", timestamp, client_ip, buffer);

        fp = fopen("messages.txt", "a");
        if(fp != NULL)
        {
            fprintf(fp, "[%s] %s: %s\n", timestamp, client_ip, buffer);
            fclose(fp);
        }

        n = write(sock, "message received", 18);
        if(n < 0)
            break;
    }

    printf("Connection from %s closed\n", client_ip);
}
