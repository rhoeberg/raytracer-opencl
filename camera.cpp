#include "camera.h"
#include "raytracer.h"
#include "math.h"
// #include <math.h>

Camera CAMERA(cl_float3 o, cl_float3 g, cl_float3 t, float angle)
{
    Camera result;
    result.o = o;
    result.g = g;
    result.t = t;
    result.angle = angle;

    result.w = Norm(g * -1);
    result.u = Norm(Cross(t, result.w));
    result.v = Cross(result.w, result.u);

    result.pitch = DEGREES(Asin(-result.g.y));
    result.yaw = DEGREES(Atan2(result.g.z, result.g.x));

    return result;
}

// Ray RayFor(Camera cam, int w, int h, int x, int y)
// {
//     Vec3 o = cam.e;
//     Vec3 rw = (cam.w * -1) * (h / 2) / tan(cam.angle / 2);
//     Vec3 ru = cam.u * (x - (w - 1) / 2);
//     Vec3 rv = cam.v * (y - (h - 1) / 2);
//     Vec3 r = rw + ru + rv;
//     Vec3 d = Norm(r);
//     Ray result = RAY(o, d);
//     return result;
// }

void ProcessMouseMovement(Camera *cam, Input *input)
{
    // double xoffset = input->mouseX - input->lastX;
    double xoffset = input->lastX - input->mouseX;
    double yoffset = input->lastY - input->mouseY;
    input->lastX = input->mouseX;
    input->lastY = input->mouseY;
    
    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;
    
    cam->yaw -= xoffset;
    cam->pitch += yoffset;
    
    if (cam->pitch > 89.0f)
        cam->pitch = 89.0f;
    if (cam->pitch < -89.0f)
        cam->pitch = -89.0f;
    
    cl_float3 dir;
    dir.x = cos(RADIANS(cam->yaw)) * cos(RADIANS(cam->pitch));
    dir.y = sin(RADIANS(cam->pitch));
    dir.z = sin(RADIANS(cam->yaw)) * cos(RADIANS(cam->pitch));
    
    cam->g = Norm(dir);
    cam->w = cam->g * -1;
    cam->u = Norm(Cross(cam->t, cam->w));
    cam->v = Cross(cam->w, cam->u);
}

void ProcessKeyboard(Camera *cam, Input *input)
{
	// TODO(rhoe) add deltatime to vel 
	float vel = 0.1f;

	if(input->keys[GLFW_KEY_W]) {
		cam->o += cam->g * vel;
	}
	else if(input->keys[GLFW_KEY_S]) {
		cam->o -= cam->g * vel;
	}

	if(input->keys[GLFW_KEY_A]) {
		cam->o -= cam->u * vel;
	}
	else if(input->keys[GLFW_KEY_D]) {
		cam->o += cam->u * vel;
	}
}
