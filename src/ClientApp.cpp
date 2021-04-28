#include <iostream>
#include <sys/socket.h>

#include "SocketUtils.h"

int main(int argc, char *argv[]) {
    SocketUtils::CreateClientSocket("google.com.br", 33333);
}