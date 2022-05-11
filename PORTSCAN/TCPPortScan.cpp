#include "PortScan.cpp"

#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unordered_set>
#include <unistd.h>

class TCPPortScan : public PortScan
{
public:
    std::vector<ip_port_t> Scan(std::unordered_set<std::string> dst_ip_addr_set, std::unordered_set<int> dst_port_set)
    {
        int sock_fd;
        if ((sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        {
            perror("socket error");
            return {};
        }
        return ConnectPort(sock_fd, dst_ip_addr_set, dst_port_set);
    }

private:
    std::vector<ip_port_t> ConnectPort(int sock_fd, std::unordered_set<std::string> dst_ip_addr_set, std::unordered_set<int> dst_port_set)
    {
        std::vector<ip_port_t> result;
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
                else
                {
                    std::cout << "the port of " << port << " of host " << ip_addr << " is open" << std::endl;
                    ip_port_t survival = {ip_addr, port};
                    result.push_back(survival);
                }
            }
        }
        return result;
    }
};