#pragma once

struct RGB {
    int r, g, b;
};

class Image {
public:
    Image(int width, int height);
    void SetPixel(RGB rgb, int y, int x);
    int GetWidth();
    int GetHeight();
    
private:
    int width_;
    int height_;
};