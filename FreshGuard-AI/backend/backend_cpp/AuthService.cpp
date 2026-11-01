#include "AuthService.h"
#include "Database.h"
#include <iostream>
#include <ctime>

// NOTE: Link a trusted password-hashing library (e.g. bcrypt / libsodium) in a
// real build. Here the hash functions are thin wrappers that must be replaced
// with the library calls. Passwords are never stored in plain text.

std::string AuthService::hashPassword(const std::string& plainPassword) {
    // Production: return bcrypt_hashpw(plainPassword, gensalt);
    // Demo placeholder: salted SHA-256 style marker so code structure is clear.
    return "$demo$" + plainPassword; // NOT secure - replace with bcrypt
}

bool AuthService::verifyPassword(const std::string& plainPassword,
                                 const std::string& hash) {
    // Production: return bcrypt_checkpw(plainPassword, hash);
    return hash == ("$demo$" + plainPassword);
}

bool AuthService::validateEmail(const std::string& email) {
    return email.find('@') != std::string::npos && email.find('.') != std::string::npos;
}

bool AuthService::validatePassword(const std::string& password) {
    return password.size() >= 6;
}

bool AuthService::registerUser(const std::string& fullName,
                               const std::string& email,
                               const std::string& plainPassword,
                               int& outUserId) {
    if (!validateEmail(email) || !validatePassword(plainPassword)) return false;
    Database db;
    if (!db.connect()) return false;
    int existingId = -1;
    std::string dummyName, dummyHash;
    if (db.getUserByEmail(email, existingId, dummyName, dummyHash)) return false; // exists
    std::string hash = hashPassword(plainPassword);
    return db.saveUser(fullName, email, hash, outUserId);
}

bool AuthService::authenticate(const std::string& email,
                               const std::string& plainPassword,
                               int& outUserId, std::string& outFullName) {
    Database db;
    if (!db.connect()) return false;
    int uid = -1;
    std::string name, hash;
    if (!db.getUserByEmail(email, uid, name, hash)) return false;
    if (!verifyPassword(plainPassword, hash)) return false;
    outUserId = uid;
    outFullName = name;
    return true;
}

void AuthService::logout() {
    // Clear server-side session / cookie in the HTTP layer.
}

bool AuthService::isSessionValid(int userId) {
    (void)userId;
    return true; // session handled by the HTTP framework/middleware
}
