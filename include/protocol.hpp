#pragma once
#include <cstdint>

#pragma pack(push, 1) // Critical: Removes padding to match PDF offsets 

// Client -> Server: NEW (16 bytes) [cite: 14]
struct NewOrderRequest {
    uint16_t length;        // Offset 0: Length of message [cite: 14]
    uint16_t messageType;   // Offset 2: 1 for NEW [cite: 14]
    int32_t price;          // Offset 4: Price [cite: 14]
    int32_t size;           // Offset 8: Size [cite: 14]
    int32_t clientOrderId;  // Offset 12: Client Order ID [cite: 14]
};

// Client -> Server: CANCEL (16 bytes) [cite: 16, 17]
struct CancelOrderRequest {
    uint16_t length;        // Offset 0: Length [cite: 16]
    uint16_t messageType;   // Offset 2: 2 for CANCEL [cite: 17]
    int32_t price;          // Offset 4: Price [cite: 17]
    int32_t size;           // Offset 8: Size [cite: 17]
    int32_t serverOrderId;  // Offset 12: Server ID to cancel [cite: 17]
};

// Server -> Client: ACK (24 bytes) [cite: 24]
struct OrderAck {
    uint16_t length = 24;   
    uint16_t messageType;   // 1 for NEW ACK, 2 for CANCEL ACK [cite: 24]
    int32_t price;          
    int32_t size;           
    int32_t clientOrderId;  
    int64_t exchOrderId;    // Offset 16: 8-byte Server ID [cite: 24]
};

// Server -> Client: REJECT (16 bytes) [cite: 28]
struct OrderReject {
    uint16_t length = 16;
    uint16_t messageType;   // 3 for NEW REJ, 4 for CANCEL REJ [cite: 28]
    int32_t price;
    int32_t size;
    int32_t serverOrderId;  // -1 for NEW REJECT [cite: 28]
};

#pragma pack(pop)