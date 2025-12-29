#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct FollowRequest {
    int64_t requestId;
    int64_t fromUserId;
    int64_t toUserId;
    std::string status;  // PENDING / ACCEPTED / REJECTED
    int64_t timestamp;

    static std::string serialize(const FollowRequest &r) {
        // requestId|from|to|status|timestamp
        return std::to_string(r.requestId) + "|" + std::to_string(r.fromUserId) + "|" +
               std::to_string(r.toUserId) + "|" + r.status + "|" + std::to_string(r.timestamp);
    }

    static bool deserialize(const std::string &s, FollowRequest &r) {
        std::vector<std::string> parts;
        std::string cur;
        for (int i = 0; i <s.size(); i++) {
            if (s[i] == '|') { 
                parts.push_back(cur);
                 cur.clear(); 
                
            }
            else cur.push_back(s[i]);
        }
        parts.push_back(cur);

        if (parts.size() < 5) {
            return false;
        }

        r.requestId = std::stoll(parts[0]);
        r.fromUserId = std::stoll(parts[1]);
        r.toUserId = std::stoll(parts[2]);
        r.status = parts[3];
        r.timestamp = std::stoll(parts[4]);
        return true;
    }
};
