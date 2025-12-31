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
    static string serialize(const Post &p) {
        // postId|authorId|timestamp|isPublic|text|tagsCsv|likes|reposts|comments|isDeleted
        return to_string(p.postId) + "|" + to_string(p.authorId) + "|" +
               to_string(p.timestamp) + "|" + (p.isPublic ? "1" : "0") + "|" +
               p.text + "|" + p.tagsCsv + "|" + to_string(p.likes) + "|" +
               to_string(p.reposts) + "|" + to_string(p.comments) + "|" +
               (p.isDeleted ? "1" : "0");
    }
    static bool deserialize(const string &s, Post &p) {
        vector< string> parts;
        string cur;
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] == '|') { parts.push_back(cur); cur.clear(); }
            else cur.push_back(s[i]);
        }
        parts.push_back(cur);
        if ((int)parts.size() < 10) return false;

        p.postId = stoll(parts[0]);
        p.authorId = stoll(parts[1]);
        p.timestamp = stoll(parts[2]);
        p.isPublic = (parts[3] == "1");
        p.text = parts[4];
        p.tagsCsv = parts[5];
        p.likes = stoi(parts[6]);
        p.reposts = stoi(parts[7]);
        p.comments = stoi(parts[8]);
        p.isDeleted = (parts[9] == "1");
        return true;
    }
};