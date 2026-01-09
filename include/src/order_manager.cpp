#include "order_manager.hpp"

uint64_t OrderManager::addOrder(const NewOrderRequest& req) {
    uint64_t id = nextId++; 
    orderBook[id] = {req.price, req.size, req.clientOrderId, false};
    return id;
}

bool OrderManager::tryCancel(const CancelOrderRequest& req, int32_t& outClientId) {
    auto it = orderBook.find(req.serverOrderId);
    // Accepted only if ID exists, matches price/size, and not already cancelled [cite: 78]
    if (it != orderBook.end() && !it->second.isCancelled && 
        it->second.price == req.price && it->second.size == req.size) {
        
        it->second.isCancelled = true; 
        outClientId = it->second.clientOrderId; 
        return true; 
    }
    return false; // Triggers REJECT [cite: 79]
}
