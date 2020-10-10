#pragma once
#include "raytracer.h"

struct DirLight
{
    Vec3 dir;
    Vec3 color;
};

struct PointLight
{
    Vec3 pos;
    Vec3 color;
};

inline DirLight DIRLIGHT(Vec3 dir, Vec3 color);
inline PointLight POINTLIGHT(Vec3 pos, Vec3 color);
Vec3 LightGetColor(Vec3 l, Vec3 lightColor, Hit hit);
bool PointLightIlluminates(World *world, PointLight light, Vec3 point);
