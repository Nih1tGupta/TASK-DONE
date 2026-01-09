#pragma once
#include <vector>
#include <deque>
#include <chrono>

struct ClientSession {
    int fd;
    std::vector<char> buffer; 
    
    // For Rate Control: store timestamps of messages received [cite: 82, 85]
    std::deque<std::chrono::steady_clock::time_point> messageTimes;
};
