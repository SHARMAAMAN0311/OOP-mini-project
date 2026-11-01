#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

// Forward declarations
class Database;
class QualityDetector;

/**
 * InspectionManager - Composition example.
 * Contains (owns) a Database and a QualityDetector and coordinates the
 * inspection workflow.
 */
class InspectionManager {
private:
    std::unique_ptr<Database> db;
    std::unique_ptr<QualityDetector> detector;

public:
    InspectionManager();
    ~InspectionManager();

    bool startInspection(int userId);
    std::string processImage(int userId, const std::string& imagePath,
                             const std::string& productName,
                             const std::string& category,
                             const std::string& batchNumber,
                             const std::string& inspectionLocation,
                             int& outInspectionId,
                             std::string& qualityStatus,
                             std::string& analysisMode);
    std::vector<std::map<std::string, std::string>> getHistory(int userId);
    bool deleteInspection(int inspectionId, int userId);
    std::map<std::string, int> calculateStatistics(int userId);
    std::vector<std::map<std::string, std::string>> getRecentInspections(int userId, int limit = 10);
};
