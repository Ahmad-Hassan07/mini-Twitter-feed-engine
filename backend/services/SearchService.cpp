#include "SearchService.h"
#include "../core/KeyUtil.h"

SearchService::SearchService(DataStore *store) {
    db = store;
}

bool SearchService::loadUser(int64_t userId, User &out) {
    int64_t offset = -1;
    if (!db->usersById.get(db->kUserId(userId), offset)) return false;
    string rec;
    if (!db->usersR.readRecord(offset, rec)) return false;
    return User::deserialize(rec, out);
}

bool SearchService::loadPost(int64_t postId, Post &out) {
    int64_t offset = -1;
    if (!db->postsById.get(db->kPostId(postId), offset)) return false;
    string rec;
    if (!db->postsR.readRecord(offset, rec)) return false;
    return Post::deserialize(rec, out);
}

vector<User> SearchService::searchUsersPrefix(const string &prefix, int limit) {
    vector<User> out;

    string start = "UN:" + prefix;
    string end = nextLex(start);

    vector<BPlusTree::KV> rows = db->usersByUsername.rangeScan(start, end, limit);

    for (int i = 0; i < (int)rows.size(); i++) {
        int64_t userId = rows[i].value;
        User u;
        if (loadUser(userId, u)) out.push_back(u);
    }

    return out;
}

vector<Post> SearchService::searchHashtag(const string &tag, int limit) {
    vector<Post> out;

    string t = toLowerSimple2(tag);
    string start = "HT:" + t + ":";
    string end = nextLex(start);

    vector<BPlusTree::KV> rows = db->postsByHashtagRank.rangeScan(start, end, limit);

    for (int i = 0; i < (int)rows.size(); i++) {
        int64_t postId = rows[i].value;
        Post p;
        if (loadPost(postId, p)) {
            if (!p.isDeleted) out.push_back(p);
        }
    }

    return out;
}
