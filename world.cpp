
// void AddPlane(World *world, Plane plane)
// {
//     if(world->planeCount < world->maxPlanes)
//     {
//         world->geometries[world->geometryCount].id = world->planeCount;
//         world->geometries[world->geometryCount].type = Geo_Plane;
//         world->planes[world->planeCount] = plane;
//         world->geometryCount++;
//         world->planeCount++;
//     }
// }

// void AddSphere(World *world, Sphere sphere)
// {
//     if(world->sphereCount < world->maxSpheres)
//     {
//         world->geometries[world->geometryCount].id = world->sphereCount;
//         world->geometries[world->geometryCount].type = Geo_Sphere;
//         world->spheres[world->sphereCount] = sphere;
//         world->geometryCount++;
//         world->sphereCount++;
//     }
// }

// void AddPointLight(World *world, PointLight pointLight)
// {
//     if(world->pointLightCount < world->maxPointLights)
//     {
//         world->pointLights[world->pointLightCount] = pointLight;
//         world->pointLightCount++;
//     }
// }

// World InitializeDefaultWorld2()
// {
//     World world = {};
//     world.maxPlanes = 10;
//     world.maxSpheres = 10;
//     world.maxPointLights = 5;
//     world.bgCol = {0,0,0};
//     world.ambient = {0.2f, 0.2f, 0.2f};
    
//     //world.planes = (Plane *)malloc(sizeof(Plane) * world.maxPlanes);
//     //world.spheres = (Sphere *)malloc(sizeof(Sphere) * world.maxSpheres);
//     //world.geometries = (Geometry *)malloc(sizeof(Geometry) * (world.maxSpheres + world.maxPlanes));
//     //world.pointLights = (PointLight*)malloc(sizeof(PointLight) * world.maxPointLights);
    
//     Material mat1 = MATERIAL({0.5f, 0.2f, 0.2f},
//                              {0.8f, 0.2f, 0.1f},
//                              {0.2f, 0.2f, 0.2f},
//                              6.0f);
//     Material mat2 = MATERIAL({1.0f, 0.2f, 0.2f},
//                              {0.3f, 1.0f, 0.3f},
//                              {0.8f, 0.8f, 0.8f},
//                              15.0f);
//     Material mat3 = MATERIAL({0.8f, 0.8f, 0.8f},
//                              {0.1f, 0.1f, 0.1f},
//                              {0.3f, 0.3f, 0.3f},
//                              15.0f);
    
    
//     Sphere sphere1 = SPHERE({0.0f, 0, -3.0f}, 0.5f, mat2);
//     // sphere1.mat.diffuse = cl_float3(VEC3(0.1, 0.5, 0.8);
//     sphere1.mat.diffuse = {0.1, 0.5, 0.8};
//     sphere1.mat.reflection = {0.7, 0.1, 0.1};
//     AddSphere(&world, sphere1);
    
    
// #if 0    
//     Sphere sphere2 = SPHERE(VEC3(1.0f, 0, -3.0f), 0.5f, mat2);
//     Sphere sphere3 = SPHERE(VEC3(-1.0f, 0, 1.0f), 0.5f, mat2);
//     Sphere sphere4 = SPHERE(VEC3(1.0f, 0, 1.0f), 0.5f, mat2);
//     sphere2.mat.reflection = VEC3(0.1, 0.1, 0.1);
//     sphere2.mat.diffuse = VEC3(0.1, 0.7, 0.1);
//     Plane plane1 = PLANE(VEC3(-3, 0, 0), VEC3(1, 0, 0), mat3);
//     Plane plane2 = PLANE(VEC3(3, 0, 0), VEC3(-1, 0, 0), mat3);
//     Plane plane3 = PLANE(VEC3(0, -1.5f, 0), VEC3(0, 1, 0), mat3);
//     Plane plane4 = PLANE(VEC3(0, 1.5f, 0), VEC3(0, -1, 0), mat3);
//     Plane plane5 = PLANE(VEC3(0, 0, -6), VEC3(0, 0, 1), mat3);
//     Plane plane6 = PLANE(VEC3(0, 0, 2), VEC3(0, 0, -1), mat3);
//     plane3.mat.mirror = true;
//     plane3.mat.reflection = VEC3(0.9, 0.9, 0.9);
//     plane3.mat.diffuse = VEC3(0.2, 0.2, 0.2);
//     AddPlane(&world, plane1);
//     AddPlane(&world, plane2);
//     AddPlane(&world, plane3);
//     AddPlane(&world, plane4);
//     AddPlane(&world, plane5);
//     AddPlane(&world, plane6);
//     AddSphere(&world, sphere2);
//     AddSphere(&world, sphere3);
//     AddSphere(&world, sphere4);
// #endif
    
    
//     world.dirLight = DIRLIGHT({0.2f, -1.0f, -0.2f}, {0.3f, 0.3f, 0});
    
