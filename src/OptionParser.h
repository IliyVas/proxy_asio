#ifndef PROXY_ASIO_OPTIONPARSER_H
#define PROXY_ASIO_OPTIONPARSER_H

#include <string>
#include <sstream>
#include <vector>

namespace proxy_asio {
    class OptionParser {
        bool ipOptionExists_ = false;
        std::string ip_;
        unsigned short port;
        std::stringstream ssError_;

        const char *usage = "Usage: proxy_asio [ipv4_address] <port>\n";

        bool parseIpOption(const char *ip);

        bool parsePortOption(const char *ip);

        bool parseInt(const std::string &str, int &out_num);

        std::vector<std::string> split(const std::string &str, const char *delim);

    public:
        bool Parse(int argc, char *argv[]);

        std::string GetErrorMsg() const;

        bool IsIpOptionExists() const;

        std::string GetIpAddr() const;

        unsigned short GetPort() const;
    };
}
#endif //PROXY_ASIO_OPTIONPARSER_H
