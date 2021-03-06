/*---------------------------------------------------------------------------------

	Simple demonstration of sprites using textured quads

---------------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/tpl.h>
#include <grrlib.h>

#include "textures_tpl.h"
#include "textures.h"
 
#define DEFAULT_FIFO_SIZE	(256*1024)

static void *frameBuffer[2] = { NULL, NULL};
static GXRModeObj *rmode;

#define NUM_SPRITES 1

//simple sprite struct
typedef struct {
	int x,y;			// screen co-ordinates 
	int dx, dy;			// velocity
	int image;
}Sprite;

Sprite sprites[NUM_SPRITES];

GXTexObj texObj;


void drawSpriteTex( int x, int y, int width, int height, int image );

//---------------------------------------------------------------------------------
int main( int argc, char **argv ){
//---------------------------------------------------------------------------------
	u32	fb; 	// initial framebuffer index
	u32 first_frame;
	f32 yscale;
	u32 xfbHeight;
	Mtx44 perspective;
	Mtx GXmodelView2D;
	void *gp_fifo = NULL;

	GXColor background = {0, 0, 100, 0xff};

	int i;
	int evolvedYet = 0;
	int presses = 0;
	int speedx = 0;
	int speedy = 0;
	int movespeed = 512;

	VIDEO_Init();
 
	rmode = VIDEO_GetPreferredMode(NULL);
	printf("hello world from jacob");
	fb = 0;
	first_frame = 1;
	// allocate 2 framebuffers for double buffering
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	fb ^= 1;

	// setup the fifo and then init the flipper
	gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gp_fifo,0,DEFAULT_FIFO_SIZE);
 
	GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);
 
	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(background, 0x00ffffff);
 
	// other gx setup
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	if (rmode->aa)
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	else
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);


	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	// setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	

	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);


	GX_InvalidateTexAll();

	TPLFile spriteTPL;
	TPL_OpenTPLFromMemory(&spriteTPL, (void *)textures_tpl,textures_tpl_size);
	//load from file
	TPL_GetTexture(&spriteTPL,bulbasaur,&texObj);
	GX_LoadTexObj(&texObj, GX_TEXMAP0);

	guOrtho(perspective,0,479,0,639,0,300);
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);

	WPAD_Init();

	srand(time(NULL));

	for(i = 0; i < NUM_SPRITES; i++) {
		//random place and speed
		sprites[i].x = rand() % (640 - 64 ) << 8;
		sprites[i].y = rand() % (480 - 64 ) << 8 ;
		sprites[i].dx = speedx;
		sprites[i].dy = speedy;
		sprites[i].image = 0;


	}

	while(1) {
		if ( evolvedYet == 0 && presses > 10 ){
			TPL_GetTexture(&spriteTPL,ivysaur,&texObj);
			GX_LoadTexObj(&texObj, GX_TEXMAP0);
			evolvedYet = 1;
		}
		WPAD_ScanPads();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);
		if(WPAD_ButtonsDown(0)& WPAD_BUTTON_LEFT) {	speedx = -movespeed;speedy=0;presses++;}
		if(WPAD_ButtonsDown(0)& WPAD_BUTTON_RIGHT) {speedx = movespeed;speedy=0;}
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_UP) 	{speedy = -movespeed;speedx=0;}
		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN) {speedy = movespeed;speedx=0;}
		
		sprites[0].dy = speedy;
		sprites[0].dx = speedx;
		
		GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
		GX_InvVtxCache();
		GX_InvalidateTexAll();

		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

		guMtxIdentity(GXmodelView2D);
		guMtxTransApply (GXmodelView2D, GXmodelView2D, 0.0F, 0.0F, -5.0F);
		GX_LoadPosMtxImm(GXmodelView2D,GX_PNMTX0);



		
		//check for collision with the screen boundaries
		if(sprites[0].x < (1<<8) || sprites[0].x > ((640-64) << 8))
			sprites[0].dx = 0;

		if(sprites[0].y < (1<<8) || sprites[0].y > ((480-64) << 8))
			sprites[0].dy = 0;
		
		
		sprites[0].x += sprites[0].dx;
		sprites[0].y += sprites[0].dy;
		
		
		drawSpriteTex( sprites[0].x >> 8, sprites[0].y >> 8, 64, 64, sprites[0].image);
		

		GX_DrawDone();
		
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetAlphaUpdate(GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer[fb],GX_TRUE);

		VIDEO_SetNextFramebuffer(frameBuffer[fb]);
		if(first_frame) {
			VIDEO_SetBlack(FALSE);
			first_frame = 0;
		}
		VIDEO_Flush();
		VIDEO_WaitVSync();
		fb ^= 1;		// flip framebuffer
	}
	return 0;
}


//---------------------------------------------------------------------------------
// Texture co-ordinates for ball sprites
//---------------------------------------------------------------------------------
float texCoords[] = {
//---------------------------------------------------------------------------------
	0.0 ,0.0 , 0.1, 0.0, 1.0, 1.0, 0.0, 1.0,
	1.0 ,0.0 , 1.0, 0.0, 1.0, 1.0, 1.0, 1.0,
	0.0 ,1.0 , 1.0, 1.0, 1.0, 1.0, 0.0, 1.0,
	1.0 ,1.0 , 1.0, 1.0, 1.0, 1.0, 1.0, 1.0
};


//---------------------------------------------------------------------------------
void drawSpriteTex( int x, int y, int width, int height, int image ) {
//---------------------------------------------------------------------------------

	int texIndex = image * 8;

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);			// Draw A Quad
		GX_Position2f32(x, y);					// Top Left
		GX_TexCoord2f32(texCoords[texIndex],texCoords[texIndex+1]);
		texIndex+=2;
		GX_Position2f32(x+width-1, y);			// Top Right
		GX_TexCoord2f32(texCoords[texIndex],texCoords[texIndex+1]);
		texIndex+=2;
		GX_Position2f32(x+width-1,y+height-1);	// Bottom Right
		GX_TexCoord2f32(texCoords[texIndex],texCoords[texIndex+1]);
		texIndex+=2;
		GX_Position2f32(x,y+height-1);			// Bottom Left
		GX_TexCoord2f32(texCoords[texIndex],texCoords[texIndex+1]);
	GX_End();									// Done Drawing The Quad 

}

