#pragma once
#include<fstream>
#include <string>
#include <cstdint>
using namespace std;
class IdGenerator{
    string path;

    int64_t nextUserId;
    int64_t nextPostId;
    int64_t nextCommentId;
    int64_t nextRequestId;
    int64_t nextThreadNodeId;
    void load();
    void save();
public:
    IdGenerator() {
        nextUserId = 1;
        nextPostId = 1;
        nextCommentId = 1;
        nextRequestId = 1;
        nextThreadNodeId = 1;
    }
    void IdGenerator::open(const std::string &filePath) {
        path = filePath;
        ifstream f(path,ios::binary);
        if (!f.good()) {
            nextUserId = 1;
            nextPostId = 1;
            nextCommentId = 1;
            nextRequestId = 1;
            nextThreadNodeId = 1;
            save();
        } else {
            f.close();
            load();
        }
    }

    void IdGenerator::load() {
        ifstream f(path,ios::binary);
        if (!f.good()) {
            return;
        }


        f.read((char*)&nextUserId, sizeof(nextUserId));
        f.read((char*)&nextPostId, sizeof(nextPostId));
        f.read((char*)&nextCommentId, sizeof(nextCommentId));
        f.read((char*)&nextRequestId, sizeof(nextRequestId));
        f.read((char*)&nextThreadNodeId, sizeof(nextThreadNodeId));

        if (!f.good()) {
            nextUserId = 1;
            nextPostId = 1;
            nextCommentId = 1;
            nextRequestId = 1;
            nextThreadNodeId = 1;
        }
        f.close();
    }
    void IdGenerator::save() {
        ofstream f(path, ios::binary);
        f.seekp(0);
        f.write((char*)&nextUserId, sizeof(nextUserId));
        f.write((char*)&nextPostId, sizeof(nextPostId));
        f.write((char*)&nextCommentId, sizeof(nextCommentId));
        f.write((char*)&nextRequestId, sizeof(nextRequestId));
        f.write((char*)&nextThreadNodeId, sizeof(nextThreadNodeId));
        f.close();
    }


    int64_t IdGenerator::newUserId(){
        int64_t id = nextUserId;
        nextUserId++;
        save();
        return id;
    }
    int64_t IdGenerator::newPostId() {
        int64_t id = nextPostId;
        nextPostId++;
        save();
        return id;
    }

    int64_t IdGenerator::newCommentId() {
        int64_t id = nextCommentId;
        nextCommentId++;
        save();
        return id;
    }

    int64_t IdGenerator::newRequestId() {
        int64_t id = nextRequestId;
        nextRequestId++;
        save();
        return id;
    }
    int64_t IdGenerator::newThreadNodeId() {
        int64_t id = nextThreadNodeId;
        nextThreadNodeId++;
        save();
        return id;
    }


};