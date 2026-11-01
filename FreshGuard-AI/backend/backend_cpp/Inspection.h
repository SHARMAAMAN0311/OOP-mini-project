#pragma once
#include <string>
#include <ctime>

/**
 * Inspection - holds a single quality-inspection record.
 * Encapsulated: data members private, accessed through methods.
 */
class Inspection {
private:
    int inspectionId;
    int userId;
    std::string productName;
    std::string category;
    std::string imagePath;
    std::string qualityStatus;      // Good / Needs Inspection / Poor
    std::string inspectionDate;
    std::string analysisMode;       // "Basic Visual Analysis" or "Trained Model"

public:
    Inspection();
    Inspection(int id, int uid, const std::string& product, const std::string& cat,
               const std::string& image, const std::string& status,
               const std::string& date, const std::string& mode);
    ~Inspection();

    int getId() const { return inspectionId; }
    int getUserId() const { return userId; }
    std::string getProductName() const { return productName; }
    std::string getCategory() const { return category; }
    std::string getImagePath() const { return imagePath; }
    std::string getQualityStatus() const { return qualityStatus; }
    std::string getInspectionDate() const { return inspectionDate; }
    std::string getAnalysisMode() const { return analysisMode; }

    void setQualityStatus(const std::string& status) { qualityStatus = status; }
    void setImagePath(const std::string& path) { imagePath = path; }

    bool createInspection();
    void displayResult() const;
    bool saveResult();
    std::string generateReport() const;
};
