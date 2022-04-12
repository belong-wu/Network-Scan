#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdarg.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/select.h>

// init socket
int InitSocket()
{
    int sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd < 0)
    {
        std::cout << "socket initialization failed" << std::endl;
        return -1;
    }
    return sock_fd;
}

void ConnectPort(int sock_fd, unsigned short port, std::string ip_addr)
{
    sockaddr_in sock_addr;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    int ret = connect(sock_fd, (sockaddr *)&sock_addr, sizeof(sockaddr));
    if (ret < 0)
    {
        perror("connect error");
    } else {
        std::cout << ret << std::endl;
    }
}

int main()
{
    std::string host_name = "192.168.50.129";
    unsigned short int port = 8081;
    int sock_fd = InitSocket();
    ConnectPort(sock_fd, port, host_name);
    
}
