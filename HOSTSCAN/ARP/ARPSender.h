
// #pragma once
// #include <iostream>
// #include <cstring>
// #include <string>

// class ARPSender
// {
// private:
//     /**
//      * MAC address of the host
//      */
//     unsigned char mac_addr[ETH_ADDR_LENGTH];
//     /**
//      * IP address of the host
//      */
//     unsigned int ip_addr;

// public:
//     ARPSender(unsigned char* mac_addr, unsigned int ip_addr) {
//         memcpy(this->mac_addr, mac_addr, ETH_ADDR_LENGTH);
//         this->ip_addr = ip_addr;
//     }

//     bool SendArp(int sock_fd, std::string dst_ip);
// };
