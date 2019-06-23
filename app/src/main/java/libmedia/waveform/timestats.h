//
// Created by macropreprocessor on 20/06/19.
//

#ifndef MEDIA_PLAYER_PRO_TIMESTATS_H
#define MEDIA_PLAYER_PRO_TIMESTATS_H

/* simple stats management */
struct FrameStats {
    double  renderTime;
    double  frameTime;
};

#define  MAX_FRAME_STATS  200
#define  MAX_PERIOD_MS    1500

struct Stats {
    double  firstTime;
    double  lastTime;
    double  frameTime;

    int         firstFrame;
    int         numFrames;
    struct FrameStats  frames[ MAX_FRAME_STATS ];
};

extern void stats_init( Stats*  s );
extern void stats_startFrame( Stats*  s );
extern void stats_endFrame( Stats*  s );

#endif //MEDIA_PLAYER_PRO_TIMESTATS_H
