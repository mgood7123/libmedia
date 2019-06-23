//
// Created by macropreprocessor on 21/06/19.
//

#ifndef MEDIA_PLAYER_PRO_PIXELTYPES_H
#define MEDIA_PLAYER_PRO_PIXELTYPES_H

int pixelFrames = 0;

void pixelDraw_RGB_565(AndroidBitmapInfo *info, void *pixels, int16_t row, int16_t column, uint16_t colour) {
    ((uint16_t*)((char*)pixels + static_cast<uint32_t>(info->stride*row)))[column] = colour;
    pixelFrames++;
}
// usage: pixelDraw<uint32_t>(pImage, 1, 0, 0xFFFFFF00u, 16);
template<typename T>
void pixelDraw2(void *pixels, int16_t row, int16_t column, T colour, uint32_t stride)
{
    T *p = reinterpret_cast<T*>(pixels);
    p[(row*(stride/sizeof(T))) + column] = colour;
    pixelFrames++;
}

template<typename T>
void pixelDraw(void *pixels, int16_t row, int16_t column, T colour, uint32_t stride)
{
    T *p = reinterpret_cast<T*>(
            reinterpret_cast<uint8_t*>(pixels) + (stride * row)
    );

    p[column] = colour;
    pixelFrames++;
}

//
// Make a colour from bytes like web colours #RRGGBB
//
constexpr uint16_t pixelColourRGB_565(uint8_t red, uint8_t green, uint8_t blue)
{
    return
            (red >> 3) << 11
            | (green >> 2) << 5
            | (blue >> 3);
}

#endif //MEDIA_PLAYER_PRO_PIXELTYPES_H
