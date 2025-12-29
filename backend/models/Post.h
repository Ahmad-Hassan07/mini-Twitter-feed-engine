#pragma once
#include <string>
#include <vector>
#include <cstdint>
using namespace std;

struct Post{
    int64_t postId;
    int64_t authorId;
    int64_t timestamp;
    bool isPublic;
    string text;
    string tagsCsv;
    int likes;
    int reposts;
    int comments;
    bool isDeleted;

    static int engagementScore(const Post& p){
        return 2*p.reposts + 1*p.likes+3*p.comments;
    }
    static std::string serialize(const Post &p) {
        // postId|authorId|timestamp|isPublic|text|tagsCsv|likes|reposts|comments|isDeleted
        return std::to_string(p.postId) + "|" + std::to_string(p.authorId) + "|" +
               std::to_string(p.timestamp) + "|" + (p.isPublic ? "1" : "0") + "|" +
               p.text + "|" + p.tagsCsv + "|" + std::to_string(p.likes) + "|" +
               std::to_string(p.reposts) + "|" + std::to_string(p.comments) + "|" +
               (p.isDeleted ? "1" : "0");
    }
    static bool deserialize(const std::string &s, Post &p) {
        std::vector<std::string> parts;
        std::string cur;
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] == '|') { parts.push_back(cur); cur.clear(); }
            else cur.push_back(s[i]);
        }
        parts.push_back(cur);
        if ((int)parts.size() < 10) return false;

        p.postId = std::stoll(parts[0]);
        p.authorId = std::stoll(parts[1]);
        p.timestamp = std::stoll(parts[2]);
        p.isPublic = (parts[3] == "1");
        p.text = parts[4];
        p.tagsCsv = parts[5];
        p.likes = std::stoi(parts[6]);
        p.reposts = std::stoi(parts[7]);
        p.comments = std::stoi(parts[8]);
        p.isDeleted = (parts[9] == "1");
        return true;
    }
};