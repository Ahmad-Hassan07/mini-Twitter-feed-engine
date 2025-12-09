#ifndef ENGINE_H
#define ENGINE_H

#include "models.h"
#include "btree.h"
#include <string>
#include <vector>
#include <cstdint>

class SocialEngine {
public:
    SocialEngine();

    bool createUser(const std::string& userId, const std::string& name);
    bool followUser(const std::string& followerId, const std::string& followeeId);
    bool userExists(const std::string& userId);

    int createPost(const std::string& userId, const std::string& text);
    bool likePost(int postId);

    std::vector<Post> getPersonalFeed(const std::string& userId, int limit);

private:
    BTree<std::string, User> usersById;
    BTree<int, Post> postsById;
    BTree<std::string, std::vector<int>> postsByUser;

    int nextPostId;

    double computeScore(const Post& p, std::int64_t now);
    std::int64_t getCurrentTime();
};

#endif
