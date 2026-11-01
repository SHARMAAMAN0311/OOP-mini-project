#pragma once
#include <string>
#include <vector>
#include <map>

class Database;

/**
 * Dashboard - retrieves statistics from actual SQLite records (no hardcoded data).
 */
class Dashboard {
private:
    Database& db;
    int userId;

public:
    Dashboard(Database& database, int uid);
    ~Dashboard();

    int getTotalInspections();
    int getGoodCount();
    int getPoorCount();
    int getNeedsInspectionCount();
    std::vector<std::map<std::string, std::string>> getCategoryStatistics();
    std::vector<std::map<std::string, std::string>> getRecentInspections(int limit = 10);
};
