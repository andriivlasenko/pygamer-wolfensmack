#ifndef TINYRAYCASTER_H
#define TINYRAYCASTER_H

#include <vector>

#include "map.h"
#include "player.h"
#include "sprite.h"
#include "framebuffer.h"
#include "textures.h"

struct GameState {
    Map map;
    Player player;
    std::vector<Sprite> monsters;
    Texture *tex_walls;
    Texture *tex_monst;
    Texture *tex_weapon;
    bool doDrawMap;
    uint8_t attack;
};

void render(FrameBuffer &fb, const GameState &gs, uint16_t *columnArr, float *depth_buffer, size_t cell_w, size_t cell_h, unsigned long tick);

#endif // TINYRAYCASTER_H

