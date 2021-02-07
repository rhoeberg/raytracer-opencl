#pragma once
#include "raytracer.h"

#pragma pack(push, 1)
struct DirLight
{
    Vec3 dir;
    Vec3 color;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct PointLight
{
    Vec3 pos;
    Vec3 color;
};
#pragma pack(pop)

inline DirLight DIRLIGHT(Vec3 dir, Vec3 color);
inline PointLight POINTLIGHT(Vec3 pos, Vec3 color);
Vec3 LightGetColor(Vec3 l, Vec3 lightColor, Hit hit);
bool PointLightIlluminates(World *world, PointLight light, Vec3 point);
