#include "BPlusTree.h"
#include <fstream>
#include <cstring>

BPlusTree::Iterator::Iterator() {
    tree = nullptr;
    nodeId = BPlusTree::NIL;
    index = 0;
    valid = false;
}

BPlusTree::Iterator::Iterator(BPlusTree *t, const std::string &startKey, const std::string &endKeyExclusive) {
    tree = t;
    endKey = endKeyExclusive;
    nodeId = BPlusTree::NIL;
    index = 0;
    valid = false;

    // find first leaf position >= startKey
    BPlusTree::Node cur;
    int64_t curId = tree->header.rootId;

    while (true) {
        tree->readNode(curId, cur);
        if (cur.isLeaf == 1) {
   
            int pos = 0;
            while (pos < cur.keyCount) {
                std::string k = tree->getKey(cur.keys[pos]);
                if (tree->compareKey(k, startKey) >= 0) break;
                pos++;
            }
            nodeId = curId;
            index = pos;
            valid = true;
            return;
        } else {
            // internal: choose child
            int i = 0;
            while (i < cur.keyCount) {
                std::string k = tree->getKey(cur.keys[i]);
                if (tree->compareKey(startKey, k) < 0) break;
                i++;
            }
            curId = cur.children[i];
        }
    }
}

bool BPlusTree::Iterator::hasNext() {
    if (!valid) return false;
    if (tree == nullptr) return false;
    if (nodeId == BPlusTree::NIL) return false;

    BPlusTree::Node n;
    tree->readNode(nodeId, n);

    // move until we either find a live value or hit endKey
    int64_t curNodeId = nodeId;
    int curIndex = index;

    while (true) {
        tree->readNode(curNodeId, n);

        while (curIndex < n.keyCount) {
            std::string k = tree->getKey(n.keys[curIndex]);
            if (tree->compareKey(k, endKey) >= 0) {
                return false;
            }
            if (n.values[curIndex] != BPlusTree::NIL) {
                return true;
            }
            curIndex++;
        }

        if (n.nextLeaf == BPlusTree::NIL) return false;
        curNodeId = n.nextLeaf;
        curIndex = 0;
    }
}

BPlusTree::KV BPlusTree::Iterator::next() {
    KV out;
    out.key = "";
    out.value = BPlusTree::NIL;

    if (!hasNext()) {
        valid = false;
        return out;
    }

    BPlusTree::Node n;
    while (true) {
        tree->readNode(nodeId, n);

        while (index < n.keyCount) {
            std::string k = tree->getKey(n.keys[index]);
            if (tree->compareKey(k, endKey) >= 0) {
                valid = false;
                return out;
            }

            int64_t v = n.values[index];
            index++;

            if (v != BPlusTree::NIL) {
                out.key = k;
                out.value = v;
                return out;
            }
        }

        if (n.nextLeaf == BPlusTree::NIL) {
            valid = false;
            return out;
        }

        nodeId = n.nextLeaf;
        index = 0;
    }
}

// ------------------------------------------------------------

BPlusTree::BPlusTree() {
    header.rootId = 0;
    header.nextNodeId = 0;
}

BPlusTree::BPlusTree(const std::string &filePath) {
    open(filePath);
}

void BPlusTree::open(const std::string &filePath) {
    path = filePath;

    std::fstream f(path, std::ios::in | std::ios::binary);
    if (!f.good()) {
        std::fstream g(path, std::ios::out | std::ios::binary);
        g.close();
        initNewTree();
    } else {
        f.close();
        readHeader();
        // if file was empty somehow
        if (header.nextNodeId == 0) {
            initNewTree();
        }
    }
}

void BPlusTree::initNewTree() {
    header.rootId = 0;
    header.nextNodeId = 0;

    // create root leaf node
    int64_t root = allocNode();
    Node n;
    std::memset(&n, 0, sizeof(n));
    n.isLeaf = 1;
    n.keyCount = 0;
    n.nextLeaf = NIL;
    writeNode(root, n);

    header.rootId = root;
    writeHeader();
}

int64_t BPlusTree::allocNode() {
    int64_t id = header.nextNodeId;
    header.nextNodeId++;
    writeHeader();
    return id;
}

