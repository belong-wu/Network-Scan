#pragma once

constexpr int MAX_SEND_NUM = 3;

constexpr int MAX_WAIT_TIME = 10;

constexpr int MAX_EPOLL_NUM = 200;

constexpr int MAX_BUF_SIZE = 65535;
// 以太网地址（等于MAC地址）长度
constexpr int ETH_ADDR_LENGTH = 6;
// IP地址长度
constexpr int IP_ADDR_LENGTH = 4;

// 目的MAC地址（广播地址）
const char BROADCAST_MAC_ADDR[ETH_ADDR_LENGTH] = {(char)0xFF,(char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF};

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