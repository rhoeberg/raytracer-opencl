#pragma once
#include "raytracer.h"

#pragma pack(push, 1)
struct DirLight
{
    cl_float3 dir;
    cl_float3 color;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PointLight
{
    cl_float3 pos;
    cl_float3 color;
};
#pragma pack(pop)

inline DirLight DIRLIGHT(cl_float3 dir, cl_float3 color);
inline PointLight POINTLIGHT(cl_float3 pos, cl_float3 color);
/* Vec3 LightGetColor(Vec3 l, Vec3 lightColor, Hit hit); */

/* bool PointLightIlluminates(World *world, PointLight light, Vec3 point); */
