#include "engine.h"
#include <algorithm>
#include <chrono>
#include<iostream>

SocialEngine::SocialEngine(): nextPostId(1) {}

std::int64_t SocialEngine::getCurrentTime() {
    using namespace std::chrono;
    return duration_cast<seconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

double SocialEngine::computeScore(const Post& p, std::int64_t now) {
    double ageHours = (now - p.timestamp) / 3600.0;
    if (ageHours < 0) {
        std::cerr<<"ageHours are in neg"; 
        ageHours=0;
    }
    double recencyPart = 1.0 / (1.0 + ageHours);
    double engagementPart = p.likes * 1.0 + p.comments * 2.0;

    return 0.6 * engagementPart + 0.4 * recencyPart;
}

bool SocialEngine::createUser(const std::string& userId, const std::string& name) {

    if (usersById.find(userId) != usersById.end()) {
        return false;
    }
    User u;
    u.id = userId;
    u.name = name;
    usersById[userId] = u;
    return true;
}

bool SocialEngine::followUser(const std::string& followerId, const std::string& followeeId) {
    auto itFollower = usersById.find(followerId);
    auto itFollowee = usersById.find(followeeId);
    if (itFollower == usersById.end() || itFollowee == usersById.end()) {
        return false;
    }

    User& follower = itFollower->second;
    for (const auto& f : follower.following) {
        if (f == followeeId) return true; 
    }
    follower.following.push_back(followeeId);
    return true;
}

int SocialEngine::createPost(const std::string& userId, const std::string& text) {
    if (usersById.find(userId) == usersById.end()) {
        return -1;
    }

    Post p;
    p.id = nextPostId++;
    p.userId = userId;
    p.text = text;
    p.timestamp = getCurrentTime();
    p.likes = 0;
    p.comments = 0;
    p.score = computeScore(p, p.timestamp);

    postsById[p.id] = p;
    postsByUser[userId].push_back(p.id);   // <--- new line

    return p.id;
}


bool SocialEngine::likePost(int postId) {
    auto it = postsById.find(postId);
    if (it == postsById.end()) return false;

    Post& p = it->second;
    p.likes += 1;
    p.score = computeScore(p, getCurrentTime());
    return true;
}

std::vector<Post> SocialEngine::getPersonalFeed(const std::string& userId, int limit) {
    std::vector<Post> result;

    auto itUser = usersById.find(userId);
    if (itUser == usersById.end()) {
        return result;
    }

    const User& u = itUser->second;

    std::vector<std::string> sources;
    sources.push_back(userId);
    for (const auto& fid : u.following) {
        sources.push_back(fid);
    }


    for (const auto& srcId : sources) {
        auto itList = postsByUser.find(srcId);
        if (itList == postsByUser.end()) continue; 

        const std::vector<int>& postIds = itList->second;
        for (int pid : postIds) {
            auto itPost = postsById.find(pid);
            if (itPost != postsById.end()) {
                result.push_back(itPost->second);
            }
        }
    }


    std::int64_t now = getCurrentTime();
    for (auto& p : result) {
        p.score = computeScore(p, now);
    }

    std::sort(result.begin(), result.end(),
              [](const Post& a, const Post& b) {
                  return a.score > b.score;
              });

    if (limit > 0 && (int)result.size() > limit) {
        result.resize(limit);
    }

    return result;
}
