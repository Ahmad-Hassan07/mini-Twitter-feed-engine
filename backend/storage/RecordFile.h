#pragma once
#include<string>
#include<cstdint>
using namespace std;

class RecordFile{
    string path;
public:
    RecordFile();
    RecordFile(const string& filePath);
    void open(const string& f);
    int64_t appendRecord(const string &data);
    bool readRecord(int64_t offset,string &output);
};