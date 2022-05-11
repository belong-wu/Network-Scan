#pragma once
#include "../Common.h"

#include <iostream>
#include <vector>
#include <string>

class HostScan
{
protected:
    /**
     * MAC address of the host
     */
    unsigned char mac_addr[ETH_ADDR_LENGTH];
    /**
     * IP address of the host
     */
    std::string ip_addr;

public:
    HostScan();
    HostScan(std::string dev_name);
    /**
     *  scan by IP address
     */
    virtual bool Scan(std::string ip_addr) = 0;

    /**
     *  scan by IP address and mask
     */
    // virtual std::vector<std::string> Scan(std::string ip_addr, unsigned short mask) = 0;

    /**
     *  scan by IP address list
     */
    // virtual std::vector<std::string> Scan(std::vector<std::string> ip_addr_vec) = 0;

private:
    bool GetMacAndIpAddr(std::string dev_name);
    bool GetMacAndIpAddr();
};