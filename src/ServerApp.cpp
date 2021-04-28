#include "SocketUtils.h"
#include <iostream>

int main(int argc, char *argv[]) {
    std::cout << SocketUtils::CreateServerSocket("localhost", 15555) << std::endl;
}