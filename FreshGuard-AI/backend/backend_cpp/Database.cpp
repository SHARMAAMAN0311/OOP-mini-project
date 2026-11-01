#include "Database.h"
#include <iostream>
#include <sstream>
#include <ctime>

// SQLite C API (link against sqlite3). Header provided by the system sqlite3 dev package.
#include <sqlite3.h>

static int callback(void* data, int argc, char** argv, char** colName) {
    std::vector<std::map<std::string, std::string>>* rows =
        static_cast<std::vector<std::map<std::string, std::string>>*>(data);
    std::map<std::string, std::string> row;
    for (int i = 0; i < argc; i++) {
        row[colName[i] ? colName[i] : ""] = (argv[i] ? argv[i] : "");
    }
    rows->push_back(row);
    return 0;
}

Database::Database() : db(nullptr), dbPath("freshguard.db") {}
Database::Database(const std::string& path) : db(nullptr), dbPath(path) {}
Database::~Database() { disconnect(); }

bool Database::connect() {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    return createTables();
}

void Database::disconnect() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::createTables() {
    if (!db) return false;
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "full_name TEXT NOT NULL,"
        "email TEXT UNIQUE NOT NULL,"
        "password_hash TEXT NOT NULL,"
        "created_at TEXT DEFAULT CURRENT_TIMESTAMP);"

        "CREATE TABLE IF NOT EXISTS inspections ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "product_name TEXT NOT NULL,"
        "category TEXT NOT NULL,"
        "image_path TEXT NOT NULL,"
        "quality_status TEXT NOT NULL,"
        "analysis_mode TEXT NOT NULL,"
        "detected_features TEXT,"
        "batch_number TEXT,"
        "inspection_location TEXT,"
        "created_at TEXT DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY(user_id) REFERENCES users(id));"

        "CREATE TABLE IF NOT EXISTS contact_messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "email TEXT NOT NULL,"
        "subject TEXT NOT NULL,"
        "message TEXT NOT NULL,"
        "created_at TEXT DEFAULT CURRENT_TIMESTAMP);"

        "CREATE INDEX IF NOT EXISTS idx_inspections_user ON inspections(user_id);"
        "CREATE INDEX IF NOT EXISTS idx_inspections_created ON inspections(created_at);";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << (errMsg ? errMsg : "") << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::executeQuery(const std::string& sql,
                            const std::vector<std::string>& params) {
    if (!db) return false;
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    for (size_t i = 0; i < params.size(); i++) {
        sqlite3_bind_text(stmt, (int)(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
    }
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE || rc == SQLITE_OK);
}

bool Database::getUserByEmail(const std::string& email, int& userId,
                              std::string& fullName, std::string& passwordHash) {
    if (!db) return false;
    const char* sql = "SELECT id, full_name, password_hash FROM users WHERE email = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_TRANSIENT);
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        userId = sqlite3_column_int(stmt, 0);
        fullName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        passwordHash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

bool Database::saveUser(const std::string& fullName, const std::string& email,
                        const std::string& passwordHash, int& outUserId) {
    const char* sql = "INSERT INTO users(full_name, email, password_hash) VALUES(?,?,?);";
    if (!executeQuery(sql, {fullName, email, passwordHash})) return false;
    outUserId = (int)sqlite3_last_insert_rowid(db);
    return true;
}

bool Database::updateUserProfile(int userId, const std::string& fullName,
                                 const std::string& email) {
    const char* sql = "UPDATE users SET full_name = ?, email = ? WHERE id = ?;";
    return executeQuery(sql, {fullName, email, std::to_string(userId)});
}

bool Database::updateUserPassword(int userId, const std::string& passwordHash) {
    const char* sql = "UPDATE users SET password_hash = ? WHERE id = ?;";
    return executeQuery(sql, {passwordHash, std::to_string(userId)});
}

bool Database::saveInspection(int userId, const std::string& productName,
                              const std::string& category, const std::string& imagePath,
                              const std::string& qualityStatus, const std::string& analysisMode,
                              const std::string& detectedFeatures,
                              const std::string& batchNumber,
                              const std::string& inspectionLocation, int& outInspectionId) {
    const char* sql =
        "INSERT INTO inspections(user_id, product_name, category, image_path, quality_status, "
        "analysis_mode, detected_features, batch_number, inspection_location) "
        "VALUES(?,?,?,?,?,?,?,?,?);";
    if (!executeQuery(sql, {std::to_string(userId), productName, category, imagePath,
                            qualityStatus, analysisMode, detectedFeatures, batchNumber,
                            inspectionLocation}))
        return false;
    outInspectionId = (int)sqlite3_last_insert_rowid(db);
    return true;
}

std::vector<std::map<std::string, std::string>> Database::getInspectionHistory(int userId) {
    std::vector<std::map<std::string, std::string>> rows;
    if (!db) return rows;
    const char* sql =
        "SELECT id, product_name, category, quality_status, analysis_mode, image_path, "
        "batch_number, inspection_location, created_at FROM inspections "
        "WHERE user_id = ? ORDER BY created_at DESC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return rows;
    sqlite3_bind_int(stmt, 1, userId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;
        row["id"] = std::to_string(sqlite3_column_int(stmt, 0));
        row["product_name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        row["category"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        row["quality_status"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        row["analysis_mode"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        row["image_path"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        row["batch_number"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        row["inspection_location"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        row["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        rows.push_back(row);
    }
    sqlite3_finalize(stmt);
    return rows;
}

bool Database::deleteInspection(int inspectionId, int userId) {
    const char* sql = "DELETE FROM inspections WHERE id = ? AND user_id = ?;";
    return executeQuery(sql, {std::to_string(inspectionId), std::to_string(userId)});
}

std::map<std::string, std::string> Database::getInspectionById(int inspectionId, int userId) {
    std::map<std::string, std::string> row;
    if (!db) return row;
    const char* sql =
        "SELECT id, product_name, category, quality_status, analysis_mode, image_path, "
        "detected_features, batch_number, inspection_location, created_at FROM inspections "
        "WHERE id = ? AND user_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return row;
    sqlite3_bind_int(stmt, 1, inspectionId);
    sqlite3_bind_int(stmt, 2, userId);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        row["id"] = std::to_string(sqlite3_column_int(stmt, 0));
        row["product_name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        row["category"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        row["quality_status"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        row["analysis_mode"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        row["image_path"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        row["detected_features"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        row["batch_number"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        row["inspection_location"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        row["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
    }
    sqlite3_finalize(stmt);
    return row;
}

int Database::getTotalInspections(int userId) {
    if (!db) return 0;
    const char* sql = "SELECT COUNT(*) FROM inspections WHERE user_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int(stmt, 1, userId);
    int total = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) total = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return total;
}

int Database::getGoodCount(int userId) {
    if (!db) return 0;
    const char* sql = "SELECT COUNT(*) FROM inspections WHERE user_id = ? AND quality_status = 'Good';";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int(stmt, 1, userId);
    int c = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) c = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return c;
}

int Database::getPoorCount(int userId) {
    if (!db) return 0;
    const char* sql = "SELECT COUNT(*) FROM inspections WHERE user_id = ? AND quality_status = 'Poor';";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int(stmt, 1, userId);
    int c = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) c = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return c;
}

int Database::getNeedsInspectionCount(int userId) {
    if (!db) return 0;
    const char* sql = "SELECT COUNT(*) FROM inspections WHERE user_id = ? AND quality_status = 'Needs Inspection';";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int(stmt, 1, userId);
    int c = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) c = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return c;
}

