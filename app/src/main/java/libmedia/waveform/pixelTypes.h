//
// Created by macropreprocessor on 21/06/19.
//

#ifndef MEDIA_PLAYER_PRO_PIXELTYPES_H
#define MEDIA_PLAYER_PRO_PIXELTYPES_H

extern int pixelFrames;

extern void pixelDraw_RGB_565(AndroidBitmapInfo *info, void *pixels, int16_t row, int16_t column, uint16_t colour);

// usage: pixelDraw<uint32_t>(pImage, 1, 0, 0xFFFFFF00u, 16);
template<typename T>
extern void pixelDraw2(void *pixels, int16_t row, int16_t column, T colour, uint32_t stride);

// usage: pixelDraw<uint32_t>(pImage, 1, 0, 0xFFFFFF00u, 16);
template<typename T>
extern void pixelDraw(void *pixels, int16_t row, int16_t column, T colour, uint32_t stride);

//
// Make a colour from bytes like web colours #RRGGBB
//
extern uint16_t pixelColourRGB_565(uint8_t red, uint8_t green, uint8_t blue);

#endif //MEDIA_PLAYER_PRO_PIXELTYPES_H
