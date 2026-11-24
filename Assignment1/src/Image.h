#pragma once

#include <string>
#include <vector>

// Simple dense image buffer: width * height pixels, each with `channels` values.
// Data is stored row-major, interleaved per pixel (e.g. RGBA, RGBA, ... for channels == 4).
class Image {
public:
    Image() = default;
    Image(int width, int height, int channels);

    // Loads an image from disk and forces it to RGBA (channels == 4),
    // as suggested by the assignment's stbi_load guidance.
    static Image Load(const std::string& path);

    void Save(const std::string& path) const;

    int Width() const { return width; }
    int Height() const { return height; }
    int Channels() const { return channels; }

    unsigned char Get(int x, int y, int c) const;
    void Set(int x, int y, int c, unsigned char value);

    unsigned char* PixelPtr(int x, int y) { return data.data() + (static_cast<size_t>(y) * width + x) * channels; }
    const unsigned char* PixelPtr(int x, int y) const { return data.data() + (static_cast<size_t>(y) * width + x) * channels; }

private:
    int width = 0;
    int height = 0;
    int channels = 0;
    std::vector<unsigned char> data;
};

// Clamps an integer intensity to the valid 0-255 byte range.
unsigned char Clip(int value);