void BPlusTree::readHeader() {
    std::fstream f(path, std::ios::in | std::ios::binary);
    f.seekg(0, std::ios::beg);
    f.read((char*)&header, sizeof(header));
    f.close();
}

void BPlusTree::writeHeader() {
    std::fstream f(path, std::ios::in | std::ios::out | std::ios::binary);
    f.seekp(0, std::ios::beg);
    f.write((char*)&header, sizeof(header));
    f.close();
}

void BPlusTree::readNode(int64_t id, Node &out) {
    std::fstream f(path, std::ios::in | std::ios::binary);
    int64_t offset = (int64_t)sizeof(Header) + id * (int64_t)sizeof(Node);
    f.seekg(offset, std::ios::beg);
    f.read((char*)&out, sizeof(Node));
    f.close();
}

void BPlusTree::writeNode(int64_t id, const Node &node) {
    std::fstream f(path, std::ios::in | std::ios::out | std::ios::binary);
    int64_t offset = (int64_t)sizeof(Header) + id * (int64_t)sizeof(Node);
    f.seekp(offset, std::ios::beg);
    f.write((char*)&node, sizeof(Node));
    f.close();
}

void BPlusTree::setKey(char dest[KEY_SIZE], const std::string &s) {
    std::memset(dest, 0, KEY_SIZE);
    int n = (int)s.size();
    if (n >= KEY_SIZE) n = KEY_SIZE - 1;
    for (int i = 0; i < n; i++) dest[i] = s[i];
    dest[n] = 0;
}

std::string BPlusTree::getKey(const char src[KEY_SIZE]) {
    std::string s;
    for (int i = 0; i < KEY_SIZE; i++) {
        if (src[i] == 0) break;
        s.push_back(src[i]);
    }
    return s;
}

