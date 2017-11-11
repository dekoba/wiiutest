/*===========================================
        GRRLIB (GX Version)
        - Template Code -

        Minimum Code To Use GRRLIB
============================================*/
#include <grrlib.h>
#include <gccore.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiiuse/wpad.h>
#include <asndlib.h>
#include <mp3player.h>
#include <fat.h>
#include "images/bulba.h"
#include "images/squirtle.h"
#include "images/bg.h"
#include "images/leaf.h"
#include "sounds/pallet.h"
#include "sounds/leafwhoosh.h"



int main(int argc, char **argv) {
    // Initialise the Graphics & Video subsystem
    GRRLIB_Init();
	
    // Initialise the Wiimotes
    WPAD_Init();
	WPADData *wd;
	
	// Initialise the audio subsystem
	ASND_Init(NULL);
	MP3Player_Init();
	
	MP3Player_PlayBuffer(pallet, pallet_size, NULL);
	//Load in things
	GRRLIB_texImg *tex_bulb = GRRLIB_LoadTexture(bulba);
	GRRLIB_texImg *tex_squirtle = GRRLIB_LoadTexture(squirtle);
	GRRLIB_texImg *tex_bg = GRRLIB_LoadTexture(bg);
	GRRLIB_texImg *tex_leaf = GRRLIB_LoadTexture(leaf);
	//set constants
	int projectileSpeed = 15;
	int speed = 5;
	int bulbaXPos = 0;
	int bulbaYPos = 380;
	int leafXPos = 0;
	int leafYPos = 0;
	//int bulbadx = 0;
	//int bulbady = 0;
	double bulbaScale = 0.2;
	double leafScale = 0.1;
	double leafRot = 270;
	
    // Loop forever
    while(1) {
		GRRLIB_DrawImg(0, 0, tex_bg, 0, 1, 1, 0xFFFFFFFF);
        WPAD_ScanPads();  // Scan the Wiimotes
		wd = WPAD_Data(0);
        // If [HOME] was pressed on the first Wiimote, break out of the loop
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)  break;
	
        // ---------------------------------------------------------------------
        // Place your drawing code here
        // ---------------------------------------------------------------------
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_B)
			leafXPos = bulbaXPos;
		    leafYPos = bulbaYPos;
			MP3Player_PlayBuffer(leafwhoosh, leafwhoosh_size, NULL);
		if(wd->btns_h & WPAD_BUTTON_RIGHT && (bulbaXPos < 600)){
			bulbaXPos = bulbaXPos + speed;
			
		}
		if(wd->btns_h & WPAD_BUTTON_LEFT && (bulbaXPos > 0)){
			bulbaXPos = bulbaXPos - speed;
			
		}
		if(wd->btns_h & WPAD_BUTTON_DOWN && (bulbaYPos > 380)){
			bulbaYPos = bulbaYPos + speed;
			
		}if(wd->btns_h & WPAD_BUTTON_UP && (bulbaYPos < 460)){
			bulbaYPos = bulbaYPos - speed;
			
		}
		if (leafXPos > -30)
		{
			GRRLIB_DrawImg(leafXPos, leafYPos, tex_leaf, leafRot, leafScale, leafScale, 0xFFFFFFFF);
			leafXPos = leafXPos - projectileSpeed;
			leafRot = (leafRot + 10) ;
			if (leafRot == 360) leafRot = 0;
		}
		
		GRRLIB_DrawImg(40, 500, tex_squirtle, 0, 0.2, 0.2, 0xFFFFFFFF);
		
		GRRLIB_DrawImg(bulbaXPos, bulbaYPos, tex_bulb, 0, bulbaScale, bulbaScale, 0xFFFFFFFF);
		
		
		
		
		
		
        GRRLIB_Render();  // Render the frame buffer to the TV
    }

    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB

    exit(0);  // Use exit() to exit a program, do not use 'return' from main()
}
