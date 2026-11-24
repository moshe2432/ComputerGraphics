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

    // Saves the image as a PNG at `path`. If `textMaxValue` is >= 0, also writes a
    // debug .txt file (one row per image row, values comma-separated with no spaces,
    // each channel-0 byte rescaled from 0-255 down to 0-textMaxValue, e.g. 255 for
    // grayscale, 1 for black/white, 15 for 16-level images). By default the .txt path
    // is `path` with its extension replaced by ".txt"; pass `textPath` to use a
    // different name (needed since the assignment's required filenames don't always
    // share a base name, e.g. "Haftone.png" / "Halftone.txt").
    void Save(const std::string& path, int textMaxValue = -1, const std::string& textPath = "") const;

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
