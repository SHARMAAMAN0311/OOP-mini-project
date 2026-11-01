#pragma once
#include <string>
#include <vector>

/**
 * QualityDetector - Abstraction (abstract base class).
 * Declares the pure virtual analyzeImage() interface.
 */
class QualityDetector {
public:
    virtual ~QualityDetector() = default;

    // Pure virtual function - Abstraction
    virtual std::string analyzeImage(const std::string& imagePath,
                                     std::string& confidence) const = 0;

    virtual std::string detectorName() const = 0;
};

/**
 * BasicQualityDetector - Inheritance from QualityDetector.
 * Performs simple visual-analysis heuristics. Result is clearly labeled as a
 * demonstration, not a trained model.
 */
class BasicQualityDetector : public QualityDetector {
public:
    ~BasicQualityDetector() override = default;
    std::string analyzeImage(const std::string& imagePath,
                             std::string& confidence) const override;
    std::string detectorName() const override { return "Basic Visual Analysis"; }
};

/**
 * ModelQualityDetector - Inheritance from QualityDetector.
 * Only to be used when a real compatible trained model exists. Otherwise it
 * reports that no model is available (no fabricated predictions).
 */
class ModelQualityDetector : public QualityDetector {
private:
    bool modelAvailable;
public:
    ModelQualityDetector();
    ~ModelQualityDetector() override = default;
    std::string analyzeImage(const std::string& imagePath,
                             std::string& confidence) const override;
    std::string detectorName() const override { return "Trained Model"; }
    bool isAvailable() const { return modelAvailable; }
};
