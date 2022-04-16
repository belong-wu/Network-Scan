#include "HostScan.h"
#include "ARPReceiver.h"
#include "ARPSender.h"

#include <net/ethernet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

class ARPHostScan : public HostScan
{
private:
    ARPSender arp_sender;
    ARPReceiver arp_receiver;

public:
    bool Scan(std::string ip_addr)
    {
        int sock_fd;
        if (sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) < 0)
        {
            perror("socket failed");
            return -1;
        }
        // receiver preparation
        arp_receiver.Prepare(sock_fd);
        
    }
};
