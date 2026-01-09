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





#pragma once
#include <cstdint>

#pragma pack(push, 1) // Ensures no padding; matches exact offsets from images

// Client -> Server: NEW (16 bytes)
struct NewOrderRequest {
    uint16_t length;        // Offset 0, Size 2
    uint16_t messageType;   // Offset 2, Size 2 (1 for NEW)
    uint32_t price;         // Offset 4, Size 4
    uint32_t size;          // Offset 8, Size 4
    int32_t  clientOrderId; // Offset 12, Size 4
};

// Client -> Server: CANCEL (20 bytes now due to 8-byte ID)
struct CancelOrderRequest {
    uint16_t length;        // Offset 0, Size 2
    uint16_t messageType;   // Offset 2, Size 2 (2 for CANCEL)
    uint32_t price;         // Offset 4, Size 4
    uint32_t size;          // Offset 8, Size 4
    uint64_t serverOrderId; // Offset 12, Size 8 (Updated to 8 bytes) [cite: 17]
};

// Server -> Client: ACK (24 bytes)
struct OrderAck {
    uint16_t length = 24;   // Size 2
    uint16_t messageType;   // Size 2 (1: NEW ACK, 2: CANCEL ACK)
    uint32_t price;         // Size 4
    uint32_t size;          // Size 4
    int32_t  clientOrderId; // Size 4
    uint64_t serverOrderId; // Offset 16, Size 8 (Renamed from ExchOrderID) [cite: 24]
};

// Server -> Client: REJECT (20 bytes now due to 8-byte ID)
struct OrderReject {
    uint16_t length = 20;   // Updated length for 8-byte ID
    uint16_t messageType;   // Size 2 (3: NEW REJ, 4: CANCEL REJ)
    uint32_t price;         // Size 4
    uint32_t size;          // Size 4
    int64_t  serverOrderId; // Offset 12, Size 8 (-1 for NEW REJECT) [cite: 28]
};

#pragma pack(pop)

