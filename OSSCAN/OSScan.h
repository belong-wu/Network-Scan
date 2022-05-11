#pragma once
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#define Windows 0
#define Linux 1

class OSScan
{
public:
    OSScan(){};
    virtual ~OSScan(){};
    virtual std::unordered_map<std::string, int> Scan(std::unordered_set<std::string> dst_ip_addr_set);
};