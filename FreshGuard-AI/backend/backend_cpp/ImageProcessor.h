#pragma once
#include <string>
#include <vector>

/**
 * ImageProcessor - loads and inspects basic visual properties of an image.
 * Uses OpenCV when available; otherwise falls back to a lightweight approach.
 */
class ImageProcessor {
private:
    std::string imagePath;
    bool loaded;
    int width;
    int height;
    int channels;

public:
    ImageProcessor();
    explicit ImageProcessor(const std::string& path);
    ~ImageProcessor();

    bool loadImage(const std::string& path);
    bool isLoaded() const { return loaded; }

    // Resize image to a target width/height (returns new path or in-memory dims)
    bool resizeImage(int targetWidth, int targetHeight);

    // Basic visual measurements
    double calculateAverageColor(int& r, int& g, int& b) const;
    double calculateBrightness() const;
    double calculateColorVariation() const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }
    std::string getPath() const { return imagePath; }
};
