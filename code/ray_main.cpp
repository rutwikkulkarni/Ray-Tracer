#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "ray_types.h"
#include "ray_maths.h"
#include "ray_image.h"
#include "ray.h"

int main(char *argc, char**argv){
    
    Image image = CreateImage(1280, 720);
    Camera camera = {};
    camera.pos = Vec3(0, 0, 0);
    camera.viewport_height = 2.0;
    camera.viewport_width = (16.0f / 9.0f) * camera.viewport_height;
    camera.focal_length = 1.0f;
    
    camera.horizontal = Vec3(camera.viewport_width, 0, 0);
    camera.vertical = Vec3(0, camera.viewport_height, 0);
    camera.lower_left_corner = camera.pos - Vec3(camera.horizontal.x / 2.0f, camera.horizontal.y / 2.0f, camera.horizontal.z / 2.0f) - Vec3(camera.vertical.x / 2.0f, camera.vertical.y / 2.0f, camera.vertical.z / 2.0f) - Vec3(0, 0, camera.focal_length);
    
    Sphere sphere;
    sphere.origin = Vec3(0, 0, -1);
    sphere.radius = 0.5f;
    sphere.colour = Vec3(255, 0, 0);
    
    u32 *current_pixel = (u32 *)image.pixels;
    for(int y = 0; y < image.height; y++){
        for(int x = 0; x < image.width; x++){
            f32 u = (f32)x / (f32)(image.width - 1.0f);
            f32 v = (f32)y / (f32)(image.height - 1.0f);
            
            Ray ray = {};
            ray.origin = camera.pos;
            ray.direction = camera.lower_left_corner + (camera.horizontal*u) + (camera.vertical*v) - camera.pos;
            Vec3 background_colour = RayBackground(ray);
            
            
            HitInfo hit_info;
            if(SphereHit(ray, sphere, &hit_info)){
                Vec3 hit_vector_colour =  Vec3((hit_info.normal.x+1.0f) * 255.0f, (hit_info.normal.y+1.0f) * 255.0f, (hit_info.normal.z+1.0f) * 255.0f) * 0.5f;
                SetU32Pixel(current_pixel, hit_vector_colour);
            }else{
                SetU32Pixel(current_pixel, background_colour);
            }
            *current_pixel++;
        }
    }
    
    WriteImageToBitmap("raytracer.bmp", image);
    return 0;
}