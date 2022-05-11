#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

struct ip_port_t {
    std::string ip;
    int port;
};

struct ip_port_hash
{	
	size_t operator()(const ip_port_t& r1) const
	{
		return std::hash<std::string>()(r1.ip) ^ std::hash<int>()(r1.port);
	}
};

struct ip_port_equal
{
	bool operator()(const ip_port_t& rc1, const ip_port_t& rc2) const noexcept
	{
		return rc1.ip == rc2.ip && rc1.port == rc2.port;
	}
 
};

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