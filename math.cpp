#include <math.h>

inline Vec3 VEC3(float x, float y, float z)
{
    Vec3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

inline Vec3 operator+(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

inline Vec3 &operator+=(Vec3 &a, Vec3 b)
{
    a = a + b;
    return a;
}

inline Vec3 operator-(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

inline Vec3 &operator-=(Vec3 &a, Vec3 b)
{
    a = a - b;
    return a;
}

inline Vec3 operator*(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

inline Vec3 &operator*=(Vec3 &a, Vec3 b)
{
    a = a * b;
    return a;
}

inline Vec3 operator*(Vec3 a, float s)
{
    Vec3 result;
    result.x = a.x * s;
    result.y = a.y * s;
    result.z = a.z * s;
    return result;
}

inline Vec3 &operator*=(Vec3 &a, float s)
{
    a = a * s;
    return a;
}

inline Vec3 operator/(Vec3 a, float s)
{
    Vec3 result;
    result.x = a.x / s;
    result.y = a.y / s;
    result.z = a.z / s;
    return result;
}

inline Vec3 &operator/=(Vec3 &a, float s)
{
    a = a / s;
    return a;
}

inline float Square(float a)
{
    return a * a;
}

inline float Length(Vec3 a)
{
    return sqrt(Square(a.x) + Square(a.y) + Square(a.z));
}

inline Vec3 Norm(Vec3 a)
{
    return a / Length(a);
}

inline float Dot(Vec3 a, Vec3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline Vec3 Cross(Vec3 a, Vec3 b)
{
    Vec3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

inline Vec3 Reflect(Vec3 v, Vec3 n)
{
    // Vec3 nn = 2 * Dot(v, n) * n - v;
    return n * Dot(v, n) * 2 - v;
    // return vec3(nn.x - v.x, nn.y - v.y, nn.z - v.z);
}



///////////////////////////////
// OPENCL FUNCTION OVERLOADS
//

inline cl_float3 operator*(cl_float3 a, float s)
{
	a = { a.x * s, a.y * s, a.z * s};
	return a;
}

inline cl_float3 &operator*=(cl_float3 &a, float b)
{
    a = a * b;
    return a;
}

inline cl_float3 operator+(cl_float3 a, cl_float3 b)
{
    cl_float3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

inline cl_float3 &operator+=(cl_float3 &a, cl_float3 b)
{
    a = a + b;
    return a;
}

inline cl_float3 operator-(cl_float3 a, cl_float3 b)
{
    cl_float3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

inline cl_float3 &operator-=(cl_float3 &a, cl_float3 b)
{
    a = a - b;
    return a;
}

inline cl_float3 operator/(cl_float3 a, float s)
{
    cl_float3 result;
    result.x = a.x / s;
    result.y = a.y / s;
    result.z = a.z / s;
    return result;
}

inline cl_float3 &operator/=(cl_float3 &a, float s)
{
    a = a / s;
    return a;
}

inline cl_float3 Norm(cl_float3 a)
{
    return a / Length(a);
}

inline float Length(cl_float3 a)
{
    return sqrt(Square(a.x) + Square(a.y) + Square(a.z));
}

inline float Dot(cl_float3 a, cl_float3 b)
{
    return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

inline cl_float3 Cross(cl_float3 a, cl_float3 b)
{
    cl_float3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}
