#pragma once
#include <string>
#include <vector>
#include <cstdint>

class BPlusTree {
public:
    static const int MAX_KEYS = 16;   // keep small for easy code
    static const int KEY_SIZE = 80;   // fixed key bytes
    static const int64_t NIL = -1;

    struct KV {
        std::string key;
        int64_t value;
    };

    class Iterator {
    private:
        BPlusTree *tree;
        std::string endKey;
        int64_t nodeId;
        int index;
        bool valid;

    public:
        Iterator();
        Iterator(BPlusTree *t, const std::string &startKey, const std::string &endKeyExclusive);

        bool hasNext();
        KV next();
    };

private:
#pragma pack(push, 1)
    struct Header {
        int64_t rootId;
        int64_t nextNodeId;
    };

    struct Node {
        char isLeaf;                  // 1 leaf, 0 internal
        int32_t keyCount;
        char keys[MAX_KEYS][KEY_SIZE];
        int64_t values[MAX_KEYS];     // leaf values
        int64_t children[MAX_KEYS + 1]; // internal pointers
        int64_t nextLeaf;             // leaf linked list
    };
#pragma pack(pop)

    std::string path;
    Header header;

    void initNewTree();
    int64_t allocNode();
    void readHeader();
    void writeHeader();
    void readNode(int64_t id, Node &out);
    void writeNode(int64_t id, const Node &node);

    void setKey(char dest[KEY_SIZE], const std::string &s);
    std::string getKey(const char src[KEY_SIZE]);
    int compareKey(const std::string &a, const std::string &b);

    // insert recursion
    bool insertRec(int64_t nodeId, const std::string &key, int64_t value,
                   std::string &splitKey, int64_t &newRightNodeId);

public:
    BPlusTree();
    BPlusTree(const std::string &filePath);

    void open(const std::string &filePath);

    bool get(const std::string &key, int64_t &outValue);

    // insert or update
    void put(const std::string &key, int64_t value);

    // lazy delete: set value = -1 if leaf key exists
    void erase(const std::string &key);

    // range scan: returns up to limit results with startKey <= key < endKey
    std::vector<KV> rangeScan(const std::string &startKey, const std::string &endKeyExclusive, int limit);

    Iterator iterator(const std::string &startKey, const std::string &endKeyExclusive);
};
