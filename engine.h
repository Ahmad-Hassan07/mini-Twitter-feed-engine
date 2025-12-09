#ifndef ENGINE_H
#define ENGINE_H

#include "models.h"
#include "btree.h"
#include <string>
#include <vector>
#include <cstdint>
using namespace std;
class SocialEngine {
public:
    SocialEngine();

    bool createUser(const string& userId, const string& name);
    bool followUser(const string& followerId, const string& followeeId);
    bool userExists(string& userId);

    int createPost(string& userId,string& text);
    bool likePost(int postId);

    vector<Post> getPersonalFeed(const string& userId, int limit);

private:
    BTree<string, User> usersById;
    BTree<int, Post> postsById;
    BTree<string, vector<int>> postsByUser;

    int nextPostId;

    double computeScore(const Post& p, int64_t now);
    int64_t getCurrentTime();
};

#endif
