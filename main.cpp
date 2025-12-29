// main.cpp
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

// storage
#include "backend/storage/RecordFile.h"
#include "backend/storage/IdGenerator.h"
#include "backend/storage/BPlusTree.h"

// core
#include "backend/core/DataStore.h"
#include "backend/core/KeyUtil.h"

// models
#include "backend/models/User.h"

using namespace std;

static void testRecordFile() {
    cout << "\n=== TEST 1: RecordFile ===\n";
    RecordFile rf;
    rf.open("data/test_records.rec");

    int64_t offA = rf.appendRecord("Hello world!");
    int64_t offB = rf.appendRecord("This is record number 2");
    int64_t offC = rf.appendRecord(""); // empty record
    int64_t offD = rf.appendRecord("Last record with #hashtag and more text");

    cout << "Offsets returned:\n";
    cout << "A offset = " << offA << "\n";
    cout << "B offset = " << offB << "\n";
    cout << "C offset = " << offC << "\n";
    cout << "D offset = " << offD << "\n";

    string out;
    cout << "\nReading A...\n";
    if (rf.readRecord(offA, out)) cout << "A = [" << out << "]\n";
    else cout << "Failed reading A\n";

    cout << "Reading B...\n";
    if (rf.readRecord(offB, out)) cout << "B = [" << out << "]\n";
    else cout << "Failed reading B\n";

    cout << "Reading C...\n";
    if (rf.readRecord(offC, out)) cout << "C = [" << out << "] (size=" << out.size() << ")\n";
    else cout << "Failed reading C\n";

    cout << "Reading D...\n";
    if (rf.readRecord(offD, out)) cout << "D = [" << out << "]\n";
    else cout << "Failed reading D\n";

    cout << "\nReading from invalid offset 999999...\n";
    if (!rf.readRecord(999999, out)) cout << "Correct: invalid offset failed.\n";
    else cout << "Unexpected: read worked at invalid offset!\n";
}

static void testIdGenerator() {
    cout << "\n=== TEST 2: IdGenerator ===\n";

    // First run: create + generate ids
    {
        IdGenerator ids;
        ids.open("data/ids_test.bin");

        cout << "Generated IDs (run 1):\n";
        cout << "userId    = " << ids.newUserId() << "\n";
        cout << "postId    = " << ids.newPostId() << "\n";
        cout << "commentId = " << ids.newCommentId() << "\n";
        cout << "requestId = " << ids.newRequestId() << "\n";
        cout << "threadId  = " << ids.newThreadNodeId() << "\n";
    }

    // Second run: reopen and see it continues (persistence check)
    {
        IdGenerator ids;
        ids.open("data/ids_test.bin");

        cout << "\nGenerated IDs (run 2, after reopen):\n";
        cout << "userId    = " << ids.newUserId() << "\n";
        cout << "postId    = " << ids.newPostId() << "\n";
        cout << "commentId = " << ids.newCommentId() << "\n";
        cout << "requestId = " << ids.newRequestId() << "\n";
        cout << "threadId  = " << ids.newThreadNodeId() << "\n";
    }
}

static void testBPlusTree() {
    cout << "\n=== TEST 3: BPlusTree ===\n";

    BPlusTree t;
    t.open("data/test_tree.bpt");

    // put
    t.put("A:001", 111);
    t.put("A:002", 222);
    t.put("B:005", 555);
    t.put("C:010", 1010);

    // update existing
    t.put("A:002", 999);

    // get
    int64_t v = -1;
    cout << "Get A:001 -> ";
    if (t.get("A:001", v)) cout << v << "\n";
    else cout << "NOT FOUND\n";

    cout << "Get A:002 -> ";
    if (t.get("A:002", v)) cout << v << " (should be 999)\n";
    else cout << "NOT FOUND\n";

    // erase
    t.erase("B:005");
    cout << "After erase B:005 -> ";
    if (t.get("B:005", v)) cout << v << " (unexpected)\n";
    else cout << "NOT FOUND (correct)\n";

    // rangeScan
    cout << "\nRangeScan [A:, C:) limit 100:\n";
    vector<BPlusTree::KV> rows = t.rangeScan("A:", "C:", 100);
    for (int i = 0; i < (int)rows.size(); i++) {
        cout << rows[i].key << " -> " << rows[i].value << "\n";
    }

    // iterator
    cout << "\nIterator [A:, D:):\n";
    auto it = t.iterator("A:", "D:");
    while (it.hasNext()) {
        auto kv = it.next();
        cout << kv.key << " -> " << kv.value << "\n";
    }
}

static void testDataStoreBasic() {
    cout << "\n=== TEST 4: DataStore basic (User store + lookup + prefix search) ===\n";

    DataStore db;
    db.openAll("data");

    // Create a dummy user and store it in usersRF + usersById + usersByUsername
    User u;
    u.userId = db.ids.newUserId();
    u.username = "ahmad";
    u.displayName = "Ahmad";
    u.bio = "DS Project Test User";
    u.isPrivate = false;
    u.salt = "salt1234";
    u.passHash = "hash1234";
    u.createdAt = nowUnixSeconds();

    string rec = User::serialize(u);
    int64_t off = db.usersR.appendRecord(rec);

    db.usersById.put(db.kUserId(u.userId), off);
    db.usersByUsername.put(db.kUsername(u.username), u.userId);

    cout << "Saved userId=" << u.userId << " username=" << u.username << " offset=" << off << "\n";

    // Lookup by ID -> offset -> record
    int64_t gotOff = -1;
    if (db.usersById.get(db.kUserId(u.userId), gotOff)) {
        string gotRec;
        if (db.usersR.readRecord(gotOff, gotRec)) {
            User u2;
            if (User::deserialize(gotRec, u2)) {
                cout << "Loaded by ID:\n";
                cout << "  userId=" << u2.userId << "\n";
                cout << "  username=" << u2.username << "\n";
                cout << "  displayName=" << u2.displayName << "\n";
                cout << "  bio=" << u2.bio << "\n";
            } else {
                cout << "Deserialize failed.\n";
            }
        } else {
            cout << "ReadRecord failed.\n";
        }
    } else {
        cout << "usersById lookup failed.\n";
    }

    // Prefix search on usernames: "a"
    cout << "\nUsername prefix search for 'a':\n";
    string start = "UN:" + string("a");
    string end = nextLex(start);

    vector<BPlusTree::KV> urows = db.usersByUsername.rangeScan(start, end, 20);
    for (int i = 0; i < (int)urows.size(); i++) {
        cout << "  key=" << urows[i].key << " -> userId=" << urows[i].value << "\n";
    }
}

int main() {
    // Create data directory (works on Windows/Linux)
    std::filesystem::create_directories("data");

    cout << "=== SIMPLE BACKEND STORAGE TESTS ===\n";

    testRecordFile();
    testIdGenerator();
    testBPlusTree();
    testDataStoreBasic();

    cout << "\n=== DONE ===\n";
    return 0;
}
