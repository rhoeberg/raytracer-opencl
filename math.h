#pragma once
#include <CL/cl.h>
#include <CL/cl_gl.h>
#include <CL/cl_gl.h>

#define MAX(a, b) (((a) >= (b)) ? (a) : (b))
#define MIN(a, b) (((a) <= (b)) ? (a) : (b))

#define PI 3.14159265358979323846264338327950288
#define RADIANS(degrees) ((degrees) * PI / 180.0)
#define DEGREES(radians) ((radians) * 180.0 / PI)

#pragma pack(push, 1)
__declspec(align(16)) union Vec3
/* union Vec3 */
{
    struct {
		float x, y, z;
    };
    struct {
		float r, g, b;
    };
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Mat3x3
{
// ROW MAJOR [ROW][COLUMN]
    float e[3][3];
};
#pragma pack(pop)

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

// OPENCL FUNCTIONS
inline cl_float3 operator+(cl_float3 a, cl_float3 b);
inline cl_float3 &operator+=(cl_float3 &a, cl_float3 b);
inline cl_float3 operator-(cl_float3 a, cl_float3 b);
inline cl_float3 &operator-=(cl_float3 &a, cl_float3 b);
inline cl_float3 operator*(cl_float3 a, float s);
inline cl_float3 &operator*=(cl_float3 &a, float s);
inline cl_float3 operator/(cl_float3 a, float s);
inline cl_float3 &operator/=(cl_float3 &a, float s);

inline cl_float3 Norm(cl_float3 a);
inline float Length(cl_float3 a);
inline float Dot(cl_float3 a, cl_float3 b);
inline cl_float3 Cross(cl_float3 a, cl_float3 b);

/////////////////////////////
// c math abstraction
inline double Atan2(double a, double b);
inline double Asin(double a);
