#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <windows.h>

#include "ray_options.h"
#include "ray_types.h"
#include "ray_maths.h"
#include "ray_image.h"
#include "ray.h"

b32 RenderSection(JobQueue *job_queue){
    u64 current_index = InterlockedExchangeAdd(&job_queue->current_job_index, 1);
    if(current_index > job_queue->job_count - 1) return false;
    Job *job = job_queue->jobs + current_index;
    u32 *pixels = (u32 *)job->image->pixels;
    u32 pixel_pointer = (job->y_start * job->image->width) + job->x_start; 
    for(int y = 0; y < job->y_end; y++){
        for(int x = 0; x < job->x_end; x++){
            Vec3 pixel_colour = Vec3(0.0f, 0.0f, 0.0f);
            for(int i = 0; i < RAY_SAMPLES; i++){
                f32 random = RandomFloat();
                f32 u = ((f32)(x + job->x_start) + random) / ((f32)job->image->width - 1.0f);
                f32 v = ((f32)(y + job->y_start) + random) / ((f32)job->image->height - 1.0f);
                Ray ray = GetRay(job->world->camera, u, v);
                f32 attenuation = 1.0f;
                for(int i = 0; i < RAYS_PER_SAMPLE; i++){
                    HitInfo hit_info;
                    b32 did_hit = 0;
                    
                    f32 hit_distance = 1000.0f;
                    Vec3 result;
                    
                    u32 material;
                    HitInfo hit_info_selected;
                    
                    for(int i = 0; i < job->world->model_count; i++){
                        if(ModelHit(ray, job->world->models[i], &hit_info)){
                            if((Vec3Length(hit_info.pos - job->world->camera.pos)) < hit_distance){
                                hit_distance = Vec3Length(hit_info.pos - job->world->camera.pos);
                                //if sphere is closer than plane, render it
                                result = job->world->models[i].colour;
                                did_hit = 1;
                                material = job->world->models[i].material;
                                hit_info_selected = hit_info;
                            }
                        }
                    }
                    if(!did_hit){
                        //we hit the sky - absorb and break
                        Vec3 background_colour = RayBackground(ray);
                        //background_colour = Vec3(0.1f, 0.1f, 0.1f);
                        pixel_colour = pixel_colour + (background_colour * attenuation);
                        break;
                    }else{
                        switch(material){
                            case DIFFUSE:{
                                ray.origin = hit_info_selected.pos;
                                ray.direction = (hit_info_selected.pos + GetRandomInHemisphere(hit_info_selected.normal)) - hit_info_selected.pos;
                                attenuation *= 0.5f;
                                pixel_colour = pixel_colour + (result * attenuation);
                            };
                            break;
                            case METAL:{
                                f32 fuzz = 0.1f;
                                Vec3 reflected_ray = Vec3Reflect(ray.direction, hit_info.normal);
                                ray.origin = hit_info.pos;
                                ray.direction = reflected_ray + GetRandomUnitSpherePoint() * fuzz;
                                attenuation *= 0.5f;
                                pixel_colour = pixel_colour + (result * attenuation);
                            };
                            break;
                            default:{
                                
                            }
                        }
                        
                    }
                }
            }
            SetU32Pixel(&pixels[pixel_pointer], pixel_colour, RAY_SAMPLES);
            if(x == job->x_end - 1) pixel_pointer += (job->image->width - job->x_end) + 1;
            else pixel_pointer++;
        }
        InterlockedExchangeAdd(&job_queue->rows_finished, 1);
        printf("\rRaytracing...%.2f%%", ((f32)(job_queue->rows_finished / job_queue->chunks_per_row)) / job->image->height * 100.0f);
    }
    InterlockedExchangeAdd(&job_queue->jobs_finished, 1);
    return true;
}

DWORD WINAPI Win32WorkThread(LPVOID lpParameter){
    JobQueue *job_queue = (JobQueue *)lpParameter;
    while(RenderSection(job_queue)){}
    return 0;
}

u32 Win32GetCoreCount(){
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    u32 cores = system_info.dwNumberOfProcessors;
    return cores;
}

