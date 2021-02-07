#include "geometry.h"

inline Plane PLANE(cl_float3 a, cl_float3 n, Material mat)
{
    Plane result;
    result.a = a;
    result.n = n;
    result.mat = mat;
    return result;
}

inline Sphere SPHERE(cl_float3 c, float r, Material mat)
{
    Sphere result;
    result.c = c;
    result.r = r;
    result.mat = mat;
    return result;
}

// bool SphereHit(Sphere sphere, Ray ray, Hit *hit)
// {
//     float a = Dot(ray.d, ray.d);
//     float b = Dot(ray.d, (ray.o - sphere.c) * 2);
//     float c = Dot(ray.o - sphere.c, ray.o - sphere.c) - Square(sphere.r);
    
//     float t = 0;
//     float d = Square(b) - 4 * a * c;
//     if(d < 0) {
//         return false;
//     }
//     else if(d == 0) {
//         t = -0.5 * b / a;
//     }
//     else {
//         float tNeg = (-b - sqrt(d)) / (2 * a);
//         float tPos = (-b + sqrt(d)) / (2 * a);
//         t = MIN(tNeg, tPos);
//     }
    
//     if(t < 0) {
//         return false;
//     }
    
//     Vec3 normal = Norm((ray.o + (ray.d * t)) - sphere.c);
    
//     hit->t = t;
//     hit->ray = ray;
//     hit->normal = normal;
//     hit->mat = sphere.mat;
    
//     return true;
// }

// bool PlaneHit(Plane plane, Ray ray, Hit *hit)
// {
//     float denom = Dot(plane.n, ray.d);
//     if(abs(denom) > EPSILON) {
//         float t = Dot(plane.a - ray.o, plane.n) / denom;
//         if(t >= 0) {
//             hit->t = t;
//             hit->ray = ray;
//             hit->normal = plane.n;
//             hit->mat = plane.mat;
//             return true;
//         }
//     }
//     return false;
// }
