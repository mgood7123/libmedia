//
// Created by konek on 6/29/2019.
//

#include <android/bitmap.h>
#include <malloc.h>
#include <iostream>
#include <android/log.h>
#include "bitmap.h"
#include "pixelTypes.h"
#include "../waveform/timestats.h"

#define  LOG_TAG "libbitmap"

Canvas::Canvas(AndroidBitmapInfo *info, void *pixels) {
    canvasInfo = info;
    canvas = pixels;
}

void Canvas::flush() {
    data.flush(canvas);
}

void Canvas::pixel(int row, int column) {
    if (!buffered) static_cast<uint16_t *>(canvas)[((canvasInfo->stride*row)/2) + column] = color.get();
    else data.add(*canvasInfo, row, column, color);
}

void Canvas::lineHorizontal(int row, int start, int end){
    for (int column = start; column <= end; column++)
        pixel(row, column);
}

void Canvas::lineCenteredHorizontal(int start, int end){
    lineHorizontal(canvasInfo->height/2, start, end);
}

void Canvas::lineVertical(int column, int start, int end){
    for (int row = start; row <= end; row ++)
        pixel(row, column);
}

void Canvas::lineCenteredVertical(int start, int end){
    lineVertical(canvasInfo->width/2, start, end);
}

void Canvas::line_segment(int x1, int y1, int x2, int y2)
{
    int steep = 0;
    int sx    = ((x2 - x1) > 0) ? 1 : -1;
    int sy    = ((y2 - y1) > 0) ? 1 : -1;
    int dx    = abs(x2 - x1);
    int dy    = abs(y2 - y1);

    if (dy > dx)
    {
        std::swap(x1,y1);
        std::swap(dx,dy);
        std::swap(sx,sy);

        steep = 1;
    }

    int e = 2 * dy - dx;

    for (int i = 0; i < dx; ++i)
    {
        if (steep)
            pixel(y1,x1);
        else
            pixel(x1,y1);

        while (e >= 0)
        {
            y1 += sy;
            e -= (dx << 1);
        }

        x1 += sx;
        e  += (dy << 1);
    }

    pixel(x2,y2);
}


void Canvas::rectangle(int row_start, int column_start, int row_end, int column_end) {
    lineVertical(row_start, column_start, column_end);
}

void Canvas::rectangle(int row, int column, int size) {
//    canvas.rectangle(400, 400, 400);
//    void lineVertical(int row, int start, int end);
//    void lineHorizontal(int column, int start, int end);
    lineVertical(row, column, column+size);        // 400, 400, 800
    lineVertical(row+size, column, column+size);   // 800, 400, 800
    lineHorizontal(row, column, column+size);      // 400, 400, 800
    lineHorizontal(row+size, column, column+size); // 800, 400, 800
}

void Canvas::circle(int centerx, int centery, int radius) {

    int x = 0;
    int d = (1 - radius) << 1;

    while (radius >= 0) {
        pixel(centerx + x, centery + radius);
        pixel(centerx + x, centery - radius);
        pixel(centerx - x, centery + radius);
        pixel(centerx - x, centery - radius);
        if ((d + radius) > 0)
            d -= ((--radius) << 1) - 1;
        if (x > d)
            d += ((++x) << 1) + 1;
    }
}

void Canvas::quartercircle(int centerx, int centery, int radius) {

    int x = 0;
    int d = (1 - radius) << 1;

    while (radius >= 0) {
        pixel(centerx - x, centery + radius);
        if ((d + radius) > 0)
            d -= ((--radius) << 1) - 1;
        if (x > d)
            d += ((++x) << 1) + 1;
    }
}

void Canvas::semicircle(int centerx, int centery, int radius) {

    int x = 0;
    int d = (1 - radius) << 1;

    while (radius >= 0) {
        pixel(centerx - x, centery + radius);
        pixel(centerx - x, centery - radius);
        if ((d + radius) > 0)
            d -= ((--radius) << 1) - 1;
        if (x > d)
            d += ((++x) << 1) + 1;
    }
}


void Canvas::clear() {
    double start = now_ms();
    memset(canvas, 0, canvasInfo->height*canvasInfo->width*2);
    double end = now_ms();
    __android_log_print(ANDROID_LOG_INFO,LOG_TAG, "cleared bitmap (with a size of %d) in %G milliseconds", canvasInfo->height*canvasInfo->width, end - start);
    color.restore(*this);
}

// color

uint16_t Canvas::Color::get() {
    return
            (red >> 3) << 11
            | (green >> 2) << 5
            | (blue >> 3);
}

void Canvas::Color::set(uint8_t red, uint8_t green, uint8_t blue){
    this->red = red;
    this->green = green;
    this->blue = blue;
}

void Canvas::Color::copy(Color oldColor){
    this->red = oldColor.red;
    this->green = oldColor.green;
    this->blue = oldColor.blue;
}

void Canvas::Color::save(Canvas canvas) {
    canvas.colorSaved.copy(*this);
}

void Canvas::Color::restore(Canvas canvas){
    this->copy(canvas.colorSaved);
}

// data


void Canvas::data::add(AndroidBitmapInfo &canvasInfo, int row, int column, Canvas::Color color) {
    dataInternal.push_back(dataClass{((canvasInfo.stride*row)/2) + column, color});
}

void Canvas::data::removeFirst() {
    dataInternal.pop_front();
}

void Canvas::data::removeLast() {
    dataInternal.pop_back();
}

void Canvas::data::flush(void *ptr) {
    while (!dataInternal.empty()) {
        dataClass data = dataInternal.front();
        static_cast<uint16_t *>(ptr)[data.index] = data.color.get();
        dataInternal.pop_front();
    }
}