int main(char *argc, char**argv){
    srand(time(NULL));
    Image image = CreateImage(RAY_WIDTH, RAY_HEIGHT);
    Camera camera = CreateCamera(Vec3(0.0f, 4.0f, 6.0f) * 0.5f, Vec3(0, 1.0f, 0), Vec3(0, 0.5f, -2.0f));
    
    Model sphere1 = CreateSphere(Vec3(0, 0.5f, -2.0f), 0.5f, Vec3(140.0f / 255.0f, 40.0f / 255.0f, 49.0f / 255.0f));
    Model sphere2 = CreateSphere(Vec3(1.0f, 0.5f, -2.0f), 0.5f, Vec3(40.0f / 255.0f, 140.0f / 255.0f, 49.0f / 255.0f));
    sphere2.material = METAL;
    
    Model sphere3 = CreateSphere(Vec3(-2.0f, 2.0f, -2.0f), 2.0f, Vec3(140.0f / 255.0f, 40.0f / 255.0f, 149.0f / 255.0f));
#if 0
    Model sphere4 = CreateSphere(Vec3(6.0f, 1.5f, -2.0f), 1.5f, Vec3(252.0f / 255.0f, 186.0f / 255.0f, 3.0f / 255.0f));
    sphere4.material = METAL;
    
    Model sphere5 = CreateSphere(Vec3(3.5f, 0.2f, -1.3f), 0.2f, Vec3(40.0f / 255.0f, 40.0f / 255.0f, 149.0f / 255.0f));
    Model sphere6 = CreateSphere(Vec3(-4.0, 1.0f, -2.0f), 1.0f, Vec3(40.0f / 255.0f, 40.0f / 255.0f, 149.0f / 255.0f));
#endif
    Model plane = CreatePlane(Vec3(0, 0, 0), Vec3(0.0f, 1.0f, 0), Vec3(0.20f, 0.25f, 0.2f));
    
    World world = {};
    world.camera = camera;
    world.models = (Model *)malloc(sizeof(Model) * MAX_MODELS);
    
    AddModel(&world, plane);
    AddModel(&world, sphere1);
    AddModel(&world, sphere2);
    AddModel(&world, sphere3);
#if 0
    AddModel(&world, sphere4);
    AddModel(&world, sphere5);
    AddModel(&world, sphere6);
#endif
    
    // TODO(NAME): make this work when it is not divisible
    u32 cores = Win32GetCoreCount() * 2;
    printf("Number of logical processors: %i\n", Win32GetCoreCount());
    u32 chunks_per_row = cores / 2;
    u32 chunks_per_column = cores / chunks_per_row;
    u32 chunk_width = image.width / chunks_per_row;
    u32 chunk_height = image.height / chunks_per_column; //chunks_per_row == chunks_per_column
    
    JobQueue job_queue = {};
    job_queue.job_count = chunks_per_row * chunks_per_column;
    job_queue.jobs = (Job *)malloc(sizeof(Job) * job_queue.job_count);
    job_queue.current_job_index = 0;
    job_queue.chunks_per_row = chunks_per_row;
    job_queue.jobs_finished = 0;
    
    clock_t t;
    t = clock();
    
    for(int i = 0; i < job_queue.job_count; i++){
        u32 chunk_x = i % chunks_per_row;
        u32 chunk_y = i / chunks_per_row;
        Job *job = job_queue.jobs + i;
        job->world = &world;
        job->image = &image;
        job->x_start = chunk_x*chunk_width;
        job->y_start = chunk_y*chunk_height;
        job->x_end = chunk_width;
        job->y_end = chunk_height;
    }
    
    for(int i = 1; i < job_queue.job_count; i++){
        CreateThread(0, 0, Win32WorkThread, &job_queue, 0, 0);
    }
    
    while(job_queue.jobs_finished < job_queue.job_count){
        RenderSection(&job_queue);
    }
    
    t = clock() - t;
    printf("\nTime taken: %fms", ((f32)t / (f32)CLOCKS_PER_SEC) * 1000);
    WriteImageToBitmap("raytracer.bmp", image);
    return 0;
}