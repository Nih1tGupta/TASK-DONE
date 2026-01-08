#include "order_manager.hpp"

int64_t OrderManager::addOrder(const NewOrderRequest& req) {
    int64_t id = nextId++; 
    orderBook[id] = {req.price, req.size, req.clientOrderId, false}; // Save state [cite: 19]
    return id;
}

bool OrderManager::tryCancel(const CancelOrderRequest& req, int32_t& outClientId) {
    auto it = orderBook.find(req.serverOrderId); // Find existing order [cite: 25]
    
    // Check if ID exists, matches price/size, and isn't already cancelled [cite: 20, 25]
    if (it != orderBook.end() && !it->second.isCancelled && 
        it->second.price == req.price && it->second.size == req.size) {
        
        it->second.isCancelled = true; 
        outClientId = it->second.clientOrderId; 
        return true; // Accepted [cite: 25]
    }
    return false; // Rejected [cite: 26]
}