//     PointLight pointLight = POINTLIGHT(cl_float3{2.0f, 0.5f, 0.0f},
//                                        cl_float3{0.5f, 0.2f, 0.2f});
//     PointLight pointLight2 = POINTLIGHT(cl_float3{-2.0f, 0.5f, -5.0f},
//                                         cl_float3{0.2f, 0.3f, 0.6f});
//     AddPointLight(&world, pointLight);
//     AddPointLight(&world, pointLight2);
    
//     return world;
// }

World InitializeDefaultWorld()
{
    World world;
    world.bgCol = {0.08f, 0.11f, 0.42f};
    world.ambient = {0.0f, 0.0f, 0.0f};
    world.geometryCount = 0;
    world.sphereCount = 0;
    world.planeCount = 0;
    world.aabbCount = 0;

    {
        world.spheres[world.sphereCount].mat.diffuse = {0, 1, 1};
        world.spheres[world.sphereCount].mat.specular = {0, 0, 0};
        world.spheres[world.sphereCount].mat.shine = 100.0f;
        world.spheres[world.sphereCount].c = {-1, 0, -3};
        world.spheres[world.sphereCount].r = 0.5f;
        world.spheres[world.sphereCount].mat.mirror = false;
        world.spheres[world.sphereCount].mat.reflection = {0.2f, 0.2f, 0.2f};
        world.geometries[world.geometryCount].id = world.sphereCount;
        world.geometries[world.geometryCount].type = Geo_Sphere;
        world.geometryCount += 1;
        world.sphereCount += 1;
    }
    
    {
        world.spheres[world.sphereCount].mat.diffuse = {1, 1, 0};
        world.spheres[world.sphereCount].mat.specular = {0, 0, 0};
        world.spheres[world.sphereCount].mat.shine = 0.0f;
        world.spheres[world.sphereCount].c = {1, 0, -3};
        world.spheres[world.sphereCount].r = 0.5f;
        world.spheres[world.sphereCount].mat.mirror = false;
        world.spheres[world.sphereCount].mat.reflection = {0.2f, 0.2f, 0.2f};
        world.geometries[world.geometryCount].id = world.sphereCount;
        world.geometries[world.geometryCount].type = Geo_Sphere;
        world.geometryCount += 1;
        world.sphereCount += 1;
    }
    
    {
        world.planes[world.planeCount].a = {0, -0.5f, 0};
        world.planes[world.planeCount].n = {0, 1, 0};
        world.planes[world.planeCount].mat.diffuse = {0, 1, 0};
        world.planes[world.planeCount].mat.specular = {0.2f, 0.2f, 0.2f};
		world.planes[world.planeCount].mat.shine = 0.0f;
		world.planes[world.planeCount].mat.reflection = {0.4f, 0.4f, 0.4f};
        world.planes[world.planeCount].mat.mirror = true;
        world.geometries[world.geometryCount].id = world.planeCount;
        world.geometries[world.geometryCount].type = Geo_Plane;
        world.geometryCount += 1;
        world.planeCount += 1;
    }

    {
        world.AABBs[world.aabbCount].lbf = {1, 0.0f, -2.0f};
        world.AABBs[world.aabbCount].run = {2, 1.0f, -1.0f};
        world.AABBs[world.aabbCount].mat.diffuse = {0, 1, 0};
        world.AABBs[world.aabbCount].mat.specular = {0.2f, 0.2f, 0.2f};
		world.AABBs[world.aabbCount].mat.shine = 100.0f;
		world.AABBs[world.aabbCount].mat.reflection = {0.4f, 0.4f, 0.4f};
        world.AABBs[world.aabbCount].mat.mirror = false;
        world.geometries[world.geometryCount].id = world.aabbCount;
        world.geometries[world.geometryCount].type = Geo_AABB;
        world.geometryCount += 1;
        world.aabbCount += 1;
    }
    
    world.dirLight.dir = {-0.5f, -0.5f, -0.5f};
    world.dirLight.color = {0.1f, 0.1f, 0.1f};
    
    world.pointLightCount = 0;
    world.pointLights[world.pointLightCount].pos = {-2.0f, 1.0f, 4.0f};
    world.pointLights[world.pointLightCount].color = {0.3f, 0.3f, 0.3f}; // BLUE
    world.pointLightCount += 1;
	
	return world;
}
