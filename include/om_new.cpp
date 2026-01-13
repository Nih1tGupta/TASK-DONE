OrderReject OrderManager::generateReject(uint16_t requestType, int32_t p, int32_t s, int32_t cId, int64_t sId) {
    OrderReject rej;
    rej.length = 24;
    
    // Dynamically set Response Type: New(1)->NewRej(3), Cancel(2)->CancelRej(4)
    rej.messageType = requestType + 2; 
    
    rej.price = p; // Echo back Price
    rej.size = s;   // Echo back Size
    rej.serverOrderId = sId;

    if (requestType == 2) { // Logic for CANCEL REJECT
        auto it = orders.find(sId);
        if (it == orders.end()) {
            // Requirement: -1 for CANCEL REJECT where ServerOrderID is wrong
            rej.clientOrderId = -1; 
        } else {
            // Requirement: Correct ClientOrderID if fields are wrong
            rej.clientOrderId = it->second.clientOrderId;
        }
    } else {
        // Requirement: Echo back for NEW REJECT
        rej.clientOrderId = cId;
        // Requirement: -1 for NEW REJECT serverOrderId
        rej.serverOrderId = -1; 
    }
    
    return rej;
}
