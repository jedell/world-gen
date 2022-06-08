#include <raylib.h>
#include "raymath.h"
#include <iostream>

inline Vector3 Vector3init(float x, float y, float z)
{
    Vector3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

inline Color get_color(Vector3 vec, Color *colors, int width, int height)
{
    Vector3 d = Vector3Negate(Vector3Normalize(vec));
    float u = 0.5 + atan2(d.x, d.z) / (2 * M_PI);
    float v = 0.5 + asin(d.y) / M_PI;
    int x = u * width;
    int y = v * height;
    std::cout << x << " " << y << std::endl;
    int index = (y * width) + x;
    std::cout << index << std::endl;
    return colors[index];
}