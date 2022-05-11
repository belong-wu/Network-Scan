#include <iostream>
#include <string>

#include "./HOSTSCAN/ARP/ARPHostScan.cpp"
#include "./HOSTSCAN/ICMP/ICMPHostScan.cpp"
#include "./HOSTSCAN/ICMPBROADCAST/ICMPBroadcastHostScan.cpp"
#include "./PORTSCAN/TCPPortScan.cpp"
#include "./PORTSCAN/UDPPortScan.cpp"
#include "./OSSCAN/TTLOsScan.cpp"

int main() {
    ARPHostScan arp_scan;
    ICMPHostScan icmp_scan;
    ICMPBroadCastHostScan icmp_broadcast_scan;
    TCPPortScan tcp_scan;
    UDPPortScan udp_scan;
    TTLOsScan tll_scan;

    while(true) {
        std::string instruction;
        std::cin >> instruction;
        StringSplit(instruction, ' ');
    }
}