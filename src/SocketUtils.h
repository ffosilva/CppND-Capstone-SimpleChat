#ifndef SIMPLECHAT_SOCKETUTILS_H
#define SIMPLECHAT_SOCKETUTILS_H
#include <exception>
#include <utility>
#include <string>

// This utility functions were inspired on this project: https://github.com/kklis/proxy

namespace SocketUtils {
    class SocketException: public std::exception {
    private:
        std::string mMessage;
    public:
        explicit SocketException(std::string message);
        [[nodiscard]] const char* what() const noexcept override {
            return mMessage.c_str();
        }
    };

    int CheckIPVersion(const std::string &address);
    int CreateServerSocket(const std::string &bindAddress, int port, int backlog = 20);
    int CreateClientSocket(const std::string &remoteHost, int remotePort);
}

#endif //SIMPLECHAT_SOCKETUTILS_H
