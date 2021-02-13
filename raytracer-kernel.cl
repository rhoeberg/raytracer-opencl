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

struct __attribute__ ((packed)) Camera
{
	// TODO (rhoe) maybe names these better?
    float3 o; // cam pos
    float3 g; // cam front (gaze direction)
    float3 w; // cam back (negative gaze)
    float3 u; // cam right 
    float3 v; // cam up 
    float3 t; // world up 
    

	float yaw;
	float pitch;

    float angle;
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

	struct Camera cam;
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

void worldDebug(__global const struct World *world)
{
	printf("pointlight count: %d\n", world->pointLightCount);
	printf("sphere count: %d\n", world->sphereCount);
	printf("plane count: %d\n", world->planeCount);
	{
		struct Material mat = world->spheres[0].mat;
		printf("sphere 0\n\tdiffuse: %f %f %f\n\tspec: %f %f %f\n\treflection: %f %f %f\n",
			   mat.diffuse.x, mat.diffuse.y, mat.diffuse.z,
			   mat.specular.x, mat.specular.y, mat.specular.z,
			   mat.reflection.x, mat.reflection.y, mat.reflection.z);
	}
	{
		struct Material mat = world->spheres[1].mat;
		printf("sphere 1\n\tdiffuse: %f %f %f\n\tspec: %f %f %f\n\treflection: %f %f %f\n",
			   mat.diffuse.x, mat.diffuse.y, mat.diffuse.z,
			   mat.specular.x, mat.specular.y, mat.specular.z,
			   mat.reflection.x, mat.reflection.y, mat.reflection.z);
	}
	{
		struct Material mat = world->planes[0].mat;
		printf("planes 0\n\tdiffuse: %f %f %f\n\tspec: %f %f %f\n\treflection: %f %f %f\n",
			   mat.diffuse.x, mat.diffuse.y, mat.diffuse.z,
			   mat.specular.x, mat.specular.y, mat.specular.z,
			   mat.reflection.x, mat.reflection.y, mat.reflection.z);
	}
	{
		struct PointLight point = world->pointLights[0];
		printf("pointlight\n\tpos:%f %f %f\n\tcolor:%f %f %f\n",
			   point.pos.x, point.pos.y, point.pos.z,
			   point.color.x, point.color.y, point.color.z);
	}
	{
		struct DirLight light = world->dirLight;
		printf("dirlight\n\tdir:%f %f %f\n\tcolor:%f %f %f\n",
			   light.dir.x, light.dir.y, light.dir.z,
			   light.color.x, light.color.y, light.color.z);
	}
}	


bool SphereHit(struct Sphere sphere, struct Ray ray, struct Hit *hit)
{
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
	/* printf("lightcolor: %f %f %f\n", lightColor.x, lightColor.y, lightColor.z); */
	/* struct Material mat = hit.mat; */
	/* printf("hit mat\n\tdiffuse: %f %f %f\n\tspec: %f %f %f\n\treflection: %f %f %f\n", */
	/* 	   mat.diffuse.x, mat.diffuse.y, mat.diffuse.z, */
	/* 	   mat.specular.x, mat.specular.y, mat.specular.z, */
	/* 	   mat.reflection.x, mat.reflection.y, mat.reflection.z); */

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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// FIXED BUG WITH WORLD PASSED AS KERNEL ARG
// when trying to send the pointlight struct here from the world in shared mem it breaks with no error!
//
// fix is to just use pointlight id since we are passing world pointer anyways
//
// bool PointLightIlluminates(__global const struct World *world, struct PointLight light, float3 point)
//
bool PointLightIlluminates(__global const struct World *world, int id, float3 point)
{
				
    float3 pointFrom = world->pointLights[id].pos - point;
    float3 epsilonV = (float3)( EPSILON, EPSILON, EPSILON );
    float3 adjustedPoint = point + (pointFrom * epsilonV);
    float tl = length(world->pointLights[id].pos - point) / length(pointFrom);
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
        float3 p = ray.o + (ray.d * hit.t);
        float3 result = hit.mat.diffuse * world->ambient;
        
        result += LightGetColor(normalize(world->dirLight.dir * -1), world->dirLight.color, hit);

        int i;
        for(i = 0; i < world->pointLightCount; i++)
        {
            if (PointLightIlluminates(world, i, p))
            {
				/* if(x == 1324 && y == 544) { */
				/* /\* if(hit.mat.diffuse.y > 0.99f) { *\/ */
				/* 	/\* printf("x:%d, y:%d\n", x, y); *\/ */
				/* 	printf("hit mat diffuse r:%f, g:%f, b:%f\n", hit.mat.diffuse.x, hit.mat.diffuse.y, hit.mat.diffuse.z); */
				/* } */

                float3 pointFrom = normalize(world->pointLights[i].pos - p);
                result += LightGetColor(pointFrom, world->pointLights[i].color, hit);
            }
        }

        
        if (hit.mat.mirror)
        {
            if (n < 3)
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

__kernel void WorldHitKernel(__global struct World *world, int width, int height, __write_only image2d_t texture, __global struct Ray *outputDebug,
                             float time)
{
    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);

	/* if(x == 0 && y == 0) { */
	/* 	worldDebug(world); */
	/* } */

    /* float3 o = (float3)(time * 0.03f, 0.0f, 0.0f); */
    /* float3 g = (float3)(0.0f, 0.0f, -1.0f); */
    /* float3 w = normalize(g * -1.0f); */
    /* float3 u = cross((float3)(0.0f, 1.0f, 0.0f), w); */
    /* float3 v = cross(w, u); */
    /* float3 t = (float3)(0.0f, 1.0f, 0.0f); */
    /* float angle = 0.785f; */
    
    float3 rw = (world->cam.w * -1.0f) * ((float)height / 2.0f) / tan(world->cam.angle / 2.0f);
    float3 ru = world->cam.u * (x - ((float)width - 1.0f) / 2.0f);
    float3 rv = world->cam.v * (y - ((float)height - 1.0f) / 2.0f);
    float3 r = rw + ru + rv;
    float3 d = normalize(r);
    struct Ray ray;
    ray.o = world->cam.o;
    ray.d = d;

    float3 col = (float3)WorldHit(world, ray, 0);

	//
	// WRITE IMAGE
	//
    int2 coord = (int2)(x, y);
    write_imagef(texture, coord, (float4)(col, 1.0f));
}
