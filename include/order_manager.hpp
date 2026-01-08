#pragma once
#include <unordered_map>
#include "protocol.hpp"

struct OrderEntry {
    int32_t price;
    int32_t size;
    int32_t clientOrderId;
    bool isCancelled = false;
};

class OrderManager {
private:
    std::unordered_map<int64_t, OrderEntry> orderBook; // Validates cancels [cite: 25]
    int64_t nextId = 1000; // Counter for ServerOrderID [cite: 19]

public:
    int64_t addOrder(const NewOrderRequest& req);
    bool tryCancel(const CancelOrderRequest& req, int32_t& outClientId);
};