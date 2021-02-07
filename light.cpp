#include "light.h"
#include <math.h>

inline DirLight DIRLIGHT(cl_float3 dir, cl_float3 color)
{
    DirLight result;
    result.dir = dir;
    result.color = color;
    return result;
}

inline PointLight POINTLIGHT(cl_float3 pos, cl_float3 color)
{
    PointLight result;
    result.pos = pos;
    result.color = color;
    return result;
}

// Vec3 LightGetColor(Vec3 l, Vec3 lightColor, Hit hit)
// {
//     Vec3 p = hit.ray.o + (hit.ray.d * hit.t);
    
//     Vec3 col = hit.mat.diffuse * lightColor;
//     float nl = Dot(hit.normal, l);
//     col = col * MAX(0.0f, nl);
	
//     Vec3 col2 = hit.mat.specular * lightColor;
//     Vec3 e = Norm(hit.ray.o - p);
//     Vec3 rl = Reflect(l, hit.normal);
//     float rle = Dot(e, rl);
//     col2 *= pow(MAX(0.0f, rle), hit.mat.shine);
    
//     Vec3 result = col + col2;
    
//     return result;
// }

// bool PointLightIlluminates(World *world, PointLight light, Vec3 point)
// {
//     Vec3 pointFrom = light.pos - point;
//     Vec3 adjustedPoint = point + (pointFrom * EPSILON);
//     float tl = Length(light.pos - point) / Length(pointFrom);
//     Ray ray = RAY(adjustedPoint, pointFrom);
//     Hit hit;
//     if(WorldHitGeometry(world, ray, &hit)) {
//         if(hit.t < tl) {
//             return false;
//         }
//     }
//     return true;
// }
