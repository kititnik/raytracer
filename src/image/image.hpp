#pragma once

#include <filesystem>
#include <png.h> 

struct RGB {
    int r, g, b;
};

class Image {
public:
    Image(int width, int height);
    void SetPixel(RGB rgb, int y, int x);
    int GetWidth();
    int GetHeight();
    void WriteImage(const std::filesystem::path& path);
    
private:
    void InitBytesBuffer();
    
    int width_;
    int height_;
    png_bytep* image_bytes_;
};