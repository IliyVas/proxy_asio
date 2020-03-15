#include <iostream>
#include "OptionParser.h"
#include "Server.h"

using namespace proxy_asio;

int main(int argc, char *argv[]) {
    OptionParser parser;
    if (parser.Parse(argc, argv)) {
        auto ip_addr = "127.0.0.1";
        if (parser.IsIpOptionExists()) {
            ip_addr = parser.GetIpAddr().c_str();
        }
        proxy_asio::Server s(ip_addr, parser.GetPort());
        s.Run();
    } else {
        std::cout << parser.GetErrorMsg() << std::endl;
    }

    return 0;
}