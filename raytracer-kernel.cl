#define EPSILON 0.0001
#define MAX_PLANES 16
#define MAX_SPHERES 16
#define MAX_GEOMETRIES (MAX_PLANES * MAX_SPHERES)
#define MAX_POINTLIGHTS 16

struct __attribute__ ((packed)) DirLight
{
    float3 dir;
    float3 color;
};

struct __attribute__ ((packed)) PointLight
{
    float3 pos;
    float3 color;
};

struct __attribute__ ((packed)) Material
{
    float3 diffuse;
    float3 specular;
    float3 reflection;
    
    float shine;
    bool mirror;
};

struct __attribute__ ((packed)) Ray
{
    float3 o;
    float3 d;
};

struct __attribute__ ((packed)) Plane
{
    float3 a;
    float3 n;
    struct Material mat;
};

struct __attribute__ ((packed)) Sphere
{
    float3 c;
    float r;
    struct Material mat;
};

enum GeometryType{
    Geo_Sphere,
    Geo_Plane
};

struct __attribute__ ((packed)) Geometry
{
    int id;
    enum GeometryType type;
};

struct __attribute__ ((packed)) World
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

struct __attribute__ ((packed)) Vec3
{
	float x;
	float y;
	float z;
};

struct Hit
{
    float t;
    struct Ray ray;
    float3 normal;
    struct Material mat;
};

inline float Square(float a)
{
    return a * a;
}

bool SphereHit(struct Sphere sphere, struct Ray ray, struct Hit *hit)
{
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);
	if(x == 0 && y == 0) {
		printf("sphere hit\n");
	}
    
    float a = dot(ray.d, ray.d);
    float b = dot(ray.d, (ray.o - sphere.c) * 2.0f);
    float c = dot(ray.o - sphere.c, ray.o - sphere.c) - Square(sphere.r);
    
    float t = 0.0f;
    
    
    
    float d = Square(b) - 4.0f * a * c;
    
    
    if(d < 0.0f) {
        return false;
    }
    else if(d == 0.0f) {
        t = -0.5f * b / a;
        
    }
    else {
        float tNeg = (-b - sqrt(d)) / (2.0f * a);
        float tPos = (-b + sqrt(d)) / (2.0f * a);
        t = min(tNeg, tPos);
        
        
        
    }
    
    if(t < 0.0f) {
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
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);
	if(x == 0 && y == 0) {
		printf("plane hit\n");
	}

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
    
    
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);
	if(x == 0 && y == 0) {
		printf("WorldHitGeometry() sphere amount: %d\n", world->sphereCount);
	}
    
    
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

		if(x == 0 && y == 0) {
			printf("here\n");
		}

    }
    *hit = closestHit;
    return isHit;
}


bool PointLightIlluminates(__global const struct World *world, struct PointLight light, float3 point)
{
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);
	if(x == 0 && y == 0) {
		printf("PointLightIlluminates sphere amount: %d\n", world->sphereCount);
	}

    float3 pointFrom = light.pos - point;
    float3 epsilonV = (float3)( EPSILON, EPSILON, EPSILON );
    float3 adjustedPoint = point + (pointFrom * epsilonV);
    float tl = length(light.pos - point) / length(pointFrom);
    struct Ray ray = RAY(adjustedPoint, pointFrom);
    struct Hit hit;
    if(WorldHitGeometry(world, ray, &hit)) {
        if(hit.t < tl) {
            return false;
        }
    }
	
	if(x == 0 && y == 0) {
		printf("test\n");
	}
        
    return true;
}



