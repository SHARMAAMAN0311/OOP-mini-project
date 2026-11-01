#include "ReportGenerator.h"
#include "Inspection.h"
#include "Database.h"
#include "Report.h"
#include <sstream>

ReportGenerator::ReportGenerator() {}
ReportGenerator::~ReportGenerator() {}

std::string ReportGenerator::generateSingleInspectionReport(int inspectionId, int userId) {
    Database db;
    if (!db.connect()) return "Database error";
    auto rec = db.getInspectionById(inspectionId, userId);
    if (rec.empty()) return "Inspection not found";
    Report report;
    return report.formatInspectionReport(rec);
}

std::string ReportGenerator::generateCSVReport(int userId) {
    Report report;
    return report.exportInspectionHistory(userId);
}

std::string ReportGenerator::generateSummaryReport(int userId) {
    Database db;
    if (!db.connect()) return "Database error";
    std::ostringstream oss;
    oss << "FreshGuard AI - Summary Report\n";
    oss << "------------------------------\n";
    oss << "Total inspections : " << db.getTotalInspections(userId) << "\n";
    oss << "Good quality      : " << db.getGoodCount(userId) << "\n";
    oss << "Needs inspection  : " << db.getNeedsInspectionCount(userId) << "\n";
    oss << "Poor quality      : " << db.getPoorCount(userId) << "\n";
    return oss.str();
}