std::vector<std::map<std::string, std::string>> Database::getCategoryStatistics(int userId) {
    std::vector<std::map<std::string, std::string>> rows;
    if (!db) return rows;
    const char* sql =
        "SELECT category, COUNT(*) as cnt FROM inspections WHERE user_id = ? GROUP BY category;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return rows;
    sqlite3_bind_int(stmt, 1, userId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;
        row["category"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        row["count"] = std::to_string(sqlite3_column_int(stmt, 1));
        rows.push_back(row);
    }
    sqlite3_finalize(stmt);
    return rows;
}

std::vector<std::map<std::string, std::string>> Database::getRecentInspections(int userId, int limit) {
    std::vector<std::map<std::string, std::string>> rows;
    if (!db) return rows;
    std::string sql =
        "SELECT id, product_name, category, quality_status, created_at FROM inspections "
        "WHERE user_id = ? ORDER BY created_at DESC LIMIT " + std::to_string(limit) + ";";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return rows;
    sqlite3_bind_int(stmt, 1, userId);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;
        row["id"] = std::to_string(sqlite3_column_int(stmt, 0));
        row["product_name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        row["category"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        row["quality_status"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        row["created_at"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        rows.push_back(row);
    }
    sqlite3_finalize(stmt);
    return rows;
}

bool Database::saveContactMessage(const std::string& name, const std::string& email,
                                  const std::string& subject, const std::string& message) {
    const char* sql = "INSERT INTO contact_messages(name, email, subject, message) VALUES(?,?,?,?);";
    return executeQuery(sql, {name, email, subject, message});
}
