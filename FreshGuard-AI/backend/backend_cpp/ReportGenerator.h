#pragma once
#include <string>

class Inspection;
class Database;

/**
 * ReportGenerator - high-level report generation.
 * Delegates to Inspection and Database rather than duplicating responsibilities.
 */
class ReportGenerator {
public:
    ReportGenerator();
    ~ReportGenerator();

    std::string generateSingleInspectionReport(int inspectionId, int userId);
    std::string generateCSVReport(int userId);
    std::string generateSummaryReport(int userId);
};
