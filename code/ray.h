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
    return Vec3(1.0f - t, 1.0f - t, 1.0f - t) + Vec3(127.0f / 255.0f, 178.0f / 255.0f, 1.0f) * t;
}

enum Materials{
    DIFFUSE,
    METAL,
    GLASS
};

enum Models{
    SPHERE,
    PLANE
};

struct HitInfo{
    f32 root;
    Vec3 pos;
    Vec3 normal;
    b32 front_face;
};

struct Model{
    u32 type;
    Vec3 colour;
    u32 material;
    union{
        /* Sphere */
        struct{
            Vec3 origin;
            f32 radius;
        };
        /* Plane */
        struct{
            Vec3 point;
            Vec3 normal;
            f32 d;
        };
    };
};

/* Need to specify a point on the plane and a normal */
Model CreatePlane(Vec3 point, Vec3 normal, Vec3 colour){
    Model plane = {};
    plane.type = PLANE;
    plane.point = point;
    plane.normal = normal;
    plane.colour = colour;
    plane.d = -Vec3Dot(point, normal);
    plane.material = DIFFUSE;
    
    return plane;
}

Model CreateSphere(Vec3 origin, f32 radius, Vec3 colour){
    Model sphere = {};
    sphere.type = SPHERE;
    sphere.origin = origin;
    sphere.radius = radius;
    sphere.colour = colour;
    //default type
    sphere.type = DIFFUSE;
    return sphere;
}

b32 SphereHit(Ray ray, Model sphere, HitInfo *hit_info){
    //calculate sphere intersection
    f32 a = Vec3Dot(ray.direction, ray.direction);
    f32 b = 2.0f * Vec3Dot(ray.direction, ray.origin - sphere.origin);
    f32 c = Vec3Dot(ray.origin - sphere.origin, ray.origin - sphere.origin) - (sphere.radius * sphere.radius);
    
    f32 discriminant = b*b - 4*a*c;
    
    if(discriminant <= 0) return 0;
    else{
        f32 root_1 = (-b - sqrt(discriminant)) / (2.0f * a);
        f32 root_2 = (-b + sqrt(discriminant)) / (2.0f * a);
        f32 tolerance = 0.001f;
        if(root_1 < tolerance && root_2 > tolerance){
            hit_info->root = root_2;
        }else if(root_2 < tolerance && root_1 > tolerance){
            hit_info->root = root_1;
        }else if((root_1 > tolerance) && (root_2 > tolerance)){
            if(root_1 < root_2) hit_info->root = root_1;
            else hit_info->root = root_2;
        }else{
            return 0;
        }
        hit_info->pos = RayAt(ray, hit_info->root);
        hit_info->normal = NormaliseVec3(RayAt(ray, hit_info->root) - sphere.origin);
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

b32 PlaneHit(Ray ray, Model plane, HitInfo *hit_info){
    // plane equation: ax + by + cz + d = 0, where n = (a, b, c)
    plane.normal = NormaliseVec3(plane.normal);
    ray.direction = NormaliseVec3(ray.direction);
    f32 denom = Vec3Dot(plane.normal, ray.direction);
    if((denom < -0.0001f) > (denom > 0.0001f)){
        hit_info->root = (-plane.d - Vec3Dot(plane.normal, ray.origin))/denom;
        if(hit_info->root > 0){
            hit_info->pos = RayAt(ray, hit_info->root);
            hit_info->normal = plane.normal;
            return 1;
        }else{
            return 0;
        }
    }else{
        return 0;
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

Camera CreateCamera(Vec3 pos, Vec3 up, Vec3 look_at){
    Camera camera = {};
    camera.pos = pos;
    camera.viewport_height = 2.0;
    camera.viewport_width = (16.0f / 9.0f) * camera.viewport_height;
    camera.focal_length = 1.0f;
    
    Vec3 camera_z = NormaliseVec3(pos - look_at);
    Vec3 camera_x = NormaliseVec3(Vec3Cross(up, camera_z));
    Vec3 camera_y = NormaliseVec3(Vec3Cross(camera_z, camera_x));
    
    camera.horizontal = camera_x * camera.viewport_width;
    camera.vertical = camera_y * camera.viewport_height;
    camera.lower_left_corner = camera.pos - Vec3(camera.horizontal.x / 2.0f, camera.horizontal.y / 2.0f, camera.horizontal.z / 2.0f) - Vec3(camera.vertical.x / 2.0f, camera.vertical.y / 2.0f, camera.vertical.z / 2.0f) - camera_z;
    //camera.lower_left_corner = camera.lower_left_corner - w;
    return camera;
}

struct World{
    Camera camera;
    u32 model_count;
    Model *models;
};

Ray GetRay(Camera camera, f32 u, f32 v){
    Ray ray = {};
    ray.origin = camera.pos;
    ray.direction = camera.lower_left_corner + (camera.horizontal*u) + (camera.vertical*v) - camera.pos;
    
    return ray;
}

Vec3 GetRandomUnitSpherePoint(){
    Vec3 sphere_point = {};
    b32 found = 0;
    while(!found){
        f32 x = (RandomFloat() * 2.0f) - 1;
        f32 y = (RandomFloat() * 2.0f) - 1;
        f32 z = (RandomFloat() * 2.0f) - 1;
        if((x*x+y*y+z*z) <= 1){
            sphere_point.x = x;
            sphere_point.y = y;
            sphere_point.z = z;
            found = 1;
        }
    }
    return sphere_point;
}

Vec3 GetRandomUnitVector(){
    return NormaliseVec3(GetRandomUnitSpherePoint());
}

Vec3 GetRandomInHemisphere(Vec3 normal){
    Vec3 unit_sphere_point = GetRandomUnitSpherePoint();
    if(Vec3Dot(unit_sphere_point, normal) > 0.0){
        return unit_sphere_point;
    }else{
        return Vec3(-unit_sphere_point.x, -unit_sphere_point.y, -unit_sphere_point.z);
    }
}

b32 ModelHit(Ray ray, Model model, HitInfo *hit_info){
    switch(model.type){
        case(SPHERE):{
            return SphereHit(ray, model, hit_info);
        }
        break;
        case(PLANE):{
            return PlaneHit(ray, model, hit_info);
        }
        break;
        default:{
            return 0;
        };
    };
}

void AddModel(World *world, Model model){
    if(world->model_count >= MAX_MODELS){
        printf("Model limit reached");
    }else{
        world->models[world->model_count++] = model;
    }
}

/* Thread work */
struct Job{
    World *world;
    Image *image;
    int x_start;
    int y_start;
    int x_end;
    int y_end;
};

struct JobQueue{
    u32 job_count;
    Job *jobs;
    
    u32 chunks_per_row;
    
    volatile u64 current_job_index;
    volatile u64 jobs_finished;
    volatile u64 rows_finished;
};


#endif //RAY_H
