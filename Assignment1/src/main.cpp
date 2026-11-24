#include <iostream>

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
        gray.Save("Lenna_gray.png");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    std::cin.get();
    return 0;
}
