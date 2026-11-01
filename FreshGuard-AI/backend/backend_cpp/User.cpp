#include "User.h"
#include "AuthService.h"
#include <iostream>

User::User() : userId(-1) {}

User::User(int id, const std::string& name, const std::string& email, const std::string& hash)
    : userId(id), fullName(name), email(email), passwordHash(hash) {}

User::~User() {}

int User::getId() const { return userId; }
std::string User::getFullName() const { return fullName; }
std::string User::getEmail() const { return email; }

bool User::passwordMatches(const std::string& plainPassword) const {
    return AuthService::verifyPassword(plainPassword, passwordHash);
}

void User::setPasswordHash(const std::string& hash) { passwordHash = hash; }
void User::setFullName(const std::string& name) { fullName = name; }
void User::setEmail(const std::string& email) { this->email = email; }

bool User::registerUser() { return AuthService::registerUser(*this); }
bool User::login(const std::string& plainPassword) {
    return AuthService::authenticate(email, plainPassword, *this);
}
bool User::updateProfile() { return AuthService::updateProfile(*this); }
bool User::changePassword(const std::string& newPassword) {
    return AuthService::changePassword(userId, newPassword);
}
