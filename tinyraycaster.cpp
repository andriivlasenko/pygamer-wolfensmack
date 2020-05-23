#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <cassert>
#include <algorithm>

#include "utils.h"
#include "tinyraycaster.h"

#define M_PIx2      6.28318530717958647692

int wall_x_texcoord(const float hitx, const float hity, Texture *tex_walls) {
    float x = hitx - floor(hitx+.5); // x and y contain (signed) fractional parts of hitx and hity,
    float y = hity - floor(hity+.5); // they vary between -0.5 and +0.5, and one of them is supposed to be very close to 0
    int tex = x*tex_walls->size;
    if (std::abs(y)>std::abs(x)) // we need to determine whether we hit a "vertical" or a "horizontal" wall (w.r.t the map)
        tex = y*tex_walls->size;
    if (tex<0) // do not forget x_texcoord can be negative, fix that
        tex += tex_walls->size;
    assert(tex>=0 && tex<(int)tex_walls->size);
    return tex;
}

void draw_map(FrameBuffer &fb, const std::vector<Sprite> &sprites, Texture *tex_walls, const Map &map, const size_t cell_w, const size_t cell_h) {
    for (size_t j=0; j<map.h; j++) {  // draw the map itself
        for (size_t i=0; i<map.w; i++) {
            if (map.is_empty(i, j)) continue; // skip empty spaces
            size_t rect_x = i*cell_w;
            size_t rect_y = j*cell_h;
            size_t texid = map.get(i, j);            
            fb.draw_rectangle(rect_x, rect_y, cell_w, cell_h, tex_walls->get(0, 0, texid)); // the color is taken from the upper left pixel of the texture #texid
        }
    }
    for (size_t i=0; i<sprites.size(); i++) { // show the monsters
        fb.draw_rectangle(sprites[i].x*cell_w-3, sprites[i].y*cell_h-3, 6, 6, 0xF800);
    }
}

void draw_sprite(FrameBuffer &fb, const GameState &gs, const Sprite &sprite, float *depth_buffer, const Player &player, Texture *tex_sprites) {
    size_t curFbWidth = fb.getFbWidth(gs.doDrawMap);
    // absolute direction from the player to the sprite (in radians)
    float sprite_dir = atan2(sprite.y - player.y, sprite.x - player.x);
    while (sprite_dir - player.a >  M_PI) sprite_dir -= M_PIx2; // remove unncesessary periods from the relative direction
    while (sprite_dir - player.a < -M_PI) sprite_dir += M_PIx2;

    size_t sprite_screen_size = std::min(MAX_STUFF, static_cast<int>(fb.h/sprite.player_dist)); // screen sprite size
    int h_offset = (sprite_dir - player.a)*(curFbWidth)/(player.fov) + (curFbWidth)/2 - sprite_screen_size/2; // do not forget the 3D view takes only a half of the framebuffer, thus curFbWidth for the screen width
    int v_offset = fb.h_half - sprite_screen_size/2;

    for (size_t i=0; i<sprite_screen_size; i++) {
        if (h_offset+int(i)<0 || h_offset+i>=curFbWidth) continue;
        if (depth_buffer[h_offset+i]<sprite.player_dist) continue; // this sprite column is occluded
        for (size_t j=0; j<sprite_screen_size; j++) {
            if (v_offset+int(j)<0 || v_offset+j>=fb.h) continue;
            uint16_t color = tex_sprites->get(i*tex_sprites->size/sprite_screen_size, j*tex_sprites->size/sprite_screen_size, sprite.tex_id);

            if(color != 0xFFFF)
            {
                fb.set_pixel(curFbWidth + h_offset+i, v_offset+j, color);
            }
        }
    }
}

void render(FrameBuffer &fb, const GameState &gs, uint16_t *columnArr, float *depth_buffer, size_t cell_w, size_t cell_h) {
    const Map &map                     = gs.map;
    const Player &player               = gs.player;
    const std::vector<Sprite> &sprites = gs.monsters;
    Texture *tex_walls           = gs.tex_walls;
    Texture *tex_monst           = gs.tex_monst;

    size_t curFbWidth = fb.getFbWidth(gs.doDrawMap);

    fb.clear(gs.doDrawMap); // clear the screen
        
    for (int i = 0; i < curFbWidth; i++)
    {
        depth_buffer[i] = 1e3;
    }
    

    for (size_t i=0; i<curFbWidth; i++) { // draw the visibility cone AND the "3D" view
        float angle = player.a-player.fov/2 + player.fov*i/float(curFbWidth);
        for (float t=0; t<10; t+=.02) {//for (float t=0; t<20; t+=.01) { // ray marching loop
            float x = player.x + t*cos(angle);
            float y = player.y + t*sin(angle);
            if(gs.doDrawMap)
            {
                fb.set_pixel(x*cell_w, y*cell_h, 0x7C5F); // this draws the visibility cone
            }

            if (map.is_empty(x, y)) continue;

            size_t texid = map.get(x, y); // our ray touches a wall, so draw the vertical column to create an illusion of 3D            
            float dist = t*cos(angle-player.a);
            depth_buffer[i] = dist;
            size_t column_height = std::min(MAX_STUFF, int(fb.h/dist));            
            if (column_height > MAX_STUFF)
            {
                column_height = MAX_STUFF;
            }
            int x_texcoord = wall_x_texcoord(x, y, tex_walls);
            tex_walls->get_scaled_column(texid, x_texcoord, columnArr, column_height);
            uint16_t *column = columnArr;
            int pix_x = i;// + curFbWidth; // we are drawing at the right half of the screen, thus +curFbWidth
            if(gs.doDrawMap)
            {
                pix_x += fb.w_half;
            }
            for (size_t j=0; j<column_height; j++) { // copy the texture column to the framebuffer
                int pix_y = j + fb.h_half - column_height/2;
                if (pix_y>=0 && pix_y<(int)fb.h) {
                    fb.set_pixel(pix_x, pix_y, column[j]);
                }
            }
            break;
        } // ray marching loop
    } // field of view ray sweeping

    if(gs.doDrawMap)
    {
        draw_map(fb, sprites, tex_walls, map, cell_w, cell_h);
    }    

    for (size_t i=0; i<sprites.size(); i++) { // draw the sprites
        draw_sprite(fb, gs, sprites[i], depth_buffer, player, tex_monst);
    }

    if(!gs.doDrawMap)
    {
        for (size_t i=0; i<gs.tex_weapon->size; i++) {        
            for (size_t j=0; j<gs.tex_weapon->size; j++) {            
                uint16_t color = gs.tex_weapon->get(i, j, gs.attack);

                if(color != 0xFFFF)
                {
                    fb.set_pixel(curFbWidth + 48+i, 64+j, color); //48 - hor offset, 65 - ver offset
                }
            }
        }
    }
}

