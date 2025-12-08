#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include <cstdint>

struct Post {
    int id;             
    std::string userId;
    std::int64_t timestamp;
    int likes = 0;
    int comments = 0;
    double score = 0.0;
    std::string text;
};

struct User {
    std::string id;
    std::string name; 
    std::vector<std::string> following; 
};

#endif
