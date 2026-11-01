#include "Report.h"
#include "Database.h"
#include <sstream>

Report::Report() {}
Report::~Report() {}

std::string Report::generateCSV(
    const std::vector<std::map<std::string, std::string>>& records) {
    std::ostringstream oss;
    oss << "id,product_name,category,quality_status,analysis_mode,created_at\n";
    for (const auto& rec : records) {
        oss << rec.at("id") << ","
            << rec.at("product_name") << ","
            << rec.at("category") << ","
            << rec.at("quality_status") << ","
            << rec.at("analysis_mode") << ","
            << rec.at("created_at") << "\n";
    }
    return oss.str();
}

std::string Report::formatInspectionReport(
    const std::map<std::string, std::string>& inspection) {
    std::ostringstream oss;
    oss << "FreshGuard AI - Single Inspection Report\n";
    oss << "------------------------------------------\n";
    oss << "Inspection ID : " << inspection.at("id") << "\n";
    oss << "Product       : " << inspection.at("product_name") << "\n";
    oss << "Category      : " << inspection.at("category") << "\n";
    oss << "Quality       : " << inspection.at("quality_status") << "\n";
    oss << "Analysis Mode : " << inspection.at("analysis_mode") << "\n";
    oss << "Image         : " << inspection.at("image_path") << "\n";
    oss << "Batch         : " << inspection.at("batch_number") << "\n";
    oss << "Location      : " << inspection.at("inspection_location") << "\n";
    oss << "Date          : " << inspection.at("created_at") << "\n";
    return oss.str();
}

std::string Report::exportInspectionHistory(int userId) {
    Database db;
    if (!db.connect()) return "";
    auto records = db.getInspectionHistory(userId);
    return generateCSV(records);
}
