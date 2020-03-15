#include <algorithm>
#include "OptionParser.h"

using namespace proxy_asio;

bool OptionParser::Parse(int argc, char **argv) {
    ssError_ << usage;
    if (argc <= 1) {
        ssError_ << "port is required\n";
        return false;
    }

    if (argc == 2) {
        if (!parsePortOption(argv[1])) return false;
    }

    if (argc >= 3) {
        if (!parseIpOption(argv[1])) return false;
        if (!parsePortOption(argv[2])) return false;
    }

    return true;
}

bool OptionParser::parsePortOption(const char *ip) {
    int parsed_port;
    if (!parseInt(ip, parsed_port) || parsed_port < 1 || parsed_port > 65535) {
        ssError_ << "port must be an integer from 1 to 65535\n";
        return false;
    }
    port = parsed_port;
    return true;
}

bool OptionParser::parseIpOption(const char *ip) {
    auto parts = split(ip, ".");
    if (parts.size() != 4) {
        ssError_ << "ipv4_address must be a valid IPv4 address (decimal form)\n";
        return false;
    }

    for (auto &part : parts) {
        int part_val;
        if (!parseInt(part, part_val) || part_val < 0 || part_val > 255) {
            ssError_ << "ipv4_address must be a valid IPv4 address (decimal form)\n";
            return false;
        }
    }
    ip_ = ip;
    ipOptionExists_ = true;
    return true;
}

bool OptionParser::IsIpOptionExists() const {
    return ipOptionExists_;
}

std::string OptionParser::GetIpAddr() const {
    if (ipOptionExists_) {
        return ip_;
    }
    return "";
}

unsigned short OptionParser::GetPort() const {
    return port;
}

std::string OptionParser::GetErrorMsg() const {
    return ssError_.str();
}

std::vector<std::string> OptionParser::split(const std::string &str, const char *delim) {
    std::vector<std::string> res;
    std::size_t current, previous = 0;
    current = str.find(delim);
    while (current != std::string::npos) {
        res.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    res.push_back(str.substr(previous, current - previous));
    return res;
}

bool OptionParser::parseInt(const std::string &str, int &out_num) {
    // leading zero is ok
    bool is_number = !str.empty() &&
                     std::find_if(str.begin(), str.end(), [](unsigned char c) { return !std::isdigit(c); }) ==
                     str.end();
    if (!is_number) {
        return false;
    }

    try {
        out_num = std::atoi(str.c_str());
    }
    catch (...) {
        return false;
    }
    return true;
}
