#include <iostream>
#include <unordered_set>

#define Windows 0
#define Linux 1

class OSScan
{
public:
    virtual int Scan(std::unordered_set<std::string> dst_ip_addr_set);
};