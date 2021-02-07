#include "camera.h"
#include "raytracer.h"
// #include "math.h"
#include <math.h>

inline Camera CAMERA(Vec3 e, Vec3 g, Vec3 t, float angle)
{
    Camera result;
    result.e = e;
    result.g = g;
    result.t = t;
    result.angle = angle;

    result.w = Norm(g * -1);
    result.u = Norm(Cross(t, result.w));
    result.v = Cross(result.w, result.u);

    result.pitch = asin(result.g.y);
    result.yaw = atan2(result.g.x, result.g.z);

    return result;
}

Ray RayFor(Camera cam, int w, int h, int x, int y)
{
    Vec3 o = cam.e;
    Vec3 rw = (cam.w * -1) * (h / 2) / tan(cam.angle / 2);
    Vec3 ru = cam.u * (x - (w - 1) / 2);
    Vec3 rv = cam.v * (y - (h - 1) / 2);
    Vec3 r = rw + ru + rv;
    Vec3 d = Norm(r);
    Ray result = RAY(o, d);
    return result;
}

void ProcessMouseMovement(Camera *cam, Input *input)
{
    double xoffset = input->mouseX - input->lastX;
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
    
    Vec3 dir;
    dir.x = cos(RADIANS(cam->yaw)) * cos(RADIANS(cam->pitch));
    dir.y = sin(RADIANS(cam->pitch));
    dir.z = sin(RADIANS(cam->yaw)) * cos(RADIANS(cam->pitch));
    
    cam->g = Norm(dir);
    cam->w = cam->g * -1;
    cam->u = Norm(Cross(cam->t, cam->w));
    cam->v = Cross(cam->w, cam->u);
}
