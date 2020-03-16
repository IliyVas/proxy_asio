#include "Session.h"
#include  <functional>
#include <iostream>

using namespace proxy_asio;

Session::Session(tcp::socket tcp_socket, Statistics &statistics) : strand_(tcp_socket.get_io_context()),
                                                                   downstreamSocket_(std::move(tcp_socket)),
                                                                   upstreamSocket_(downstreamSocket_.get_io_context()),
                                                                   statistics_(statistics),
                                                                   uBytesSent_(0), uBytesRead_(0),
                                                                   dBytesSent_(0), dBytesRead_(0),
                                                                   closed_(false) {}

void Session::Start() {
    asio::error_code err;
    auto d_remote_endpoint = downstreamSocket_.remote_endpoint(err);
    if (err) {
        Close();
        return;
    }
    auto upstream_ip = make_address_v4(d_remote_endpoint.address().to_v4().to_uint() + 2);
    upstreamSocket_.async_connect(tcp::endpoint(upstream_ip, downstreamSocket_.local_endpoint().port()),
                                  bind_executor(strand_, [this](const asio::error_code &err) {
                                      handleUpstreamConnect(err);
                                  }));
}

void Session::handleDownstreamRead(const asio::error_code &err,
                                   size_t bytes_transferred) {
    if (err) {
        Close();
        return;
    }
    dBytesRead_ += bytes_transferred;

    upstreamSocket_.async_send(buffer(dBuffer_, bytes_transferred),
                               bind_executor(strand_, [this](const asio::error_code &err, size_t bytes_transferred) {
                                   handleUpstreamSend(err, bytes_transferred);
                               }));

    downstreamSocket_.async_read_some(buffer(dBuffer_),
                                      bind_executor(strand_,
                                                    [this](const asio::error_code &err, size_t bytes_transferred) {
                                                        handleDownstreamRead(err, bytes_transferred);
                                                    }));
}


void Session::handleDownstreamSend(const asio::error_code &err,
                                   size_t bytes_transferred) {
    if (err) {
        return;
    }
    dBytesSent_ += bytes_transferred;
}

void Session::handleUpstreamSend(const asio::error_code &err,
                                 size_t bytes_transferred) {
    if (err) {
        return;
    }
    uBytesSent_ += bytes_transferred;
}

void Session::handleUpstreamConnect(const asio::error_code &err) {
    if (err) {
        Close();
        return;
    }

    downstreamSocket_.async_read_some(buffer(dBuffer_),
                                      bind_executor(strand_,
                                                    [this](const asio::error_code &err, size_t bytes_transferred) {
                                                        handleDownstreamRead(err, bytes_transferred);
                                                    }));

    upstreamSocket_.async_read_some(buffer(uBuffer_),
                                    bind_executor(strand_,
                                                  [this](const asio::error_code &err, size_t bytes_transferred) {
                                                      handleUpstreamRead(err, bytes_transferred);
                                                  }));
}

void Session::handleUpstreamRead(const asio::error_code &err, size_t bytes_transferred) {
    if (err) {
        Close();
        return;
    }
    uBytesRead_ += bytes_transferred;

    downstreamSocket_.async_send(buffer(uBuffer_, bytes_transferred),
                                 bind_executor(strand_,
                                               [this](const asio::error_code &err, size_t bytes_transferred) {
                                                   handleDownstreamSend(err, bytes_transferred);
                                               }));

    upstreamSocket_.async_read_some(buffer(uBuffer_),
                                    bind_executor(strand_,
                                                  [this](const asio::error_code &err, size_t bytes_transferred) {
                                                      handleUpstreamRead(err, bytes_transferred);
                                                  }));
}

void Session::Close() {
    std::lock_guard<std::mutex> lock(close_mutex_);
    if (!closed_) {
        if (upstreamSocket_.is_open()) {
            upstreamSocket_.cancel();
            upstreamSocket_.close();
        }
        if (downstreamSocket_.is_open()) {
            asio::error_code err;
            auto remote_endpoint = downstreamSocket_.remote_endpoint(err);
            downstreamSocket_.cancel();
            downstreamSocket_.close();
            if (!err) {
                std::cout << remote_endpoint.address().to_string() << ':' << remote_endpoint.port()
                          << " disconnected" << std::endl;
            }
        }
        statistics_.AddData(uBytesRead_, dBytesRead_, uBytesSent_, dBytesSent_);
        closed_ = true;
    }
}

Session::~Session() {
    Close();
}
