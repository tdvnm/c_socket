/* A simple server in the internet domain using TCP
   The port number is passed as an argument
   This version runs forever, forking off a separate
   process for each connection
*/
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void dostuff(int); /* function prototype */
void
error(const char *msg)
{
    perror(msg);
    exit(1);
}

int
main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    //checking for port number
    if(argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    // create sockfc, returns a file descriptor, and reutrns-1 when error
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        error("ERROR opening socket");

    // bzero() clears the memory, sets all bytes to 0, 
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno                    = atoi(argv[1]);
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(portno);
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    // listen() marks the socket as a passive socket that will be used to accept incoming connection requests
    listen(sockfd, 5);
    // cli_addr is used to accept the incoming connection
    clilen = sizeof(cli_addr);
    char *text = argv[3];
    while(1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        if(newsockfd < 0)
            error("ERROR on accept");
        pid = fork();
        if(pid < 0)
            error("ERROR on fork");
        if(pid == 0)
        {
            close(sockfd);
            dostuff(newsockfd);
            exit(0);
        }
        else
            close(newsockfd);
    } /* end of while */
    close(sockfd);
    return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void
dostuff(int sock)
{
    int n;
    char buffer[256];
    char text[256];
    FILE *fp;

    bzero(buffer, 256);
    n = read(sock, buffer, 255);
    if(n < 0)
        error("ERROR reading from socket");

    // Print message to terminal
    printf("Here is the message: %s\n", buffer);
    printf("your textfile is %s\n", text);

    // Open the file in append mode
    fp = fopen("messages.txt", "a");
    if(fp == NULL)
    {
        perror("ERROR opening file");
        return;
    }

    // Write the message to the file
    fprintf(fp, "%s\n", buffer);
    fclose(fp); // Close the file

    // Send acknowledgment to client
    n = write(sock, "I got your message", 18);
    if(n < 0)
        error("ERROR writing to socket");
}
