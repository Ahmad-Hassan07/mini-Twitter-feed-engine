#pragma once
#include <string>
#include "storage/RecordFile.h"
#include "storage/BPlusTree.h"
#include "storage/IdGenerator.h"
#include "core/KeyUtil.h"
class DataStore{
public:
    IdGenerator ids;
    RecordFile usersR;
    RecordFile postsR;
    RecordFile commentsR;
    RecordFile requestsR;
    RecordFile threadNodesR;
    RecordFile threadMetaR;
    BPlusTree usersById;
    BPlusTree usersByUsername;
    BPlusTree postsById;
    BPlusTree postsByUserTime;
    BPlusTree followingByFollower;
    BPlusTree followersByFollowee;
    BPlusTree followReqByToTime;
    BPlusTree followReqByFromTo;
    BPlusTree likesByUserPost;
    BPlusTree repostsByUserPost;
    BPlusTree commentsById;
    BPlusTree commentsByPostTime;
    BPlusTree postsByHashtagRank;
    BPlusTree publicPostsByTrend;
    BPlusTree threadNodeById;
    BPlusTree threadChildrenByParentTime;
    BPlusTree threadMetaByRootId;
    BPlusTree threadsByPopularity;
    void openAll(const string &Dir){

        usersR.open(Dir + "/users.rec");
        postsR.open(Dir + "/posts.rec");
        commentsR.open(Dir + "/comments.rec");
        requestsR.open(Dir + "/follow_requests.rec");
        threadNodesR.open(Dir + "/thread_nodes.rec");
        threadMetaR.open(Dir + "/thread_meta.rec");
        usersById.open(Dir + "/idx_users_by_id.bpt");
        usersByUsername.open(Dir + "/idx_users_by_username.bpt");
        postsById.open(Dir + "/idx_posts_by_id.bpt");
        postsByUserTime.open(Dir + "/idx_posts_by_user_time.bpt");
        followingByFollower.open(Dir + "/idx_following_by_follower.bpt");
        followersByFollowee.open(Dir + "/idx_followers_by_followee.bpt");
        followReqByToTime.open(Dir + "/idx_req_by_to_time.bpt");
        followReqByFromTo.open(Dir + "/idx_req_by_from_to.bpt");
        likesByUserPost.open(Dir + "/idx_likes_by_user_post.bpt");
        repostsByUserPost.open(Dir + "/idx_reposts_by_user_post.bpt");
        commentsById.open(Dir + "/idx_comments_by_id.bpt");
        commentsByPostTime.open(Dir + "/idx_comments_by_post_time.bpt");
        postsByHashtagRank.open(Dir + "/idx_posts_by_hashtag_rank.bpt");
        publicPostsByTrend.open(Dir + "/idx_public_posts_by_trend.bpt");
        threadNodeById.open(Dir + "/idx_thread_node_by_id.bpt");
        threadChildrenByParentTime.open(Dir + "/idx_thread_children_by_parent_time.bpt");
        threadMetaByRootId.open(Dir + "/idx_thread_meta_by_root.bpt");
        threadsByPopularity.open(Dir + "/idx_threads_by_popularity.bpt");
        ids.open(Dir + "/ids.bin");


    }
    string kUserId(int64_t userId) {
        return "U:" + FixedInt(userId, 20);

    }
    string kUsername(const string &username) {
        return "UN:" + username;

    }

    string kPostId(int64_t postId) {
        return "P:" + FixedInt(postId, 20);

    }

    string kPostUserTime(int64_t authorId, int64_t timestamp, int64_t postId) {
        return "PUT:" + FixedInt(authorId, 20) + ":" + invTimeKey(timestamp) + ":" + FixedInt(postId, 20);

    }

    string kFollowFF(int64_t followerId, int64_t followeeId) {
        return "FF:" + FixedInt(followerId, 20) + ":" + FixedInt(followeeId, 20);

    }


    string kFollowFE(int64_t followeeId, int64_t followerId) {
        return "FE:" + FixedInt(followeeId, 20) + ":" + FixedInt(followerId, 20);

    }

    string kReqToTime(int64_t toUserId, int64_t timestamp, int64_t reqId) {
        return "FRTO:" + FixedInt(toUserId, 20) + ":" + FixedInt(timestamp, 20) + ":" + FixedInt(reqId, 20);

    }

    string kReqFromTo(int64_t fromUserId, int64_t toUserId) {
        return "FRFT:" + FixedInt(fromUserId, 20) + ":" + FixedInt(toUserId, 20);

    }

    string kLike(int64_t userId, int64_t postId) {
        return "L:" + FixedInt(userId, 20) + ":" + FixedInt(postId, 20);

    }
    string kRepost(int64_t userId, int64_t postId) {
        return "R:" + FixedInt(userId, 20) + ":" + FixedInt(postId, 20);

    }
    string kCommentId(int64_t commentId) {
        return "C:" + FixedInt(commentId, 20);

    }

    string kCommentPostTime(int64_t postId, int64_t timestamp, int64_t commentId) {
        return "CPT:" + FixedInt(postId, 20) + ":" + FixedInt(timestamp, 20) + ":" + FixedInt(commentId, 20);

    }


    string kHashtagRank(const string &tag, int score, int64_t timestamp, int64_t postId) {
        return "HT:" + tag + ":" + invScoreKey(score) + ":" + invTimeKey(timestamp) + ":" + FixedInt(postId, 20);

    }

    string kPublicTrend(int trendScore, int64_t timestamp, int64_t postId) {
        return "TR:" + invScoreKey(trendScore) + ":" + invTimeKey(timestamp) + ":" + FixedInt(postId, 20);

    }
    string kThreadNodeId(int64_t nodeId) {
        return "TN:" + FixedInt(nodeId, 20);

    }

    string kThreadChild(int64_t parentNodeId, int64_t timestamp, int64_t childNodeId) {
        return "TC:" + FixedInt(parentNodeId, 20) + ":" + FixedInt(timestamp, 20) + ":" + FixedInt(childNodeId, 20);

    }

    string kThreadMeta(int64_t rootId) {
        return "TM:" + FixedInt(rootId, 20);

    }

    string kThreadPopularity(int threadScore, int64_t lastUpdated, int64_t rootId) {
        return "TP:" + invScoreKey(threadScore) + ":" + invTimeKey(lastUpdated) + ":" + FixedInt(rootId, 20);

    }
};