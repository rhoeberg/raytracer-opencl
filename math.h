#pragma once

#define MAX(a, b) (((a) >= (b)) ? (a) : (b))
#define MIN(a, b) (((a) <= (b)) ? (a) : (b))

#define PI 3.14159265358979323846264338327950288
#define RADIANS(degrees) ((degrees) * PI / 180.0)
#define DEGREES(radians) ((radians) * 180.0 / PI)

union Vec3
{
    struct {
	float x, y, z;
    };
    struct {
	float r, g, b;
    };
};

struct Mat3x3
{
// ROW MAJOR [ROW][COLUMN]
    float e[3][3];
};

inline Vec3 VEC3(float x, float y, float z);
inline Vec3 operator+(Vec3 a, Vec3 b);
inline Vec3 &operator+=(Vec3 &a, Vec3 b);
inline Vec3 operator-(Vec3 a, Vec3 b);
inline Vec3 &operator-=(Vec3 &a, Vec3 b);
inline Vec3 operator*(Vec3 a, Vec3 b);
inline Vec3 &operator*=(Vec3 &a, Vec3 b);
inline Vec3 operator*(Vec3 a, float s);
inline Vec3 &operator*=(Vec3 &a, float s);
inline Vec3 operator/(Vec3 a, float s);
inline Vec3 &operator/=(Vec3 &a, float s);
inline float Square(float a);
inline float Length(Vec3 a);
inline Vec3 Norm(Vec3 a);
inline float Dot(Vec3 a, Vec3 b);
inline Vec3 Cross(Vec3 a, Vec3 b);
inline Vec3 Reflect(Vec3 v, Vec3 n);
