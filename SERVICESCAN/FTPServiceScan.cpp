#include <iostream>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unordered_set>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <unordered_map>

struct ip_port_t
{
    std::string ip;
    int port;
};

struct ip_port_hash
{
    size_t operator()(const ip_port_t &r1) const
    {
        return std::hash<std::string>()(r1.ip) ^ std::hash<int>()(r1.port);
    }
};

struct ip_port_equal
{
    bool operator()(const ip_port_t &rc1, const ip_port_t &rc2) const noexcept
    {
        return rc1.ip == rc2.ip && rc1.port == rc2.port;
    }
};

class FTPServiceScan
{
public:
    std::unordered_map<ip_port_t, std::string, ip_port_hash, ip_port_equal> Scan(std::unordered_set<std::string> dst_ip_addr_set, std::unordered_set<int> dst_port_set)
    {
        int sock_fd;
        std::unordered_map<ip_port_t, std::string, ip_port_hash, ip_port_equal> result;
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            perror("socket error");
            return result;
        }
        return ConnectFTP(sock_fd, dst_ip_addr_set, dst_port_set);
    }

private:
    std::unordered_map<ip_port_t, std::string, ip_port_hash, ip_port_equal> ConnectFTP(int sock_fd, std::unordered_set<std::string> dst_ip_addr_set, std::unordered_set<int> dst_port_set)
    {
        std::unordered_map<ip_port_t, std::string, ip_port_hash, ip_port_equal> result;
        sockaddr_in sock_addr;
        sock_addr.sin_family = AF_INET;

        for (std::string ip_addr : dst_ip_addr_set)
        {
            sock_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
            for (int port : dst_port_set)
            {
                sock_addr.sin_port = htons(port);
                int ret = connect(sock_fd, (sockaddr *)&sock_addr, sizeof(sockaddr));
                if (ret < 0)
                {
                    perror("connect error");
                    std::cout << "the port of " << port << " of host " << ip_addr << " is cloesd" << std::endl;
                }
                // allocate buffer
                char buffer[1500];
                struct sockaddr_in peer_addr;

                // receive another packet
                unsigned int addr_len = sizeof(peer_addr);
                int bytes_num = recvfrom(sock_fd, buffer, sizeof(buffer), 0,
                                      (struct sockaddr *)&peer_addr, &addr_len);
                if (bytes_num <= 0) {
                    perror("recv from error");
                    return result;
                }

                char banner[50];
                bzero(banner,sizeof(banner));
                memcpy(banner, buffer + 5, 12);
                std::cout << banner << std::endl;
                // else
                // {
                //     std::cout << "the port of " << port << " of host " << ip_addr << " is open" << std::endl;
                //     ip_port_t survival = {ip_addr, port};
                //     result.push_back(survival);
                // }
            }
        }
        return result;
    }
};