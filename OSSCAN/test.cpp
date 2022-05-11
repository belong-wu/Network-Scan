#include <iostream>
#include "TTLOsScan.cpp"

int main() {
    TTLOsScan ttl_scan;
    std::unordered_set<std::string> ip_set;
    ip_set.insert("192.168.50.129");
    ip_set.insert("192.168.159.1");
    std::unordered_map<std::string, int> map = ttl_scan.Scan(ip_set);
}