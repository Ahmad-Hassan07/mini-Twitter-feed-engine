#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include <cstdint>
using namespace std;
struct Post {
    int id = 0;
    string userId;
    int64_t timestamp = 0;
    int likes = 0;
    int comments = 0;
    double score = 0.0;
    string text;
};

struct User {
    string id;
    string name;
    vector<string> following;
};

#endif
