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

// Shoelace formula
double signed_triangle_area (int ax, int ay, int bx, int by, int cx, int cy)
{
    return 0.5 * ((by * ax - ay * bx) + (cy * bx - by * cx) + (ay * cx - cy * ax));
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor a_color, TGAColor b_color, TGAColor c_color) 
{
    // line(ax, ay, bx, by, framebuffer, color);
    // line(bx, by, cx, cy, framebuffer, color);
    // line(cx, cy, ax, ay, framebuffer, color);

    // Calculate the bouding box:
    int boundMinX = std::min(std::min(ax, bx), cx);
    int boundMinY = std::min(std::min(ay, by), cy);
    int boundMaxX = std::max(std::max(ax, bx), cx);
    int boundMaxY = std::max(std::max(ay, by), cy);

    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);

    // Iterate each point in AABB and judging if it's in triangle:
    for (int x = boundMinX; x <= boundMaxX; x++)
        for (int y = boundMinY; y <= boundMaxY; y++)
        {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha < 0 || beta < 0 || gamma < 0)
                continue;
            double frac = alpha + beta + gamma;
            double max_weight = std::max(std::max(alpha, beta), gamma);
            double min_weight = std::min(std::min(alpha, beta), gamma);
            uint8_t B = (alpha * a_color[0] + beta * b_color[0] + gamma * c_color[0]) / frac;
            uint8_t G = (alpha * a_color[1] + beta * b_color[1] + gamma * c_color[1]) / frac;
            uint8_t R = (alpha * a_color[2] + beta * b_color[2] + gamma * c_color[2]) / frac;
            uint8_t A = (alpha * a_color[3] + beta * b_color[3] + gamma * c_color[3]) / frac;
            TGAColor color = {B, G, R, A};
            if (min_weight < 0.1)
                framebuffer.set(x, y, color);
        }
}

int main (int argc, char** argv) 
{
    constexpr int width  = 200;
    constexpr int height = 200;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    // triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    // triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    // triangle(115, 83, 80,  90, 85, 120, framebuffer, green);

    triangle(0, 0, 150, 50, 50, 160, framebuffer, red, green, blue);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

