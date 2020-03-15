cmake_minimum_required(VERSION 3.15)
project(proxy_asio)

set(CMAKE_CXX_STANDARD 17)

include_directories(lib/asio/include)

add_executable(proxy_asio
        src/main.cpp
        src/Server.cpp
        src/Session.cpp
        src/ThreadPool.hpp
        src/Statistics.cpp
        src/OptionParser.cpp)

target_compile_definitions(proxy_asio PUBLIC ASIO_STANDALONE)
target_link_libraries(proxy_asio pthread)