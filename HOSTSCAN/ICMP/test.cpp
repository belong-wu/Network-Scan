#include <iostream>
#include <unordered_map>
#include "ICMPHostScan.cpp"

int main()
{
    ICMPHostScan arp_host_scan;
    std::unordered_set<std::string> ip_addr_set;
    ip_addr_set.insert("192.168.50.129");
    ip_addr_set.insert("192.168.159.1");
    ip_addr_set.insert("192.168.50.138");
    arp_host_scan.Scan(ip_addr_set);
}