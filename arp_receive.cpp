#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <sys/types.h>

int main()
{
    int sock;

    /* buf is buffer containing the ethernet frame */
    char buf[65535];
    struct ether_arp *arp_frame;

    /* skipping the 14 bytes of ethernet frame header */
    arp_frame = (struct ether_arp *)(buf + 14);

    if ((sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0)
    {
        perror("socket() failed ");
        exit(EXIT_FAILURE);
    }

    /* read until we got an arp packet or socket got a problem */
    while (recv(sock, buf, sizeof(buf), 0))
    {
        /* skip to the next frame if it's not an ARP packet */
        if ((((buf[12]) << 8) + buf[13]) != ETH_P_ARP)
            continue;

        /* skip to the next frame if it's not an ARP REPLY */
        if (ntohs(arp_frame->arp_op) != ARPOP_REPLY)
            continue;

        /* got an arp reply! this is where i'm printing what you need */
        printf("I got an arp reply from host with ip: %u.%u.%u.%u\n", arp_frame->arp_spa[0],
               arp_frame->arp_spa[1],
               arp_frame->arp_spa[2],
               arp_frame->arp_spa[3]);
    }
}