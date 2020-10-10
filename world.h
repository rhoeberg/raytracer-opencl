/* date = July 10th 2020 7:11 pm */

#ifndef WORLD_H
#define WORLD_H

#include "geometry.h"
#include "light.h"

#define MAX_PLANES 16
#define MAX_SPHERES 16
#define MAX_GEOMETRIES (MAX_PLANES * MAX_SPHERES)
#define MAX_POINTLIGHTS 16

struct World
{
    Vec3 bgCol;
    Vec3 ambient;
    
    Plane planes[MAX_PLANES];
    Sphere spheres[MAX_SPHERES];
    
    int maxPlanes;
    int maxSpheres;
    int maxPointLights;
    
    int planeCount;
    int sphereCount;
    
    Geometry geometries[MAX_GEOMETRIES];
    int geometryCount;
    
    DirLight dirLight;
    
    PointLight pointLights[MAX_POINTLIGHTS];
    int pointLightCount;
};

#endif //WORLD_H
