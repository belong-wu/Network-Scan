#include "OSScan.h"
#include "../ICMPPacket.h"
#include "../Common.h"
#include "../CommonUtil.cpp"

#include <iostream>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <cstring>
#include <unordered_set>
#include <unordered_map>
#include <vector>

class TTLOsScan
{
private:
    int epoll_fd;

public:
    TTLOsScan()
    {
        if ((epoll_fd = epoll_create1(0)) <= 0)
        {
            perror("epoll_create1 failed");
            exit(EXIT_FAILURE);
        }
    }

public:
    std::unordered_map<std::string, int> Scan(std::unordered_set<std::string> dst_ip_addr_set)
    {
        int sock_fd;
        if ((sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
        {
            perror("socket error");
            return {};
        }

        if (!Prepare(sock_fd) || !SendIcmp(sock_fd, dst_ip_addr_set))
        {
            close(sock_fd);
            return {};
        }
        std::unordered_map<std::string, int> ip_os_map = ReceiveIcmp(sock_fd, dst_ip_addr_set);
        close(sock_fd);
        return ip_os_map;
    }

private:
    bool Prepare(int sock_fd)
    {
        epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = sock_fd;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev) == -1)
        {
            perror("epoll_ctl failed");
            return false;
        }
        return true;
    }

    bool SendIcmp(int sock_fd, std::unordered_set<std::string> dst_ip_addr_set)
    {
        sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = 0;

        icmp_packet_t icmp_packet;
        bzero(&icmp_packet, sizeof(icmp_packet));

        icmp_packet.type_ = 8;
        icmp_packet.code_ = 0;
        icmp_packet.checksum_ = htons(
            CalculateChecksum((unsigned char *)&icmp_packet, sizeof(icmp_packet)));

        for (auto it = dst_ip_addr_set.begin(); it != dst_ip_addr_set.end(); it++)
        {
            if (inet_aton((*it).c_str(), (struct in_addr *)&addr.sin_addr.s_addr) == 0)
            {
                perror("inet aton error");
                return false;
            };

            for (int i = 0; i < MAX_SEND_NUM; i++)
            {
                if (sendto(sock_fd, &icmp_packet, sizeof(icmp_packet), 0,
                           (sockaddr *)&addr, sizeof(addr)) <= 0)
                {
                    perror("send to error");
                    return false;
                }
            }
        }
        return true;
    }

    std::unordered_map<std::string, int> ReceiveIcmp(int sock_fd, std::unordered_set<std::string> ip_set)
    {
        std::unordered_map<std::string, int> ip_os_map;
        epoll_event events[MAX_EPOLL_NUM];
        double start_time = GetTimeStamp();
        while (GetTimeStamp() < start_time + MAX_WAIT_TIME && ip_set.size() != 0)
        {
            int nfds = epoll_wait(epoll_fd, events, MAX_EPOLL_NUM, MAX_WAIT_TIME * 1000);
            if (nfds == -1)
            {
                perror("epoll wait error");
                return ip_os_map;
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
                        return ip_os_map;
                    }

                    // find icmp packet in ip packet
                    struct icmp_packet_t *icmp = (struct icmp_packet_t *)(buffer + 20);

                    // check type
                    if (icmp->type_ != 0 || icmp->code_ != 0)
                    {
                        continue;
                    }

                    std::string src_ip = inet_ntoa(peer_addr.sin_addr);
                    if (ip_set.find(src_ip) != ip_set.end())
                    {
                        unsigned char ttl = *(buffer + 8);
                        int os_type;
                        switch (ttl)
                        {
                        case 128:
                            os_type = Windows;
                            std::cout << "ip : " + src_ip + " is Windows" << std::endl;
                            break;
                        case 64:
                            os_type = Linux;
                            std::cout << "ip : " + src_ip + " is Linux" << std::endl;
                            break;
                        default:
                            continue;
                        }
                        ip_set.erase(src_ip);
                        ip_os_map[src_ip] = os_type;
                    }
                }
                else
                {
                    perror("epoll error");
                    return ip_os_map;
                }
            }
        }
        return ip_os_map;
    }
};