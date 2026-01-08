#pragma once
#include <vector>
#include <deque>
#include <chrono>

struct ClientSession {
    int fd;
    std::vector<char> buffer; // Buffer to hold binary stream for this client
    
    // For Rate Control: store timestamps of messages received [cite: 32]
    std::deque<std::chrono::steady_clock::time_point> messageTimes; 
};