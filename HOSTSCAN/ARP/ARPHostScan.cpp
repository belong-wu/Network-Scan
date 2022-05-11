#include "../HostScan.cpp"
#include "ARPPacket.h"
#include "../../CommonUtil.cpp"

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

constexpr int STR_BUF_SIZE = 20;

class ARPHostScan : public HostScan
{
private:
    int epoll_fd;

public:
    ARPHostScan() : HostScan::HostScan()
    {
        if ((epoll_fd = epoll_create1(0)) <= 0)
        {
            perror("epoll_create1 failed");
            exit(EXIT_FAILURE);
        }
    }

    ~ARPHostScan()
    {
        close(epoll_fd);
    }

    bool Scan(std::string ip_addr)
    {
        int sock_fd;
        if ((sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
        {
            perror("socket error");
            return false;
        }
        sockaddr_in addr;
        addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
        bind(sock_fd, (sockaddr *)&addr, sizeof(addr));

        std::unordered_set<std::string> ip_addr_set;
        ip_addr_set.insert(ip_addr);
        if (!Prepare(sock_fd) || !SendArp(sock_fd, ip_addr_set))
        {
            close(sock_fd);
            return false;
        }
        std::vector<std::string> survival_ip_vec = ReceiveARP(sock_fd, ip_addr_set);
        if (survival_ip_vec.size() == 1)
        {
            close(sock_fd);
            return true;
        }
        close(sock_fd);
        return true;
    }

    std::vector<std::string> Scan(std::unordered_set<std::string> ip_addr_set)
    {
        int sock_fd;
        if ((sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
        {
            perror("socket error");
            return {};
        }

        if (!Prepare(sock_fd) || !SendArp(sock_fd, ip_addr_set))
        {
            close(sock_fd);
            return {};
        }
        std::vector<std::string> survival_ip_vec = ReceiveARP(sock_fd, ip_addr_set);
        close(sock_fd);
        return survival_ip_vec;
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

    bool SendArp(int sock_fd, std::unordered_set<std::string> dst_ip_addr_set)
    {
        arp_packet_t arp_packet;
        // build eth header
        memcpy(arp_packet.eth_header_.src_eth_, mac_addr, sizeof(mac_addr));
        memcpy(arp_packet.eth_header_.dst_eth_, BROADCAST_MAC_ADDR, sizeof(BROADCAST_MAC_ADDR));
        arp_packet.eth_header_.type_ = htons(0x0806);
        // build arp header
        arp_packet.arp_header_.htype_ = htons(0x01);
        arp_packet.arp_header_.ptype_ = htons(0x800);
        arp_packet.arp_header_.hlen_ = ETH_ADDR_LENGTH;
        arp_packet.arp_header_.plen_ = IP_ADDR_LENGTH;
        arp_packet.arp_header_.opcode_ = htons(0x01);
        memcpy(arp_packet.arp_header_.src_mac_, mac_addr, sizeof(mac_addr));
        inet_aton(ip_addr.c_str(), (in_addr *)&arp_packet.arp_header_.src_ip_);
        memcpy(arp_packet.arp_header_.dst_mac_, BROADCAST_MAC_ADDR, sizeof(BROADCAST_MAC_ADDR));

        sockaddr_ll sl;
        bzero(&sl, sizeof(sl));
        sl.sll_family = AF_PACKET;
        sl.sll_ifindex = IFF_BROADCAST;

        for (auto it = dst_ip_addr_set.begin(); it != dst_ip_addr_set.end(); it++)
        {
            if (inet_aton((*it).c_str(), (in_addr *)&arp_packet.arp_header_.dst_ip_) == 0)
            {
                perror("inet aton error");
                return false;
            };

            for (int i = 0; i < MAX_SEND_NUM; i++)
            {
                if (sendto(sock_fd, &arp_packet, sizeof(arp_packet), 0, (struct sockaddr *)&sl, sizeof(sl)) <= 0)
                {
                    perror("send failed");
                    return false;
                }
            }
        }
        return true;
    }

    // bool ReceiveARP(int sock_fd)
    // {
    //     epoll_event events[MAX_EPOLL_NUM];
    //     double start_time = GetTimeStamp();
    //     while (GetTimeStamp() < start_time + MAX_WAIT_TIME)
    //     {
    //         int nfds = epoll_wait(epoll_fd, events, MAX_EPOLL_NUM, MAX_WAIT_TIME * 1000);
    //         if (nfds == -1)
    //         {
    //             perror("epoll_wait");
    //             return false;
    //         }

    //         for (int n = 0; n < nfds; ++n)
    //         {
    //             if (events[n].data.fd == sock_fd)
    //             {
    //                 char buf[65535];
    //                 struct ether_arp *arp_frame;
    //                 arp_frame = (struct ether_arp *)(buf + 14);

    //                 if (recv(sock_fd, buf, sizeof(buf), 0))
    //                 {
    //                     /* skip to the next frame if it's not an ARP packet */
    //                     if ((((buf[12]) << 8) + buf[13]) != ETH_P_ARP)
    //                         continue;

    //                     /* skip to the next frame if it's not an ARP REPLY */
    //                     if (ntohs(arp_frame->arp_op) != ARPOP_REPLY)
    //                         continue;

    //                     /* got an arp reply! this is where i'm printing what you need */
    //                     printf("I got an arp reply from host with ip: %u.%u.%u.%u\n", arp_frame->arp_spa[0],
    //                            arp_frame->arp_spa[1],
    //                            arp_frame->arp_spa[2],
    //                            arp_frame->arp_spa[3]);
    //                     return true;
    //                 }
    //             }
    //             else
    //             {
    //                 perror("epoll error");
    //                 return false;
    //             }
    //         }
    //     }
    //     return false;
    // }

    std::vector<std::string> ReceiveARP(int sock_fd, std::unordered_set<std::string> ip_set)
    {
        std::vector<std::string> survival_ip;
        epoll_event events[MAX_EPOLL_NUM];
        double start_time = GetTimeStamp();
        while (GetTimeStamp() < start_time + MAX_WAIT_TIME && ip_set.size() != 0)
        {
            int nfds = epoll_wait(epoll_fd, events, MAX_EPOLL_NUM, MAX_WAIT_TIME * 1000);
            if (nfds == -1)
            {
                perror("epoll_wait");
                return survival_ip;
            }

            for (int n = 0; n < nfds; ++n)
            {
                if (events[n].data.fd == sock_fd)
                {
                    char buf[MAX_BUF_SIZE];
                    struct ether_arp *arp_frame;
                    arp_frame = (struct ether_arp *)(buf + 14);

                    if (recv(sock_fd, buf, sizeof(buf), 0))
                    {
                        /* skip to the next frame if it's not an ARP packet */
                        if ((((buf[12]) << 8) + buf[13]) != ETH_P_ARP)
                            continue;

                        /* skip to the next frame if it's not an ARP REPLY */
                        if (ntohs(arp_frame->arp_op) != ARPOP_REPLY)
                            continue;

                        /* got an arp reply! this is where i'm printing what you need */
                        // printf("I got an arp reply from host with ip: %u.%u.%u.%u\n", arp_frame->arp_spa[0],
                        //        arp_frame->arp_spa[1],
                        //        arp_frame->arp_spa[2],
                        //        arp_frame->arp_spa[3]);
                        char str_buf[STR_BUF_SIZE];
                        snprintf(str_buf, STR_BUF_SIZE, "%u.%u.%u.%u", arp_frame->arp_spa[0], arp_frame->arp_spa[1], arp_frame->arp_spa[2], arp_frame->arp_spa[3]);
                        if (ip_set.find(str_buf) != ip_set.end())
                        {
                            std::cout << str_buf << " is survival" << std::endl;
                            ip_set.erase(str_buf);
                            survival_ip.push_back(str_buf);
                        }
                    }
                }
                else
                {
                    perror("epoll error");
                    return survival_ip;
                }
            }
        }
        return survival_ip;
    }
};
