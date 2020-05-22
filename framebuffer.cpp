#include <cassert>

#include "framebuffer.h"
#include "utils.h"
#include <string.h>

void FrameBuffer::set_pixel(const size_t x, const size_t y, const uint32_t color) {    
    img[x+y*w] = color;
}

void FrameBuffer::draw_rectangle(const size_t rect_x, const size_t rect_y, const size_t rect_w, const size_t rect_h, const uint32_t color) {    
    for (size_t i=0; i<rect_w; i++) {
        for (size_t j=0; j<rect_h; j++) {
            size_t cx = rect_x+i;
            size_t cy = rect_y+j;
            if (cx<w && cy<h) // no need to check for negative values (unsigned variables)
                set_pixel(cx, cy, color);
        }
    }
}

void FrameBuffer::clear(const uint32_t color) {        
    uint32_t size = w * h * 2;    
    memset(img, 0xFFFF, size);    
}

size_t FrameBuffer::getFbWidth(bool doDrawMap)
{
    if (doDrawMap)
    {
        return w_half;
    }
    return w;
}

