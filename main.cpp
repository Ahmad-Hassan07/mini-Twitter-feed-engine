#include <iostream>
#include "backend/storage/RecordFile.h"   // change to "storage/RecordFile.h" if your header is inside storage/

using namespace std;

int main() {
    cout << "=== RecordFile Test ===\n";

    // 1) Create and open file
    RecordFile rf;
    rf.open("test_records.rec");   // this file will be created if not present

    // 2) Append some records
    string a = "Hello world!";
    string b = "This is record number 2";
    string c = ""; // empty record test
    string d = "Last record with #hashtag and more text";

    int64_t offA = rf.appendRecord(a);
    int64_t offB = rf.appendRecord(b);
    int64_t offC = rf.appendRecord(c);
    int64_t offD = rf.appendRecord(d);

    cout << "Offsets returned:\n";
    cout << "A offset = " << offA << "\n";
    cout << "B offset = " << offB << "\n";
    cout << "C offset = " << offC << "\n";
    cout << "D offset = " << offD << "\n\n";

    // 3) Read them back
    string out;

    cout << "Reading A...\n";
    if (rf.readRecord(offA, out)) cout << "A = [" << out << "]\n";
    else cout << "Failed to read A\n";

    cout << "Reading B...\n";
    if (rf.readRecord(offB, out)) cout << "B = [" << out << "]\n";
    else cout << "Failed to read B\n";

    cout << "Reading C (empty record)...\n";
    if (rf.readRecord(offC, out)) cout << "C = [" << out << "] (size=" << out.size() << ")\n";
    else cout << "Failed to read C\n";

    cout << "Reading D...\n";
    if (rf.readRecord(offD, out)) cout << "D = [" << out << "]\n";
    else cout << "Failed to read D\n";

    // 4) Try reading from a wrong offset (should fail)
    cout << "\nReading from invalid offset 999999...\n";
    if (rf.readRecord(999999, out)) cout << "Invalid offset read = [" << out << "]\n";
    else cout << "Correct: invalid offset failed.\n";

    cout << "\n=== Done ===\n";
    return 0;
}
