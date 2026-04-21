#include <cmath>
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line (int ax, int ay, int bx, int by, TGAImage& image, TGAColor color)
{
    bool swapped = false;
    if (std::abs(ax - bx) < std::abs(ay - by))
    {
        std::swap(ax, ay);
        std::swap(bx, by);
        swapped = true;
    }

    if (ax > bx)
    {
        std::swap(ax, bx);
        std::swap(ay, by);
    }

    int dx = bx - ax;
    int dy = by - ay;
    int ierror = 0;
    int y = ay;

    for (int x = ax; x <= bx; x++)
    {
        if (swapped)
            image.set(y, x, color);
        else
            image.set(x, y, color);

        ierror += 2 * std::abs(dy);
        if (ierror > dx)
        {
            y += (dy > 0 ? 1 : -1);
            ierror -= 2 * dx;
        }
    }
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) 
{
    line(ax, ay, bx, by, framebuffer, color);
    line(bx, by, cx, cy, framebuffer, color);
    line(cx, cy, ax, ay, framebuffer, color);

    // Sort the vertices int x direction:
    if (ax > bx) { std::swap(ax, bx); std::swap(ay, by); }
    if (ax > cx) { std::swap(ax, cx); std::swap(ay, cy); }
    if (bx > cx) { std::swap(bx, cx); std::swap(by, cy); }

    // Step from left to right:
    float ac_slope = (cy - ay) / (float)(cx - ax);
    float ab_slope = (by - ay) / (float)(bx - ax);
    float bc_slope = (cy - by) / (float)(cx - bx);

    for (int x = ax + 1; x < cx; x++)
    {
        if (x <= bx)
        {
            int ab_y = std::floor(ab_slope * (x - ax) + ay);
            int ac_y = std::floor(ac_slope * (x - ax) + ay);
            line(x, ab_y, x, ac_y, framebuffer, color);
        }
        else
        {
            int bc_y = std::floor(bc_slope * (x - bx) + by);
            int ac_y = std::floor(ac_slope * (x - ax) + ay);
            line(x, bc_y, x, ac_y, framebuffer, color);
        }
    }
}

int main (int argc, char** argv) 
{
    constexpr int width  = 200;
    constexpr int height = 200;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

