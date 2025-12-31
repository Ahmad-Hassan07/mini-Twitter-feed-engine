#pragma once
#include <vector>
#include "../core/DataStore.h"
#include "../models/Post.h"

#include "../models/Comments.h"
using namespace std;
class PostService {
    DataStore *db;

    bool loadPost(int64_t postId, Post &out);
    void savePostAndUpdatePrimary(const Post &p);

    void removeHashtagIndexEntries(const Post &oldP);
    void addHashtagIndexEntries(const Post &newP);

    void removeTrendIndexEntry(const Post &oldP);
    void addTrendIndexEntry(const Post &newP);

public:
    PostService(DataStore *store);

    int64_t createPost(int64_t authorId, bool authorIsPrivate, const string &text);

    bool likeToggle(int64_t userId, int64_t postId);
    bool repostToggle(int64_t userId, int64_t postId);

    int64_t addComment(int64_t userId, int64_t postId, const string &text);

    vector<Comment> listComments(int64_t postId, int limit);

    bool getPost(int64_t postId, Post &out);
};
