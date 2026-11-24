#include "Image.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>

#include "stb_image.h"
#include "stb_image_write.h"

namespace {
std::string WithTextExtension(const std::string& path) {
    size_t slash = path.find_last_of("/\\");
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) {
        return path + ".txt";
    }
    return path.substr(0, dot) + ".txt";
}
}  // namespace

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

void Image::Save(const std::string& path, int textMaxValue) const {
    const int stride = width * channels;
    stbi_write_png(path.c_str(), width, height, channels, data.data(), stride);

    if (textMaxValue < 0) return;

    std::ofstream out(WithTextExtension(path));
    if (!out) {
        throw std::runtime_error("Failed to open text output for: " + path);
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int raw = Get(x, y, 0);
            int scaled = static_cast<int>(std::lround(raw * textMaxValue / 255.0));
            if (x > 0) out << ',';
            out << scaled;
        }
        out << '\n';
    }
}

unsigned char Image::Get(int x, int y, int c) const {
    return data[(static_cast<size_t>(y) * width + x) * channels + c];
}

void Image::Set(int x, int y, int c, unsigned char value) {
    data[(static_cast<size_t>(y) * width + x) * channels + c] = value;
}
