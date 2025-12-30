#pragma once
#include<cstdint>
#include<chrono>
#include<string>
using namespace std;

int64_t nowUnixSeconds(){
    return chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();

}

string FixedInt(int64_t num,int width){
    string s= to_string(num);
    while(s.size()<width){
        s="0"+s;
    }
    return s;

}
string invTimeKey(int64_t t) {
    const int64_t Largest = 9999999999;
    int64_t inver = Largest - t;
    if (inver < 0) {
        inver = 0;

    }
    return FixedInt(inver, 10);
}


string invScoreKey(int t) {
    const int Largest = 999999999;
    int inver = Largest - t;
    if (inver < 0) {
        inver = 0;

    }
    return FixedInt(inver, 10);
}

string nextLex(const string &pre) {
    if (pre.empty()) {
        return "~";
    }
    string s = pre;
    int i = (int)s.size() - 1;
    while (i >= 0) {
        unsigned char c = (unsigned char)s[i];
        if (c < 126) {
            s[i] = char(c + 1);
            return s;
        }
        i--;
    }
    return pre + char(0);
}




/*
#include "core/KeyUtil.h"
#include <chrono>

int64_t nowUnixSecondsSimple() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}

std::string padInt(int64_t x, int width) {
    std::string s = std::to_string(x);
    while ((int)s.size() < width) s = "0" + s;
    return s;
}

std::string nextLex(const std::string &prefix) {
    if (prefix.empty()) return std::string(1, char(127));
    std::string s = prefix;
    int i = (int)s.size() - 1;
    while (i >= 0) {
        unsigned char c = (unsigned char)s[i];
        if (c < 126) {
            s[i] = char(c + 1);
            return s;
        }
        i--;
    }
    return prefix + char(0);
}

std::string joinCsv(const std::vector<std::string> &items) {
    std::string out;
    for (int i = 0; i < (int)items.size(); i++) {
        if (i > 0) out += ",";
        out += items[i];
    }
    return out;
}

std::vector<std::string> splitCsv(const std::string &csv) {
    std::vector<std::string> out;
    std::string cur;
    for (int i = 0; i < (int)csv.size(); i++) {
        if (csv[i] == ',') {
            out.push_back(cur);
            cur.clear();
        } else cur.push_back(csv[i]);
    }
    if (!cur.empty() || !out.empty()) out.push_back(cur);
    return out;
}

std::string toLowerSimple2(std::string s) {
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] = char(s[i] - 'A' + 'a');
    }
    return s;
}

std::vector<std::string> parseHashtagsSimple(const std::string &text) {
    std::vector<std::string> tags;
    int n = (int)text.size();

    for (int i = 0; i < n; i++) {
        if (text[i] == '#') {
            std::string tag;
            i++;
            while (i < n) {
                char c = text[i];
                bool ok = false;
                if (c >= 'a' && c <= 'z') ok = true;
                if (c >= 'A' && c <= 'Z') ok = true;
                if (c >= '0' && c <= '9') ok = true;
                if (c == '_') ok = true;

                if (!ok) break;
                tag.push_back(c);
                i++;
            }
            if (!tag.empty()) {
                tag = toLowerSimple2(tag);
                tags.push_back(tag);
            }
        }
    }
    return tags;
}

// descending sort helper: store inverted numbers as fixed-width strings
std::string invTimeKey(int64_t t) {
    const int64_t MAX_T = 9999999999LL;
    int64_t inv = MAX_T - t;
    if (inv < 0) inv = 0;
    return padInt(inv, 10);
}

std::string invScoreKey(int score) {
    const int MAX_S = 999999999;
    int inv = MAX_S - score;
    if (inv < 0) inv = 0;
    return padInt(inv, 10);
}
*/