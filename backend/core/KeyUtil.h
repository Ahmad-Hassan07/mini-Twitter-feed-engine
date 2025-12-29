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

