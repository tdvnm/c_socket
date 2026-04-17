#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void
error(const char *msg)
{
    perror(msg);
    exit(1);
}

void
print_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        perror("Error opening file");
        return;
    }
    char line[256];
    while(fgets(line, sizeof(line), file))
        printf("%s", line);
    fclose(file);
}

int
main(int argc, char *argv[])
{
    int                sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent    *server;
    char               buffer[256];

    if(argc < 3)
    {
        fprintf(stderr, "usage: %s hostname port [read]\n", argv[0]);
        exit(1);
    }

    if(argc >= 4 && strcmp(argv[3], "read") == 0)
    {
        print_file("messages.txt");
        return 0;
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if(server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    printf("Connected. Type a message and press enter. Type 'quit' to exit.\n");

    while(1)
    {
        printf("> ");
        fflush(stdout);
        bzero(buffer, 256);

        if(fgets(buffer, 255, stdin) == NULL)
            break;

        buffer[strcspn(buffer, "\n")] = '\0';

        if(strcmp(buffer, "quit") == 0)
            break;
        if(strlen(buffer) == 0)
            continue;

        n = write(sockfd, buffer, strlen(buffer));
        if(n < 0)
            error("ERROR writing to socket");

        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);
        if(n <= 0)
        {
            printf("Server closed connection\n");
            break;
        }
        printf("Server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
