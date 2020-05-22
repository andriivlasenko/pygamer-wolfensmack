#include <iostream>
#include <cassert>

#include "utils.h"
#include "textures.h"
#include "Adafruit_ImageReader.h"


Texture::Texture(void *imageReader, char *path, bool isWhiteTrans) : img_w(0), img_h(0), count(0), size(0), img() 
{    
  // Serial.print("In texture creation");
    uint16_t red_mask = 0xF800;
    uint16_t green_mask = 0x7E0;
    uint16_t blue_mask = 0x1F;    

    Adafruit_Image image;
// Serial.print("Loading bmp");
    if(((Adafruit_ImageReader *)imageReader)->loadBMP(path, image) != IMAGE_SUCCESS)
    {
        // Serial.print("Failed to load image");
        while (1);
    }
// Serial.print("Loaded bmp");
    int w = image.width();
    int h = image.height();

    count = w/h;
    size = w/count;
    img_w = w;
    img_h = h;    
// Serial.print("Transfering colors");
    img = std::vector<uint32_t>(w*h);
    // Serial.print("Buffer allocated");
    for (int j=0; j<h; j++) {
        for (int i=0; i<w; i++) {            
            uint8_t r = ((((GFXcanvas16 *)image.getCanvas())->getBuffer()[i+j*w] & red_mask) >> 11) << 3;
            uint8_t g = ((((GFXcanvas16 *)image.getCanvas())->getBuffer()[i+j*w] & green_mask) >> 5) << 2;
            uint8_t b = (((GFXcanvas16 *)image.getCanvas())->getBuffer()[i+j*w] & blue_mask) << 3;
            uint8_t a = 255;
            if(isWhiteTrans &&
                r == 255 &&
                g == 255 &&
                b == 255)
            {
                a = 0;
            }
            
            img[i+j*w] = pack_color(r, g, b, a);
        }
    }
        // Serial.print("All done");
}

uint32_t Texture::get(const size_t i, const size_t j, const size_t idx) const {
    assert(i<size && j<size && idx<count);
    return img[i+idx*size+j*img_w];
}

std::vector<uint32_t> Texture::get_scaled_column(const size_t texture_id, const size_t tex_coord, const size_t column_height) const {
    assert(tex_coord<size && texture_id<count);
    std::vector<uint32_t> column(column_height);
    for (size_t y=0; y<column_height; y++) {
        column[y] = get(tex_coord, (y*size)/column_height, texture_id);
    }
    return column;
}
