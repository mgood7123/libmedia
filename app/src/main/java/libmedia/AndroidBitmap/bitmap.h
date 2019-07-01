//
// Created by konek on 6/29/2019.
//

#ifndef MEDIA_PLAYER_PRO_BITMAP_H
#define MEDIA_PLAYER_PRO_BITMAP_H

#include <android/bitmap.h>
#include <cassert>
#include <deque>

class Canvas {
public:
    Canvas(AndroidBitmapInfo *info, void *pixels);

    AndroidBitmapInfo *canvasInfo;
    void *canvas = nullptr;
    class Color {
    public:
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        void set(uint8_t red, uint8_t green, uint8_t blue);
        uint16_t get();
        void save(Canvas canvas);
        void restore(Canvas canvas);
        void copy(Color oldColor);
    };
    Color color, colorSaved;
    class data {
    public:
        int size;
        class dataClass {
        public:
            unsigned long index;
            Color color;
        };
        std::deque<dataClass> dataInternal;
        void add(AndroidBitmapInfo & canvasInfo, int row, int column, Canvas::Color color);
        void removeLast();

        void flush(void *ptr);

        void removeFirst();
    } data;

    void pixel(int row, int column);

    void lineVertical(int column, int start, int end);

    void lineHorizontal(int row, int start, int end);

    void lineCenteredVertical(int start, int end);

    void lineCenteredHorizontal(int start, int end);

    void rectangle(int row_start, int column_start, int row_end, int column_end);

    void rectangle(int row, int column, int size);

    void circle(int centerx, int centery, int radius);

    void semicircle(int centerx, int centery, int radius);

    void quartercircle(int centerx, int centery, int radius);

    void flush();

    void clear();

    bool buffered = false;

    void line_segment(int x1, int y1, int x2, int y2);

};

#endif //MEDIA_PLAYER_PRO_BITMAP_H
