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

#define PORT 8080
#define MAX_CLIENTS 8
#define RATE_LIMIT 100

bool isRateLimited(ClientSession& session) {
    auto now = std::chrono::steady_clock::now();
    // Remove timestamps older than 1 second 
    while (!session.messageTimes.empty() && 
           std::chrono::duration_cast<std::chrono::seconds>(now - session.messageTimes.front()).count() >= 1) {
        session.messageTimes.pop_front();
    }
    
    if (session.messageTimes.size() >= RATE_LIMIT) return true;
    
    session.messageTimes.push_back(now);
    return false;
}

void handleClient(ClientSession& session, OrderManager& om) {
    while (session.buffer.size() >= 2) {
        uint16_t len = *reinterpret_cast<uint16_t*>(session.buffer.data());
        if (session.buffer.size() < len) break;

        uint16_t type = *reinterpret_cast<uint16_t*>(session.buffer.data() + 2);

        if (isRateLimited(session)) { // Rate check [cite: 61, 86]
            OrderReject rej;
            rej.messageType = (type == 1) ? 3 : 4;
            rej.serverOrderId = -1; // Specific for NEW REJECT [cite: 81]
            send(session.fd, &rej, sizeof(rej), 0);
        } else {
            if (type == 1) { // NEW
                auto* req = reinterpret_cast<NewOrderRequest*>(session.buffer.data());
                uint64_t sid = om.addOrder(*req);
                OrderAck ack;
                ack.messageType = 1; ack.price = req->price; ack.size = req->size;
                ack.clientOrderId = req->clientOrderId; ack.exchOrderId = sid;
                send(session.fd, &ack, sizeof(ack), 0);
            } else if (type == 2) { // CANCEL
                auto* req = reinterpret_cast<CancelOrderRequest*>(session.buffer.data());
                int32_t outId = 0;
                if (om.tryCancel(*req, outId)) {
                    OrderAck ack;
                    ack.messageType = 2; ack.price = req->price; ack.size = req->size;
                    ack.clientOrderId = outId; ack.exchOrderId = req->serverOrderId;
                    send(session.fd, &ack, sizeof(ack), 0);
                } else {
                    OrderReject rej;
                    rej.messageType = 4; rej.price = req->price; rej.size = req->size;
                    rej.serverOrderId = req->serverOrderId;
                    send(session.fd, &rej, sizeof(rej), 0);
                }
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
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, MAX_CLIENTS);

    std::vector<pollfd> fds;
    fds.push_back({server_fd, POLLIN, 0});
    std::map<int, ClientSession> sessions;
    OrderManager om;

    std::cout << "Server running on port " << PORT << "..." << std::endl;

    while (true) {
        poll(fds.data(), fds.size(), -1);
        for (size_t i = 0; i < fds.size(); ++i) {
            if (!(fds[i].revents & POLLIN)) continue;
            if (fds[i].fd == server_fd) {
                int c_fd = accept(server_fd, nullptr, nullptr);
                if (fds.size() <= MAX_CLIENTS) {
                    fds.push_back({c_fd, POLLIN, 0});
                    sessions[c_fd] = {c_fd, {}};
                } else { close(c_fd); }
            } else {
                char tmp[1024];
                int n = read(fds[i].fd, tmp, 1024);
                if (n <= 0) {
                    close(fds[i].fd); sessions.erase(fds[i].fd);
                    fds.erase(fds.begin() + i);
                } else {
                    sessions[fds[i].fd].buffer.insert(sessions[fds[i].fd].buffer.end(), tmp, tmp + n);
                    handleClient(sessions[fds[i].fd], om);
                }
            }
        }
    }
    return 0;
}
