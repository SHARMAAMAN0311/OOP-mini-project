#include "Inspection.h"
#include "Database.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Inspection::Inspection()
    : inspectionId(-1), userId(-1) {}

Inspection::Inspection(int id, int uid, const std::string& product,
                       const std::string& cat, const std::string& image,
                       const std::string& status, const std::string& date,
                       const std::string& mode)
    : inspectionId(id), userId(uid), productName(product), category(cat),
      imagePath(image), qualityStatus(status), inspectionDate(date),
      analysisMode(mode) {}

Inspection::~Inspection() {}

bool Inspection::createInspection() {
    Database db;
    if (!db.connect()) return false;
    int newId = -1;
    bool ok = db.saveInspection(userId, productName, category, imagePath,
                                qualityStatus, analysisMode,
                                "", "", "", newId);
    if (ok) inspectionId = newId;
    return ok;
}

void Inspection::displayResult() const {
    std::cout << "=== Inspection Result ===" << std::endl;
    std::cout << "Inspection ID : " << inspectionId << std::endl;
    std::cout << "Product       : " << productName << std::endl;
    std::cout << "Category      : " << category << std::endl;
    std::cout << "Quality       : " << qualityStatus << std::endl;
    std::cout << "Mode          : " << analysisMode << std::endl;
    std::cout << "Date          : " << inspectionDate << std::endl;
}

bool Inspection::saveResult() {
    Database db;
    if (!db.connect()) return false;
    return db.saveInspection(userId, productName, category, imagePath,
                             qualityStatus, analysisMode, "", "", "", inspectionId);
}

std::string Inspection::generateReport() const {
    std::ostringstream oss;
    oss << "FreshGuard AI - Inspection Report\n";
    oss << "--------------------------------\n";
    oss << "Inspection ID : " << inspectionId << "\n";
    oss << "Product       : " << productName << "\n";
    oss << "Category      : " << category << "\n";
    oss << "Quality       : " << qualityStatus << "\n";
    oss << "Analysis Mode : " << analysisMode << "\n";
    oss << "Date          : " << inspectionDate << "\n";
    oss << "Image         : " << imagePath << "\n";
    oss << "\nNote: Basic visual analysis demonstration only.\n";
    oss << "Not a guarantee of internal freshness or food safety.\n";
    return oss.str();
}
