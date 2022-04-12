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

#define BUFFER_SIZE 20

int InitSocket()
{
    /* 创建 socket */
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0)
        perror("create socket failed:");
    return sock_fd;
}

void SendUdp(int sock_fd, std::string dst_ip, unsigned short port)
{
    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());
    server_addr.sin_port = htons(port);

    char buffer[BUFFER_SIZE];
    bzero(buffer, BUFFER_SIZE);
    /* 发送文件名 */
    if (sendto(sock_fd, buffer, 10, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        perror("udp scan failed:");
}

void ReceiveUdp(int sock, std::string dst_ip, unsigned short port)
{
    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(dst_ip.c_str());
    server_addr.sin_port = htons(port);


}

int main()
{
    int sock_fd = InitSocket();
    if (sock_fd < 0)
    {
        return 0;
    }
    std::string dst_ip = "192.168.50.129";
    unsigned short port = 8080;
    SendUdp(sock_fd, dst_ip, port);
    close(sock_fd);
    return 0;
}