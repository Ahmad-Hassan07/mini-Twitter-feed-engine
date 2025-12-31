#pragma once
#include <vector>
#include <cstdint>
#include "../core/DataStore.h"

#include "../core/KeyUtil.h"
#include <queue>

#include "../models/Post.h"
struct HeapItem {
    int64_t postId;
    int64_t timestamp;  
    int followeeIndex; 
};

struct HeapCmp {
    bool operator()(const HeapItem &a, const HeapItem &b) {
       
        return a.timestamp < b.timestamp;

    }
};
class FeedService {

    DataStore *db;

public:
    FeedService(DataStore *dbbb){
         this->db = dbbb;

    }

    vector<Post> homeFeed(int64_t userId, int limit) {
        vector<Post> out;


        string followPrefix = "FF:" + FixedInt(userId, 20) + ":";
        string followEnd = nextLex(followPrefix);
        vector<BPlusTree::KV> followRows = db->followingByFollower.rangeScan(followPrefix, followEnd, 0);


        vector<BPlusTree::Iterator> iters;
        vector<string> itEnds;

        for (int i = 0; i < (int)followRows.size(); i++) {

            string k = followRows[i].key;

            vector<string> parts;
            string cur;
            for (int j = 0; j < (int)k.size(); j++) {
                if (k[j] == ':') { parts.push_back(cur); cur.clear(); }
                else cur.push_back(k[j]);
            }
            parts.push_back(cur);

            if ((int)parts.size() >= 3) {
                int64_t followeeId = stoll(parts[2]);

                string tlPrefix = "PUT:" + FixedInt(followeeId, 20) + ":";
                string tlEnd = nextLex(tlPrefix);

                BPlusTree::Iterator it = db->postsByUserTime.iterator(tlPrefix, tlEnd);

                iters.push_back(it);
                itEnds.push_back(tlEnd);
            }
        }
        priority_queue<HeapItem, vector<HeapItem>, HeapCmp> pq;

        for (int i = 0; i < (int)iters.size(); i++) {
            if (iters[i].hasNext()) {
                BPlusTree::KV kv = iters[i].next();
                HeapItem hi;
                hi.postId = kv.value;
                hi.timestamp = decodeTimestampFromPUTKey(kv.key);
                hi.followeeIndex = i;
                pq.push(hi);
            }
        }


        while (!pq.empty() && (int)out.size() < limit) {
            HeapItem top = pq.top();
            pq.pop();

            Post p;
            if (loadPostById(top.postId, p)) {
                if (!p.isDeleted) {
                    out.push_back(p);
                }
            }

            int idx = top.followeeIndex;
            if (idx >= 0 && idx < (int)iters.size()) {
                if (iters[idx].hasNext()) {
                    BPlusTree::KV kv = iters[idx].next();
                    HeapItem hi;
                    hi.postId = kv.value;
                    hi.timestamp = decodeTimestampFromPUTKey(kv.key);
                    hi.followeeIndex = idx;
                    pq.push(hi);
                }
            }
        }

        return out;
    }


    vector<Post> exploreFeed(int limit){
         vector<Post> out;

        // TR index already sorted by trendScore desc then newest (because we store inverted)
        string start = "TR:";
        string end = nextLex(start);

        vector<BPlusTree::KV> rows = db->publicPostsByTrend.rangeScan(start, end, limit);

        for (int i = 0; i < (int)rows.size(); i++) {
            int64_t postId = rows[i].value;
            Post p;
            if (loadPostById(postId, p)) {
                if (!p.isDeleted && p.isPublic) out.push_back(p);
            }
        }

        return out;
    }

    bool loadPostById(int64_t postId, Post &out){
        int64_t offset = -67;
        if (!db->postsById.get(db->kPostId(postId), offset)) {
            return false;
        }
        string x;
        if (!db->postsR.readRecord(offset, x)) {
            return false;

        }
        return Post::deserialize(x, out);
    }






































    static int64_t decodeTimestampFromPUTKey(const string &key) {
    // key = PUT:<author20>:<invTime10>:<postId20>
    // invTime = MAX - t
    // t = MAX - invTime
    const int64_t MAX_T = 9999999999LL;

    vector<string> parts;
    string cur;
    for (int i = 0; i < (int)key.size(); i++) {
        if (key[i] == ':') { parts.push_back(cur); cur.clear(); }
        else cur.push_back(key[i]);
    }
    parts.push_back(cur);

    if ((int)parts.size() < 4) return 0;
    int64_t inv = stoll(parts[2]);
    int64_t t = MAX_T - inv;
    return t;
}

};
