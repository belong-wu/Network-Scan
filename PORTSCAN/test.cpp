#include <iostream>
#include "UDPPortScan.cpp"
#include <unordered_set>

int main()
{
    UDPPortScan udp_port_scan;
    std::unordered_set<std::string> dst_ip_addr_set;
    std::unordered_set<int> dst_port_set;
    dst_ip_addr_set.insert("192.168.50.138");
    dst_port_set.insert(8000);
    dst_port_set.insert(700);
    udp_port_scan.Scan(dst_ip_addr_set, dst_port_set);
}