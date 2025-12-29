#pragma once
#include <string>
#include <vector>
#include <cstdint>
using namespace std;
struct User{
    int64_t userId;
    string username;
    string displayName;
    string bio;
    bool isPrivate;
    string salt;
    string passHash;
    int64_t createdAt;

    static string serialize(const User& u){
        return to_string(u.userId)+"|"+u.username+"|"+u.displayName+ "|" +u.bio + "|" + (u.isPrivate ? "1" : "0") + "|" + u.salt + "|" +u.passHash + "|" + to_string(u.createdAt);
    }

    static bool deserialize(const string &s,User &u){
        vector<string> parts;
        string cur;
        for(int i=0;i<(int)s.size();i++){
            if(s[i]=='|'){
                parts.push_back(cur);
                cur.clear();
            }
            else{
                cur.push_back(s[i]);
            }
        }
        parts.push_back(cur);

        if(parts.size()<8){
            return false;
        }
        u.userId = std::stoll(parts[0]);
        u.username = parts[1];
        u.displayName = parts[2];
        u.bio = parts[3];
        u.isPrivate = (parts[4] == "1");
        u.salt = parts[5];
        u.passHash = parts[6];
        u.createdAt = std::stoll(parts[7]);
        return true;

    }

};