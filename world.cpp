World InitializeDefaultWorld()
{
    World world;
    world.bgCol = {0.6f, 0.6f, 1};
    world.ambient = {0.3f, 0.3f, 0.3f};
    world.geometryCount = 0;
    world.sphereCount = 0;
    world.planeCount = 0;
    world.aabbCount = 0;

    {
        world.spheres[world.sphereCount].mat.diffuse = {0.7f, 0.7f, 0.7f};
        world.spheres[world.sphereCount].mat.specular = {0.9f, 0.9f, 0.9f};
        world.spheres[world.sphereCount].mat.shine = 100.0f;
        world.spheres[world.sphereCount].c = {-1, 1, -3};
        world.spheres[world.sphereCount].r = 0.7f;
        world.spheres[world.sphereCount].mat.mirror = true;
        world.spheres[world.sphereCount].mat.reflection = {0.6f, 0.6f, 0.6f};
        world.geometries[world.geometryCount].id = world.sphereCount;
        world.geometries[world.geometryCount].type = Geo_Sphere;
        world.geometryCount += 1;
        world.sphereCount += 1;
    }
    
    {
        world.spheres[world.sphereCount].mat.diffuse = {0, 1, 0};
        world.spheres[world.sphereCount].mat.specular = {0, 0, 0};
        world.spheres[world.sphereCount].mat.shine = 0.0f;
        world.spheres[world.sphereCount].c = {1, 2, -3};
        world.spheres[world.sphereCount].r = 0.5f;
        world.spheres[world.sphereCount].mat.mirror = true;
        world.spheres[world.sphereCount].mat.reflection = {0.3f, 0.3f, 0.3f};
        world.geometries[world.geometryCount].id = world.sphereCount;
        world.geometries[world.geometryCount].type = Geo_Sphere;
        world.geometryCount += 1;
        world.sphereCount += 1;
    }
    
    {
        world.planes[world.planeCount].a = {0, 0, 0};
        world.planes[world.planeCount].n = {0, 1, 0};
        world.planes[world.planeCount].mat.diffuse = {0.8f, 0, 1};
        world.planes[world.planeCount].mat.specular = {0.2f, 0.2f, 0.2f};
		world.planes[world.planeCount].mat.shine = 0.0f;
		world.planes[world.planeCount].mat.reflection = {0.4f, 0.4f, 0.4f};
        world.planes[world.planeCount].mat.mirror = false;
        world.geometries[world.geometryCount].id = world.planeCount;
        world.geometries[world.geometryCount].type = Geo_Plane;
        world.geometryCount += 1;
        world.planeCount += 1;
    }

    {
        world.AABBs[world.aabbCount].lbf = {1, 0.0f, 1.0f};
        world.AABBs[world.aabbCount].run = {2, 1.0f, 2.0f};
        world.AABBs[world.aabbCount].mat.diffuse = {1, 0, 0};
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
    world.pointLights[world.pointLightCount].pos = {2.0f, 3.0f, -1.0f};
    world.pointLights[world.pointLightCount].color = {0.3f, 0.3f, 0.3f}; // BLUE
    world.pointLightCount += 1;
	
	return world;
}