int BPlusTree::compareKey(const std::string &a, const std::string &b) {
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

bool BPlusTree::get(const std::string &key, int64_t &outValue) {
    Node cur;
    int64_t curId = header.rootId;

    while (true) {
        readNode(curId, cur);

        if (cur.isLeaf == 1) {
            for (int i = 0; i < cur.keyCount; i++) {
                std::string k = getKey(cur.keys[i]);
                if (k == key) {
                    if (cur.values[i] == NIL) return false;
                    outValue = cur.values[i];
                    return true;
                }
            }
            return false;
        }

        int idx = 0;
        while (idx < cur.keyCount) {
            std::string k = getKey(cur.keys[idx]);
            if (compareKey(key, k) < 0) break;
            idx++;
        }
        curId = cur.children[idx];
    }
}

void BPlusTree::put(const std::string &key, int64_t value) {
    std::string splitKey;
    int64_t newRight = NIL;

    bool split = insertRec(header.rootId, key, value, splitKey, newRight);

    if (split) {
        // new root needed
        int64_t newRoot = allocNode();
        Node r;
        std::memset(&r, 0, sizeof(r));
        r.isLeaf = 0;
        r.keyCount = 1;

        setKey(r.keys[0], splitKey);
        r.children[0] = header.rootId;
        r.children[1] = newRight;

        writeNode(newRoot, r);
        header.rootId = newRoot;
        writeHeader();
    }
}

bool BPlusTree::insertRec(int64_t nodeId, const std::string &key, int64_t value,
                          std::string &splitKey, int64_t &newRightNodeId) {
    Node node;
    readNode(nodeId, node);

    if (node.isLeaf == 1) {
        // update if exists
        for (int i = 0; i < node.keyCount; i++) {
            std::string k = getKey(node.keys[i]);
            if (k == key) {
                node.values[i] = value;
                writeNode(nodeId, node);
                return false;
            }
        }

        // insert sorted
        int pos = 0;
        while (pos < node.keyCount) {
            std::string k = getKey(node.keys[pos]);
            if (compareKey(key, k) < 0) break;
            pos++;
        }

        // shift right
        for (int i = node.keyCount; i > pos; i--) {
            std::memcpy(node.keys[i], node.keys[i - 1], KEY_SIZE);
            node.values[i] = node.values[i - 1];
        }

        setKey(node.keys[pos], key);
        node.values[pos] = value;
        node.keyCount++;

        if (node.keyCount <= MAX_KEYS) {
            writeNode(nodeId, node);
            return false;
        }

        // split leaf
        int mid = node.keyCount / 2;
        int64_t rightId = allocNode();
        Node right;
        std::memset(&right, 0, sizeof(right));
        right.isLeaf = 1;
        right.keyCount = 0;

        for (int i = mid; i < node.keyCount; i++) {
            std::memcpy(right.keys[right.keyCount], node.keys[i], KEY_SIZE);
            right.values[right.keyCount] = node.values[i];
            right.keyCount++;
        }

        node.keyCount = mid;

        right.nextLeaf = node.nextLeaf;
        node.nextLeaf = rightId;

        writeNode(nodeId, node);
        writeNode(rightId, right);

        splitKey = getKey(right.keys[0]);
        newRightNodeId = rightId;
        return true;
    }
    int childIndex = 0;
    while (childIndex < node.keyCount) {
        std::string k = getKey(node.keys[childIndex]);
        if (compareKey(key, k) < 0) break;
        childIndex++;
    }

    std::string childSplitKey;
    int64_t childNewRight = NIL;

    bool childSplit = insertRec(node.children[childIndex], key, value, childSplitKey, childNewRight);

    if (!childSplit) {
        return false;
    }

    // insert promoted key into this node
    int insertPos = childIndex;

    for (int i = node.keyCount; i > insertPos; i--) {
        std::memcpy(node.keys[i], node.keys[i - 1], KEY_SIZE);
    }
    for (int i = node.keyCount + 1; i > insertPos + 1; i--) {
        node.children[i] = node.children[i - 1];
    }

    setKey(node.keys[insertPos], childSplitKey);
    node.children[insertPos + 1] = childNewRight;
    node.keyCount++;

    if (node.keyCount <= MAX_KEYS) {
        writeNode(nodeId, node);
        return false;
    }

    // split internal
    int mid = node.keyCount / 2;
    std::string promote = getKey(node.keys[mid]);

    int64_t rightId = allocNode();
    Node right;
    std::memset(&right, 0, sizeof(right));
    right.isLeaf = 0;
    right.keyCount = 0;

    // keys after mid go to right
    int j = 0;
    for (int i = mid + 1; i < node.keyCount; i++) {
        std::memcpy(right.keys[j], node.keys[i], KEY_SIZE);
        j++;
        right.keyCount++;
    }

    // children after mid go to right
    int childJ = 0;
    for (int i = mid + 1; i < node.keyCount + 1; i++) {
        right.children[childJ] = node.children[i];
        childJ++;
    }

    node.keyCount = mid;

    writeNode(nodeId, node);
    writeNode(rightId, right);

    splitKey = promote;
    newRightNodeId = rightId;
    return true;
}

void BPlusTree::erase(const std::string &key) {
    Node cur;
    int64_t curId = header.rootId;

    while (true) {
        readNode(curId, cur);

        if (cur.isLeaf == 1) {
            for (int i = 0; i < cur.keyCount; i++) {
                std::string k = getKey(cur.keys[i]);
                if (k == key) {
                    cur.values[i] = NIL; // lazy delete
                    writeNode(curId, cur);
                    return;
                }
            }
            return;
        }

        int idx = 0;
        while (idx < cur.keyCount) {
            std::string k = getKey(cur.keys[idx]);
            if (compareKey(key, k) < 0) break;
            idx++;
        }
        curId = cur.children[idx];
    }
}

std::vector<BPlusTree::KV> BPlusTree::rangeScan(const std::string &startKey,
                                                const std::string &endKeyExclusive,
                                                int limit) {
    std::vector<KV> out;

    Iterator it(this, startKey, endKeyExclusive);
    while (it.hasNext()) {
        KV kv = it.next();
        if (kv.value != NIL) {
            out.push_back(kv);
            if (limit > 0 && (int)out.size() >= limit) break;
        }
    }
    return out;
}

BPlusTree::Iterator BPlusTree::iterator(const std::string &startKey, const std::string &endKeyExclusive) {
    return Iterator(this, startKey, endKeyExclusive);
}