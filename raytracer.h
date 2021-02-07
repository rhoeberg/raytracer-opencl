#pragma once

#include "world.h"

struct World;

#pragma pack(push, 1)
struct Material
{
    cl_float3 diffuse;
    cl_float3 specular;
    cl_float3 reflection;

    float shine;
    bool mirror;
};
#pragma pack(pop)

struct Ray
{
    Vec3 o;
    Vec3 d;
};

struct Hit
{
    float t;
    Ray ray;
    Vec3 normal;
    Material mat;
};

struct HitData
{
    World *world;
    Ray ray;
    bool nolight;
    int n;
    
    Vec3 *outColor;
};

inline Material MATERIAL(cl_float3 diffuse, cl_float3 specular, cl_float3 reflection, float shine);
inline Ray RAY(Vec3 o, Vec3 d);
/* bool WorldHitGeometry(World *world, Ray ray, Hit *hit); */
/* Vec3 WorldHit(World *world, Ray ray, bool nolight = false, int n = 0); */
