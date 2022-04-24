#pragma once

#include <iostream>
#include <vector>
#include <string>

class PortScan
{
public:

    /**
     *  scan by IP address and port
     */
    virtual bool Scan(std::vector<std::string> ip_addr, int port);

};