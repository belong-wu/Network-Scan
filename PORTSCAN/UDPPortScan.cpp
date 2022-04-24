#include "PortScan.h"
#include "./Common.h"
#include "./CommonUtil.cpp"
#include "./ICMPPacket.h"

#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unordered_set>
#include <unistd.h>

class UDPPortScan : public PortScan
{
private:
    int epoll_fd;

public:
    UDPPortScan() : PortScan::PortScan()
    {
        if ((epoll_fd = epoll_create1(0)) <= 0)
        {
            perror("epoll_create1 failed");
            exit(EXIT_FAILURE);
        }
    }

    ~UDPPortScan()
    {
        close(epoll_fd);
    }
    std::vector<std::string> Scan(std::unordered_set<std::string> dst_ip_addr_set, std::unordered_set<int> port_set)
    {
    }

private:
    void SendUdp(int sock_fd, std::unordered_set<std::string> dst_ip_addr_set, std::unordered_set<int> port_set)
    {
        sockaddr_in server_addr;
        bzero(&server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;

        for (auto ip_it = dst_ip_addr_set.begin(); ip_it != dst_ip_addr_set.end(); ip_it++)
        {
            server_addr.sin_addr.s_addr = inet_addr((*ip_it).c_str());
            for (auto port_it = port_set.begin(); port_it != port_set.end(); port_it++)
            {
                server_addr.sin_port = htons(*port_it);
                char buffer[MAX_BUF_SIZE];
                bzero(buffer, sizeof(buffer));
                if (sendto(sock_fd, buffer, 10, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
                    perror("sendto error");
                return;
            }
        }
    }

    std::vector<std::string> ReceiveIcmp(int sock_fd, std::unordered_set<std::string> ip_set)
    {
        std::vector<std::string> survival_ip;
        epoll_event events[MAX_EPOLL_NUM];
        double start_time = GetTimeStamp();
        while (GetTimeStamp() < start_time + MAX_WAIT_TIME && ip_set.size() != 0)
        {
            int nfds = epoll_wait(epoll_fd, events, MAX_EPOLL_NUM, MAX_WAIT_TIME * 1000);
            if (nfds == -1)
            {
                perror("epoll wait error");
                return survival_ip;
            }

            for (int n = 0; n < nfds; ++n)
            {
                if (events[n].data.fd == sock_fd)
                {
                    char buffer[MAX_BUF_SIZE];
                    struct sockaddr_in peer_addr;
                    unsigned int addr_len = sizeof(peer_addr);
                    if (recvfrom(sock_fd, buffer, sizeof(buffer), 0,
                                 (struct sockaddr *)&peer_addr, &addr_len) <= 0)
                    {
                        perror("recvfrom error");
                        return survival_ip;
                    }

                    // find icmp packet in ip packet
                    struct icmp_packet_t *icmp = (struct icmp_packet_t *)(buffer + 20);

                    // check type
                    if (icmp->type_ != 3)
                        continue;
                    
                    switch(icmp->type_) {
                        case 3 :

                    }
                    

                    std::string src_ip = inet_ntoa(peer_addr.sin_addr);
                    if (ip_set.find(src_ip) != ip_set.end())
                    {
                        std::cout << src_ip << " is survival" << std::endl;
                        ip_set.erase(src_ip);
                        survival_ip.push_back(src_ip);
                    }
                }
                else
                {
                    perror("epoll error");
                    return survival_ip;
                }
            }
        }
    }
};
