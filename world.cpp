
void AddPlane(World *world, Plane plane)
{
    if(world->planeCount < world->maxPlanes)
    {
        world->geometries[world->geometryCount].id = world->planeCount;
        world->geometries[world->geometryCount].type = Geo_Plane;
        world->planes[world->planeCount] = plane;
        world->geometryCount++;
        world->planeCount++;
    }
}

void AddSphere(World *world, Sphere sphere)
{
    if(world->sphereCount < world->maxSpheres)
    {
        world->geometries[world->geometryCount].id = world->sphereCount;
        world->geometries[world->geometryCount].type = Geo_Sphere;
        world->spheres[world->sphereCount] = sphere;
        world->geometryCount++;
        world->sphereCount++;
    }
}

void AddPointLight(World *world, PointLight pointLight)
{
    if(world->pointLightCount < world->maxPointLights)
    {
        world->pointLights[world->pointLightCount] = pointLight;
        world->pointLightCount++;
    }
}

World InitializeDefaultWorld()
{
    World world = {};
    world.maxPlanes = 10;
    world.maxSpheres = 10;
    world.maxPointLights = 5;
    world.bgCol = VEC3(0,0,0);
    world.ambient = VEC3(0.2f, 0.2f, 0.2f);
    
    //world.planes = (Plane *)malloc(sizeof(Plane) * world.maxPlanes);
    //world.spheres = (Sphere *)malloc(sizeof(Sphere) * world.maxSpheres);
    //world.geometries = (Geometry *)malloc(sizeof(Geometry) * (world.maxSpheres + world.maxPlanes));
    //world.pointLights = (PointLight*)malloc(sizeof(PointLight) * world.maxPointLights);
    
    Material mat1 = MATERIAL(VEC3(0.5f, 0.2f, 0.2f),
                             VEC3(0.8f, 0.2f, 0.1f),
                             VEC3(0.2f, 0.2f, 0.2f),
                             6.0f);
    Material mat2 = MATERIAL(VEC3(1.0f, 0.2f, 0.2f),
                             VEC3(0.3f, 1.0f, 0.3f),
                             VEC3(0.8f, 0.8f, 0.8f),
                             15.0f);
    Material mat3 = MATERIAL(VEC3(0.8f, 0.8f, 0.8f),
                             VEC3(0.1f, 0.1f, 0.1f),
                             VEC3(0.3f, 0.3f, 0.3f),
                             15.0f);
    
    
    Sphere sphere1 = SPHERE(VEC3(0.0f, 0, -3.0f), 0.5f, mat2);
    sphere1.mat.diffuse = VEC3(0.1, 0.5, 0.8);
    sphere1.mat.reflection = VEC3(0.7, 0.1, 0.1);
    AddSphere(&world, sphere1);
    
    
#if 0    
    Sphere sphere2 = SPHERE(VEC3(1.0f, 0, -3.0f), 0.5f, mat2);
    Sphere sphere3 = SPHERE(VEC3(-1.0f, 0, 1.0f), 0.5f, mat2);
    Sphere sphere4 = SPHERE(VEC3(1.0f, 0, 1.0f), 0.5f, mat2);
    sphere2.mat.reflection = VEC3(0.1, 0.1, 0.1);
    sphere2.mat.diffuse = VEC3(0.1, 0.7, 0.1);
    Plane plane1 = PLANE(VEC3(-3, 0, 0), VEC3(1, 0, 0), mat3);
    Plane plane2 = PLANE(VEC3(3, 0, 0), VEC3(-1, 0, 0), mat3);
    Plane plane3 = PLANE(VEC3(0, -1.5f, 0), VEC3(0, 1, 0), mat3);
    Plane plane4 = PLANE(VEC3(0, 1.5f, 0), VEC3(0, -1, 0), mat3);
    Plane plane5 = PLANE(VEC3(0, 0, -6), VEC3(0, 0, 1), mat3);
    Plane plane6 = PLANE(VEC3(0, 0, 2), VEC3(0, 0, -1), mat3);
    plane3.mat.mirror = true;
    plane3.mat.reflection = VEC3(0.9, 0.9, 0.9);
    plane3.mat.diffuse = VEC3(0.2, 0.2, 0.2);
    AddPlane(&world, plane1);
    AddPlane(&world, plane2);
    AddPlane(&world, plane3);
    AddPlane(&world, plane4);
    AddPlane(&world, plane5);
    AddPlane(&world, plane6);
    AddSphere(&world, sphere2);
    AddSphere(&world, sphere3);
    AddSphere(&world, sphere4);
#endif
    
    
    world.dirLight = DIRLIGHT(VEC3(0.2f, -1.0f, -0.2f), VEC3(0.3f, 0.3f, 0));
    
    PointLight pointLight = POINTLIGHT(VEC3(2.0f, 0.5f, 0.0f),
                                       VEC3(0.5f, 0.2f, 0.2f));
    PointLight pointLight2 = POINTLIGHT(VEC3(-2.0f, 0.5f, -5.0f),
                                        VEC3(0.2f, 0.3f, 0.6f));
    AddPointLight(&world, pointLight);
    AddPointLight(&world, pointLight2);
    
    return world;
}