#ifndef PROXY_ASIO_STATISTICS_H
#define PROXY_ASIO_STATISTICS_H

#include <atomic>
#include <cstddef>

namespace proxy_asio {
    class Statistics {
        std::atomic_size_t uBytesReceived_, dBytesReceived_, uBytesSent_, dBytesSent_;

    public:
        Statistics();

        void AddData(size_t u_bytes_received, size_t d_bytes_received, size_t u_bytes_sent, size_t d_bytes_sent);

        size_t GetUpstreamBytesReceived();

        size_t GetUpstreamBytesSent();

        size_t GetDownstreamBytesReceived();

        size_t GetDownstreamBytesSent();
    };
}

#endif //PROXY_ASIO_STATISTICS_H
