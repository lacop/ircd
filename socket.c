#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <asm-generic/socket.h>

#include "util.h"

int socket_create_and_bind(unsigned long addr, unsigned short port)
{
    // Get an INET+STREAM socket (TCP/IP)
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        return error_print("socket");
    }
    
    int reuse_addr = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
    
    struct sockaddr_in socket_addr;
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_addr.s_addr = addr;
    socket_addr.sin_port = htons(port);
    
    if (bind(socketfd, (struct sockaddr *) &socket_addr, sizeof(socket_addr)) < 0)
    {
        return error_print("bind");
    }
    
    return socketfd;    
}

int socket_set_nonblocking(int socketfd)
{
    // Get old flags
    int flags = fcntl(socketfd, F_GETFL);
    if (flags < 0)
    {
        return error_print("fcntl(F_GETFL)");
    }
    // Add nonblocking flag
    flags |= O_NONBLOCK;
    if (fcntl(socketfd, F_SETFL, flags) < 0)
    {
        return error_print("fcntl(F_SETFL)");
    }
    
    return 0;    
}

int socket_listen(int socketfd, int backlog)
{
    // Default size for backlog is maximum possible
    if (backlog < 0)
    {
        backlog = SOMAXCONN;
    }
    
    if (listen(socketfd, backlog) < 0)
    {
        return error_print("listen");
    }
    
    return 0;
}

int socket_epoll_create_and_setup(int socketfd)
{
    int epollfd = epoll_create1(0);
    if (epollfd < 0)
    {
        return error_print("epoll_create1");
    }
    
    struct epoll_event event;
    event.data.fd = socketfd;
    event.events = EPOLLIN | EPOLLET; // TODO: remove edge triggering flag, use level triggering
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &event) < 0)
    {
        return error_print("epoll_ctl");
    }
    
    return epollfd;
}