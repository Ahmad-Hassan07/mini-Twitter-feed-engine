#ifndef ENGINE_H
#define ENGINE_H

#include "models.h"
#include <map>
#include <vector>
#include <string>

class SocialEngine {
    std::map<std::string, User> usersById; 
    std::map<int, Post> postsById;
    int nextPostId;

    double computeScore(const Post& p, std::int64_t now);
    std::int64_t getCurrentTime();
public:
    SocialEngine();
    bool createUser(const std::string& userId, const std::string& name);
    bool followUser(const std::string& followerId, const std::string& followeeId);
    int  createPost(const std::string& userId, const std::string& text);
    bool likePost(int postId);
    std::vector<Post> getPersonalFeed(const std::string& userId, int limit);

};

#endif 
