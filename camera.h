#pragma once
#include "math.h"

struct Camera
{
    Vec3 e;
    Vec3 g;
    Vec3 t;
    
    Vec3 w;
    Vec3 u;
    Vec3 v;
    
    float angle;

	float yaw;
	float pitch;
};
