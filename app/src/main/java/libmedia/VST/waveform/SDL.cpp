#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "SDL.h"
#include "SDL_main.h"
#include <VST.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;
SDL_Texture *texture;
SDL_Event event;
int H, W;

VST_TYPEDEF_RETURN_TYPE main() {
    SDL_Init(SDL_INIT_VIDEO);
    if (SDL_CreateWindowAndRenderer(0, 0, 0, &window, &renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "SDL_CreateWindowAndRenderer() failed: %s", SDL_GetError());
        return VST_RETURN_STOP;
    }
    return VST_RETURN_CONTINUE;
}

extern "C" VST_FUNCTION_CREATE {
    return main();
}

extern "C" VST_FUNCTION_RESIZE {
    return VST_RETURN_CONTINUE;
    H = height;
    W = width;
    SDL_SetWindowSize(window, W, H);
    return true;
}

bool drawonce = false;

extern "C" VST_FUNCTION_DRAW {
    return VST_RETURN_STOP;
    if (!drawonce) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        for (int i = 0; i < W; ++i)
            SDL_RenderDrawPoint(renderer, i, i);
        SDL_RenderPresent(renderer);
        drawonce = true;
    }
    return true;
}

extern "C" VST_FUNCTION_DESTROY {
    return VST_RETURN_CONTINUE;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;
}