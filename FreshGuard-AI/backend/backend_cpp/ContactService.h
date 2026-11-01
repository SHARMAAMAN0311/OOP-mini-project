#pragma once
#include <string>

class Database;

/**
 * ContactService - handles contact-form messages stored in SQLite.
 */
class ContactService {
private:
    Database& db;

public:
    explicit ContactService(Database& database);
    ~ContactService();
    bool saveMessage(const std::string& name, const std::string& email,
                     const std::string& subject, const std::string& message);
};
