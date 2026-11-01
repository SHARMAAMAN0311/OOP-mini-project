#pragma once
#include <string>
#include <vector>
#include <map>

/**
 * Report - generates downloadable CSV and formatted reports.
 * Uses Inspection and Database instead of duplicating their logic.
 */
class Report {
public:
    Report();
    ~Report();

    std::string generateCSV(const std::vector<std::map<std::string, std::string>>& records);
    std::string formatInspectionReport(const std::map<std::string, std::string>& inspection);
    std::string exportInspectionHistory(int userId);
};
