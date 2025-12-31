#pragma once
#include "../core/DataStore.h"
#include "../models/FollowRequest.h"
#include "../core/KeyUtil.h"
#include<vector>
using namespace std;


class UserService{

    DataStore *db;

public:
    UserService(){


    }
    UserService(DataStore *dbb){
        this->db=dbb;
    }
    bool isFollowing(int64_t meId,int64_t wohId){
        int64_t x=-67;
        if(db->followingByFollower.get(db->kFollowFF(meId,wohId),x)){
            return true;
        }
        return false;
    }
    bool findRequestByFromTo(int64_t fromUserId, int64_t toUserId, FollowRequest &out) {
        int64_t offset = -67;
        if (!db->followReqByFromTo.get(db->kReqFromTo(fromUserId, toUserId), offset)) {
            
            return false;
        }
        string x;
        if (!db->requestsR.readRecord(offset, x)) {
            
            return false;
        }
        return FollowRequest::deserialize(x, out);
    }
    bool follow(int64_t fromUserId,int64_t toUserId,bool toIsPrivate){
        if(fromUserId== toUserId){

            return false;
        }
        if(isFollowing(fromUserId,toUserId)){
            return true;

        }

        if(!toIsPrivate){
            db->followingByFollower.put(db->kFollowFF(fromUserId, toUserId), 1);
            db->followersByFollowee.put(db->kFollowFE(toUserId, fromUserId), 1);

            return true;
        }

        FollowRequest existing;
        if (findRequestByFromTo(fromUserId, toUserId, existing)) {
            if (existing.status == "PENDING") {
                
                return false;
            }
        }

        FollowRequest r;
        r.requestId = db->ids.newRequestId();
        r.fromUserId = fromUserId;
        r.toUserId = toUserId;
        r.status = "PENDING";


        r.timestamp = nowUnixSeconds();

        int64_t offset = db->requestsR.appendRecord(r.serialize(r));

        db->followReqByToTime.put(db->kReqToTime(toUserId, r.timestamp, r.requestId), offset);
        db->followReqByFromTo.put(db->kReqFromTo(fromUserId, toUserId), offset);

        return true;
    }   

    void unfollow(int64_t fromUserId, int64_t toUserId) {

        db->followingByFollower.erase(db->kFollowFF(fromUserId, toUserId));
        db->followersByFollowee.erase(db->kFollowFE(toUserId, fromUserId));


    }
    vector<int64_t> listFollowing(int64_t userId) {
         vector<int64_t> out;

         string prefix = "FF:" + FixedInt(userId, 20) + ":";
         string end = nextLex(prefix);

         vector<BPlusTree::KV> rows = db->followingByFollower.rangeScan(prefix, end, 0);

        for (int i = 0; i < (int)rows.size(); i++) {
            // key = FF:<follower>:<followee>
             string k = rows[i].key;
             vector< string> parts = splitCsv(""); // dummy to keep style
            // split by ':'
             vector< string> p;
             string cur;
            for (int j = 0; j < (int)k.size(); j++) {
                if (k[j] == ':') { p.push_back(cur); cur.clear(); }
                else cur.push_back(k[j]);
            }
            p.push_back(cur);

            if ((int)p.size() >= 3) {
                int64_t followee =  stoll(p[2]);
                out.push_back(followee);
            }
        }

        return out;
    }
    vector<int64_t> listFollowers(int64_t userId) {
         vector<int64_t> out;

         string prefix = "FE:" + FixedInt(userId, 20) + ":";
         string end = nextLex(prefix);

         vector<BPlusTree::KV> rows = db->followersByFollowee.rangeScan(prefix, end, 0);

        for (int i = 0; i < (int)rows.size(); i++) {
             string k = rows[i].key;
             vector< string> p;
             string cur;
            for (int j = 0; j < (int)k.size(); j++) {
                if (k[j] == ':') { 
                    p.push_back(cur); 
                    cur.clear();
                
                }
                else cur.push_back(k[j]);
            }
            p.push_back(cur);

            if ((int)p.size() >= 3) {
                int64_t follower =  stoll(p[2]);
                out.push_back(follower);
            }
        }

        return out;
    }

    


















      vector<FollowRequest> incomingRequests(int64_t toUserId, int limit) {
          vector<FollowRequest> out;

          string prefix = "FRTO:" + FixedInt(toUserId, 20) + ":";
          string end = nextLex(prefix);

          vector<BPlusTree::KV> rows = db->followReqByToTime.rangeScan(prefix, end, limit);

        for (int i = 0; i < (int)rows.size(); i++) {
            FollowRequest r;
              string rec;
            if (db->requestsR.readRecord(rows[i].value, rec)) {
                if (FollowRequest::deserialize(rec, r)) {
                    if (r.status == "PENDING") {
                        out.push_back(r);
                    }
                }
            }
        }
        return out;
    }

    bool acceptRequest(int64_t toUserId, int64_t requestId) {

          vector<FollowRequest> inc = incomingRequests(toUserId, 1000);
        for (int i = 0; i < (int)inc.size(); i++) {
            if (inc[i].requestId == requestId) {
                FollowRequest r = inc[i];
                r.status = "ACCEPTED";

                int64_t newOffset = db->requestsR.appendRecord(FollowRequest::serialize(r));
                db->followReqByFromTo.put(db->kReqFromTo(r.fromUserId, r.toUserId), newOffset);

                db->followingByFollower.put(db->kFollowFF(r.fromUserId, r.toUserId), 1);
                db->followersByFollowee.put(db->kFollowFE(r.toUserId, r.fromUserId), 1);
                db->followReqByToTime.erase(db->kReqToTime(r.toUserId, r.timestamp, r.requestId));

                return true;
            }
        }
        return false;
    }

    bool rejectRequest(int64_t toUserId, int64_t requestId) {
          vector<FollowRequest> inc = incomingRequests(toUserId, 1000);
        for (int i = 0; i < (int)inc.size(); i++) {
            if (inc[i].requestId == requestId) {
                FollowRequest r = inc[i];
                r.status = "REJECTED";

                int64_t newOffset = db->requestsR.appendRecord(FollowRequest::serialize(r));
                db->followReqByFromTo.put(db->kReqFromTo(r.fromUserId, r.toUserId), newOffset);
                db->followReqByToTime.erase(db->kReqToTime(r.toUserId, r.timestamp, r.requestId));

                return true;
            }
        }
        return false;
    }

};