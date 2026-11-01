#include "InspectionManager.h"
#include "Database.h"
#include "QualityDetector.h"
#include <iostream>

InspectionManager::InspectionManager()
    : db(std::make_unique<Database>()),
      detector(std::make_unique<BasicQualityDetector>()) {}

InspectionManager::~InspectionManager() {}

bool InspectionManager::startInspection(int userId) {
    (void)userId;
    return db->connect();
}

std::string InspectionManager::processImage(int userId,
                                            const std::string& imagePath,
                                            const std::string& productName,
                                            const std::string& category,
                                            const std::string& batchNumber,
                                            const std::string& inspectionLocation,
                                            int& outInspectionId,
                                            std::string& qualityStatus,
                                            std::string& analysisMode) {
    if (!db->connect()) {
        qualityStatus = "Needs Inspection";
        analysisMode = "Basic Visual Analysis";
        return "Database connection failed";
    }

    std::string confidence;
    // Polymorphism: call analyzeImage through a QualityDetector pointer/reference.
    QualityDetector* det = detector.get();
    qualityStatus = det->analyzeImage(imagePath, confidence);
    analysisMode = det->detectorName();

    bool ok = db->saveInspection(userId, productName, category, imagePath,
                                 qualityStatus, analysisMode, "",
                                 batchNumber, inspectionLocation, outInspectionId);
    if (!ok) {
        qualityStatus = "Needs Inspection";
        return "Failed to save inspection";
    }
    return "Inspection completed";
}

std::vector<std::map<std::string, std::string>> InspectionManager::getHistory(int userId) {
    if (!db->connect()) return {};
    return db->getInspectionHistory(userId);
}

bool InspectionManager::deleteInspection(int inspectionId, int userId) {
    if (!db->connect()) return false;
    return db->deleteInspection(inspectionId, userId);
}

std::map<std::string, int> InspectionManager::calculateStatistics(int userId) {
    std::map<std::string, int> stats;
    if (!db->connect()) return stats;
    stats["total"] = db->getTotalInspections(userId);
    stats["good"] = db->getGoodCount(userId);
    stats["poor"] = db->getPoorCount(userId);
    stats["needs_inspection"] = db->getNeedsInspectionCount(userId);
    return stats;
}

std::vector<std::map<std::string, std::string>>
InspectionManager::getRecentInspections(int userId, int limit) {
    if (!db->connect()) return {};
    return db->getRecentInspections(userId, limit);
}
