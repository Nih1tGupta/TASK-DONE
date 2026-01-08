#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "protocol.hpp"

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) return -1;

    // Send NEW ORDER
    NewOrderRequest n; n.length = 16; n.messageType = 1; n.price = 500; n.size = 10; n.clientOrderId = 55;
    send(sock, &n, sizeof(n), 0);

    char buf[24];
    recv(sock, buf, 24, 0);
    auto* ack = reinterpret_cast<OrderAck*>(buf);
    std::cout << "Order Accepted! ServerID: " << ack->exchOrderId << std::endl;

    // Send CANCEL ORDER
    CancelOrderRequest c; c.length = 16; c.messageType = 2; c.price = 500; c.size = 10; 
    c.serverOrderId = (int32_t)ack->exchOrderId;
    send(sock, &c, sizeof(c), 0);

    recv(sock, buf, 24, 0);
    std::cout << "Cancel Accepted!" << std::endl;

    close(sock);
    return 0;
}