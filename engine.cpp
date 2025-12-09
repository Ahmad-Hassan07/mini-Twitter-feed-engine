#include "engine.h"
#include <algorithm>
#include <chrono>
#include <iostream>

using namespace std;

SocialEngine::SocialEngine() {
    nextPostId = 1;

}

int64_t SocialEngine::getCurrentTime() {
    using namespace chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();

}

double SocialEngine::computeScore(const Post& p, int64_t TimeNow) {
    double ageHours = ((TimeNow - p.timestamp) / (3600* 1.0));

    if (ageHours < 0) {
        cout<<"Error! age is neg";
        ageHours = 0;

    }
    double recencyPart = 1.0 / (1.0 + ageHours);
    double engagementPart = ((p.likes * 1.0) + (p.comments * 2.0));
    return ((0.6 * engagementPart) + (0.4 * recencyPart));

}

bool SocialEngine::createUser(string& userId,string& name) {
    User* u = usersById.search(userId);
    if (u) {
        return false;

    }

    User nu;
    nu.id = userId;
    nu.name = name;
    usersById.insert(userId, nu);

    return true;

}

bool SocialEngine::userExists(string& userId) {
    if (usersById.search(userId) != nullptr){
        return true;

    }
    return false;

}

bool SocialEngine::followUser(const string& followerId, const string& followingId) {
    User* follower = usersById.search(followerId);
    User* followee = usersById.search(followingId);
    if ((!follower) || (!followee)) {
        return false;

    }

    for (string& x : (*follower).following) {
        if (x == followingId) {
            return true;
        
        }
    }
    (*follower).following.push_back(followingId);
    return true;

}

int SocialEngine::createPost(string& userId, string& text) {
    User* u = usersById.search(userId);

    if (!u) 
        return -1;

    Post p;
    p.id = nextPostId++;
    p.userId = userId;
    p.text = text;
    p.timestamp = getCurrentTime();
    p.likes = 0;
    p.comments = 0;
    p.score = computeScore(p, p.timestamp);
    postsById.insert(p.id, p);
    vector<int>* listPtr = postsByUser.search(userId);


    if (!listPtr) {
        vector<int> v;
        v.push_back(p.id);
        postsByUser.insert(userId, v);

    } else {
        listPtr->push_back(p.id);

    }
    return p.id;

}

bool SocialEngine::likePost(int postId) {
    Post* p = postsById.search(postId);
    if (!p) {
        return false;
    }

    p->likes += 1;
    p->score = computeScore(*p, getCurrentTime());
    return true;

}

vector<Post> SocialEngine::getPersonalFeed(const string& userId, int limit) {
    vector<Post> result;
    User* u = usersById.search(userId);
    if (!u) {
        return result;

    }


    vector<string> sources;
    sources.push_back(userId);

    for (string& x : u->following) {
        sources.push_back(x);
    
    }
    for (string& src : sources) {
        vector<int>* listPtr = postsByUser.search(src);
        if (!listPtr) {
            continue;

        }
        

        for (int pid : *listPtr) {
            Post* p = postsById.search(pid);

            if (p) {
                result.push_back(*p);

            }
        }
    }

    int64_t now = getCurrentTime();
    for (Post& p : result) {
        p.score = computeScore(p, now);

    }

    sort(result.begin(), result.end(), [](const Post& a, const Post& b) {
        return a.score > b.score;

    });

    if (limit > 0 && (int)result.size() > limit) {
        result.resize(limit);

    }

    return result;

}
