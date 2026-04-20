#include <cmath>
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void Line (int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    bool swapped = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        swapped = true;
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    float derror = std::abs(dy / float(dx));
    float error = 0;
    int y = y0;

    for (int x = x0; x <= x1; x++)
    {
        if (swapped)
            image.set(y, x, color);
        else
            image.set(x, y, color);

        error += derror;
        if (error > .5)
        {
            y += (dy > 0 ? 1 : -1);
            error -= 1;
        }
    }
}

int main (int argc, char** argv) 
{
    constexpr int width  = 1000;
    constexpr int height = 1000;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    Model model{"obj/african_head/african_head.obj"};

    for (int i = 0; i < model.nfaces(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            vec4 v0 = model.vert(i, j);
            vec4 v1 = model.vert(i, (j + 1) % 3);

            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;

            Line(x0, y0, x1, y1, framebuffer, yellow);
        }
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

