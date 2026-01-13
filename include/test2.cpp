#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "../structure/protocol.hpp"




int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    std::cout << "Connected to server\n\n";

    NewOrderRequest newReq;
    newReq.length = 16;
    newReq.messageType = 1;
    newReq.price = 500;
    newReq.size = 10;
    newReq.clientOrderId = 55;

    std::cout << "Sending NEW order:\n";
    std::cout << "  Price          : " << newReq.price << "\n";
    std::cout << "  Size           : " << newReq.size << "\n";
    std::cout << "  ClientOrderID  : " << newReq.clientOrderId << "\n\n";

    send(sock, &newReq, sizeof(newReq), 0);

    char buf[64];
    int n = recv(sock, buf, sizeof(buf), 0);
    if(n<=0){std::cerr<<"DISCONNECTED"<<std::endl;return -1;}
    uint16_t msgType = *reinterpret_cast<uint16_t*>(buf + 2);

    int64_t serverOrderId = -1;

    if (msgType == 1) { // NEW ACK
        auto* ack = reinterpret_cast<OrderAck*>(buf);
        serverOrderId = ack->exchOrderId;

        std::cout << "NEW ACK received:\n";
        std::cout << "  Price          : " << ack->price << "\n";
        std::cout << "  Size           : " << ack->size << "\n";
        std::cout << "  ClientOrderID  : " << ack->clientOrderId << "\n";
        std::cout << "  ServerOrderID  : " << ack->exchOrderId << "\n\n";
    } else {
        auto* rej = reinterpret_cast<OrderReject*>(buf);
        std::cout << "NEW REJECT received:\n";
        std::cout << "  Price          : " << rej->price << "\n";
        std::cout << "  Size           : " << rej->size << "\n";
        std::cout << "  ServerOrderID  : " << rej->serverOrderId << "\n\n";
        close(sock);
        return 0;
    }

    CancelOrderRequest cancelReq;
    cancelReq.length = 16;
    cancelReq.messageType = 2;
    cancelReq.price = 500;
    cancelReq.size = 10;
    cancelReq.serverOrderId = (int32_t)serverOrderId;

    std::cout << "Sending CANCEL order:\n";
    std::cout << "  Price          : " << cancelReq.price << "\n";
    std::cout << "  Size           : " << cancelReq.size << "\n";
    std::cout << "  ServerOrderID  : " << cancelReq.serverOrderId << "\n\n";

    send(sock, &cancelReq, sizeof(cancelReq), 0);

    n = recv(sock, buf, sizeof(buf), 0);
    msgType = *reinterpret_cast<uint16_t*>(buf + 2);

    if (msgType == 2) { // CANCEL ACK
        auto* ack = reinterpret_cast<OrderAck*>(buf);
        std::cout << "CANCEL ACK received:\n";
        std::cout << "  Price          : " << ack->price << "\n";
        std::cout << "  Size           : " << ack->size << "\n";
        std::cout << "  ClientOrderID  : " << ack->clientOrderId << "\n";
        std::cout << "  ServerOrderID  : " << ack->exchOrderId << "\n\n";
    } else {
        auto* rej = reinterpret_cast<OrderReject*>(buf);
        std::cout << "CANCEL REJECT received:\n";
        std::cout << "  Price          : " << rej->price << "\n";
        std::cout << "  Size           : " << rej->size << "\n";
        std::cout << "  ServerOrderID  : " << rej->serverOrderId << "\n\n";
    }

    close(sock);
    std::cout << "Client finished\n";
    return 0;
}



// ... initial connection remains same ...
    if (msgType == 1) { // NEW ACK
        auto* ack = reinterpret_cast<OrderAck*>(buf);
        serverOrderId = ack->serverOrderId; // Renamed 8-byte field [cite: 24]

        std::cout << "NEW ACK received. ServerOrderID: " << ack->serverOrderId << "\n";
    }

    CancelOrderRequest cancelReq;
    cancelReq.length = 20; // 16 (previous) -> 20 (with 8-byte ID)
    cancelReq.messageType = 2;
    cancelReq.price = 500;
    cancelReq.size = 10;
    cancelReq.serverOrderId = serverOrderId; // Correctly sending 8-byte ID [cite: 17]
