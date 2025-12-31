#include <iostream>
#include <string>
#include <vector>

#include "backend/core/DataStore.h"
#include "backend/services/AuthService.h"
#include "backend/services/UserService.h"
#include "backend/services/PostService.cpp"
#include "backend/services/FeedService.h"
#include "backend/services/SearchService.cpp"
#include "backend/services/PlotTwistService.cpp"

using namespace std;

int main() {
    DataStore db;
    db.openAll("data");

    AuthService auth(&db);
    UserService users(&db);
    PostService posts(&db);
    FeedService feeds(&db);
    SearchService search(&db);
    PlotTwistService plot(&db);

    int64_t currentUser = -1;

    while (true) {
        cout << "\n=== MENU ===\n";
        cout << "1) Signup\n";
        cout << "2) Login\n";
        cout << "3) Create post\n";
        cout << "4) Follow user\n";
        cout << "5) View home feed\n";
        cout << "6) Explore feed\n";
        cout << "7) Search users prefix\n";
        cout << "8) Search hashtag\n";
        cout << "9) Like toggle\n";
        cout << "10) Comment\n";
        cout << "11) Incoming follow requests\n";
        cout << "12) Accept request\n";
        cout << "13) PlotTwist: create thread\n";
        cout << "14) PlotTwist: continue node\n";
        cout << "15) PlotTwist: top threads\n";
        cout << "0) Exit\n";
        cout << "Choice: ";

        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 0) break;

        if (choice == 1) {
            string u, p, dn, age, bio;
            int priv;
            cout << "username: "; getline(cin, u);
            cout << "password: "; getline(cin, p);
            cout << "displayName: "; getline(cin, dn);
            cout<<"age: ";getline(cin,age);
            cout << "bio: "; getline(cin, bio);
            cout << "isPrivate (1/0): "; cin >> priv; cin.ignore();

            int64_t uid;
            bool ok = !(auth.signup(u, p, dn,stoi(age), bio, (priv==1), uid));
            if (ok) cout << "Signup ok, userId=" << uid << "\n";
            else cout << "Signup failed (username exists)\n";
        }

        if (choice == 2) {
            string u, p;
            cout << "username: "; getline(cin, u);
            cout << "password: "; getline(cin, p);

            int64_t uid;
            bool ok = auth.login(u, p, uid);
            if (!ok) {
                currentUser = uid;
                cout << "Login ok, userId=" << currentUser << "\n";
            } else {
                cout << "Login failed\n";
            }
        }

        if (choice == 3) {
            if (currentUser < 0) { cout << "Login first\n"; continue; }
            User me;
            auth.getUserById(currentUser, me);

            string text;
            cout << "text: "; getline(cin, text);

            int64_t postId = posts.createPost(currentUser, me.isPrivate, text);
            cout << "Posted, postId=" << postId << "\n";
        }

        if (choice == 4) {
            if (currentUser < 0) { cout << "Login first\n"; continue; }

            string username;
            cout << "follow username: "; getline(cin, username);

            User target;
            if (!auth.getUserByUsername(username, target)) {
                cout << "User not found\n";
                continue;
            }

            bool direct = users.follow(currentUser, target.userId, target.isPrivate);
            if (direct) cout << "Followed successfully\n";
            else cout << "Follow request sent (private)\n";
        }

        if (choice == 5) {
            if (currentUser < 0) { cout << "Login first\n"; continue; }
            vector<Post> f = feeds.homeFeed(currentUser, 10);
            cout << "HOME FEED:\n";
            for (int i = 0; i < (int)f.size(); i++) {
                cout << "PostId=" << f[i].postId << " author=" << f[i].authorId
                     << " likes=" << f[i].likes << " text=" << f[i].text << "\n";
            }
        }

        if (choice == 6) {
            vector<Post> e = feeds.exploreFeed(10);
            cout << "EXPLORE:\n";
            for (int i = 0; i < (int)e.size(); i++) {
                cout << "PostId=" << e[i].postId << " author=" << e[i].authorId
                     << " score=" << Post::engagementScore(e[i]) << " text=" << e[i].text << "\n";
            }
        }

        if (choice == 7) {
            string pref;
            cout << "prefix: "; getline(cin, pref);
            vector<User> res = search.searchUsersPrefix(pref, 10);
            for (int i = 0; i < (int)res.size(); i++) {
                cout << res[i].username << " (id=" << res[i].userId << ")\n";
            }
        }

        if (choice == 8) {
            string tag;
            cout << "hashtag (without #): "; getline(cin, tag);
            vector<Post> res = search.searchHashtag(tag, 10);
            for (int i = 0; i < (int)res.size(); i++) {
                cout << "PostId=" << res[i].postId << " score=" << Post::engagementScore(res[i])
                     << " text=" << res[i].text << "\n";
            }
        }

        if (choice == 9) {
            if (currentUser < 0) { cout << "Login first\n"; continue; }
            int64_t postId;
            cout << "postId: "; cin >> postId; cin.ignore();
            bool ok = posts.likeToggle(currentUser, postId);
            if (ok) cout << "toggled like\n";
            else cout << "failed\n";
        }

        if (choice == 10) {
            if (currentUser < 0) { cout << "Login first\n"; continue; }
            int64_t postId;
            cout << "postId: "; cin >> postId; cin.ignore();
            string text;
            cout << "comment: "; getline(cin, text);
            int64_t cid = posts.addComment(currentUser, postId, text);
            cout << "commentId=" << cid << "\n";
        }

        if (choice == 11) {
            if (currentUser < 0) { cout << "Login first\n"; continue; }
            vector<FollowRequest> inc = users.incomingRequests(currentUser, 20);
            for (int i = 0; i < (int)inc.size(); i++) {
                cout << "reqId=" << inc[i].requestId << " from=" << inc[i].fromUserId
                     << " status=" << inc[i].status << "\n";
            }
        }

        if (choice == 12) {
            if (currentUser < 0) { cout << "Login first\n"; continue; }
            int64_t reqId;
            cout << "requestId: "; cin >> reqId; cin.ignore();
            bool ok = users.acceptRequest(currentUser, reqId);
            cout << (ok ? "accepted\n" : "not found\n");
        }

        if (choice == 13) {
            if (currentUser < 0) { cout << "Login first\n"; continue; }
            string s;
            cout << "Root sentence: "; getline(cin, s);
            int64_t rootId = plot.createThread(currentUser, s);
            cout << "Created thread rootId=" << rootId << "\n";
        }

        if (choice == 14) {
            if (currentUser < 0) { cout << "Login first\n"; continue; }
            int64_t parent;
            cout << "parent nodeId: "; cin >> parent; cin.ignore();
            string s;
            cout << "continuation sentence: "; getline(cin, s);
            int64_t child = plot.continueThread(currentUser, parent, s);
            cout << "child nodeId=" << child << "\n";
        }

        if (choice == 15) {
            vector<int64_t> roots = plot.topThreads(10);
            cout << "TOP THREAD ROOTS:\n";
            for (int i = 0; i < (int)roots.size(); i++) {
                cout << "rootId=" << roots[i] << "\n";
            }
        }
    }

    return 0;
}
