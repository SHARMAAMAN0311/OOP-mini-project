#include "Dashboard.h"
#include "Database.h"

Dashboard::Dashboard(Database& database, int uid) : db(database), userId(uid) {}
Dashboard::~Dashboard() {}

int Dashboard::getTotalInspections() { return db.getTotalInspections(userId); }
int Dashboard::getGoodCount() { return db.getGoodCount(userId); }
int Dashboard::getPoorCount() { return db.getPoorCount(userId); }
int Dashboard::getNeedsInspectionCount() { return db.getNeedsInspectionCount(userId); }

std::vector<std::map<std::string, std::string>> Dashboard::getCategoryStatistics() {
    return db.getCategoryStatistics(userId);
}

std::vector<std::map<std::string, std::string>> Dashboard::getRecentInspections(int limit) {
    return db.getRecentInspections(userId, limit);
}
