#pragma once
#include <vector>
#include <string>
#include "core/DataStore.h"
#include "models/User.h"
#include "models/Post.h"

class SearchService {
private:
    DataStore *db;

public:
    SearchService(DataStore *store);

    std::vector<User> searchUsersPrefix(const std::string &prefix, int limit);
    std::vector<Post> searchHashtag(const std::string &tag, int limit);

    bool loadUser(int64_t userId, User &out);
    bool loadPost(int64_t postId, Post &out);
};
