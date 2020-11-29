/* date = November 29th 2020 4:14 pm */

#ifndef RAY_H
#define RAY_H

struct Ray{
    Vec3 origin;
    Vec3 direction;
};

Vec3 RayAt(Ray ray, f32 t){
    Vec3 pos = ray.origin + (ray.direction*t);
    return pos;
}

Vec3 RayBackground(Ray ray){
    Vec3 unit_direction = NormaliseVec3(ray.direction);
    f32 t = 0.5*(unit_direction.y + 1.0f);
    return Vec3(255, 255, 255) * (1.0f - t)  + Vec3(127, 178, 255) * t;
}

struct HitInfo{
    f32 root;
    Vec3 pos;
    Vec3 normal;
    b32 front_face;
};

struct Sphere{
    Vec3 origin;
    f32 radius;
    Vec3 colour;
};

b32 SphereHit(Ray ray, Sphere sphere, HitInfo *hit_info){
    //calculate sphere intersection
    f32 a = Vec3Dot(ray.direction, ray.direction);
    f32 b = 2.0f * Vec3Dot(ray.direction, ray.origin - sphere.origin);
    f32 c = Vec3Dot(ray.origin - sphere.origin, ray.origin - sphere.origin) - (sphere.radius * sphere.radius);
    
    f32 discriminant = b*b - 4*a*c;
    
    if(discriminant < 0) return 0;
    else{
        hit_info->root = (-b - sqrt(discriminant)) / (2.0f * a);
        hit_info->pos = RayAt(ray, hit_info->root);
        hit_info->normal = NormaliseVec3(RayAt(ray, hit_info->root) - Vec3(0, 0, -1));
        if(Vec3Dot(ray.direction, hit_info->normal) > 0.0f){
            //ray is inside(back face) - reverse normal to point against ray
            hit_info->normal = Vec3(-hit_info->normal.x, -hit_info->normal.y, -hit_info->normal.z);
            hit_info->front_face = 0;
        }else{
            //ray is outside - front face
            hit_info->front_face = 1;
        }
        return 1;
    }
}

struct Camera{
    Vec3 pos;
    f32 viewport_height;
    f32 viewport_width;
    f32 focal_length;
    
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 lower_left_corner;
};





#endif //RAY_H
