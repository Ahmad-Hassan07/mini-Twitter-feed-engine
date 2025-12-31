#pragma once
#include<cstdint>
#include<chrono>
#include<string>
#include <vector>
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
vector<string> splitCsv(const string &csv) {
    vector<string> out;
    string c;
    for (int i = 0; i < (int)csv.size(); i++) {
        if (csv[i] == ',') {
            out.push_back(c);
            c.clear();

        } else {
            c.push_back(csv[i]);

        }
    }
    if (!c.empty() || !out.empty()) {
        
        out.push_back(c);
    }
    return out;
}

string joinCsv(const vector<string> &items) {
    string output;

    for (int i = 0; i < int(items.size()); i++) {
        if (i > 0) {
            output += ",";

        }
        output += items[i];

    }
    return output;


}

vector<string> parseHashtagsSimple(const string &text) {
    vector<string> tags;
    int n = (int)text.size();

    for (int i = 0; i < n; i++) {
        if (text[i] == '#') {
            string tag;
            i++;
            while (i < n) {

                char c = text[i];
                bool ok = false;
                if (c >= 'a' && c <= 'z') {
                    ok = true;
                }
                if (c >= 'A' && c <= 'Z'){
                    ok = true;
                } 
                if (c >= '0' && c <= '9') {
                    ok = true;
                }
                if (c == '_') {
                    ok = true;

                }

                if (!ok) {
                    break;

                }
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
string toLowerSimple2(string s) {
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] >= 'A' && s[i] <= 'Z') 
        s[i] = char(s[i] - 'A' + 'a');


    }
    return s;
}

/*








hi*/