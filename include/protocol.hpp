#pragma once
#include <cstdint>

#pragma pack(push, 1) // Removes padding to match PDF offsets [cite: 67]

// Client -> Server: NEW (16 bytes) [cite: 67]
struct NewOrderRequest {
    uint16_t length;        // Offset 0: 2 bytes
    uint16_t messageType;   // Offset 2: 2 bytes (1 for NEW)
    uint32_t price;          // Offset 4: 4 bytes 
    uint32_t size;           // Offset 8: 4 bytes 
    int32_t  clientOrderId;  // Offset 12: 4 bytes (Echoed back) [cite: 110]
};

// Client -> Server: CANCEL (16 bytes) [cite: 70]
struct CancelOrderRequest {
    uint16_t length;        // Offset 0: 2 bytes
    uint16_t messageType;   // Offset 2: 2 bytes (2 for CANCEL)
    uint32_t price;          // Offset 4: 4 bytes 
    uint32_t size;           // Offset 8: 4 bytes 
    uint32_t serverOrderId;  // Offset 12: 4 bytes (Assigned by Server)
};

// Server -> Client: ACK (24 bytes) [cite: 77]
struct OrderAck {
    uint16_t length = 24;
    uint16_t messageType;   // 1 for NEW ACK, 2 for CANCEL ACK
    uint32_t price;
    uint32_t size;
    int32_t  clientOrderId;
    uint64_t exchOrderId;   // Offset 16: 8-byte Server ID [cite: 77]
};

// Server -> Client: REJECT (16 bytes) [cite: 81]
struct OrderReject {
    uint16_t length = 16;
    uint16_t messageType;   // 3 for NEW REJ, 4 for CANCEL REJ
    uint32_t price;
    uint32_t size;
    int32_t  serverOrderId; // -1 for NEW REJECT [cite: 81]
};

#pragma pack(pop)
