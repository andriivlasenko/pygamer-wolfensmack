#ifndef TEXTURES_H
#define TEXTURES_H

#include <vector>
#include <cstdint>

struct Texture {
    size_t img_w, img_h; // overall image dimensions
    size_t count, size;  // number of textures and size in pixels
    std::vector<uint16_t> img; // textures storage container

    Texture(void *imageReader, char *path);
    uint16_t get(const size_t i, const size_t j, const size_t idx) const; // get the pixel (i,j) from the texture idx
    void get_scaled_column(const size_t texture_id, const size_t tex_coord, uint16_t *columnArr, const size_t column_height); // retrieve one column (tex_coord) from the texture texture_id and scale it to the destination size
};

#endif // TEXTURES_H

