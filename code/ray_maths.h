/* date = November 29th 2020 1:09 pm */

#ifndef RAY_MATHS_H
#define RAY_MATHS_H

#define PI 3.14159256

f32 ToRadians(f32 degrees){
    return degrees*PI/180.0f;
}

f32 Lerp(float a, float b, float t){
    return a + t * (b-a);
}

struct Vec3{
    union{
        struct{
            float x, y, z;
        };
        struct{
            float r, g, b;
        };
    };
};

Vec3 Vec3Lerp(Vec3 a, Vec3 b, float t){
    Vec3 result = {};
    result.x = Lerp(a.x, b.x, t);
    result.y = Lerp(a.y, b.y, t);
    result.z = Lerp(a.z, b.z, t);
    return result;
}

Vec3 InitVec3(float x, float y, float z){
    Vec3 v3 = {x, y, z};
    return v3;
}
#define Vec3(x, y, z) InitVec3(x, y, z)

Vec3 NormaliseVec3(Vec3 vec3){
    f32 length = sqrt((vec3.x * vec3.x) + (vec3.y * vec3.y) + (vec3.z * vec3.z));
    if(length != 0.0f){
        Vec3 final = Vec3((vec3.x / length), (vec3.y / length), (vec3.z / length));
        return final;
    }else{
        return Vec3(0.0f, 0.0f, 0.0f);
    }
}

Vec3 Vec3Cross(Vec3 a, Vec3 b){
    return Vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

f32 Vec3Dot(Vec3 a, Vec3 b){
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 operator+(Vec3 a, Vec3 b){
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 operator-(Vec3 a, Vec3 b){
    return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 operator*(Vec3 a, f32 b){
    return Vec3(a.x * b, a.y * b, a.z * b);
}

#endif //RAY_MATHS_H
