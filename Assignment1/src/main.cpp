#include <iostream>
#include <cmath>
#include "Image.h"

Image GrayScale(const Image &image)
{
    Image gray(image.Width(), image.Height(), 1);

    for (int y = 0; y < image.Height(); ++y)
    {
        for (int x = 0; x < image.Width(); ++x)
        {
            const unsigned char *p = image.PixelPtr(x, y);
            unsigned char grayValue = Clip(static_cast<unsigned char>(0.299f * p[0] + 0.587f * p[1] + 0.114f * p[2]) + 0.5f);
            gray.Set(x, y, 0, grayValue);
        }
    }

    return gray;
}

Image Halftone(const Image &grayImage)
{
    const int width = grayImage.Width();
    const int height = grayImage.Height();
    Image out(2 * width, 2 * height, 1);

    // pattern[level] = the 2x2 pattern for the given level, in order
    // top-left, top-right, bottom-left, bottom-right. 1 = white, 0 = black.
    const unsigned char pattern[5][4] = {
        {0, 0, 0, 0}, // level 0
        {0, 0, 1, 0}, // level 1
        {0, 1, 1, 0}, // level 2
        {0, 1, 1, 1}, // level 3
        {1, 1, 1, 1}  // level 4
    };
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const unsigned char grayValue = grayImage.Get(x, y, 0) * 5 / 256; // 0 .. 4
            const unsigned char *block = pattern[grayValue];

            // one input pixel becomes a 2x2 block of output pixels at once
            out.Set(2 * x, 2 * y, 0, block[0] * 255);
            out.Set(2 * x + 1, 2 * y, 0, block[1] * 255);
            out.Set(2 * x, 2 * y + 1, 0, block[2] * 255);
            out.Set(2 * x + 1, 2 * y + 1, 0, block[3] * 255);
        }
    }
    return out;
}

Image FloyedSteinbergScale(const Image &grayImage)
{
    // we are going to change the image from 256 intensity grayscale values to 16 intensity grayscale values, so we need to divide by 16 and round to the nearest integer
    const int width = grayImage.Width();
    const int height = grayImage.Height();
    Image out(width, height, 1);

    std::vector<float> work(static_cast<size_t>(width) * height);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            work[y * width + x] = static_cast<float>(grayImage.Get(x, y, 0));
        }
    }

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const float oldPixel = work[y * width + x];
            // the nearest of the 16 levels is 0, 17, 34, ..., 255.
            int level = static_cast<int>(std::floor((oldPixel / 17.0f) + 0.5f));
            if (level < 0)
                level = 0;
            if (level > 15)
                level = 15;
            const float choosen = level * 17.0f;
            out.Set(x, y, 0, static_cast<unsigned char>(choosen));

            const float error = oldPixel - choosen;

            // which of the four nighbours exist?
            const bool hasRight = x + 1 < width;
            const bool hasBelow = y + 1 < height;
            const bool hasLeft = x > 0;

            if (hasRight && hasBelow && hasLeft)
            {
                // inside the picture, the floyd-steinberg weights.
                work[y * width + (x + 1)] += error * (7.0f / 16.0f);
                work[(y + 1) * width + (x - 1)] += error * (3.0f / 16.0f);
                work[(y + 1) * width + x] += error * (5.0f / 16.0f);
                work[(y + 1) * width + (x + 1)] += error * (1.0f / 16.0f);
            }
            else
            {
                // on the edge, equal shares for the neighbours that exist.
                const int count = (hasRight ? 1 : 0) + (hasBelow ? 1 : 0) + (hasLeft && hasBelow ? 1 : 0) + (hasRight && hasBelow ? 1 : 0);
                if (hasRight)
                    work[y * width + (x + 1)] += error / count;
                if (hasBelow)
                    work[(y + 1) * width + x] += error / count;
                if (hasLeft && hasBelow)
                    work[(y + 1) * width + (x - 1)] += error / count;
            }
        }
    }
    return out;
}

int main()
{
    try
    {
        Image image = Image::Load("Lenna.png");
        std::cout << "Loaded Lenna.png: " << image.Width() << "x" << image.Height()
                  << ", " << image.Channels() << " channels\n";

        const unsigned char *p0 = image.PixelPtr(0, 0);
        std::cout << "Pixel (0,0) = R:" << (int)p0[0] << " G:" << (int)p0[1]
                  << " B:" << (int)p0[2] << " A:" << (int)p0[3] << "\n";

        Image gray = GrayScale(image);
        gray.Save("Grayscale.png");

        Image halftone = Halftone(gray);
        halftone.Save("Halftone.png");

        Image floydSteinberg = FloyedSteinbergScale(gray);
        floydSteinberg.Save("FloyedSteinberg.png");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    std::cout << "Done. Press Enter to exit.\n";
    std::cin.get();
    return 0;
}
