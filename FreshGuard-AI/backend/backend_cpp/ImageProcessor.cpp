#include "ImageProcessor.h"
#include <iostream>
#include <cmath>

// OpenCV is optional. When available, uncomment and link against opencv.
// #include <opencv2/opencv.hpp>

ImageProcessor::ImageProcessor() : loaded(false), width(0), height(0), channels(0) {}
ImageProcessor::ImageProcessor(const std::string& path)
    : loaded(false), width(0), height(0), channels(0) {
    loadImage(path);
}
ImageProcessor::~ImageProcessor() {}

bool ImageProcessor::loadImage(const std::string& path) {
    imagePath = path;
    // Lightweight fallback: inspect file extension only; real pixel data would
    // be read via OpenCV (cv::imread) when the library is installed.
    loaded = true;
    width = 0;
    height = 0;
    channels = 3;
    std::cout << "[ImageProcessor] Loaded (demo): " << path << std::endl;
    return loaded;
}

bool ImageProcessor::resizeImage(int targetWidth, int targetHeight) {
    if (!loaded) return false;
    width = targetWidth;
    height = targetHeight;
    return true;
}

double ImageProcessor::calculateAverageColor(int& r, int& g, int& b) const {
    // Placeholder demonstration values. With OpenCV, iterate pixels and average.
    r = 120; g = 180; b = 90; // greenish (fresh produce demo)
    return 0.0;
}

double ImageProcessor::calculateBrightness() const {
    return 0.5; // 0.0 dark .. 1.0 bright (demo)
}

double ImageProcessor::calculateColorVariation() const {
    return 0.3; // 0.0 uniform .. 1.0 high variation (demo)
}
