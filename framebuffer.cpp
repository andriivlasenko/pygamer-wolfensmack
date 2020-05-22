#include <cassert>

#include "framebuffer.h"
#include "utils.h"
#include <string.h>

void FrameBuffer::set_pixel(const size_t x, const size_t y, const uint16_t color) {    
    img[x+y*w] = color;
}

void FrameBuffer::draw_rectangle(const size_t rect_x, const size_t rect_y, const size_t rect_w, const size_t rect_h, const uint16_t color) {    
    for (size_t i=0; i<rect_w; i++) {
        for (size_t j=0; j<rect_h; j++) {
            size_t cx = rect_x+i;
            size_t cy = rect_y+j;
            if (cx<w && cy<h) // no need to check for negative values (unsigned variables)
                set_pixel(cx, cy, color);
        }
    }
}

void FrameBuffer::clear(bool doDrawMap) {                    
    int x = 0;
    if (doDrawMap)
    {
        memset(img, 0xFFFF, bufByteSize);
        x = w_half;
    }    
    for (; x < w; x++)
    {
        for (int y = 0; y < h_half; y++)
        {
            img[y * w + x] = 0x39A7;
        }
        for (int y = h_half; y < h; y++)
        {
            img[y * w + x] = 0x738E;
        }
    }        
}

size_t FrameBuffer::getFbWidth(bool doDrawMap)
{
    if (doDrawMap)
    {
        return w_half;
    }
    return w;
}

