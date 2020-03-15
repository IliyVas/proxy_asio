#ifndef PROXY_ASIO_SERVER_H
#define PROXY_ASIO_SERVER_H

#include <asio.hpp>
#include <string>
#include "Session.h"
#include "Statistics.h"

namespace proxy_asio {
    using namespace asio;
    using namespace asio::ip;

    class Server {
        io_context io_;
        io_context::strand strand_;
        tcp::acceptor acceptor_;
        signal_set signals_;
        Statistics statistics_;
        std::vector<std::shared_ptr<Session>> sessions_;

        void handleSignal(const asio::error_code &err, int sig);

        void handleAccept(const asio::error_code &err, tcp::socket socket);

        void printStatistics();

    public:
        Server(const std::string &ip, unsigned short port);

        void Run();
    };
}

#endif //PROXY_ASIO_SERVER_H
