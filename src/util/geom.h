#ifndef GEOM_H_INCLUDED
#define GEOM_H_INCLUDED


namespace GEOM
{

struct Vec2
{
    static const Vec2 null;
    float x, y;

    Vec2() = default;
    Vec2(float X, float Y) : x(X), y(Y){}

    float* ptr(){return &x;}
    const float* ptr() const {return &x;}
};

struct Mat2
{
    static const Mat2 id;
    float entry[4];

    Mat2() = default;
    Mat2(float a11, float a12, float a21, float a22)
    {
        entry[0] = a11;
        entry[1] = a12;
        entry[2] = a21;
        entry[3] = a22;
    }

    Vec2 operator()(const Vec2& v) const
    {
        return Vec2(entry[0] * v.x + entry[1] * v.y, entry[2] * v.x + entry[3] * v.y);
    }
};

inline bool operator==(const Vec2& v1, const Vec2& v2)
{
    return v1.x == v2.x && v1.y == v2.y;
}

inline bool operator==(const Mat2& m1, const Mat2& m2)
{
    for(int i = 0; i < 4; ++i)
    {
        if(m1.entry[i] != m2.entry[i])
            return false;
    }
    return true;
}

inline Vec2 operator+(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1.x + v2.x, v1.y + v2.y);
}

inline Vec2& operator+=(Vec2& v1, const Vec2& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	return v1;
}

inline Vec2 operator-(const Vec2& v)
{
	return Vec2(-v.x, -v.y);
}

inline Vec2 operator-(const Vec2& v1, const Vec2& v2)
{
    return Vec2(v1.x - v2.x, v1.y - v2.y);
}

inline Vec2& operator-=(Vec2& v1, const Vec2& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	return v1;
}

inline Vec2 operator/(const Vec2& v, float scalar)
{
    return Vec2(v.x / scalar, v.y / scalar);
}

inline Vec2& operator/=(Vec2& v, float scalar)
{
	v.x *= scalar;
	v.y *= scalar;
	return v;
}

inline Vec2 operator*(float scalar, const Vec2& v)
{
    return Vec2(v.x * scalar, v.y * scalar);
}

inline Vec2 operator*(const Vec2& v, float scalar)
{
	return Vec2(v.x * scalar, v.y * scalar);
}

inline Vec2& operator*=(Vec2& v, float scalar)
{
	v.x *= scalar;
	v.y *= scalar;
	return v;
}

inline Mat2 operator*(float scalar, const Mat2& m)
{
    return Mat2(m.entry[0] * scalar, m.entry[1] * scalar,
               m.entry[2] * scalar, m.entry[3] * scalar);
}

inline Mat2 operator/(const Mat2& m, float scalar)
{
    return Mat2(m.entry[0] / scalar, m.entry[1] / scalar,
               m.entry[2] / scalar, m.entry[3] / scalar);
}

inline Vec2 operator*(const Mat2& m, const Vec2& v)
{
    return m(v);
}

inline Mat2 transpose(const Mat2& m)
{
	return Mat2(m.entry[0], m.entry[2],
				m.entry[1], m.entry[3]);
}

float length(const Vec2& v);

inline float sqlength(const Vec2& v)
{
	return v.x * v.x + v.y * v.y;
}

inline Vec2 normalize(const Vec2& v)
{
    return v / length(v);
}

inline float dot(const Vec2& v1, const Vec2& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

inline float det(const Mat2& m)
{
    return m.entry[0] * m.entry[3] - m.entry[1] * m.entry[2];
}

inline Mat2 invert(const Mat2& m)
{
    float d = det(m);
    if(d == 0)
    {
        Mat2 ret(0, 0, 0, 0);
        return ret;
    }

    Mat2 ret
    (m.entry[3], -m.entry[1],
     -m.entry[2], m.entry[0]);

    return ret / d;
}


inline Mat2 operator*(const Mat2& m1, const Mat2& m2)
{
    Mat2 ret;
    ret.entry[0] = m1.entry[0] * m2.entry[0] + m1.entry[1] * m2.entry[2];
    ret.entry[2] = m1.entry[2] * m2.entry[0] + m1.entry[3] * m2.entry[2];
    ret.entry[1] = m1.entry[0] * m2.entry[1] + m1.entry[1] * m2.entry[3];
    ret.entry[3] = m1.entry[2] * m2.entry[1] + m1.entry[3] * m2.entry[3];

    return ret;
}


// clockwise rotation around 0,0
Mat2 rot(float deg);
Mat2 rrot(float rad);

float get_angle(const Vec2& v);
float get_angle_r(const Vec2& v);

// ******** 3d *********

struct Vec3
{
    static const Vec3 null;
    float x, y, z;

    Vec3() = default;
    Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z){}
    Vec3(const Vec2& v) : x(v.x), y(v.y), z(0.0f){}

    float* ptr(){return &x;}
    const float* ptr() const {return &x;}
};

struct Mat3
{
    static const Mat3 id;
    float entry[9];

    Mat3() = default;
    Mat3(const Vec3& v1, const Vec3& v2, const Vec3& v3)
    {
        entry[0] = v1.x;
        entry[1] = v2.x;
        entry[2] = v3.x;
        entry[3] = v1.y;
        entry[4] = v2.y;
        entry[5] = v3.y;
        entry[6] = v1.z;
        entry[7] = v2.z;
        entry[8] = v3.z;
    }

    Vec3 operator()(const Vec3& v) const
    {
        return Vec3(entry[0] * v.x + entry[1] * v.y + entry[2] * v.z,
                    entry[3] * v.x + entry[4] * v.y + entry[5] * v.z,
                    entry[6] * v.x + entry[7] * v.y + entry[8] * v.z);
    }
};

inline Mat3 operator*(const Mat3& m1, const Mat3& m2)
{
    Vec3 x(m2.entry[0], m2.entry[3], m2.entry[6]);
    Vec3 y(m2.entry[1], m2.entry[4], m2.entry[7]);
    Vec3 z(m2.entry[2], m2.entry[5], m2.entry[8]);

    return Mat3(m1(x), m1(y), m1(z));
}

inline Mat3 transpose(const Mat3& m)
{
    Vec3 x(m.entry[0], m.entry[1], m.entry[2]);
    Vec3 y(m.entry[3], m.entry[4], m.entry[5]);
    Vec3 z(m.entry[6], m.entry[7], m.entry[8]);

    return Mat3(x, y, z);
}

Mat3 xrot(float deg);
Mat3 yrot(float deg);
Mat3 zrot(float deg);
Mat3 rot(float deg, const Vec3& a);

inline bool operator==(const Vec3& v1, const Vec3& v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

inline Vec3 operator+(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline Vec3& operator+=(Vec3& v1, const Vec3& v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

inline Vec3 operator-(const Vec3& v)
{
	return Vec3(-v.x, -v.y, -v.z);
}

inline Vec3 operator-(const Vec3& v1, const Vec3& v2)
{
    return Vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline Vec3& operator-=(Vec3& v1, const Vec3& v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}

inline Vec3 operator/(const Vec3& v, float scalar)
{
    return Vec3(v.x / scalar, v.y / scalar, v.z / scalar);
}

inline Vec3 operator*(float scalar, const Vec3& v)
{
    return Vec3(v.x * scalar, v.y * scalar, v.z * scalar);
}

float length(const Vec3& v);

inline Vec3 normalize(const Vec3& v)
{
    return v / length(v);
}

inline float dot(const Vec3& v1, const Vec3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b)
{
    return Vec3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
                );
}

}

#endif // GEOM_H_INCLUDED
