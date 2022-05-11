#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

#include "../Common.h"

class PortScan
{
public:
	PortScan();
	virtual ~PortScan();

    /**
     *  scan by IP address and port
     */
    virtual std::vector<ip_port_t> Scan(std::unordered_set<std::string> dst_ip_addr_set, std::unordered_set<int> dst_port_set) = 0;

};