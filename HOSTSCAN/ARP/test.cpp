#include <iostream>
#include <unordered_map>
#include "ARPHostScan.cpp"

int main()
{
    ARPHostScan arp_host_scan;
    std::unordered_set<std::string> ip_addr_set;
    ip_addr_set.insert("192.168.50.129");
    ip_addr_set.insert("192.168.50.138");
    arp_host_scan.Scan(ip_addr_set);
}