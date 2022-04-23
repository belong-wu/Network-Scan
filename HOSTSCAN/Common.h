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