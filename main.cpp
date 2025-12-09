#include <iostream>
#include <string>
#include "engine.h"

using namespace std;

void showMenu(const string& currentUser) {
    cout << "\n========== Social App ==========\n";
    if (currentUser.empty()) cout << "Current user: [none]\n";
    else cout << "Current user: " << currentUser << "\n";
    cout << "1. Create user\n";
    cout << "2. Login\n";
    cout << "3. Follow user\n";
    cout << "4. Create post\n";
    cout << "5. Like post\n";
    cout << "6. View my feed\n";
    cout << "0. Exit\n";
    cout << "Choose: ";
}

int main() {
    SocialEngine app;
    string currentUser;

    while (true) {
        showMenu(currentUser);
        int choice;
        if (!(cin >> choice)) break;
        cin.ignore();

        if (choice == 0) {
            cout << "Goodbye\n";
            break;
        } else if (choice == 1) {
            string id, name;
            cout << "Enter new user ID: ";
            getline(cin, id);
            cout << "Enter name: ";
            getline(cin, name);
            if (app.createUser(id, name)) cout << "User created\n";
            else cout << "User ID already exists\n";
        } else if (choice == 2) {
            string id;
            cout << "Enter user ID: ";
            getline(cin, id);
            if (app.userExists(id)) {
                currentUser = id;
                cout << "Logged in as " << id << "\n";
            } else {
                cout << "No such user\n";
            }
        } else if (choice == 3) {
            if (currentUser.empty()) {
                cout << "Login first\n";
                continue;
            }
            string target;
            cout << "Enter user ID to follow: ";
            getline(cin, target);
            if (app.followUser(currentUser, target)) cout << "Now following " << target << "\n";
            else cout << "Follow failed\n";
        } else if (choice == 4) {
            if (currentUser.empty()) {
                cout << "Login first\n";
                continue;
            }
            string text;
            cout << "Enter post text: ";
            getline(cin, text);
            int id = app.createPost(currentUser, text);
            if (id == -1) cout << "Error creating post\n";
            else cout << "Post created with ID " << id << "\n";
        } else if (choice == 5) {
            int pid;
            cout << "Enter post ID: ";
            cin >> pid;
            cin.ignore();
            if (app.likePost(pid)) cout << "Post liked\n";
            else cout << "No such post\n";
        } else if (choice == 6) {
            if (currentUser.empty()) {
                cout << "Login first\n";
                continue;
            }
            auto feed = app.getPersonalFeed(currentUser, 50);
            cout << "\n----- Feed for " << currentUser << " -----\n";
            if (feed.empty()) cout << "No posts\n";
            else {
                for (auto& p : feed) {
                    cout << "Post ID: " << p.id
                         << " | User: " << p.userId
                         << " | Likes: " << p.likes
                         << " | Score: " << p.score << "\n";
                    cout << "Text: " << p.text << "\n\n";
                }
            }
        } else {
            cout << "Invalid choice\n";
        }
    }

    return 0;
}
