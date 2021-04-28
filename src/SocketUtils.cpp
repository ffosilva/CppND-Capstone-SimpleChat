#include <string>
#include <netinet/in.h>
#include <arpa/inet.h> // inet_ntop & inet_pton
#include <netdb.h>
#include <cstring>
#include <unistd.h>

#include "SocketUtils.h"

// This utility functions were inspired on this project: https://github.com/kklis/proxy

SocketUtils::SocketException::SocketException(std::string message) : mMessage(std::move(message)) {}

int SocketUtils::CheckIPVersion(const std::string &address) {
    const char *addressChar = address.c_str();
    struct in6_addr bindaddr{};

    if (inet_pton(AF_INET, addressChar, &bindaddr) == 1) {
        return AF_INET;
    } else {
        if (inet_pton(AF_INET6, addressChar, &bindaddr) == 1) {
            return AF_INET6;
        }
    }
    return 0;
}

int SocketUtils::CreateServerSocket(const std::string &bindAddress, int port, int backlog) {
    int server_sock, optVal = 1;
    int validFamily = 0;
    struct addrinfo hints{}, *res = nullptr;
    char portStr[12];
    const char *bind_addr = bindAddress.c_str();

    memset(&hints, 0x00, sizeof(hints));
    server_sock = -1;

    hints.ai_flags = AI_NUMERICSERV;   /* numeric service number, not resolve */
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (bind_addr != nullptr) {
        validFamily = SocketUtils::CheckIPVersion(bind_addr);
        if (validFamily) {
            hints.ai_family = validFamily;
            hints.ai_flags |= AI_NUMERICHOST; /* bind_addr is a valid numeric ip, skip resolve */
        }
    } else {
/* if bind_address is NULL, will bind to IPv6 wildcard */
        hints.ai_family = AF_INET6; /* Specify IPv6 socket, also allow ipv4 clients */
        hints.ai_flags |= AI_PASSIVE; /* Wildcard address */
    }

    sprintf(portStr, "%d", port);

/* Check if specified socket is valid. Try to resolve address if bind_address is a hostname */
    if (getaddrinfo(bind_addr, portStr, &hints, &res) != 0) {
        throw SocketException("Unable to resolve '" + bindAddress + "'.");
    }

    if ((server_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        throw SocketException("Unable to create the server socket.");;
    }


    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal)) < 0) {
        throw SocketException("Unable to set socket options.");;
    }

    if (bind(server_sock, res->ai_addr, res->ai_addrlen) == -1) {
        close(server_sock);
        throw SocketException("Unable to bind the server socket.");
    }

    if (listen(server_sock, backlog) < 0) {
        throw SocketException("Unable to listen on socket.");
    }

    if (res != nullptr) {
        freeaddrinfo(res);
    }

    return server_sock;
}

int SocketUtils::CreateClientSocket(const std::string &remoteHost, int remotePort) {
    struct addrinfo hints{}, *res = nullptr;
    const char *remote_host = remoteHost.c_str();
    int sock;
    int validFamily = 0;
    char portStr[12];

    memset(&hints, 0x00, sizeof(hints));

    hints.ai_flags = AI_NUMERICSERV; /* numeric service number, not resolve */
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    sprintf(portStr, "%d", remotePort);

    /* check for numeric IP to specify IPv6 or IPv4 socket */
    validFamily = SocketUtils::CheckIPVersion(remote_host);
    if (validFamily) {
        hints.ai_family = validFamily;
        hints.ai_flags |= AI_NUMERICHOST;  /* remote_host is a valid numeric ip, skip resolve */
    }

    /* Check if specified host is valid. Try to resolve address if remote_host is a hostname */
    if (getaddrinfo(remote_host, portStr, &hints, &res) != 0) {
        errno = EFAULT;
        throw SocketException("Unable to resolve '" + remoteHost + "'.");
    }

    if ((sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        throw SocketException("Unable to create the client socket.");;
    }

    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        throw SocketException("Unable to connect to the remote: '" + remoteHost + ":" + portStr + "'.");
    }

    if (res != nullptr) {
        freeaddrinfo(res);
    }

    return sock;
}