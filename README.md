# c_socket

A TCP client/server pair written in C. The server accepts connections, prints each message to stdout with a timestamp and the client's IP, and appends them to `messages.txt`. The client lets you send multiple messages in a single session without reconnecting each time. Made for my COMP205 system programming course.

## Compiling

``` bash
    make
```
Or manually if you don't have make:

``` bash
    gcc -Wall -o server server.c
    gcc -Wall -o client client.c
```

## Running

Start the server on a port (8080 or whatever you like):

``` bash
    ./server 8080
```

From another terminal, connect and start sending messages:

``` bash
    ./client localhost 8080
```

Type a message and hit enter. Type `quit` or press Ctrl+D to disconnect.

To view the message log without connecting to the server:

``` bash
    ./client localhost 8080 read
```

This reads from `messages.txt` in the current directory without the server.

## How it works

The server forks a child process for each incoming connection so multiple
clients can connect at the same time. Each message gets a timestamp and the
sender's IP prepended before being written to `messages.txt`. The server
replies with a short acknowledgment after each message.

Messages in the log look like:

    [2024-03-15 14:23:01] 127.0.0.1: hello there
