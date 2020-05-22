#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#define _USE_MATH_DEFINES
#include <cmath>

#include "utils.h"
#include "tinyraycaster.h"
#include "gui.h"
#include "SdFat.h"
#include "Adafruit_ImageReader.h"


int demoMain() 
{
    // Serial.begin(9600);
    // Serial.print("Tiny ray caster demo");

    SdFat sdCard;         // SD card filesystem

    if (!sdCard.begin(SS, SD_SCK_MHZ(50))) {
        sdCard.initErrorHalt();
    }    

    Adafruit_ImageReader imageReader(sdCard);

    Adafruit_Arcada arcada;
    uint16_t *framebuffer;    


    if (!arcada.arcadaBegin()) 
    {
        // Serial.print("Failed to begin");
        while (1);
    }
    arcada.displayBegin();
    
    // Turn on backlight
    arcada.setBacklight(255);

    if (! arcada.createFrameBuffer(ARCADA_TFT_WIDTH, ARCADA_TFT_HEIGHT)) 
    {
        // Serial.print("Failed to allocate framebuffer");
        while (1);
    }
    framebuffer = arcada.getFrameBuffer();

    FrameBuffer fb{ARCADA_TFT_WIDTH, ARCADA_TFT_HEIGHT, std::vector<uint32_t>(ARCADA_TFT_WIDTH*ARCADA_TFT_HEIGHT, pack_color(255, 255, 255))};  

    Texture wallText = Texture((void *)(&imageReader), "walltext24half.bmp", false);
    // Serial.print("Loaded texture 1");
    // Texture monsterText = Texture((void *)(&imageReader), "monsters24half.bmp", false);    
    GameState gs{ Map(),                                // game map
                  {3.456, 2.345, 1.523, M_PI/3., 0, 0}, // player
                  { {3.523, 3.812, 2, 0},               // monsters lists
                    {1.834, 8.765, 0, 0},
                    {5.323, 5.365, 1, 0},
                    {14.32, 13.36, 3, 0},
                    {4.123, 10.76, 1, 0} },
                  &wallText,  // textures for the walls
                  &wallText}; // textures for the monsters
    if (!gs.tex_walls->count || !gs.tex_monst->count) {        
        // Serial.print("Failed to load textures");
        return -1;
    }
    // Serial.print("Created game state");

   // SDL_Window   *window   = nullptr;
   // SDL_Renderer *renderer = nullptr;

   // if (SDL_CreateWindowAndRenderer(fb.w, fb.h, SDL_WINDOW_SHOWN | SDL_WINDOW_INPUT_FOCUS, &window, &renderer)) {
   //     std::cerr << "Failed to create window and renderer: " << SDL_GetError() << std::endl;
   //     return -1;
   // }

   // SDL_Texture *framebuffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, fb.w, fb.h);
   // if (!framebuffer_texture) {
   //     std::cerr << "Failed to create framebuffer texture : " << SDL_GetError() << std::endl;
   //     return -1;
   // }

   auto t1 = millis();
   while (1) {
       { // sleep if less than 20 ms since last re-rendering; TODO: decouple rendering and event polling frequencies
           auto t2 = millis();
           auto fp_ms = t2 - t1;
           if (fp_ms < 20) {
               delay(3);
               continue;
           }
           t1 = t2;
       }


    uint8_t pressed_buttons = arcada.readButtons();  
  
    if (pressed_buttons & ARCADA_BUTTONMASK_A) 
    {
        gs.player.walk =  1;
    }
    else if (pressed_buttons & ARCADA_BUTTONMASK_B) 
    {
        gs.player.walk =  -1;
    } 
    else 
    {
        gs.player.walk =  0;
    }
    
    if (pressed_buttons & ARCADA_BUTTONMASK_SELECT) 
    {
        gs.player.turn = -1;
    } 
    else if (pressed_buttons & ARCADA_BUTTONMASK_START) 
    {
        gs.player.turn = 1;
    }
    else
    {
        gs.player.turn = 0;
    }
    
  
    int joyX = arcada.readJoystickX() / 512.0 * 15.0 + 20; 
    int joyY = arcada.readJoystickY() / 512.0 * 15.0 + 100; 
  



// Serial.print("Inside main loop");
       { // poll events and update player's state (walk/turn flags); TODO: move this block to a more appropriate place
           // SDL_Event event;
           // if (SDL_PollEvent(&event)) {
           //     if (SDL_QUIT==event.type || (SDL_KEYDOWN==event.type && SDLK_ESCAPE==event.key.keysym.sym)) break;
           //     if (SDL_KEYUP==event.type) {
           //         if ('a'==event.key.keysym.sym || 'd'==event.key.keysym.sym) gs.player.turn = 0;
           //         if ('w'==event.key.keysym.sym || 's'==event.key.keysym.sym) gs.player.walk = 0;
           //     }
           //     if (SDL_KEYDOWN==event.type) {
           //         if ('a'==event.key.keysym.sym) gs.player.turn = -1;
           //         if ('d'==event.key.keysym.sym) gs.player.turn =  1;
           //         if ('w'==event.key.keysym.sym) gs.player.walk =  1;
           //         if ('s'==event.key.keysym.sym) gs.player.walk = -1;
           //     }
           // }
       }       

       { // update player's position; TODO: move this block to a more appropriate place
           gs.player.a += float(gs.player.turn)*.05; // TODO measure elapsed time and modify the speed accordingly
           float nx = gs.player.x + gs.player.walk*cos(gs.player.a)*.05;
           float ny = gs.player.y + gs.player.walk*sin(gs.player.a)*.05;

           if (int(nx)>=0 && int(nx)<int(gs.map.w) && int(ny)>=0 && int(ny)<int(gs.map.h)) {
               if (gs.map.is_empty(nx, gs.player.y)) gs.player.x = nx;
               if (gs.map.is_empty(gs.player.x, ny)) gs.player.y = ny;
           }
           for (size_t i=0; i<gs.monsters.size(); i++) { // update the distances from the player to each sprite
               gs.monsters[i].player_dist = std::sqrt(pow(gs.player.x - gs.monsters[i].x, 2) + pow(gs.player.y - gs.monsters[i].y, 2));
           }
           std::sort(gs.monsters.begin(), gs.monsters.end()); // sort it from farthest to closest
       }
       // Serial.print("Before render");
       render(fb, gs); // render the scene to the frambuffer
       // Serial.print("After render");
       for (int x = 0; x < ARCADA_TFT_WIDTH; x++)            
       {
           for (int y = 0; y < ARCADA_TFT_HEIGHT; y++)
           {
               framebuffer[y * ARCADA_TFT_WIDTH + x] = fb.img[y * ARCADA_TFT_WIDTH + x];  
           }
       }                
 // Serial.print("Before blit");
       arcada.blitFrameBuffer(0, 0, true, false); // block on blit
       // Serial.print("After blit");
   }

    return 0;
}
