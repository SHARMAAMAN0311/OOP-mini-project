#pragma once
#include <string>

// Forward declaration to avoid circular dependency with AuthService.
class AuthService;

/**
 * User - Encapsulation example.
 * Sensitive data (passwordHash) is private and never exposed directly.
 * Public methods are the only way to read/change user data.
 */
class User {
private:
    int userId;
    std::string fullName;
    std::string email;
    std::string passwordHash; // private: never stored or returned as plain text

public:
    User();
    User(int id, const std::string& name, const std::string& email, const std::string& hash);
    ~User();

    // Getters (read-only access)
    int getId() const;
    std::string getFullName() const;
    std::string getEmail() const;

    // No getter for passwordHash - encapsulation
    bool passwordMatches(const std::string& plainPassword) const;
    void setPasswordHash(const std::string& hash);
    void setFullName(const std::string& name);
    void setEmail(const std::string& email);

    // Business operations delegated to AuthService
    bool registerUser();
    bool login(const std::string& plainPassword);
    bool updateProfile();
    bool changePassword(const std::string& newPassword);
};
