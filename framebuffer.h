#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <cstdint>
#include <cstdlib>
#include <vector>

struct FrameBuffer {
    size_t w, h; // image dimensions
    size_t w_half, h_half; // image dimensions
    uint32_t bufByteSize;
    uint16_t *img; // storage container

    size_t getFbWidth(bool doDrawMap);
    void clear(bool doDrawMap);
    void set_pixel(const size_t x, const size_t y, const uint16_t color);
    void draw_rectangle(const size_t x, const size_t y, const size_t w, const size_t h, const uint16_t color);
};

#endif // FRAMEBUFFER_H

