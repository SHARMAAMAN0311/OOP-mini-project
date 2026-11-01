#pragma once
#include <string>
#include <vector>
#include <map>

// Forward declaration
struct sqlite3;

class Database {
private:
    sqlite3* db;
    std::string dbPath;

    // Prevent copying (manages a raw SQLite connection resource)
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

public:
    Database();
    explicit Database(const std::string& path);
    ~Database();

    bool connect();
    void disconnect();
    bool createTables();

    // Parameterized / prepared-statement helpers
    bool executeQuery(const std::string& sql,
                      const std::vector<std::string>& params = {});

    // User operations
    bool getUserByEmail(const std::string& email, int& userId,
                        std::string& fullName, std::string& passwordHash);
    bool saveUser(const std::string& fullName, const std::string& email,
                  const std::string& passwordHash, int& outUserId);
    bool updateUserProfile(int userId, const std::string& fullName,
                           const std::string& email);
    bool updateUserPassword(int userId, const std::string& passwordHash);

    // Inspection operations
    bool saveInspection(int userId, const std::string& productName,
                        const std::string& category, const std::string& imagePath,
                        const std::string& qualityStatus, const std::string& analysisMode,
                        const std::string& detectedFeatures,
                        const std::string& batchNumber,
                        const std::string& inspectionLocation, int& outInspectionId);
    std::vector<std::map<std::string, std::string>> getInspectionHistory(int userId);
    bool deleteInspection(int inspectionId, int userId);
    std::map<std::string, std::string> getInspectionById(int inspectionId, int userId);

    // Statistics (Dashboard)
    int getTotalInspections(int userId);
    int getGoodCount(int userId);
    int getPoorCount(int userId);
    int getNeedsInspectionCount(int userId);
    std::vector<std::map<std::string, std::string>> getCategoryStatistics(int userId);
    std::vector<std::map<std::string, std::string>> getRecentInspections(int userId, int limit = 10);

    // Contact
    bool saveContactMessage(const std::string& name, const std::string& email,
                            const std::string& subject, const std::string& message);

    sqlite3* getHandle() const { return db; }
};
