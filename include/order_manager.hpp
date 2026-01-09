#pragma once
#include <unordered_map>
#include "protocol.hpp"

struct OrderEntry {
    uint32_t price;
    uint32_t size;
    int32_t clientOrderId;
    bool isCancelled = false;
};

class OrderManager {
private:
    std::unordered_map<uint64_t, OrderEntry> orderBook; // Validates cancels [cite: 78]
    uint64_t nextId = 1000; // Counter for ServerOrderID [cite: 72]

public:
    uint64_t addOrder(const NewOrderRequest& req);
    bool tryCancel(const CancelOrderRequest& req, int32_t& outClientId);
};

#pragma once
#include <unordered_map>
#include "protocol.hpp"

struct OrderEntry {
    uint32_t price;
    uint32_t size;
    int32_t clientOrderId;
    bool isCancelled = false;
};

class OrderManager {
private:
    // Using uint64_t to store the 8-byte ServerOrderID
    std::unordered_map<uint64_t, OrderEntry> orderBook; 
    uint64_t nextId = 1000;

public:
    uint64_t addOrder(const NewOrderRequest& req);
    bool tryCancel(const CancelOrderRequest& req, int32_t& outClientId);
};
