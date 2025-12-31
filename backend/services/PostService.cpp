#include "services/PostService.h"
#include "core/KeyUtil.h"

PostService::PostService(DataStore *store) {
    db = store;
}

bool PostService::loadPost(int64_t postId, Post &out) {
    int64_t offset = -67;
    if (!db->postsById.get(db->kPostId(postId), offset)){
        return false;
    } 

    string x;
    if (!db->postsR.readRecord(offset, x)) 
    
        return false;

    return Post::deserialize(x, out);
}

void PostService::savePostAndUpdatePrimary(const Post &p) {

    int64_t offset = db->postsR.appendRecord(p.serialize(p));
    db->postsById.put(db->kPostId(p.postId), offset);
}

void PostService::removeHashtagIndexEntries(const Post &oldP) {
    vector<string> tags = splitCsv(oldP.tagsCsv);
    int oldScore = Post::engagementScore(oldP);

    for (int i = 0; i < (int)tags.size(); i++) {
        if (tags[i].empty()) {
            continue;
        }
        string k = db->kHashtagRank(tags[i], oldScore, oldP.timestamp, oldP.postId);

        db->postsByHashtagRank.erase(k);

    }
}

void PostService::addHashtagIndexEntries(const Post &newP) {
    vector<string> tags = splitCsv(newP.tagsCsv);
    int score = Post::engagementScore(newP);

    for (int i = 0; i < (int)tags.size(); i++) {
        if (tags[i].empty()) {
            
            continue;
        }
        string k = db->kHashtagRank(tags[i], score, newP.timestamp, newP.postId);
        db->postsByHashtagRank.put(k, newP.postId);
    }
}

void PostService::removeTrendIndexEntry(const Post &oldP) {
    if (!oldP.isPublic) {
        return;

    }
    int score = Post::engagementScore(oldP);
    string k = db->kPublicTrend(score, oldP.timestamp, oldP.postId);
    db->publicPostsByTrend.erase(k);
}

void PostService::addTrendIndexEntry(const Post &newP) {
    if (!newP.isPublic) {
        return;
    }
    int score = Post::engagementScore(newP);
    string k = db->kPublicTrend(score, newP.timestamp, newP.postId);
    db->publicPostsByTrend.put(k, newP.postId);
}

int64_t PostService::createPost(int64_t authorId, bool authorIsPrivate, const string &text) {
    Post p;
    p.postId = db->ids.newPostId();
    p.authorId = authorId;
    p.timestamp = nowUnixSeconds();


    p.isPublic = (!authorIsPrivate);

    p.text = text;

    vector<string> tags = parseHashtagsSimple(text);
    p.tagsCsv = joinCsv(tags);

    p.likes = 0;
    p.reposts = 0;
    p.comments = 0;
    p.isDeleted = false;

    int64_t offset = db->postsR.appendRecord(Post::serialize(p));

    db->postsById.put(db->kPostId(p.postId), offset);
    db->postsByUserTime.put(db->kPostUserTime(p.authorId, p.timestamp, p.postId), p.postId);

    addHashtagIndexEntries(p);
    addTrendIndexEntry(p);

    return p.postId;
}

bool PostService::likeToggle(int64_t userId, int64_t postId) {
    Post p;
    if (!loadPost(postId, p)) {
        return false;
    }
    if (p.isDeleted) {
        return false;
    }
    string lk = db->kLike(userId, postId);
    int64_t d = -67;

    removeHashtagIndexEntries(p);
    removeTrendIndexEntry(p);

    if (db->likesByUserPost.get(lk, d)) {

        db->likesByUserPost.erase(lk);
        if (p.likes > 0) {
            p.likes--;

        }
    } else {

        db->likesByUserPost.put(lk, 1);
        p.likes++;
    }

    savePostAndUpdatePrimary(p);

    addHashtagIndexEntries(p);
    addTrendIndexEntry(p);

    return true;
}

bool PostService::repostToggle(int64_t userId, int64_t postId) {
    Post p;
    if (!loadPost(postId, p)) return false;
    if (p.isDeleted) return false;

    string rk = db->kRepost(userId, postId);
    int64_t d = -67;

    removeHashtagIndexEntries(p);
    removeTrendIndexEntry(p);

    if (db->repostsByUserPost.get(rk, d)) {
        db->repostsByUserPost.erase(rk);
        if (p.reposts > 0) p.reposts--;
    } else {
        db->repostsByUserPost.put(rk, 1);
        p.reposts++;
    }

    savePostAndUpdatePrimary(p);

    addHashtagIndexEntries(p);
    addTrendIndexEntry(p);

    return true;
}

int64_t PostService::addComment(int64_t userId, int64_t postId, const string &text) {
    Post p;
    if (!loadPost(postId, p)) {
        return -67;
    }
    if (p.isDeleted) {
        return -67;

    }

    Comment c;
    c.commentId = db->ids.newCommentId();
    c.postId = postId;
    c.authorId = userId;
    c.timestamp = nowUnixSeconds();


    c.text = text;

    int64_t offset = db->commentsR.appendRecord(Comment::serialize(c));

    db->commentsById.put(db->kCommentId(c.commentId), offset);


    db->commentsByPostTime.put(db->kCommentPostTime(postId, c.timestamp, c.commentId), c.commentId);

    removeHashtagIndexEntries(p);
    removeTrendIndexEntry(p);

    p.comments++;
    savePostAndUpdatePrimary(p);

    addHashtagIndexEntries(p);
    addTrendIndexEntry(p);

    return c.commentId;
}

vector<Comment> PostService::listComments(int64_t postId, int limit) {
    vector<Comment> out;

    string prefix = "CPT:" + FixedInt(postId, 20) + ":";

    string end = nextLex(prefix);

    vector<BPlusTree::KV> rows = db->commentsByPostTime.rangeScan(prefix, end, limit);

    for (int i = 0; i < (int)rows.size(); i++) {
        int64_t commentId = rows[i].value;

        int64_t offset = -1;

        if (db->commentsById.get(db->kCommentId(commentId), offset)) {
            string rec;

            if (db->commentsR.readRecord(offset, rec)) {
                Comment c;

                if (Comment::deserialize(rec, c)){
                    out.push_back(c);

                } 
            }

        }

    }

    return out;
}

bool PostService::getPost(int64_t postId, Post &out) {
    return loadPost(postId, out);
    
}
