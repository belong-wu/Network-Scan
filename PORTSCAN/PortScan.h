#pragma once

#include <iostream>
#include <vector>
#include <string>

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

    /**
     *  scan by IP address and port
     */
    virtual bool Scan(std::vector<std::string> ip_addr, int port);

};