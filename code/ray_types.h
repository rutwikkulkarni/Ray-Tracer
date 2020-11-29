/* date = November 29th 2020 10:10 am */

#ifndef RAY_TYPES_H
#define RAY_TYPES_H

#define global        static;
#define local_persist static;
#define internal      static;

#define bytes(n)      n
#define kilobytes(n) (bytes(n)*1024)
#define megabytes(n) (kilobytes(n)*1024)
#define gigabytes(n) (megabytes(n)*1024)

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef bool     b32;
typedef float    f32;
typedef double   f64;

#endif //RAY_TYPES_H
