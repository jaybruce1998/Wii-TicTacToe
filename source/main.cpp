#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/system.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "letters.h"

static GXRModeObj *rmode = NULL;

#define DEFAULT_FIFO_SIZE	(256*1024)
 
static void *frameBuffer[2] = { NULL, NULL};

// Define spacing and grid offsets
f32 spacing = 0.2f; // Space between quads
f32 quad_size = 1.0f; // Size of each quad
f32 grid_start_x = -2.0f; // Starting X position of the grid
f32 grid_start_y = 1.25f;  // Starting Y position of the grid
char board[3][3];
char turn, winner;
int cur;

void drawGrid(Mtx view) {
	Mtx model, modelview;
	// Draw 3x3 grid of quads
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {
			// Calculate position of each quad
			f32 posX = grid_start_x + col * (quad_size + spacing);
			f32 posY = grid_start_y - row * (quad_size + spacing);

			// Create transformation matrix for the current quad
			guMtxIdentity(model);
			guMtxTransApply(model, model, posX, posY, -6.0f); // Z = -6.0 to bring it into view
			guMtxConcat(view, model, modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xA5, 0xA5, 0x56, 0x00 });

			// Draw the quad
			GX_Begin(GX_QUADS, GX_VTXFMT0, 4);			
				GX_Position3f32(-quad_size / 2, quad_size / 2, 0.0f);	// Top Left
				GX_Position3f32( quad_size / 2, quad_size / 2, 0.0f);	// Top Right
				GX_Position3f32( quad_size / 2,-quad_size / 2, 0.0f);	// Bottom Right
				GX_Position3f32(-quad_size / 2,-quad_size / 2, 0.0f);	// Bottom Left
			GX_End();
		}
	}
}

void drawCursor(Mtx view, int row, int col) {
    Mtx model, modelview;
    f32 cursorThickness = 0.1f;  // Thin line thickness for the cursor
    f32 posX = grid_start_x + col * (quad_size + spacing);
    f32 posY = grid_start_y - row * (quad_size + spacing);

    // Top horizontal line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY + quad_size / 2, -6.0f); // Top of the square
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
	GX_SetTevColor(GX_TEVREG1, (GXColor){ 0x00, 0xFF, 0x00, 0xFF }); //Green

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, cursorThickness / 2, 0.0f);  // Top Left
        GX_Position3f32( quad_size / 2, cursorThickness / 2, 0.0f);  // Top Right
        GX_Position3f32( quad_size / 2, -cursorThickness / 2, 0.0f); // Bottom Right
        GX_Position3f32(-quad_size / 2, -cursorThickness / 2, 0.0f); // Bottom Left
    GX_End();

    // Bottom horizontal line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY - quad_size / 2, -6.0f); // Bottom of the square
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, cursorThickness / 2, 0.0f);  // Top Left
        GX_Position3f32( quad_size / 2, cursorThickness / 2, 0.0f);  // Top Right
        GX_Position3f32( quad_size / 2, -cursorThickness / 2, 0.0f); // Bottom Right
        GX_Position3f32(-quad_size / 2, -cursorThickness / 2, 0.0f); // Bottom Left
    GX_End();

    // Left vertical line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX - quad_size / 2, posY, -6.0f); // Left side of the square
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-cursorThickness / 2,  quad_size / 2, 0.0f); // Top Left
        GX_Position3f32( cursorThickness / 2,  quad_size / 2, 0.0f); // Top Right
        GX_Position3f32( cursorThickness / 2, -quad_size / 2, 0.0f); // Bottom Right
        GX_Position3f32(-cursorThickness / 2, -quad_size / 2, 0.0f); // Bottom Left
    GX_End();

    // Right vertical line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX + quad_size / 2, posY, -6.0f); // Right side of the square
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-cursorThickness / 2,  quad_size / 2, 0.0f); // Top Left
        GX_Position3f32( cursorThickness / 2,  quad_size / 2, 0.0f); // Top Right
        GX_Position3f32( cursorThickness / 2, -quad_size / 2, 0.0f); // Bottom Right
        GX_Position3f32(-cursorThickness / 2, -quad_size / 2, 0.0f); // Bottom Left
    GX_End();
}

