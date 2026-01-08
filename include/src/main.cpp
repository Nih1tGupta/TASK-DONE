#include <iostream>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include "protocol.hpp"
#include "order_manager.hpp"
#include "session.hpp"

#define MAX_CLIENTS 8

void handleClient(ClientSession& session, OrderManager& om) {
    while (session.buffer.size() >= 2) {
        uint16_t len = *reinterpret_cast<uint16_t*>(session.buffer.data());
        if (session.buffer.size() < len) break;

        uint16_t type = *reinterpret_cast<uint16_t*>(session.buffer.data() + 2);

        if (type == 1) { // NEW ORDER
            auto* req = reinterpret_cast<NewOrderRequest*>(session.buffer.data());
            int64_t sid = om.addOrder(*req);
            
            OrderAck ack; ack.messageType = 1; ack.price = req->price; 
            ack.size = req->size; ack.clientOrderId = req->clientOrderId; ack.exchOrderId = sid;
            send(session.fd, &ack, sizeof(ack), 0);
        } 
        else if (type == 2) { // CANCEL ORDER
            auto* req = reinterpret_cast<CancelOrderRequest*>(session.buffer.data());
            int32_t cid = 0;
            if (om.tryCancel(*req, cid)) {
                OrderAck ack; ack.messageType = 2; ack.price = req->price; 
                ack.size = req->size; ack.clientOrderId = cid; ack.exchOrderId = req->serverOrderId;
                send(session.fd, &ack, sizeof(ack), 0);
            } else {
                OrderReject rej; rej.messageType = 4; rej.price = req->price; 
                rej.size = req->size; rej.serverOrderId = req->serverOrderId;
                send(session.fd, &rej, sizeof(rej), 0);
            }
        }
        session.buffer.erase(session.buffer.begin(), session.buffer.begin() + len);
    }
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1; setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { perror("Bind failed"); return -1; }
    listen(server_fd, MAX_CLIENTS);

    std::vector<pollfd> fds; fds.push_back({server_fd, POLLIN, 0});
    std::map<int, ClientSession> sessions;
    OrderManager om;

    std::cout << "Server listening on port 8080..." << std::endl;

    while (true) {
        poll(fds.data(), fds.size(), -1);
        for (size_t i = 0; i < fds.size(); ++i) {
            if (!(fds[i].revents & POLLIN)) continue;
            if (fds[i].fd == server_fd) {
                int cfd = accept(server_fd, nullptr, nullptr);
                if (fds.size() <= MAX_CLIENTS) {
                    fds.push_back({cfd, POLLIN, 0});
                    sessions[cfd] = {cfd, {}};
                } else { close(cfd); }
            } else {
                char tmp[1024]; int n = read(fds[i].fd, tmp, 1024);
                if (n <= 0) { close(fds[i].fd); sessions.erase(fds[i].fd); fds.erase(fds.begin() + i); }
                else {
                    sessions[fds[i].fd].buffer.insert(sessions[fds[i].fd].buffer.end(), tmp, tmp + n);
                    handleClient(sessions[fds[i].fd], om);
                }
            }
        }
    }
    return 0;
}