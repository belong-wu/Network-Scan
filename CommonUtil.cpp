#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <sys/time.h>

long int GetTimeStamp()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        perror("get time error");
        exit(EXIT_FAILURE);
    }
    return tv.tv_sec;
}

unsigned short int CalculateChecksum(unsigned char* buffer, int bytes)
{
    unsigned int checksum = 0;
    unsigned char* end = buffer + bytes;

    if (bytes % 2 == 1) {
        end = buffer + bytes - 1;
        checksum += (*end) << 8;
    }

    while (buffer < end) {
        checksum += buffer[0] << 8;
        checksum += buffer[1];
        buffer += 2;
    }

    unsigned int carray = checksum >> 16;
    while (carray) {
        checksum = (checksum & 0xffff) + carray;
        carray = checksum >> 16;
    }

    checksum = ~checksum;

    return checksum & 0xffff;
}


void StringSplit(std::string str, const char split)
{
	std::istringstream iss(str);	// 输入流
	std::string token;			// 接收缓冲区
	while (getline(iss, token, split))	// 以split为分隔符
	{
		std::cout << token << std::endl; // 输出
	}
}