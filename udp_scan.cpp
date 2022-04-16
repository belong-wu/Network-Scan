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
#define MTU 1500
#define MAGIC_LEN 20
struct icmp_echo {
    // header
    uint8_t type;
    uint8_t code;
    uint16_t checksum;

    uint16_t ident;
    uint16_t seq;

    // data
    double sending_ts;
    char magic[MAGIC_LEN];
};

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

double get_timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + ((double)tv.tv_usec) / 1000000;
}


// Type: 3 (Destination unreachable)
// Code: 3 (Port unreachable), 13 (communication administratively filtered)
int RecvIcmpReply(int sock) {
    // allocate buffer
    char buffer[MTU];
    struct sockaddr_in peer_addr;

    // receive another packet
    unsigned int addr_len = sizeof(peer_addr);
    printf("before receive\n");
    int bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                         (struct sockaddr *)&peer_addr, &addr_len);
    printf("received\n");
    if (bytes == -1) {
        // normal return when timeout
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return 0;
        }
        return -1;
    }

    // find icmp packet in ip packet
    struct icmp_echo *icmp = (struct icmp_echo *)(buffer + 20);

    // check type
    if (icmp->type != 3 || icmp->code != 3) {
        return 0;
    }
    // print info
    printf("%s seq=%d %5.2fms\n",
           inet_ntoa(peer_addr.sin_addr),
           ntohs(icmp->seq),
           (get_timestamp() - icmp->sending_ts) * 1000);

    return 0;
}

int main()
{
    int sock_fd = InitSocket();
    if (sock_fd < 0) {
        return 0;
    }
    std::string dst_ip = "192.168.50.129";
    unsigned short port = 8005;
    SendUdp(sock_fd, dst_ip, port);
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == -1) {
        perror("create sock failed");
        return -1;
    }
    RecvIcmpReply(sock);
    close(sock_fd);
    return 0;
}