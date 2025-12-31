#include <iostream>
#include <string>

// include your backend headers
#include "backend/core/DataStore.h"
#include "backend/services/AuthService.h"
#include "backend/models/User.h"

int main() {
    std::cout << "=== AUTH SERVICE TEST ===\n";

    DataStore db;

    // If you run from Final_Project folder, this creates/uses backend/data/
    db.openAll("backend/data");

    AuthService auth(&db);

    int64_t newUserId = -1;

    std::cout << "\n--- Signup ---\n";
    int code = auth.signup(
        "ahmad",
        "pass123",
        "Ahmad",
        18,
        "Hello, this is my bio",
        false,
        newUserId
    );

    std::cout << "signup return code = " << code << "\n";
    if (code == 0) {
        std::cout << "Signup success. userId = " << newUserId << "\n";
    } else if (code == 1) {
        std::cout << "Signup failed: underage\n";
    } else if (code == 2) {
        std::cout << "Signup failed: username exists\n";
    } else {
        std::cout << "Signup failed: unknown code\n";
    }

    std::cout << "\n--- Login ---\n";
    int64_t loggedInId = -1;
    int loginCode = auth.login("ahmad", "pass123", loggedInId);

    std::cout << "login return code = " << loginCode << "\n";
    if (loginCode == 0) {
        std::cout << "Login success. userId = " << loggedInId << "\n";
    } else if (loginCode == 1) {
        std::cout << "Login failed: username not found\n";
    } else if (loginCode == 2) {
        std::cout << "Login failed: user record not found\n";
    } else if (loginCode == 3) {
        std::cout << "Login failed: wrong password\n";
    } else {
        std::cout << "Login failed: unknown code\n";
    }

    std::cout << "\n--- Get user by ID ---\n";
    User u;
    if (newUserId != -1 && auth.getUserById(newUserId, u)) {
        std::cout << "Loaded user:\n";
        std::cout << "  id          = " << u.userId << "\n";
        std::cout << "  username    = " << u.username << "\n";
        std::cout << "  displayName = " << u.displayName << "\n";
        std::cout << "  bio         = " << u.bio << "\n";
        std::cout << "  private     = " << (u.isPrivate ? "yes" : "no") << "\n";
    } else {
        std::cout << "Failed to load user\n";
    }

    std::cout << "\n=== DONE ===\n";
    return 0;
}
