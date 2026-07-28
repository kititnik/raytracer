#include "image.hpp"
#include <filesystem>
#include <stdexcept>

Image::Image(int width, int height) {
    width_ = width;
    height_ = height;
    InitBytesBuffer();
}

void Image::SetPixel(RGB rgb, int y, int x) {
    auto pixel = &image_bytes_[y][4 * x];
    pixel[0] = rgb.r;
    pixel[1] = rgb.g;
    pixel[2] = rgb.b;
}

void Image::InitBytesBuffer() {
    image_bytes_ = new png_bytep[height_];
    for(int y = 0; y < height_; y++) {
        image_bytes_[y] = new png_byte[4 * width_]{};
        for(int x = 0; x < width_; x++) {
            image_bytes_[y][4 * x + 3] = 255;
        }
    }
}

void Image::WriteImage(const std::filesystem::path& path) {
    auto* file_ptr = std::fopen(path.c_str(), "wb");
    if(file_ptr == nullptr) {
        throw std::runtime_error("Error opening .png file");
    }
    auto png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    auto info = png_create_info_struct(png);

    if(setjmp(png_jmpbuf(png))) {
        throw std::runtime_error("Unexpected error in libpng!");
    }

    png_init_io(png, file_ptr);
    png_set_IHDR(png, info, width_, height_, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                         PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);
    png_write_image(png, image_bytes_);
    png_write_end(png, nullptr);

    std::fclose(file_ptr);
    png_destroy_write_struct(&png, &info);
}

int Image::GetWidth() {
    return width_;
}

int Image::GetHeight() {
    return height_;
}