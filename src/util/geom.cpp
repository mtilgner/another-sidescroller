#include "geom.h"
#include <cmath>

namespace GEOM
{

static const float PI = 3.14159265;
static const float DEG_TO_RAD = PI / 180.0f;

const Vec2 Vec2::null(0.0f, 0.0f);
const Vec3 Vec3::null(0.0f, 0.0f, 0.0f);

const Mat2 Mat2::id(1.0f, 0.0f,
                    0.0f, 1.0f);

const Mat3 Mat3::id(Vec3(1.0f, 0.0f, 0.0f),
                    Vec3(0.0f, 1.0f, 0.0f),
                    Vec3(0.0f, 0.0f, 1.0f));

float length(const Vec2& v)
{
    return std::sqrt(dot(v, v));
}

Mat2 rot(float deg)
{
    float rad = DEG_TO_RAD * deg;
    float s = std::sin(rad);
    float c = std::cos(rad);

    return Mat2
    (c, s,
    -s, c);
}

Mat2 rrot(float rad)
{
    float s = std::sin(rad);
    float c = std::cos(rad);

    return Mat2
    (c, s,
    -s, c);
}

float get_angle(const Vec2& v)
{
	return std::atan2(v.x, v.y) / DEG_TO_RAD - 90.0f;
}

float get_angle_r(const Vec2& v)
{
	return std::atan2(v.x, v.y);
}

Mat3 xrot(float deg)
{
    float rad = DEG_TO_RAD * deg;
    float s = std::sin(rad);
    float c = std::cos(rad);

    Vec3 x(1.0f, 0.0f, 0.0f);
    Vec3 y(0.0f, c, -s);
    Vec3 z(0.0f, s, c);

    return Mat3(x, y, z);
}

Mat3 yrot(float deg)
{
    float rad = DEG_TO_RAD * deg;
    float s = std::sin(rad);
    float c = std::cos(rad);

    Vec3 x(c, 0.0f, s);
    Vec3 y(0.0f, 1.0f, 0.0f);
    Vec3 z(-s, 0.0f, c);

    return Mat3(x, y, z);
}

Mat3 zrot(float deg)
{
    float rad = DEG_TO_RAD * deg;
    float s = std::sin(rad);
    float c = std::cos(rad);

    Vec3 x(c, -s, 0.0f);
    Vec3 y(s, c, 0.0f);
    Vec3 z(0.0f, 0.0f, 1.0f);

    return Mat3(x, y, z);
}

Mat3 rot(float deg, const Vec3& a)
{
    float rad = DEG_TO_RAD * deg;
    float s = std::sin(rad);
    float c = std::cos(rad);

    Vec3 x(c + a.x * a.x * (1 - c),
           a.x * a.y * (1 - c) + a.z * s,
           a.z * a.x * (1 - c) - a.y * s);

    Vec3 y(a.x * a.y * (1 - c),
           c + a.y * a.y * (1 - c),
           a.z * a.y * (1 - c) + a.x * s);

    Vec3 z(a.x * a.z * (1 - c) + a.y * s,
           a.y * a.z * (1 - c) - a.x * s,
           c + a.z * a.z * (1 - c));

    return Mat3(x, y, z);
}

float length(const Vec3& v)
{
    return std::sqrt(dot(v, v));
}

}