void drawX(Mtx view, int row, int col) {
    Mtx model, modelview;
    guVector axis;
    f32 cursorThickness = 0.1f;  // Thickness for the lines
    f32 posX = grid_start_x + col * (quad_size + spacing);
    f32 posY = grid_start_y - row * (quad_size + spacing);

    // First diagonal line
    guMtxIdentity(model);
    
    axis.x = 0;
    axis.y = 0;
    axis.z = 1.0f;  // Rotate around the Z-axis to create diagonal
    guMtxRotAxisDeg(model, &axis, 45.0f);  // 45-degree rotation

    guMtxTransApply(model, model, posX, posY, -6.0f);  // Apply translation
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xFF, 0x14, 0x93, 0xFF }); // Pink color
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, cursorThickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, cursorThickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -cursorThickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -cursorThickness / 2, 0.0f);
    GX_End();

    // Second diagonal line (opposite direction)
    guMtxIdentity(model);

    axis.x = 0;
    axis.y = 0;
    axis.z = 1.0f;
    guMtxRotAxisDeg(model, &axis, -45.0f);  // Opposite 45-degree rotation

    guMtxTransApply(model, model, posX, posY, -6.0f);  // Apply translation
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, cursorThickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, cursorThickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -cursorThickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -cursorThickness / 2, 0.0f);
    GX_End();
}

void drawO(Mtx view, int row, int col) {
    Mtx model, modelview;
    int numSegments = 40;  // Number of segments for the circle
    f32 radiusOuter = quad_size / 2;  // Outer radius for the "O"
    f32 radiusInner = radiusOuter - 0.2f;  // Inner radius for the "O"
    f32 angleStep = 2 * M_PI / numSegments;  // Step size for each segment
    f32 posX = grid_start_x + col * (quad_size + spacing);
    f32 posY = grid_start_y - row * (quad_size + spacing);

    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY, -6.0f);  // Position the O
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
	GX_SetTevColor(GX_TEVREG1, (GXColor){ 0x80, 0x00, 0x80, 0xFF }); // Purple color
    
    // Draw outer ring
    GX_Begin(GX_TRIANGLESTRIP, GX_VTXFMT0, 2 * numSegments + 2);
    for (int i = 0; i <= numSegments; i++) {
        f32 angle = i * angleStep;
        f32 xOuter = radiusOuter * cos(angle);
        f32 yOuter = radiusOuter * sin(angle);
        f32 xInner = radiusInner * cos(angle);
        f32 yInner = radiusInner * sin(angle);

        // Outer vertex
        GX_Position3f32(xOuter, yOuter, 0.0f);

        // Inner vertex
        GX_Position3f32(xInner, yInner, 0.0f);
    }
    GX_End();
}

void drawWon(Mtx view) {
    float startX = 1.2f;
    float startY = 0.0f;
    float spacing = 0.5f;

    drawLetterW(view, startX+spacing*1.2, startY);
    drawLetterO(view, startX + spacing*2.3, startY);
    drawLetterN(view, startX + spacing*3.2, startY);
}

void drawWinner(Mtx view) {
	switch(winner)
	{
		case 'x':
			GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xFF, 0x14, 0x93, 0xFF }); // Pink color
			drawLetterX(view, 1.2f, 0.0f);
			drawWon(view);
			break;
		case 'o':
			GX_SetTevColor(GX_TEVREG1, (GXColor){ 0x80, 0x00, 0x80, 0xFF }); // Purple color
			drawLetterO(view, 1.2f, 0.0f);
			drawWon(view);
			break;
		case 't':
			GX_SetTevColor(GX_TEVREG1, (GXColor){ 0x00, 0x00, 0xFF, 0xFF }); // Blue color
			drawLetterT(view, 1.2f, 0.0f);
			drawLetterI(view, 1.8f, 0.0f);
			drawLetterE(view, 2.4f, 0.0f);
	}
}

void setupBoard() {
	for(int i=0; i<3; i++)
		for(int j=0; j<3; j++)
			board[i][j]=0;
	turn='x';
	winner='w';
	cur=1;
}

