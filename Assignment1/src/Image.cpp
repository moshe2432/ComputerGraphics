#include "Image.h"

#include <algorithm>
#include <stdexcept>

#include "stb_image.h"
#include "stb_image_write.h"

unsigned char Clip(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return static_cast<unsigned char>(value);
}

Image::Image(int width, int height, int channels)
    : width(width), height(height), channels(channels),
      data(static_cast<size_t>(width) * height * channels, 0) {}

Image Image::Load(const std::string& path) {
    int w, h, origChannels;
    const int forcedChannels = 4; // RGBA, per assignment guidance
    unsigned char* raw = stbi_load(path.c_str(), &w, &h, &origChannels, forcedChannels);
    if (!raw) {
        throw std::runtime_error("Failed to load image: " + path);
    }

    Image image(w, h, forcedChannels);
    const size_t byteCount = static_cast<size_t>(w) * h * forcedChannels;
    std::copy(raw, raw + byteCount, image.data.begin());
    stbi_image_free(raw);
    return image;
}

void Image::Save(const std::string& path) const {
    const int stride = width * channels;
    stbi_write_png(path.c_str(), width, height, channels, data.data(), stride);
}

unsigned char Image::Get(int x, int y, int c) const {
    return data[(static_cast<size_t>(y) * width + x) * channels + c];
}

void Image::Set(int x, int y, int c, unsigned char value) {
    data[(static_cast<size_t>(y) * width + x) * channels + c] = value;
}
