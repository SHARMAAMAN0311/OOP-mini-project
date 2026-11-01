#include "QualityDetector.h"
#include "ImageProcessor.h"
#include <iostream>

std::string BasicQualityDetector::analyzeImage(const std::string& imagePath,
                                                std::string& confidence) const {
    ImageProcessor proc(imagePath);
    if (!proc.isLoaded()) {
        confidence = "0";
        return "Needs Inspection";
    }

    int r = 0, g = 0, b = 0;
    proc.calculateAverageColor(r, g, b);
    double brightness = proc.calculateBrightness();
    double variation = proc.calculateColorVariation();

    // Simple heuristic demo (NOT a real freshness/safety test).
    std::string result;
    if (g > r && g > b && brightness > 0.35) {
        result = "Good";
        confidence = "78";
    } else if (variation > 0.5 || brightness < 0.25) {
        result = "Poor";
        confidence = "65";
    } else {
        result = "Needs Inspection";
        confidence = "60";
    }
    return result;
}

ModelQualityDetector::ModelQualityDetector() : modelAvailable(false) {}

std::string ModelQualityDetector::analyzeImage(const std::string& imagePath,
                                                std::string& confidence) const {
    confidence = "0";
    if (!modelAvailable) {
        return "Needs Inspection"; // no model -> cannot claim a prediction
    }
    // Real model inference would go here when a trained model is available.
    return "Needs Inspection";
}
