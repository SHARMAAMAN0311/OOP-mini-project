#pragma once
#include <string>

/**
 * AuthService - authentication and password security.
 * Uses secure password hashing (bcrypt in production) and session handling.
 */
class AuthService {
public:
    static bool registerUser(const std::string& fullName,
                             const std::string& email,
                             const std::string& plainPassword,
                             int& outUserId);
    static bool authenticate(const std::string& email,
                             const std::string& plainPassword,
                             // out params
                             int& outUserId, std::string& outFullName);
    static void logout(); // clear session
    static bool validateEmail(const std::string& email);
    static bool validatePassword(const std::string& password);

private:
    static std::string hashPassword(const std::string& plainPassword);
    static bool verifyPassword(const std::string& plainPassword,
                               const std::string& hash);
    static bool isSessionValid(int userId); // placeholder for session handling
};
