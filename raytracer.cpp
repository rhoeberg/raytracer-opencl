#include "raytracer.h"

inline Material MATERIAL(Vec3 diffuse, Vec3 specular, Vec3 reflection, float shine)
{
    Material result;
    result.diffuse = diffuse;
    result.specular = specular;
    result.reflection = reflection;
    result.shine = shine;
    result.mirror = false;
    return result;
}

inline Ray RAY(Vec3 o, Vec3 d)
{
    Ray result;
    result.o = o;
    result.d = d;
    return result;
}

bool WorldHitGeometry(World *world, Ray ray, Hit *hit)
{
    bool isHit = false;
    Hit nextHit;
    Hit closestHit;
    
    for (int i = 0; i < world->geometryCount; i++)
    {
        bool hitSuccess = false;
        if (world->geometries[i].type == Geo_Sphere)
        {
            hitSuccess = SphereHit(world->spheres[world->geometries[i].id], ray, &nextHit);
        }
        else if (world->geometries[i].type = Geo_Plane)
        {
            hitSuccess = PlaneHit(world->planes[world->geometries[i].id], ray, &nextHit);
        }
        
        if (hitSuccess)
        {
            if (!isHit)
            {
                isHit = true;
                closestHit = nextHit;
            }
            else if (nextHit.t < closestHit.t)
            {
                closestHit = nextHit;
            }
        }
    }
    *hit = closestHit;
    return isHit;
}

Vec3 WorldHit(World *world, Ray ray, bool nolight, int n)
{
    n++;
    
    Hit hit;
    if (WorldHitGeometry(world, ray, &hit))
    {
        return hit.mat.diffuse;
        
        Vec3 p = ray.o + (ray.d * hit.t);
        
        Vec3 result = hit.mat.diffuse * world->ambient;
        
        for(int i = 0; i < world->pointLightCount; i++)
        {
            if (PointLightIlluminates(world, world->pointLights[i], p))
            {
                result += LightGetColor(Norm(world->dirLight.dir * -1), world->dirLight.color, hit);
                Vec3 pointFrom = Norm(world->pointLights[i].pos - p);
                result += LightGetColor(pointFrom, world->pointLights[i].color, hit);
            }
        }
        
        
        if (hit.mat.mirror)
        {
            if (n < 100)
            {
                Vec3 reflectionDir = Norm(Reflect(hit.ray.o - p, hit.normal));
                Vec3 adjustedP = p + (reflectionDir * EPSILON);
                Ray reflectionRay;
                reflectionRay.o = adjustedP;
                reflectionRay.d = reflectionDir;
                result += hit.mat.reflection * WorldHit(world, reflectionRay, false, n);
            }
        }
        
        return result;
    }
    
    return world->bgCol;
}
