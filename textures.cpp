#include <iostream>
#include <cassert>

#include "utils.h"
#include "textures.h"
#include "Adafruit_ImageReader.h"


Texture::Texture(void *imageReader, char *path) : img_w(0), img_h(0), count(0), size(0), img() 
{       
    Adafruit_Image image;
    if(((Adafruit_ImageReader *)imageReader)->loadBMP(path, image) != IMAGE_SUCCESS)
    {        
        while (1);
    }

    int w = image.width();
    int h = image.height();

    count = w/h;
    size = w/count;
    img_w = w;
    img_h = h;    

    img = std::vector<uint16_t>(w*h);    
    for (int j=0; j<h; j++) {
        for (int i=0; i<w; i++) {            
            img[i+j*w] = ((GFXcanvas16 *)image.getCanvas())->getBuffer()[i+j*w];
        }
    }        
}

uint16_t Texture::get(const size_t i, const size_t j, const size_t idx) const {    
    return img[i+idx*size+j*img_w];
}

void Texture::get_scaled_column(const size_t texture_id, const size_t tex_coord, uint16_t *columnArr, const size_t column_height) {        
    for (size_t y=0; y<column_height; y++) {
        columnArr[y] = get(tex_coord, (y*size)/column_height, texture_id);
    }    
}