int convert(int n) {
	return n>579&&n<610?2:(n-280)/100;
}

int main(int argc, char **argv) {
	f32 yscale;

	u32 xfbHeight;

	Mtx view;
	Mtx44 perspective;
	u32	fb = 0; 	// initial framebuffer index
	GXColor background = {0, 0, 0, 0};

	VIDEO_Init();
 
	rmode = VIDEO_GetPreferredMode(NULL);

    // This function initialises the attached controllers
    WPAD_Init();

    // Enable the IR sensor
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);

	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	// setup the fifo and then init the flipper
	void *gp_fifo = NULL;
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
 
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
 
	// setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
 
	// setup the vertex attribute table
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
 
	GX_SetNumChans(1);
	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevColor(GX_TEVREG1, (GXColor){ 0xFF, 0xFF, 0xFF, 0 });
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1);
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);

	// setup our camera at the origin
	// looking down the -z axis with y up
	guVector cam =  {0.0F, 0.0F, 0.0F},
			 up =   {0.0F, 1.0F, 0.0F},
		  	 look = {0.0F, 0.0F, -1.0F};
	guLookAt(view, &cam, &up, &look);
 

	// setup our projection matrix
	// this creates a perspective matrix with a view angle of 45 degrees,
	// and aspect ratio based on the display resolution
    f32 w = rmode->viWidth;
    f32 h = rmode->viHeight;
	guPerspective(perspective, 45, (f32)w/h, 0.1F, 300.0F);
	GX_LoadProjectionMtx(perspective, GX_PERSPECTIVE);
	
 	setupBoard();
	int cursorRow=0, cursorCol=0;

    // Create an ir_t structure to store IR data
    ir_t ir;

    while(1) {
        // Call WPAD_ScanPads each loop, this reads the latest controller states
        WPAD_ScanPads();

        // Get the IR (Infrared) data
        WPAD_IR(0, &ir); // Get the IR data from the first Wii Remote (channel 0)

        if (ir.valid) {
            int x = ir.sx; // Screen X coordinate
            int y = ir.sy; // Screen Y coordinate
            //SYS_Report("x=%d, y=%d\n", x, y);
            cursorRow = convert(y);
            cursorCol = convert(x);
        }

        // WPAD_ButtonsDown tells us which buttons were pressed in this loop
        u32 pressed = WPAD_ButtonsDown(0);

		if (pressed & WPAD_BUTTON_PLUS && winner!='w') {
		    setupBoard();
		}
		if (pressed & WPAD_BUTTON_A && cursorRow>=0&&cursorRow<3&&cursorCol>=0&&cursorCol<3 && board[cursorRow][cursorCol] == 0 && winner=='w') {
			board[cursorRow][cursorCol]=turn;
			if((board[0][cursorCol]==turn&&board[1][cursorCol]==turn&&board[2][cursorCol]==turn)||(board[cursorRow][0]==turn&&board[cursorRow][1]==turn&&board[cursorRow][2]==turn)||(board[1][1]==turn&&
				((board[0][0]==turn&&board[2][2]==turn)||(board[0][2]==turn&&board[2][0]==turn))))
				winner=turn;
			else if(cur++==9)
				winner='t';
			else
				turn=turn=='x'?'o':'x';
		}
		
        // Exit the program if the Home button is pressed
        if (pressed & WPAD_BUTTON_HOME)
        	return 0;
        	
		// Do this before drawing
		GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
		
		drawGrid(view);
		if(cursorRow>=0&&cursorRow<3&&cursorCol>=0&&cursorCol<3)
			drawCursor(view, cursorRow, cursorCol);
		for (int row = 0; row < 3; row++) {
		    for (int col = 0; col < 3; col++) {
		        if (board[row][col] == 'x') {
		            drawX(view, row, col);
		        }
		        else if(board[row][col] == 'o') {
		        	drawO(view, row, col);
		        }
		    }
		}
		drawWinner(view);
		
		// Do this stuff after drawing
		GX_DrawDone();
		fb ^= 1;  // Flip framebuffer
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer[fb], GX_TRUE);
		VIDEO_SetNextFramebuffer(frameBuffer[fb]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
    }
    return 0;
}

