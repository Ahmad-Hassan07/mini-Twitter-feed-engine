#include "engine.h"
#include <algorithm>
#include <chrono>
#include <iostream>

using namespace std;

SocialEngine::SocialEngine() {
    nextPostId = 1;
}

std::int64_t SocialEngine::getCurrentTime() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

double SocialEngine::computeScore(const Post& p, std::int64_t now) {
    double ageHours = (now - p.timestamp) / 3600.0;
    if (ageHours < 0) ageHours = 0;
    double recencyPart = 1.0 / (1.0 + ageHours);
    double engagementPart = p.likes * 1.0 + p.comments * 2.0;
    return 0.6 * engagementPart + 0.4 * recencyPart;
}

bool SocialEngine::createUser(const std::string& userId, const std::string& name) {
    User* u = usersById.search(userId);
    if (u) return false;
    User nu;
    nu.id = userId;
    nu.name = name;
    usersById.insert(userId, nu);
    return true;
}

bool SocialEngine::userExists(const std::string& userId) {
    return usersById.search(userId) != nullptr;
}

bool SocialEngine::followUser(const std::string& followerId, const std::string& followeeId) {
    User* follower = usersById.search(followerId);
    User* followee = usersById.search(followeeId);
    if (!follower || !followee) return false;
    for (auto& x : follower->following) {
        if (x == followeeId) return true;
    }
    follower->following.push_back(followeeId);
    return true;
}

int SocialEngine::createPost(const std::string& userId, const std::string& text) {
    User* u = usersById.search(userId);
    if (!u) return -1;
    Post p;
    p.id = nextPostId++;
    p.userId = userId;
    p.text = text;
    p.timestamp = getCurrentTime();
    p.likes = 0;
    p.comments = 0;
    p.score = computeScore(p, p.timestamp);
    postsById.insert(p.id, p);
    std::vector<int>* listPtr = postsByUser.search(userId);
    if (!listPtr) {
        std::vector<int> v;
        v.push_back(p.id);
        postsByUser.insert(userId, v);
    } else {
        listPtr->push_back(p.id);
    }
    return p.id;
}

bool SocialEngine::likePost(int postId) {
    Post* p = postsById.search(postId);
    if (!p) return false;
    p->likes += 1;
    p->score = computeScore(*p, getCurrentTime());
    return true;
}

std::vector<Post> SocialEngine::getPersonalFeed(const std::string& userId, int limit) {
    std::vector<Post> result;
    User* u = usersById.search(userId);
    if (!u) return result;

    std::vector<std::string> sources;
    sources.push_back(userId);
    for (auto& x : u->following) sources.push_back(x);

    for (auto& src : sources) {
        std::vector<int>* listPtr = postsByUser.search(src);
        if (!listPtr) continue;
        for (int pid : *listPtr) {
            Post* p = postsById.search(pid);
            if (p) result.push_back(*p);
        }
    }

    std::int64_t now = getCurrentTime();
    for (auto& p : result) {
        p.score = computeScore(p, now);
    }

    std::sort(result.begin(), result.end(), [](const Post& a, const Post& b) {
        return a.score > b.score;
    });

    if (limit > 0 && (int)result.size() > limit) {
        result.resize(limit);
    }

    return result;
}
