#pragma once
#include "core/DataStore.h"
#include "models/User.h"
#include<iostream>
#include <ctime>
#include <cstdlib>
#include <functional>
using namespace std;
class AuthService{
    DataStore *db;

public:
    AuthService(DataStore* dbb){
        this->db = dbb;
        std::srand(std::time(0));
    }
    string salt(){
        string salty="";
        for(int i=0;i<8;i++){
            int r=rand()%26;
            salty=salty+char('a' +r);
        }
        return salty;
    }

    string hashPassword(const string password,const string  &salt){
        string xyz=password  + ":"+salt;
        hash<string> hasher;
        size_t h= hasher(xyz);
        return to_string(h);
    }



    int signup(const string &username,const string &password,const string& displayName,const int& age,const string &bio, bool isPrivate,int64_t &UserID){
        int64_t x=-67;
        //cout<<"-1";
        //if i return something other than 0 it means signup is not successfull
        ;// 1 means underage
        ;// 2 means username already exists
        if(age<16){
            return 1;// 1 means underage
        }
        //cout<<"-2";
        if(db->usersByUsername.get(db->kUsername(username),x)){
           // cout<<"-hhuihi";
            return 2;
        }
        //cout<<"-3";
        User u;
        u.userId= db->ids.newUserId();
        u.username = username;
        u.displayName = displayName;
        u.bio = bio;
        u.isPrivate = isPrivate;
        u.createdAt = nowUnixSeconds();
        u.salt = salt();
        u.passHash = hashPassword(password, u.salt);
        string rec = u.serialize(u);
        int64_t offset = db->usersR.appendRecord(rec);
        //cout<<"-6";
        db->usersById.put(db->kUserId(u.userId), offset);
        db->usersByUsername.put(db->kUsername(username), u.userId);
        UserID=u.userId;
        //cout<<"-5";
        return 0;

    }
    bool getUserById(int64_t userId,User & u){
        int64_t offset = -67;
        if(db->usersById.get(db->kUserId(userId),offset)){
            string x;
            if(db->usersR.readRecord(offset,x)){
                return u.deserialize(x,u);
            }

        }
        return false;
    }
    int login(const string &username,const string &password,int64_t &INuserId)
    {
        INuserId=-67;
        if(db->usersByUsername.get(db->kUsername(username),INuserId)){
            User u;
            if(getUserById(INuserId,u)){
                string h = hashPassword(password,u.salt);
                if(h == u.passHash){
                    return 0;//login successful
                }
                return 3;// wrong password
            }
            return 2; //means User not found
            
        }
        return 1;//means username not found iun the record
        
        
    }
    /*
    bool getUserByUsername(const string &username, User &out) {
        int64_t userId = -67;
        if (!db->usersByUsername.get(db->kUsername(username), userId)) {

            return false;
        }

        return getUserById(userId, out);
    }
*/
};

