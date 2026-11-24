#include <iostream>
#include <math.h>

#include "Image.h"

#define PI 3.14159265358979323846

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
                bw.Set(x * 2, y * 2, 0, 255);
                bw.Set(x * 2 + 1, y * 2, 0, 0);
                bw.Set(x * 2, y * 2 + 1, 0, 0);
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

Image GaussianBlur(const Image &gray)
{
    int kernelSize = 5;
    int kernel[5][5] = {
        {1, 4, 6, 4, 1},
        {4, 16, 24, 16, 4},
        {6, 24, 36, 24, 6},
        {4, 16, 24, 16, 4},
        {1, 4, 6, 4, 1}};
    int kernelSum = 256; // Sum of the kernel values

    Image blurred(gray.Width(), gray.Height(), gray.Channels());
    for (int y = 0; y < gray.Height(); ++y)
    {
        for (int x = 0; x < gray.Width(); ++x)
        {
            int sum = 0;
            for (int ky = -kernelSize / 2; ky <= kernelSize / 2; ++ky)
            {
                for (int kx = -kernelSize / 2; kx <= kernelSize / 2; ++kx)
                {
                    int px = std::min(std::max(x + kx, 0), gray.Width() - 1);
                    int py = std::min(std::max(y + ky, 0), gray.Height() - 1);
                    sum += gray.Get(px, py, 0) * kernel[ky + kernelSize / 2][kx + kernelSize / 2];
                }
            }
            blurred.Set(x, y, 0, static_cast<unsigned char>(sum / kernelSum));
        }
    }
    return blurred;
}

struct Gradients
{
    std::vector<float> magnitude;
    std::vector<int> direction;
};
Gradients ComputeGradients(const Image &blurred)
{
    Gradients gradients;
    gradients.magnitude.resize(blurred.Width() * blurred.Height());
    gradients.direction.resize(blurred.Width() * blurred.Height());
    for (int y = 0; y < blurred.Height(); ++y)
    {
        for (int x = 0; x < blurred.Width(); ++x)
        {
            int gx = 0;
            int gy = 0;
            if (x > 0 && x < blurred.Width() - 1 && y > 0 && y < blurred.Height() - 1)
            {
                gx += blurred.Get(x + 1, y - 1, 0) - blurred.Get(x - 1, y - 1, 0);
                gx += 2 * blurred.Get(x + 1, y, 0) - 2 * blurred.Get(x - 1, y, 0);
                gx += blurred.Get(x + 1, y + 1, 0) - blurred.Get(x - 1, y + 1, 0);

                gy += blurred.Get(x - 1, y + 1, 0) - blurred.Get(x - 1, y - 1, 0);
                gy += 2 * blurred.Get(x, y + 1, 0) - 2 * blurred.Get(x, y - 1, 0);
                gy += blurred.Get(x + 1, y + 1, 0) - blurred.Get(x + 1, y - 1, 0);
            }

            float magnitude = std::sqrt(gx * gx + gy * gy);
            int direction = static_cast<int>(std::atan2(gy, gx) * 180 / PI);
            if (direction < 0)
                direction += 180; //
            gradients.magnitude[y * blurred.Width() + x] = magnitude;
            gradients.direction[y * blurred.Width() + x] = direction;
        }
    }
    return gradients;
}

std::vector<float> NonMaxSuppress(const Gradients &grad, int width, int height)
{

    std::vector<float> suppressed(grad.magnitude.size(), 0.0f);

    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 1; x < width - 1; ++x)
        {
            int idx = y * width + x;
            float mag = grad.magnitude[idx];
            int dir = grad.direction[idx];

            float mag1 = 0.0f, mag2 = 0.0f;

            if ((dir >= 0 && dir < 22) || (dir >= 157 && dir <= 180))
            {
                mag1 = grad.magnitude[idx - 1];
                mag2 = grad.magnitude[idx + 1];
            }
            else if (dir >= 22 && dir < 67)
            {
                mag1 = grad.magnitude[idx - width - 1];
                mag2 = grad.magnitude[idx + width + 1];
            }
            else if (dir >= 67 && dir < 112)
            {
                mag1 = grad.magnitude[idx - width];
                mag2 = grad.magnitude[idx + width];
            }
            else if (dir >= 112 && dir < 157)
            {
                mag1 = grad.magnitude[idx - width + 1];
                mag2 = grad.magnitude[idx + width - 1];
            }

            if (mag >= mag1 && mag >= mag2)
                suppressed[idx] = mag;
        }
    }

    return suppressed;
}

Image DoubleThreshold(const std::vector<float> &suppressed, int width, int height)
{
    Image edges(width, height, 1);
    float highThreshold = 100.0f;
    float lowThreshold = 50.0f;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = y * width + x;
            float mag = suppressed[idx];

            if (mag >= highThreshold)
            {
                edges.Set(x, y, 0, 255);
            }
            else if (mag >= lowThreshold)
            {
                edges.Set(x, y, 0, 128);
            }
            else
            {
                edges.Set(x, y, 0, 0);
            }
        }
    }
    return edges;
}

Image Hysteresis(const Image &edges)
{
    Image finalEdges(edges.Width(), edges.Height(), 1);

    for (int y = 0; y < edges.Height(); ++y)
    {
        for (int x = 0; x < edges.Width(); ++x)
        {
            unsigned char value = edges.Get(x, y, 0);
            if (value == 255)
            {
                finalEdges.Set(x, y, 0, 255);
            }
            else if (value == 128)
            {
                bool connectedToStrongEdge = false;
                for (int dy = -1; dy <= 1; ++dy)
                {
                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        if (dx == 0 && dy == 0)
                            continue;
                        int nx = x + dx;
                        int ny = y + dy;
                        if (nx >= 0 && nx < edges.Width() && ny >= 0 && ny < edges.Height())
                        {
                            if (edges.Get(nx, ny, 0) == 255)
                            {
                                connectedToStrongEdge = true;
                                break;
                            }
                        }
                    }
                    if (connectedToStrongEdge)
                        break;
                }
                if (connectedToStrongEdge)
                {
                    finalEdges.Set(x, y, 0, 255);
                }
                else
                {
                    finalEdges.Set(x, y, 0, 0);
                }
            }
            else
            {
                finalEdges.Set(x, y, 0, 0);
            }
        }
    }

    return finalEdges;
}

Image CannyEdgeDetection(const Image &gray)
{
    Image blurred = GaussianBlur(gray);
    Gradients gradients = ComputeGradients(blurred);
    std::vector<float> suppressed = NonMaxSuppress(gradients, gray.Width(), gray.Height());
    Image edges = DoubleThreshold(suppressed, gray.Width(), gray.Height());
    Image finalEdges = Hysteresis(edges);
    return finalEdges;
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
        sketch.Save("Halftone.png", 1, "Halftone.txt");

        Image edges = CannyEdgeDetection(gray);
        edges.Save("Canny.png", 1, "Canny.txt");
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    std::cin.get();
    return 0;
}
