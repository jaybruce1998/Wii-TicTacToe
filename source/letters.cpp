#include <gccore.h>
#include <math.h>

void drawLetterX(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    guVector axis;
    float quad_size = 0.5f; // Size of the letter's components halved
    float thickness = 0.05f; // Thickness halved

    // First diagonal line (top-left to bottom-right)
    guMtxIdentity(model);
    axis.x = 0.0f;
    axis.y = 0.0f;
    axis.z = 1.0f;
    guMtxRotAxisDeg(model, &axis, 45); // Rotate to diagonal
    guMtxTransApply(model, model, posX, posY, -6.0f); // Position the X on the screen
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -thickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -thickness / 2, 0.0f);
    GX_End();

    // Second diagonal line (bottom-left to top-right)
    guMtxIdentity(model);
    guMtxRotAxisDeg(model, &axis, -45); // Rotate to opposite diagonal
    guMtxTransApply(model, model, posX, posY, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -thickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -thickness / 2, 0.0f);
    GX_End();
}

void drawLetterW(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    guVector axis;
    float quad_size = 0.5f; // Size halved
    float thickness = 0.05f; // Thickness halved
    float offset = 0.125f;    // Adjusted for size

    // First diagonal line of the first V (top-left to bottom-middle)
    guMtxIdentity(model);
    axis.x = 0.0f;
    axis.y = 0.0f;
    axis.z = 1.0f;
    guMtxRotAxisDeg(model, &axis, 15); // Rotate to the left diagonal
    guMtxTransApply(model, model, posX - offset, posY, -6.0f); // Position to the left side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    // Second diagonal line of the first V (bottom-middle to top-right)
    guMtxIdentity(model);
    guMtxRotAxisDeg(model, &axis, -15); // Rotate to the right diagonal
    guMtxTransApply(model, model, posX, posY, -6.0f); // Position to the right side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    // First diagonal line of the second V (top-right to bottom-middle)
    guMtxIdentity(model);
    guMtxRotAxisDeg(model, &axis, 15); // Rotate for the right side of the W
    guMtxTransApply(model, model, posX + offset, posY, -6.0f); // Position to the right side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    guMtxIdentity(model);
    guMtxRotAxisDeg(model, &axis, -15); // Rotate to the right diagonal
    guMtxTransApply(model, model, posX + offset*2, posY, -6.0f); // Position to the right side
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();
}

void drawLetterO(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    int numSegments = 40;  // Number of segments for the circle
    f32 radiusOuter = 0.25f; // Halved outer radius
    f32 radiusInner = radiusOuter - 0.1f; // Halved inner radius
    f32 angleStep = 2 * M_PI / numSegments;  // Step size for each segment

    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY, -6.0f);  // Position the O
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
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

void drawLetterN(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    guVector axis;
    float quad_size = 0.5f; // Size halved
    float thickness = 0.05f; // Thickness halved
    float offset = 0.125f; // Adjusted for size

    // Left vertical line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX - offset, posY, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    // Right vertical line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX + offset, posY, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    guMtxIdentity(model);
    axis.x = 0.0f;
    axis.y = 0.0f;
    axis.z = 1.0f;
    guMtxRotAxisDeg(model, &axis, -65); // Rotate to diagonal
    guMtxTransApply(model, model, posX, posY, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32( quad_size / 2, -thickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -thickness / 2, 0.0f);
    GX_End();
}

void drawLetterT(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    float quad_size = 0.5f; // Define the size of the letter's components
    float thickness = 0.1f; // Define how thick the lines should be
    float offset = 0.25f;    // Offset for horizontal lines
    
    // Vertical line (the body of the T)
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    // Top horizontal line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY + quad_size / 2, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-offset, thickness / 2, 0.0f);
        GX_Position3f32(offset, thickness / 2, 0.0f);
        GX_Position3f32(offset, -thickness / 2, 0.0f);
        GX_Position3f32(-offset, -thickness / 2, 0.0f);
    GX_End();
}
void drawBottomLine(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    float thickness = 0.05f; // Define how thick the lines should be
    float offset = 0.25f;   // Offset for horizontal lines
    
    // Bottom horizontal line (match the length of the top line)
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY - offset, -6.0f); // Position below the vertical line
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-offset, thickness / 2, 0.0f); // Left side of the bottom line
        GX_Position3f32(offset, thickness / 2, 0.0f);  // Right side of the bottom line
        GX_Position3f32(offset, -thickness / 2, 0.0f); // Lower right corner
        GX_Position3f32(-offset, -thickness / 2, 0.0f);// Lower left corner
    GX_End();
}
void drawLetterI(Mtx view, float posX, float posY) {
    drawLetterT(view, posX, posY);
    drawBottomLine(view, posX, posY);
}

void drawLetterE(Mtx view, float posX, float posY) {
    Mtx model, modelview;
    float quad_size = 0.5f; // Size halved
    float thickness = 0.05f; // Thickness halved

    // Vertical line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX-quad_size/2, posY, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, -quad_size / 2, 0.0f);
        GX_Position3f32(thickness / 2, quad_size / 2, 0.0f);
        GX_Position3f32(-thickness / 2, quad_size / 2, 0.0f);
    GX_End();

    // Top horizontal line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY + quad_size / 2, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32(quad_size / 2, thickness / 2, 0.0f);
        GX_Position3f32(quad_size / 2, -thickness / 2, 0.0f);
        GX_Position3f32(-quad_size / 2, -thickness / 2, 0.0f);
    GX_End();

    // Middle horizontal line
    guMtxIdentity(model);
    guMtxTransApply(model, model, posX, posY, -6.0f);
    guMtxConcat(view, model, modelview);
    GX_LoadPosMtxImm(modelview, GX_PNMTX0);
    
    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
        GX_Position3f32(-quad_size / 2, 0.0f, 0.0f);
        GX_Position3f32(quad_size / 2, 0.0f, 0.0f);
        GX_Position3f32(quad_size / 2, -thickness, 0.0f);
        GX_Position3f32(-quad_size / 2, -thickness, 0.0f);
    GX_End();
    drawBottomLine(view, posX, posY);
}