// ... rest of client recv remains same ...














#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "structure/protocol.hpp" // Adjusted path based on your setup

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return -1;
    }

    std::cout << "Connected to server\n\n";

    // --- 1. SEND NEW ORDER ---
    NewOrderRequest newReq;
    newReq.length = 16;
    newReq.messageType = 1;
    newReq.price = 500;
    newReq.size = 10;
    newReq.clientOrderId = 55;

    std::cout << ">>> Sending NEW order (Type: " << newReq.messageType << ", Len: " << newReq.length << ")\n\n";
    send(sock, &newReq, sizeof(newReq), 0);

    // --- 2. RECEIVE RESPONSE ---
    char buf[64];
    int n = recv(sock, buf, sizeof(buf), 0);
    if(n <= 0) { std::cerr << "DISCONNECTED\n"; return -1; }

    // Extract Header Info
    uint16_t rxLen = *reinterpret_cast<uint16_t*>(buf);
    uint16_t rxType = *reinterpret_cast<uint16_t*>(buf + 2);
    int64_t savedServerOrderId = -1;

    if (rxType == 1) { // NEW ACK
        auto* ack = reinterpret_cast<OrderAck*>(buf);
        savedServerOrderId = ack->serverOrderId;
        std::cout << "<<< RECEIVED NEW ACK:\n";
        std::cout << "  Msg Length    : " << rxLen << "\n";
        std::cout << "  Msg Type      : " << rxType << "\n";
        std::cout << "  Price         : " << ack->price << "\n";
        std::cout << "  Size          : " << ack->size << "\n";
        std::cout << "  ClientOrderID : " << ack->clientOrderId << "\n";
        std::cout << "  ServerOrderID : " << ack->serverOrderId << "\n\n";
    } else { // REJECT (Type 3)
        auto* rej = reinterpret_cast<OrderReject*>(buf);
        std::cout << "<<< RECEIVED NEW REJECT:\n";
        std::cout << "  Msg Length    : " << rxLen << "\n";
        std::cout << "  Msg Type      : " << rxType << "\n";
        std::cout << "  ServerOrderID : " << rej->serverOrderId << "\n\n";
        close(sock); return 0;
    }

    // --- 3. SEND CANCEL ORDER ---
    CancelOrderRequest cancelReq;
    cancelReq.length = 20; // Updated to 20 for 8-byte ID
    cancelReq.messageType = 2;
    cancelReq.price = 500;
    cancelReq.size = 10;
    cancelReq.serverOrderId = savedServerOrderId;

    std::cout << ">>> Sending CANCEL (Type: " << cancelReq.messageType << ", Len: " << cancelReq.length << ")\n\n";
    send(sock, &cancelReq, sizeof(cancelReq), 0);

    // --- 4. RECEIVE FINAL RESPONSE ---
    n = recv(sock, buf, sizeof(buf), 0);
    if(n <= 0) return -1;

    rxLen = *reinterpret_cast<uint16_t*>(buf);
    rxType = *reinterpret_cast<uint16_t*>(buf + 2);

    if (rxType == 2) { // CANCEL ACK
        auto* ack = reinterpret_cast<OrderAck*>(buf);
        std::cout << "<<< RECEIVED CANCEL ACK:\n";
        std::cout << "  Msg Length    : " << rxLen << "\n";
        std::cout << "  Msg Type      : " << rxType << "\n";
        std::cout << "  Price         : " << ack->price << "\n";
        std::cout << "  Size          : " << ack->size << "\n";
        std::cout << "  ServerOrderID : " << ack->serverOrderId << "\n\n";
    } else { // CANCEL REJECT (Type 4)
        auto* rej = reinterpret_cast<OrderReject*>(buf);
        std::cout << "<<< RECEIVED CANCEL REJECT:\n";
        std::cout << "  Msg Length    : " << rxLen << "\n";
        std::cout << "  Msg Type      : " << rxType << "\n";
        std::cout << "  ServerOrderID : " << rej->serverOrderId << "\n\n";
    }

    close(sock);
    std::cout << "Test Client Finished.\n";
    return 0;
}
