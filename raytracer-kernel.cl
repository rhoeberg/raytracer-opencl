#define EPSILON 0.0001
#define MAX_PLANES 2
#define MAX_SPHERES 2
#define MAX_AABB 2
#define MAX_GEOMETRIES (MAX_PLANES + MAX_SPHERES + MAX_AABB)
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

struct __attribute__ ((packed)) AABB
{
	float3 lbf; // left bottom far
	float3 run; // right upper near
	struct Material mat;
};

enum GeometryType{
    Geo_Sphere,
    Geo_Plane,
    Geo_AABB,
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
    struct AABB AABBs[MAX_AABB];
    
    int maxPlanes;
    int maxSpheres;
    int maxPointLights;
    
    int planeCount;
    int sphereCount;
    int aabbCount;
    
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

bool AABBHit(struct AABB aabb, struct Ray ray, struct Hit *hit)
{
	struct Hit finalHit;

	/* struct Plane planes[1024]; */
	/* struct Hit hit; */
	/* bool hitSuccess = PlaneHit(planes[0], ray, &hit); */

	struct Plane planes[6];
	int planeHitCount = 0;
	bool hitPlane = false;

	///////////////////////
	// PROBLEM IS MAYBE HERE????
	//


	// nearPlane
	/* if(dot(normalize(ray.o - aabb.run), (float3)(0, 0, 1)) > 0) { */
	/* 	struct Plane nearPlane; */
	/* 	nearPlane.a = aabb.run; */
	/* 	nearPlane.n = (float3)(0, 0, 1); */
	/* 	struct Hit hit; */
	/* 	if(PlaneHit(nearPlane, ray, &hit)) { */
	/* 		hitPlane = true; */
	/* 		planes[planeHitCount] = nearPlane; */
	/* 		planeHitCount++; */
	/* 		/\* planes[planeHitCount].a = (float3)(0,0,0); *\/ */
	/* 	} */
	/* } */

	/* // bottomPlane */
	/* if(dot(normalize(ray.o - aabb.lbf), (float3)(0, -1, 0)) > 0) { */
	/* 	struct Plane bottomPlane; */
	/* 	bottomPlane.a = aabb.run; */
	/* 	bottomPlane.n = (float3)(0, -1, 0); */
	/* 	struct Hit hit; */
	/* 	if(PlaneHit(bottomPlane, ray, &hit)) { */
	/* 		hitPlane = true; */
	/* 		planes[planeHitCount].a = (float3)(1,2,3); */
	/* 		/\* planes[planeHitCount].n = bottomPlane.n; *\/ */
	/* 		/\* planeHitCount++; *\/ */
	/* 	} */
	/* } */

	/* // topPlane */
	/* if(dot(normalize(ray.o - aabb.run), (float3)(0, 1, 0)) > 0) { */
	/* 	struct Plane topPlane; */
	/* 	topPlane.a = aabb.run; */
	/* 	topPlane.n = (float3)(0, 1, 0); */
	/* 	struct Hit hit; */
	/* 	if(PlaneHit(topPlane, ray, &hit)) { */
	/* 		hitPlane = true; */
	/* 		planes[planeHitCount] = topPlane; */
	/* 		planeHitCount++; */
	/* 	} */
	/* } */

	/* // leftPlane */
	/* if(dot(normalize(ray.o - aabb.lbf), (float3)(-1, 0, 0)) > 0) { */
	/* 	struct Plane leftPlane; */
	/* 	leftPlane.a = aabb.run; */
	/* 	leftPlane.n = (float3)(-1, 0, 0); */
	/* 	struct Hit hit; */
	/* 	if(PlaneHit(leftPlane, ray, &hit)) { */
	/* 		hitPlane = true; */
	/* 		planes[planeHitCount] = leftPlane; */
	/* 		planeHitCount++; */
	/* 	} */
	/* } */

	/* // rightPlane */
	/* if(dot(normalize(ray.o - aabb.run), (float3)(1, 0, 0)) > 0) { */
	/* 	struct Plane rightPlane; */
	/* 	rightPlane.a = aabb.run; */
	/* 	rightPlane.n = (float3)(1, 0, 0); */
	/* 	struct Hit hit; */
	/* 	if(PlaneHit(rightPlane, ray, &hit)) { */
	/* 		hitPlane = true; */
	/* 		planes[planeHitCount] = rightPlane; */
	/* 		planeHitCount++; */
	/* 	} */
	/* } */

	for(int i = 0; i < planeHitCount; i++) {
		if(i == 0) {
			PlaneHit(planes[i], ray, &finalHit);
		}
		else {
			struct Hit newHit;
			if(PlaneHit(planes[i], ray, &finalHit) && newHit.t > finalHit.t) {
				finalHit = newHit;
			}
		}
	}

	if(hitPlane) {
		float3 p = ray.o + ray.d * finalHit.t;
		if(p.x + EPSILON >= aabb.lbf.x && p.x - EPSILON <= aabb.run.x &&
		   p.y + EPSILON >= aabb.lbf.y && p.y - EPSILON <= aabb.run.y &&
		   p.z + EPSILON >= aabb.lbf.z && p.y - EPSILON <= aabb.run.z) {
			*hit = finalHit;
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
        else if (world->geometries[i].type == Geo_AABB)
        {
            hitSuccess = AABBHit(world->AABBs[world->geometries[i].id], ray, &nextHit);
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

__kernel void WorldHitKernel(__global struct World *world, int width, int height, __write_only image2d_t texture)
{

    const unsigned int x = get_global_id(0);
    const unsigned int y = get_global_id(1);
    
    float3 rw = (world->cam.w * -1.0f) * ((float)height / 2.0f) / tan(world->cam.angle / 2.0f);
    float3 ru = world->cam.u * (x - ((float)width - 1.0f) / 2.0f);
    float3 rv = world->cam.v * (y - ((float)height - 1.0f) / 2.0f);
    float3 r = rw + ru + rv;
    float3 d = normalize(r);
    struct Ray ray;
    ray.o = world->cam.o;
    ray.d = d;

	//////////////////
	// Here we have no problem putting a lot of planes on the stack
	//
	/* struct Plane planes[1024]; */
	/* struct Hit hit; */
	/* bool hitSuccess = PlaneHit(planes[0], ray, &hit); */
	/* for(int i = 0; i < 12; i++) { */
	/* 	planes[i].a = (float3)(0,i,0); */
	/* } */
	/* planes[400].a = (float3)(0,0,0); */
    /* /\* float3 col = planes[700].a; *\/ */

    float3 col = (float3)WorldHit(world, ray, 0);
	//
	// WRITE IMAGE
	//
    int2 coord = (int2)(x, y);
    write_imagef(texture, coord, (float4)(col, 1.0f));
}
