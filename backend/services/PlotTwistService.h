#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "../core/DataStore.h"
#include "../models/ThreadNode.h"
#include "../models/ThreadMeta.h"

class PlotTwistService {
private:
    DataStore *db;

    bool loadNode(int64_t nodeId, ThreadNode &out);
    bool loadMeta(int64_t rootId, ThreadMeta &out);

    void saveNodeAndIndex(const ThreadNode &n);
    void saveMetaAndIndex(const ThreadMeta &m);

public:
    PlotTwistService(DataStore *store);

   int64_t createThread(int64_t authorId, const string &sentence);

    int64_t continueThread(int64_t authorId, int64_t parentNodeId, const string &sentence);

    vector<ThreadNode> childrenOf(int64_t parentNodeId, int limit);
    vector<int64_t> topThreads(int limit);
};
