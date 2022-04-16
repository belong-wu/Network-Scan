
#pragma once
#include <iostream>
#include <cstring>
#include <string>

class ARPSender
{
private:
    /**
     * MAC address of the host
     */
    unsigned char mac_addr[MAC_LENGTH];
    /**
     * IP address of the host
     */
    unsigned int ip_addr;

public:
    ARPSender(unsigned char* mac_addr, unsigned int ip_addr) {
        memcpy(this->mac_addr, mac_addr, MAC_LENGTH);
        this->ip_addr = ip_addr;
    }

    bool SendArp(std::string ) {
        
    }
};
