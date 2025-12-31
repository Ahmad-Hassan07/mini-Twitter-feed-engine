#include "services/PlotTwistService.h"
#include "core/KeyUtil.h"

PlotTwistService::PlotTwistService(DataStore *store) {
    db = store;
}

bool PlotTwistService::loadNode(int64_t nodeId, ThreadNode &out) {
    int64_t offset = -1;
    if (!db->threadNodeById.get(db->kThreadNodeId(nodeId), offset)) return false;
    string rec;
    if (!db->threadNodesR.readRecord(offset, rec)) return false;
    return ThreadNode::deserialize(rec, out);
}

bool PlotTwistService::loadMeta(int64_t rootId, ThreadMeta &out) {
    int64_t offset = -1;
    if (!db->threadMetaByRootId.get(db->kThreadMeta(rootId), offset)) return false;
    string rec;
    if (!db->threadMetaR.readRecord(offset, rec)) return false;
    return ThreadMeta::deserialize(rec, out);
}

void PlotTwistService::saveNodeAndIndex(const ThreadNode &n) {
    int64_t off = db->threadNodesR.appendRecord(ThreadNode::serialize(n));
    db->threadNodeById.put(db->kThreadNodeId(n.nodeId), off);

    if (n.parentNodeId != 0) {
        db->threadChildrenByParentTime.put(db->kThreadChild(n.parentNodeId, n.timestamp, n.nodeId), n.nodeId);
    }
}

void PlotTwistService::saveMetaAndIndex(const ThreadMeta &m) {
    // remove old popularity entry if exists
    ThreadMeta oldM;
    if (loadMeta(m.rootId, oldM)) {
        string oldKey = db->kThreadPopularity(oldM.score, oldM.lastUpdated, oldM.rootId);
        db->threadsByPopularity.erase(oldKey);
    }

    int64_t off = db->threadMetaR.appendRecord(ThreadMeta::serialize(m));
    db->threadMetaByRootId.put(db->kThreadMeta(m.rootId), off);

    string newKey = db->kThreadPopularity(m.score, m.lastUpdated, m.rootId);
    db->threadsByPopularity.put(newKey, m.rootId);
}

int64_t PlotTwistService::createThread(int64_t authorId, const string &sentence) {
    ThreadNode n;
    n.nodeId = db->ids.newThreadNodeId();
    n.rootId = n.nodeId;
    n.parentNodeId = 0;
    n.authorId = authorId;
    n.timestamp = nowUnixSeconds();
    n.sentenceText = sentence;
    n.likes = 0;
    n.reposts = 0;
    n.comments = 0;

    saveNodeAndIndex(n);

    ThreadMeta m;
    m.rootId = n.rootId;
    m.score = 1; // base score
    m.lastUpdated = n.timestamp;
    saveMetaAndIndex(m);

    return n.rootId;
}

int64_t PlotTwistService::continueThread(int64_t authorId, int64_t parentNodeId, const string &sentence) {
    ThreadNode parent;
    if (!loadNode(parentNodeId, parent)) return -1;

    ThreadNode child;
    child.nodeId = db->ids.newThreadNodeId();
    child.rootId = parent.rootId;
    child.parentNodeId = parentNodeId;
    child.authorId = authorId;
    child.timestamp = nowUnixSeconds();
    child.sentenceText = sentence;
    child.likes = 0;
    child.reposts = 0;
    child.comments = 0;

    saveNodeAndIndex(child);


    ThreadMeta m;
    if (!loadMeta(child.rootId, m)) {
        m.rootId = child.rootId;
        m.score = 1;
        m.lastUpdated = child.timestamp;
    }
    m.score += 1;
    m.lastUpdated = child.timestamp;

    saveMetaAndIndex(m);

    return child.nodeId;
}

vector<ThreadNode> PlotTwistService::childrenOf(int64_t parentNodeId, int limit) {
    vector<ThreadNode> out;

    string prefix = "TC:" + FixedInt(parentNodeId, 20) + ":";
    string end = nextLex(prefix);

    vector<BPlusTree::KV> rows = db->threadChildrenByParentTime.rangeScan(prefix, end, limit);

    for (int i = 0; i < (int)rows.size(); i++) {
        int64_t childId = rows[i].value;
        ThreadNode n;
        if (loadNode(childId, n)) out.push_back(n);
    }

    return out;
}

vector<int64_t> PlotTwistService::topThreads(int limit) {
    vector<int64_t> out;

    string start = "TP:";
    string end = nextLex(start);

    vector<BPlusTree::KV> rows = db->threadsByPopularity.rangeScan(start, end, limit);

    for (int i = 0; i < (int)rows.size(); i++) {
        out.push_back(rows[i].value); // rootId
    }

    return out;
}
