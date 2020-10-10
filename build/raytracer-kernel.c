#define EPSILON 0.0001
#define MAX_PLANES 16
#define MAX_SPHERES 16
#define MAX_GEOMETRIES (MAX_PLANES * MAX_SPHERES)
#define MAX_POINTLIGHTS 16

struct DirLight
{
    float3 dir;
    float3 color;
};

struct PointLight
{
    float3 pos;
    float3 color;
};

struct Material
{
    float3 diffuse;
    float3 specular;
    
    bool mirror;
    float3 reflection;
    float shine;
};

struct Ray
{
    float3 o;
    float3 d;
};

struct Plane
{
    float3 a;
    float3 n;
    struct Material mat;
};

struct Sphere
{
    float3 c;
    float r;
    struct Material mat;
};

enum GeometryType{
    Geo_Sphere,
    Geo_Plane
};

struct Geometry
{
    int id;
    enum GeometryType type;
};

struct World
{
    float3 bgCol;
    float3 ambient;
    
    struct Plane planes[MAX_PLANES];
    struct Sphere spheres[MAX_SPHERES];
    
    int maxPlanes;
    int maxSpheres;
    int maxPointLights;
    
    int planeCount;
    int sphereCount;
    
    struct Geometry geometries[MAX_GEOMETRIES];
    int geometryCount;
    
    struct DirLight dirLight;
    
    struct PointLight pointLights[MAX_POINTLIGHTS];
    int pointLightCount;
};


struct Hit
{
    float t;
    struct Ray ray;
    float3 normal;
    struct Material mat;
};


bool SphereHit(struct Sphere sphere, struct Ray ray, struct Hit *hit)
{
    float a = dot(ray.d, ray.d);
    float b = dot(ray.d, (ray.o - sphere.c) * 2);
    float c = dot(ray.o - sphere.c, ray.o - sphere.c) - sqrt(sphere.r);
    
    float t = 0;
    float d = sqrt(b) - 4 * a * c;
    if(d < 0) {
        return false;
    }
    else if(d == 0) {
        t = -0.5 * b / a;
    }
    else {
        float tNeg = (-b - sqrt(d)) / (2 * a);
        float tPos = (-b + sqrt(d)) / (2 * a);
        t = min(tNeg, tPos);
    }
    
    if(t < 0) {
        return false;
    }
    
    float3 normal = normalize((ray.o + (ray.d * t)) - sphere.c);
    
    hit->t = t;
    hit->ray = ray;
    hit->normal = normal;
    hit->mat = sphere.mat;
    
    return true;
}

bool PlaneHit(struct Plane plane, struct Ray ray, struct Hit *hit)
{
    float denom = dot(plane.n, ray.d);
    if(fabs(denom) > EPSILON) {
        float t = dot(plane.a - ray.o, plane.n) / denom;
        if(t >= 0) {
            hit->t = t;
            hit->ray = ray;
            hit->normal = plane.n;
            hit->mat = plane.mat;
            return true;
        }
    }
    return false;
}

inline struct Ray RAY(float3 o, float3 d)
{
    struct Ray result;
    result.o = o;
    result.d = d;
    return result;
}

inline float3 Reflect(float3 v, float3 n)
{
    return n * dot(v, n) * 2 - v;
}


float3 LightGetColor(float3 l, float3 lightColor, struct Hit hit)
{
    float3 p = hit.ray.o + (hit.ray.d * hit.t);
    
    float3 col = hit.mat.diffuse * lightColor;
    float nl = dot(hit.normal, l);
    col = col * max(0.0f, nl);
	
    float3 col2 = hit.mat.specular * lightColor;
    float3 e = normalize(hit.ray.o - p);
    float3 rl = Reflect(l, hit.normal);
    float rle = dot(e, rl);
    col2 *= pow(max(0.0f, rle), hit.mat.shine);
    
    float3 result = col + col2;
    
    return result;
}

bool WorldHitGeometry(__global const struct World *world, struct Ray ray, struct Hit *hit)
{
    bool isHit = false;
    struct Hit nextHit;
    struct Hit closestHit;
    
    int i;
    for (i = 0; i < world->geometryCount; i++)
    {
        bool hitSuccess = false;
        if (world->geometries[i].type == Geo_Sphere)
        {
            hitSuccess = SphereHit(world->spheres[world->geometries[i].id], ray, &nextHit);
        }
        else if (world->geometries[i].type == Geo_Plane)
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


bool PointLightIlluminates(__global const struct World *world, struct PointLight light, float3 point)
{
    float3 pointFrom = light.pos - point;
    float3 epsilonV = { EPSILON, EPSILON, EPSILON };
    float3 adjustedPoint = point + (pointFrom * epsilonV);
    float tl = length(light.pos - point) / length(pointFrom);
    struct Ray ray = RAY(adjustedPoint, pointFrom);
    struct Hit hit;
    if(WorldHitGeometry(world, ray, &hit)) {
        if(hit.t < tl) {
            return false;
        }
    }
    return true;
}



float3 WorldHit(__global const struct World *world, struct Ray ray, int n)
{
    n++;
    
    struct Hit hit;
    if (WorldHitGeometry(world, ray, &hit))
    {
        //if (nolight)
        //return hit.mat.diffuse;
        
        float3 p = ray.o + (ray.d * hit.t);
        
        float3 result = hit.mat.diffuse * world->ambient;
        
        int i;
        for(i = 0; i < world->pointLightCount; i++)
        {
            if (PointLightIlluminates(world, world->pointLights[i], p))
            {
                result += LightGetColor(normalize(world->dirLight.dir * -1), world->dirLight.color, hit);
                float3 pointFrom = normalize(world->pointLights[i].pos - p);
                result += LightGetColor(pointFrom, world->pointLights[i].color, hit);
            }
        }
        
        
        if (hit.mat.mirror)
        {
            if (n < 100)
            {
                float3 reflectionDir = normalize(Reflect(hit.ray.o - p, hit.normal));
                float3 epsilonV = { EPSILON, EPSILON, EPSILON };
                float3 adjustedP = p + (reflectionDir * epsilonV);
                struct Ray reflectionRay;
                reflectionRay.o = adjustedP;
                reflectionRay.d = reflectionDir;
                result += hit.mat.reflection * WorldHit(world, reflectionRay, n);
            }
        }
        
        return result;
    }
    
    return world->bgCol;
}


// OpenCL Kernel Function for element by element
__kernel void WorldHitKernel(__global const struct World *world, __global const struct Ray *rays,  __global float3 *pixelData)
{
    int i = get_global_id(0);
    
    float3 col = WorldHit(world, rays[i], 0);
    pixelData[i] = col;
}

