#pragma once

#include <iostream>
#include <string>

// 以太网头部
struct eth_header_t {
	// 目标以太网地址
	unsigned char dst_eth_[ETH_ADDR_LENGTH];

	// 源以太网地址
	unsigned char src_eth_[ETH_ADDR_LENGTH];

	// 帧类型
	unsigned short type_;
};

// arp头部
struct arp_header_t {
	// 硬件类型
	unsigned short htype_;

	// 协议类型
	unsigned short ptype_;

	// 硬件地址长度
	unsigned char hlen_;

	// 协议地址长度
	unsigned char plen_;

	// 操作码（1为ARP请求，2为ARP应答）
	unsigned short opcode_;

	// 源MAC地址
	unsigned char src_mac_[ETH_ADDR_LENGTH];

	// 源IP地址
	unsigned char src_ip_[IP_ADDR_LENGTH];

	// 目的MAC地址
	unsigned char dst_mac_[ETH_ADDR_LENGTH];

	// 目的IP地址
	unsigned char dst_ip_[IP_ADDR_LENGTH];
};

// arp包结构（包括以太网头部和arp头部）
struct arp_packet_t {
	// 以太网头部
	struct eth_header_t eth_header_;

	// arp头部
	struct arp_header_t arp_header_;
};