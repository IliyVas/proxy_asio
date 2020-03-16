#include <functional>
#include "Server.h"
#include "ThreadPool.hpp"

using namespace proxy_asio;

Server::Server(const std::string &ip, unsigned short port) :
        strand_(io_),
        acceptor_(io_, tcp::endpoint(make_address_v4(ip), port)),
        signals_(io_) {
    //acceptor_.set_option(tcp::acceptor::reuse_address(true));
}

void Server::Run() {
    acceptor_.async_accept(bind_executor(strand_, [this](const asio::error_code &err, tcp::socket new_socket) {
        handleAccept(err, std::move(new_socket));
    }));

    ThreadPool thread_pool(2);
    for (unsigned i = 0; i < thread_pool.GetWorkersNum(); i++) {
        thread_pool.Enqueue<io_context::count_type(io_context::*)(), io_context *>(&io_context::run, &io_);
    }

    signals_.add(SIGINT);
    signals_.async_wait(bind_executor(strand_, [this](const asio::error_code &err, int sig) {
        handleSignal(err, sig);
    }));

    std::cout << "Server started" << std::endl;
    io_.run();
}

void Server::handleAccept(const asio::error_code &err, tcp::socket socket) {
    if (err || !acceptor_.is_open()) {
        return;
    }

    asio::error_code re_err;
    auto remote_endpoint = socket.remote_endpoint(re_err);
    if (re_err) {
        return;
    }
    std::cout << remote_endpoint.address().to_string() << ':' << remote_endpoint.port() << " connected" << std::endl;

    auto session = std::make_unique<Session>(std::move(socket), statistics_);
    session->Start();
    sessions_.push_back(std::move(session));
    acceptor_.async_accept(bind_executor(strand_, [this](const asio::error_code &err, tcp::socket new_socket) {
        handleAccept(err, std::move(new_socket));
    }));
}

void Server::handleSignal(const asio::error_code &err, int sig) {
    if (sig != SIGINT || err) return;

    if (acceptor_.is_open()) {
        acceptor_.close();
    }

    for (auto &session : sessions_) {
        session->Close();
    }

    printStatistics();
    std::cout << "\nPress <Enter>" << std::endl;
    std::cin.get();
}

void Server::printStatistics() {
    std::cout << "\nUpstream bytes received: " << statistics_.GetUpstreamBytesReceived() << std::endl;
    std::cout << "Upstream bytes sent: " << statistics_.GetUpstreamBytesSent() << std::endl;
    std::cout << "Downstream bytes received: " << statistics_.GetDownstreamBytesReceived() << std::endl;
    std::cout << "Downstream bytes sent: " << statistics_.GetDownstreamBytesSent() << std::endl;
}

