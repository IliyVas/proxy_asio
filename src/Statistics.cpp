#include "Statistics.h"

using namespace proxy_asio;

Statistics::Statistics() : uBytesReceived_(0), dBytesReceived_(0), uBytesSent_(0), dBytesSent_(0) {}

void Statistics::AddData(size_t u_bytes_received, size_t d_bytes_received, size_t u_bytes_sent, size_t d_bytes_sent) {
    uBytesReceived_ += u_bytes_received;
    uBytesSent_ += u_bytes_sent;
    dBytesReceived_ += d_bytes_received;
    dBytesSent_ += d_bytes_sent;
}

size_t Statistics::GetUpstreamBytesReceived() {
    return uBytesReceived_.load();
}

size_t Statistics::GetUpstreamBytesSent() {
    return uBytesSent_.load();
}

size_t Statistics::GetDownstreamBytesReceived() {
    return dBytesReceived_.load();
}

size_t Statistics::GetDownstreamBytesSent() {
    return dBytesSent_.load();
}

