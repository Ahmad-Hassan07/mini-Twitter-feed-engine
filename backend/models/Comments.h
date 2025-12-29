#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct Comment {
    int64_t commentId;
    int64_t postId;
    int64_t authorId;
    int64_t timestamp;
    std::string text;

    static std::string serialize(const Comment &c) {
        // id|postId|authorId|timestamp|text
        return std::to_string(c.commentId) + "|" + std::to_string(c.postId) + "|" +
               std::to_string(c.authorId) + "|" + std::to_string(c.timestamp) + "|" + c.text;
    }

    static bool deserialize(const std::string &s, Comment &c) {
        std::vector<std::string> parts;
        std::string cur;
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] == '|') { parts.push_back(cur); cur.clear(); }
            else cur.push_back(s[i]);
        }
        parts.push_back(cur);

        if (parts.size() < 5) {
            return false;
        }


        c.commentId = std::stoll(parts[0]);
        c.postId = std::stoll(parts[1]);
        c.authorId = std::stoll(parts[2]);
        c.timestamp = std::stoll(parts[3]);
        c.text = parts[4];
        return true;
    }
};
