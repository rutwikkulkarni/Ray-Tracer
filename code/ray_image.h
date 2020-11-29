/* date = November 29th 2020 1:08 pm */

#ifndef RAY_IMAGE_H
#define RAY_IMAGE_H

#pragma pack(push, 1)
struct BitmapHeader{
    //File Header
    u16 signature;
    u32 bitmap_size;
    u16 reserved_1;
    u16 reserved_2;
    u32 data_offset;
    
    //DIB Header
    u32 header_size; //should be 40
    i32 bitmap_width;
    i32 bitmap_height;
    u16 colour_planes; //should be 1
    u16 bits_per_pixel;
    u32 compression; //should be 0
    u32 image_size; //should be 0
    i32 horizontal_resolution; //0
    i32 vertical_resolution; //0
    u32 num_colours; //0
    u32 used_colours; //0
};
#pragma pack(pop)

struct Image{
    u32 width;
    u32 height;
    u32 size_bytes;
    void *pixels;
};

Image CreateImage(u32 width, u32 height){
    Image image;
    image.width = 1280;
    image.height = 720;
    image.size_bytes = sizeof(u32) * image.width * image.height;
    image.pixels = (u32 *)malloc(image.size_bytes);
    
    return image;
}

void SetU32Pixel(u32 *pixel, Vec3 colour){
    u8 a = 0x00;
    u8 r = colour.r;
    u8 g = colour.g;
    u8 b = colour.b;
    *pixel = (a << 24) | (r << 16) | (g << 8) | b;
}

void WriteImageToBitmap(char *file_name, Image image){
    BitmapHeader out_header = {};
    out_header.signature      = 0x4D42;
    out_header.bitmap_size    = sizeof(out_header) + image.size_bytes;
    out_header.data_offset    = sizeof(out_header);
    out_header.header_size    = sizeof(out_header) - 14;
    out_header.bitmap_width   = image.width;
    out_header.bitmap_height  = image.height;
    out_header.colour_planes  = 1;
    out_header.bits_per_pixel = 32;
    out_header.image_size     = image.size_bytes;
    
    FILE *out_fp = fopen(file_name, "wb");
    if(out_fp){
        fwrite(&out_header, sizeof(out_header), 1, out_fp);
        fwrite(image.pixels, image.size_bytes, 1, out_fp);
        fclose(out_fp);
    }
}

#endif //RAY_IMAGE_H
