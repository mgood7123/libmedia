//
// Created by konek on 6/29/2019.
//

#include <android/bitmap.h>
#include <malloc.h>
#include <iostream>
#include "bitmap.h"
#include "pixelTypes.h"

Canvas::Canvas(AndroidBitmapInfo *info, void *pixels) {
    canvas = pixels;
    canvasInfo = info;
}

void Canvas::flush() {
    data.flush(canvas);
}

void Canvas::pixel(int row, int column) {
    data.add(*canvasInfo, row, column, color);
    if (!buffered) flush();
}

void Canvas::setColor(uint8_t red, uint8_t green, uint8_t blue) {
    color.red = red;
    color.green = green;
    color.blue = blue;
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

uint16_t Canvas::Color::get() {
    return
            (red >> 3) << 11
            | (green >> 2) << 5
            | (blue >> 3);
}

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
