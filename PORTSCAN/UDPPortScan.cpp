#include "PortScan.cpp"
#include "../Common.h"
#include "../CommonUtil.cpp"
#include "../ICMPPacket.h"

#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unordered_set>
#include <unordered_map>
#include <unistd.h>

class UDPPortScan : public PortScan
{
private:
    int epoll_fd;

public:
    UDPPortScan() : PortScan()
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

    std::vector<ip_port_t> Scan(std::unordered_set<std::string> dst_ip_addr_set, std::unordered_set<int> port_set)
    {
        int send_sock_fd;
        if ((send_sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        {
            perror("send socket error");
            return {};
        }

        int recv_sock_fd;
        if ((recv_sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
        {
            perror("receive socket error");
            close(send_sock_fd);
            return {};
        }

        if (!Prepare(recv_sock_fd) || !SendUdp(send_sock_fd, dst_ip_addr_set, port_set))
        {
            close(send_sock_fd);
            close(recv_sock_fd);
            return {};
        }

        std::unordered_set<ip_port_t, ip_port_hash, ip_port_equal> dst_ip_port_set;
        for (std::string ip : dst_ip_addr_set)
        {
            for (int port : port_set)
            {
                ip_port_t ip_port = {ip, port};
                dst_ip_port_set.insert(ip_port);
            }
        }

        std::vector<ip_port_t> survival_ip_vec = ReceiveIcmp(recv_sock_fd, dst_ip_port_set);
        close(send_sock_fd);
        close(recv_sock_fd);
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

    bool SendUdp(int sock_fd, std::unordered_set<std::string> dst_ip_addr_set, std::unordered_set<int> port_set)
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
                {
                    perror("send to error");
                    return false;
                }
            }
        }
        return true;
    }

    std::vector<ip_port_t> ReceiveIcmp(int sock_fd, std::unordered_set<ip_port_t, ip_port_hash, ip_port_equal> ip_port_set)
    {
        std::vector<ip_port_t> survival_ip_port;
        epoll_event events[MAX_EPOLL_NUM];
        double start_time = GetTimeStamp();
        while (GetTimeStamp() < start_time + MAX_WAIT_TIME && ip_port_set.size() != 0)
        {
            int nfds = epoll_wait(epoll_fd, events, MAX_EPOLL_NUM, MAX_WAIT_TIME * 1000);
            if (nfds == -1)
            {
                perror("epoll wait error");
                return {};
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
                        return {};
                    }

                    // find icmp packet in ip packet
                    struct icmp_packet_t *icmp = (struct icmp_packet_t *)(buffer + 20);
                
                    // check type
                    if (icmp->type_ != 3)
                        continue;

                    std::string src_ip = inet_ntoa(peer_addr.sin_addr);
                    int src_port = ntohs(*(unsigned short*) (buffer + 50));
                    ip_port_t result;
                    result.ip = src_ip;
                    result.port = src_port;
                    if (ip_port_set.find(result) != ip_port_set.end())
                    {
                        std::cout << "the port of " << src_port << " of host " << src_ip << " is cloesd" << std::endl;
                        ip_port_set.erase(result);
                    }
                }
                else
                {
                    perror("epoll error");
                    return {};
                }
            }
        }
        for (ip_port_t ip_port : ip_port_set)
        {
            std::cout << "the port of " << ip_port.port << " of host " << ip_port.ip << " is open" << std::endl;
            survival_ip_port.push_back(ip_port);
        }
        return survival_ip_port;
    }
};
