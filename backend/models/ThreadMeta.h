#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct ThreadMeta {
    int64_t rootId;
    int score;          
    int64_t lastUpdated; 

    static std::string serialize(const ThreadMeta &m) {
        // rootId|score|lastUpdated
        return std::to_string(m.rootId) + "|" + std::to_string(m.score) + "|" + std::to_string(m.lastUpdated);
    }

    static bool deserialize(const std::string &s, ThreadMeta &m) {
        std::vector<std::string> parts;
        std::string cur;
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] == '|') { parts.push_back(cur); cur.clear(); }
            else cur.push_back(s[i]);
        }
        parts.push_back(cur);
        if ((int)parts.size() < 3) {
            return false;

        }


        m.rootId = std::stoll(parts[0]);
        m.score = std::stoi(parts[1]);
        m.lastUpdated = std::stoll(parts[2]);
        return true;
    }
};
