#pragma once

#include "world.h"

struct World;

struct Material
{
    Vec3 diffuse;
    Vec3 specular;
    
    bool mirror;
    Vec3 reflection;
    float shine;
};

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

inline Material MATERIAL(Vec3 diffuse, Vec3 specular, Vec3 reflection, float shine);
inline Ray RAY(Vec3 o, Vec3 d);
bool WorldHitGeometry(World *world, Ray ray, Hit *hit);
Vec3 WorldHit(World *world, Ray ray, bool nolight = false, int n = 0);