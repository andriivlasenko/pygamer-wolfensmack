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
#include "audio.h"


#define MONSTER_LIFE 60


void play_tune(const uint8_t *audio, uint32_t audio_length) {
    uint32_t t;
    uint32_t prior, usec = 1000000L / SAMPLE_RATE;
    analogWriteResolution(8);
    for (uint32_t i=0; i<audio_length; i++) {
        while((t = micros()) - prior < usec);
        analogWrite(A0, (uint16_t)audio[i] / 8);
        analogWrite(A1, (uint16_t)audio[i] / 8);
        prior = t;
    }
}

uint32_t display_pixelneo_lights(byte wheelPos, Adafruit_Arcada &arcada) {
    if(wheelPos < 85) {
        return arcada.pixels.Color(wheelPos * 3, 255 - wheelPos * 3, 0);
    } else if(wheelPos < 170) {
        wheelPos -= 85;
        return arcada.pixels.Color(255 - wheelPos * 3, 0, wheelPos * 3);
    } else {
        wheelPos -= 170;
        return arcada.pixels.Color(0, wheelPos * 3, 255 - wheelPos * 3);
    }
}

int demoMain() 
{
    //  Serial.begin(9600);
    //  Serial.print("Tiny ray caster demo");

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
    //arcada.display->fillScreen(ARCADA_BLACK);

    if (! arcada.createFrameBuffer(ARCADA_TFT_WIDTH, ARCADA_TFT_HEIGHT)) 
    {
        // Serial.print("Failed to allocate framebuffer");
        while (1);
    }

    framebuffer = arcada.getFrameBuffer();

    FrameBuffer fb{ARCADA_TFT_WIDTH, ARCADA_TFT_HEIGHT,ARCADA_TFT_WIDTH/2, ARCADA_TFT_HEIGHT/2, ARCADA_TFT_WIDTH * ARCADA_TFT_HEIGHT * 2, framebuffer};  

    Texture wallTex = Texture((void *)(&imageReader), "walltext24.bmp");    
    Texture monsterTex = Texture((void *)(&imageReader), "monsters24.bmp");    
    Texture chainsawTex = Texture((void *)(&imageReader), "chainsaw24.bmp");    

    GameState gs{ Map(),                                // game map
                  {3.456, 2.345, 1.523, M_PI/3., 0, 0}, // player
                  { {3.523, 3.812, 2, 0, MONSTER_LIFE },               // monsters lists
                    {1.834, 8.765, 0, 0, MONSTER_LIFE * 2},
                    {5.323, 5.365, 1, 0, MONSTER_LIFE},
                    {14.32, 13.36, 3, 0, MONSTER_LIFE},
                    {4.123, 10.76, 1, 0, MONSTER_LIFE} },
                  &wallTex,  // textures for the walls
                  &monsterTex, // textures for the monsters
                  &chainsawTex, // textures for the weapons
                  false,
                  0}; 
    if (!gs.tex_walls->count || !gs.tex_monst->count) {        
        // Serial.print("Failed to load textures");
        return -1;
    }

   uint16_t *columnArr = new uint16_t [MAX_STUFF];
   float *depth_buffer = new float [fb.w];

   size_t cell_w = fb.w/(gs.map.w*2); // size of one map cell on the screen
   size_t cell_h = fb.h/gs.map.h;

   unsigned long t1 = millis();
   while (1) 
   {       
        unsigned long t2 = millis();
        float delta_ms = (float)(t2 - t1) / 400.;
        t1 = t2;    


        uint8_t pressed_buttons = arcada.readButtons();  
        int joyX = arcada.readJoystickX(); 
        int joyY = arcada.readJoystickY(); 

        if(joyY < -300)
        {
            gs.player.walk =  1;
        }
        else if(joyY > 300)
        {
            gs.player.walk =  -1;
        }
        else
        {
            gs.player.walk = 0;
        }

        if(joyX < -300)
        {
            gs.player.turn =  -1;
        }
        else if(joyX > 300)
        {
            gs.player.turn =  1;
        }
        else
        {
            gs.player.turn = 0;
        }
            
        if (pressed_buttons & ARCADA_BUTTONMASK_START) 
        {
            gs.doDrawMap = true;
        }
        else
        {
            gs.doDrawMap = false;
        }

        if (pressed_buttons & ARCADA_BUTTONMASK_A) 
        {
            gs.attack = 1;
        }
        else
        {
            gs.attack = 0;
        }             

        gs.player.a += float(gs.player.turn)*delta_ms*.5; // TODO measure elapsed time and modify the speed accordingly
        float nx = gs.player.x + gs.player.walk*cos(gs.player.a)*delta_ms;
        float ny = gs.player.y + gs.player.walk*sin(gs.player.a)*delta_ms;

        if (int(nx)>=0 && int(nx)<int(gs.map.w) && int(ny)>=0 && int(ny)<int(gs.map.h)) {
            if (gs.map.is_empty(nx, gs.player.y)) gs.player.x = nx;
            if (gs.map.is_empty(gs.player.x, ny)) gs.player.y = ny;
        }

        bool isVictory = true;

        for (size_t i=0; i<gs.monsters.size(); i++) { // update the distances from the player to each sprite
            gs.monsters[i].player_dist = std::sqrt(pow(gs.player.x - gs.monsters[i].x, 2) + pow(gs.player.y - gs.monsters[i].y, 2));
            if(gs.monsters[i].life>0)
            {
                isVictory = false;
            }
        }
        std::sort(gs.monsters.begin(), gs.monsters.end()); // sort it from farthest to closest
       
        render(fb, gs, columnArr, depth_buffer, cell_w, cell_h, t2); // render the scene to the frambuffer

        arcada.blitFrameBuffer(0, 0, true, false); // block on blit

        if (isVictory)
        {
            arcada.enableSpeaker(true);
            play_tune(audio, sizeof(audio));
            arcada.enableSpeaker(false);
            break;            
        }                
    }

    arcada.display->fillScreen(ARCADA_BLACK);
    arcada.display->setCursor(0, 0);
    arcada.display->setTextWrap(true);
    arcada.display->setTextColor(ARCADA_GREEN);

    arcada.display->println("");
    arcada.display->println("");
    arcada.display->println("");
    arcada.display->println("");
    arcada.display->println("");
    arcada.display->println("");
    arcada.display->println("");
    arcada.display->println("");
    arcada.display->println("     !!!  VICTORY !!!");

    uint8_t j = 0;
    while(1)
    {
        for(int32_t i=0; i< arcada.pixels.numPixels(); i++) {
            arcada.pixels.setPixelColor(i, display_pixelneo_lights(((i * 256 / arcada.pixels.numPixels()) + j*5) & 255, arcada));
        }
        arcada.pixels.show();
        j++;
        delay(25);
    }


    delete columnArr;
    delete depth_buffer;
    return 0;
}


