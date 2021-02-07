#pragma once
#include "raytracer.h"
#include "math.h"

#pragma pack(push, 1)
struct Plane
{
    cl_float3 a;
    cl_float3 n;
    Material mat;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Sphere
{
    cl_float3 c;
    float r;
    Material mat;
};
#pragma pack(pop)

enum GeometryType{
    Geo_Sphere,
    Geo_Plane
};

#pragma pack(push, 1)
struct Geometry
{
    int id;
    GeometryType type;
};
#pragma pack(pop)
