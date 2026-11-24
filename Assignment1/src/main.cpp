#include <iostream>
#include <math.h>

#include "Image.h"

Image GrayScale(const Image &image)
{
    Image gray(image.Width(), image.Height(), 1);

    for (int y = 0; y < image.Height(); ++y)
    {
        for (int x = 0; x < image.Width(); ++x)
        {
            const unsigned char *p = image.PixelPtr(x, y);
            unsigned char grayValue = static_cast<unsigned char>(0.299f * p[0] + 0.587f * p[1] + 0.114f * p[2]);
            gray.Set(x, y, 0, grayValue);
        }
    }

    return gray;
}

/*
this function should get a gray scale image and apply Floyd-Steinberg dithering to it, returning a black and white image (1 channel)
*/
Image Floyd_SteinbergDithering(const Image &image)
{
    Image bw(image.Width(), image.Height(), 1);
    for (int y = 0; y < image.Height(); y++)
    {
        for (int x = 0; x < image.Width(); x++)
        {
            bw.Set(x, y, 0, image.Get(x, y, 0));
        }
    }
    for (int y = 0; y < bw.Height(); ++y)
    {
        for (int x = 0; x < bw.Width(); ++x)
        {
            unsigned char op = bw.Get(x, y, 0);
            int idx = round(op / 17);
            unsigned char np = idx * 17;
            unsigned char error = op - np;
            bw.Set(x, y, 0, np);

            if (x + 1 < bw.Width() && y + 1 < bw.Height() && x - 1 >= 0)
            {
                unsigned char unerror = error / 4;
                bw.Set(x + 1, y, 0, bw.Get(x + 1, y, 0) + unerror);
                bw.Set(x - 1, y + 1, 0, bw.Get(x - 1, y + 1, 0) + unerror);
                bw.Set(x, y + 1, 0, bw.Get(x, y + 1, 0) + unerror);
                bw.Set(x + 1, y + 1, 0, bw.Get(x + 1, y + 1, 0) + error - 3 * unerror);
            }

            // Handle edge cases for the last row and last column
            if (x + 1 < bw.Width() && y + 1 == bw.Height())
            {
                bw.Set(x + 1, y, 0, bw.Get(x + 1, y, 0) + error);
            }
            else if (x + 1 == bw.Width() && y + 1 < bw.Height())
            {
                unsigned char unerror = error / 2;
                bw.Set(x - 1, y + 1, 0, bw.Get(x - 1, y + 1, 0) + unerror);
                bw.Set(x, y + 1, 0, bw.Get(x, y + 1, 0) + error - unerror);
            }
            else if (x - 1 == 0 && y + 1 < bw.Height())
            {
                unsigned char unerror = error / 3;
                bw.Set(x + 1, y, 0, bw.Get(x + 1, y, 0) + unerror);
                bw.Set(x, y + 1, 0, bw.Get(x, y + 1, 0) + error - unerror);
                bw.Set(x + 1, y + 1, 0, bw.Get(x + 1, y + 1, 0) + unerror);
            }
        }
    }
    return bw;
}

Image Sketch_HalfTone(const Image &image)
{
    Image bw(image.Width() * 2, image.Height() * 2, 1);
    for (int y = 0; y < image.Height(); ++y)
    {
        for (int x = 0; x < image.Width(); ++x)
        {
            unsigned char grayValue = image.Get(x, y, 0);
            if (grayValue < 255 / 5)
            {
                bw.Set(x * 2, y * 2, 0, 0);
                bw.Set(x * 2 + 1, y * 2, 0, 0);
                bw.Set(x * 2, y * 2 + 1, 0, 0);
                bw.Set(x * 2 + 1, y * 2 + 1, 0, 0);
            }
            else if (grayValue < 2 * 255 / 5)
            {
                bw.Set(x * 2, y * 2, 0, 0);
                bw.Set(x * 2 + 1, y * 2, 0, 0);
                bw.Set(x * 2, y * 2 + 1, 0, 0);
                bw.Set(x * 2 + 1, y * 2 + 1, 0, 255);
            }
            else if (grayValue < 3 * 255 / 5)
            {
                bw.Set(x * 2, y * 2, 0, 0);
                bw.Set(x * 2 + 1, y * 2, 0, 0);
                bw.Set(x * 2, y * 2 + 1, 0, 255);
                bw.Set(x * 2 + 1, y * 2 + 1, 0, 255);
            }
            else if (grayValue < 4 * 255 / 5)
            {
                bw.Set(x * 2, y * 2, 0, 0);
                bw.Set(x * 2 + 1, y * 2, 0, 255);
                bw.Set(x * 2, y * 2 + 1, 0, 255);
                bw.Set(x * 2 + 1, y * 2 + 1, 0, 255);
            }
            else
            {
                bw.Set(x * 2, y * 2, 0, 255);
                bw.Set(x * 2 + 1, y * 2, 0, 255);
                bw.Set(x * 2, y * 2 + 1, 0, 255);
                bw.Set(x * 2 + 1, y * 2 + 1, 0, 255);
            }
        }
    }
    return bw;
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
        gray.Save("Grayscale.png", 255);

        Image bw = Floyd_SteinbergDithering(gray);
        bw.Save("FloydSteinberg.png", 15);

        Image sketch = Sketch_HalfTone(gray);
        sketch.Save("Sketch_HalfTone.png", 15);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    std::cin.get();
    return 0;
}
