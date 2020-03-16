#ifndef PROXY_ASIO_SESSION_H
#define PROXY_ASIO_SESSION_H

#include <asio.hpp>
#include <array>
#include <memory>

#include "Statistics.h"

namespace proxy_asio {
    using namespace asio;
    using namespace asio::ip;

    class Session {
        io_context::strand strand_;
        std::mutex close_mutex_;
        tcp::socket downstreamSocket_, upstreamSocket_;
        std::array<char, 128 * 1024> dBuffer_, uBuffer_;
        Statistics &statistics_;
        unsigned long uBytesSent_, uBytesRead_, dBytesSent_, dBytesRead_;
        bool closed_;

        void handleDownstreamRead(const asio::error_code &err,
                                  size_t bytes_transferred);

        void handleUpstreamRead(const asio::error_code &err,
                                size_t bytes_transferred);

        void handleDownstreamSend(const asio::error_code &err,
                                  size_t bytes_transferred);

        void handleUpstreamSend(const asio::error_code &err,
                                size_t bytes_transferred);

        void handleUpstreamConnect(const asio::error_code &err);

    public:
        Session(tcp::socket tcp_socket, Statistics &statistics);

        ~Session();

        void Start();

        void Close();
    };
}

#endif //PROXY_ASIO_SESSION_H
