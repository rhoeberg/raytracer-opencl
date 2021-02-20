/* date = July 10th 2020 7:11 pm */

#ifndef WORLD_H
#define WORLD_H

#include "geometry.h"
#include "light.h"

#define MAX_PLANES 2
#define MAX_SPHERES 2
#define MAX_AABB 2
#define MAX_GEOMETRIES (MAX_PLANES + MAX_SPHERES + MAX_AABB)
#define MAX_POINTLIGHTS 16

#pragma pack(push, 1)
struct World
{
    cl_float3 bgCol;
    cl_float3 ambient;
    
    Plane planes[MAX_PLANES];
    Sphere spheres[MAX_SPHERES];
    AABB AABBs[MAX_AABB];
    
    int maxPlanes;
    int maxSpheres;
    int maxPointLights;
    
    int planeCount;
    int sphereCount;
    int aabbCount;
    
    Geometry geometries[MAX_GEOMETRIES];
    int geometryCount;
    
    DirLight dirLight;
    
    PointLight pointLights[MAX_POINTLIGHTS];
    int pointLightCount;

	Camera cam;
};
#pragma pack(pop)

#endif //WORLD_H
