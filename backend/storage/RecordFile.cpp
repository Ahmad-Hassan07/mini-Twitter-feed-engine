#include"RecordFile.h"
#include<fstream>

RecordFile::RecordFile(){

}
RecordFile::RecordFile(const string& filePath){
    open(filePath);
}
void RecordFile::open(const string& filePath){
    path=filePath;
    fstream rw(path,ios::in | ios::binary);
    if(!rw.good()){
        ofstream w(path,ios::binary);
        w.close();
    }
    else{
        rw.close();
    }

}

int64_t RecordFile::appendRecord(const string &data){
    fstream f(path, ios::in | ios::out | ios:: binary);
    f.seekp(0,std::ios::end);
    int64_t offset = (int64_t)f.tellp();
    int32_t len= (int32_t)data.size();

    f.write((char*)&len,sizeof(len));
    if(len>0){
        f.write(data.data(),len);

    }
    f.close();
    return offset;
}
bool RecordFile::readRecord(int64_t offset,string &output){
    ifstream r(path,ios::binary);
    if(!r.good()){
        return false;

    }
    r.seekg(offset,ios::beg);
    int32_t len=0;
    r.read((char*)&len,sizeof(len));
    if (!r.good()||len<0||len>10*1000*1000) {
        r.close();
        return false;
    }
    output.clear();
    if(len>0){
        output.resize(len);
        r.read(output.data(),len);

    }

    if (!r.good()) {
        r.close();
        return false;
    }
    r.close();
    return true;

}