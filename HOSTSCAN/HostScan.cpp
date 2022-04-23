#include "HostScan.h"
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unordered_set>

constexpr int MAX_DEVICE_NUM = 16;

const std::unordered_set<std::string> dev_name_list_ = {"eth0", "ens33"};

HostScan::HostScan() {
    if (!GetMacAndIpAddr()) {
        exit(EXIT_FAILURE);
    }
}

HostScan::HostScan(std::string dev_name) {
        if (!GetMacAndIpAddr(dev_name)) {
        exit(EXIT_FAILURE);
    }
}

bool HostScan::GetMacAndIpAddr(std::string dev_name)
{
    int sock_fd;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket error");
        return false;
    }

    ifreq if_req;
    strcpy(if_req.ifr_name, dev_name.c_str());
    if (ioctl(sock_fd, SIOCGIFADDR, &if_req) < 0)
    {
        perror("ioctl error");
        return false;
    }
    ip_addr = inet_ntoa(((struct sockaddr_in*)&(if_req.ifr_addr))->sin_addr);

    if (ioctl(sock_fd, SIOCGIFHWADDR, &if_req) < 0)
    {
        perror("ioctl error");
        return false;
    }
    for (int i = 0; i < ETH_ADDR_LENGTH; i++)
    {
        mac_addr[i] = static_cast<unsigned char>(if_req.ifr_hwaddr.sa_data[i]);
    }
    return true;
}

bool HostScan::GetMacAndIpAddr()
{
    int sock_fd;
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket error");
        return false;
    }

    ifreq if_req[MAX_DEVICE_NUM];
    ifconf if_conf;

    if_conf.ifc_len = sizeof(if_req);
    if_conf.ifc_buf = (caddr_t)if_req;
    if (ioctl(sock_fd, SIOCGIFCONF, (char *)&if_conf) < 0)
    {
        perror("ioctl failed");
        return false;
    }

    int dev_num = if_conf.ifc_len / sizeof(struct ifreq);
    for (int i = 0; i < dev_num; i++)
        if (dev_name_list_.find(if_req[i].ifr_name) != dev_name_list_.end())
            return GetMacAndIpAddr(if_req[i].ifr_name);
    return false;
}