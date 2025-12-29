#pragma once
#include <string>
#include <vector>
#include <cstdint>
using  namespace std;
struct ThreadNode {
    int64_t nodeId;
    int64_t rootId;
    int64_t parentNodeId;
    int64_t authorId;
    int64_t timestamp;
    string sentenceText;

    int likes;
    int reposts;
    int comments;

    static int engagementScore(const ThreadNode &n) {
        return 2*n.reposts + 1*n.likes + 3*n.comments;
    }

    static  string serialize(const ThreadNode &n) {
        // nodeId|rootId|parent|author|timestamp|sentence|likes|reposts|comments
        return  to_string(n.nodeId) + "|" +  to_string(n.rootId) + "|" +
                to_string(n.parentNodeId) + "|" +  to_string(n.authorId) + "|" +
                to_string(n.timestamp) + "|" + n.sentenceText + "|" +
                to_string(n.likes) + "|" +  to_string(n.reposts) + "|" +
                to_string(n.comments);
    }

    static bool deserialize(const  string &s, ThreadNode &n) {
         vector< string> parts;
         string cur;
        for (int i = 0; i < (int)s.size(); i++) {
            if (s[i] == '|') { 
                parts.push_back(cur); 
                cur.clear(); 
            }
            else cur.push_back(s[i]);
        }
        parts.push_back(cur);
        if ((int)parts.size() < 9) 
            return false;

        n.nodeId =  stoll(parts[0]);
        n.rootId =  stoll(parts[1]);
        n.parentNodeId =  stoll(parts[2]);
        n.authorId =  stoll(parts[3]);
        n.timestamp =  stoll(parts[4]);
        n.sentenceText = parts[5];
        n.likes =  stoi(parts[6]);
        n.reposts =  stoi(parts[7]);
        n.comments =  stoi(parts[8]);
        return true;
    }
};