float3 WorldHit(__global const struct World *world, struct Ray ray, int n)
{
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);
	if(x == 0 && y == 0) {
		printf("WorldHit() sphere amount: %d\n", world->sphereCount);
	}

    n++;
    
    struct Hit hit;
    if (WorldHitGeometry(world, ray, &hit))
    {

		if(x == 0 && y == 0) {
			printf("here\n");
		}

        float3 p = ray.o + (ray.d * hit.t);
        float3 result = hit.mat.diffuse * world->ambient;
        
        result += LightGetColor(normalize(world->dirLight.dir * -1), world->dirLight.color, hit);
        
        
        int i;
        for(i = 0; i < world->pointLightCount; i++)
        {
            if (PointLightIlluminates(world, world->pointLights[i], p))
            {
                float3 pointFrom = normalize(world->pointLights[i].pos - p);
                result += LightGetColor(pointFrom, world->pointLights[i].color, hit);
            }
        }
        
        if (hit.mat.mirror)
        {
            if (n < 2)
            {
                float3 reflectionDir = normalize(Reflect(hit.ray.o - p, hit.normal));
                float3 epsilonV = (float3)(EPSILON, EPSILON, EPSILON);
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
__kernel void WorldHitKernel(__global struct World *world2, int width, int height, __write_only image2d_t texture, __global struct Ray *outputDebug,
                             float time)
{
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);
    
    float3 o = (float3)(time * 0.03f, 0.0f, 0.0f);
    float3 g = (float3)(0.0f, 0.0f, -1.0f);
    float3 w = normalize(g * -1.0f);
    float3 u = cross((float3)(0.0f, 1.0f, 0.0f), w);
    float3 v = cross(w, u);
    
    float3 t = (float3)(0.0f, 1.0f, 0.0f);
    float angle = 0.785f;
    
    float3 rw = (w * -1.0f) * ((float)height / 2.0f) / tan(angle / 2.0f);
    float3 ru = u * (x - ((float)width - 1.0f) / 2.0f);
    float3 rv = v * (y - ((float)height - 1.0f) / 2.0f);
    float3 r = rw + ru + rv;
    float3 d = normalize(r);
    struct Ray ray;
    ray.o = o;
    ray.d = d;
    
    /* struct World world; */
    /* world.bgCol = (float3)(1.0f, 1.0f, 0.0f); */
    /* world.ambient = (float3)(0.0f, 0.0f, 0.0f); */
    /* //world.sphereCount = 2; */
    /* world.geometryCount = 0; */
    /* world.sphereCount = 0; */
    /* world.planeCount = 0; */
    /* { */
    /*     world.spheres[0].mat.diffuse = (float3)(0.8f,0.8f,0.0f); */
    /*     world.spheres[0].mat.specular = (float3)(0.5f, 0.5f, 0.5f); */
    /*     world.spheres[0].mat.shine = 100.0f; */
    /*     world.spheres[0].c = (float3)(-0.8f, sin(time * 0.9f) * 0.2f, -3.0f); */
    /*     world.spheres[0].r = 0.5f; */
    /*     world.spheres[0].mat.mirror = true; */
    /*     world.spheres[0].mat.reflection = (float3)(0.5f, 0.5f, 0.5f); */
    /*     world.geometries[world.geometryCount].id = world.sphereCount; */
    /*     world.geometries[world.geometryCount].type = Geo_Sphere; */
    /*     world.geometryCount += 1; */
    /*     world.sphereCount += 1; */
    /* } */
    
    /* { */
    /*     world.spheres[1].mat.diffuse = (float3)(0.0f,0.8f,0.8f); */
    /*     world.spheres[1].mat.specular = (float3)(0.5f, 0.5f, 0.5f); */
    /*     world.spheres[1].mat.shine = 100.0f; */
    /*     float3 center = (float3)(0.8f, 0.0f, -3.0f); */
    /*     world.spheres[1].c = center; */
    /*     world.spheres[1].r = 0.5f; */
    /*     world.spheres[1].mat.mirror = true; */
    /*     world.spheres[1].mat.reflection = (float3)(0.5f, 0.5f, 0.5f); */
    /*     world.geometries[world.geometryCount].id = world.sphereCount; */
    /*     world.geometries[world.geometryCount].type = Geo_Sphere; */
    /*     world.geometryCount += 1; */
    /*     world.sphereCount += 1; */
    /* } */
    
    /* { */
    /*     world.planes[0].a = (float3)(0, 0.5f, 0); */
    /*     world.planes[0].n = (float3)(0, -1, 0); */
    /*     world.planes[0].mat.diffuse = (float3)(0, 1, 0); */
    /*     world.planes[0].mat.specular = (float3)(0.2f, 0.2f, 0.2f); */
    /*     world.geometries[world.geometryCount].id = world.planeCount; */
    /*     world.geometries[world.geometryCount].type = Geo_Plane; */
    /*     world.geometryCount += 1; */
    /*     world.planeCount += 1; */
    /* } */
    
    /* world.dirLight.dir = (float3)(-0.5f, sin(time * 0.5f), -0.5f); */
    /* world.dirLight.color = (float3)(0.4f, 0.4f, 0.4f); */
    
    /* world.pointLightCount = 0; */
    /* world.pointLights[world.pointLightCount].pos = (float3)(-2.0f, -2.0f, 4.0f); */
    /* world.pointLights[world.pointLightCount].color = (float3)(1.0f, 0.0f, 0.0f); */
    /* world.pointLightCount += 1; */
    
    /* float3 col = (float3)WorldHit(&world, ray, 0); */

    float3 col = (float3)WorldHit(world2, ray, 0);
	/* float3 col = (float3)(0,1,0); */

    /* if(x == 0 && y == 0) { */
    /*     printf("bgcol: %f %f %f\n", world2->bgCol.x, world2->bgCol.y, world2->bgCol.z); */
    /*     printf("ambient: %f %f %f\n", world2->ambient.x, world2->ambient.y, world2->ambient.z); */
	/* 	printf("size of planes: %d\n", sizeof(world2->planes)); */
    /*     printf("world planeCount: %d\n", world2->planeCount); */
    /*     printf("world sphereCount: %d\n", world2->sphereCount); */
    /*     printf("world pointlightCount: %d\n", world2->pointLightCount); */
    /*     printf("world sphere1 pos: %f %f %f\n", world2->spheres[0].c.x, world2->spheres[0].c.y, world2->spheres[0].c.z); */

	/* 	printf("sizeof float3: %d\n", sizeof(float3)); */
	/* 	struct Vec3 v; */
	/* 	printf("sizeof vec3: %d\n", sizeof(v)); */
	/* 	struct Plane p; */
	/* 	printf("sizeof plane: %d\n", sizeof(p)); */
	/* 	struct Material m; */
	/* 	printf("sizeof mat: %d\n", sizeof(m)); */
	/* 	printf("sizeof bool: %d\n", sizeof(bool)); */
	/* 	printf("sizeof float: %d\n", sizeof(float)); */
    /* } */

    
		// WRITE IMAGE
    int2 coord = (int2)(x, y);
    write_imagef(texture, coord, (float4)(col, 1.0f));
}
