#include <iostream>
#include <unordered_map>
#include "ICMPBroadcastHostScan.cpp"

int main()
{
    ICMPBroadCastHostScan arp_host_scan;
    std::unordered_set<std::string> ip_addr_set;
    arp_host_scan.Scan();
}